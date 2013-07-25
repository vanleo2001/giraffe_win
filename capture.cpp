#include <map>
#include "capture.h"

void Capture::Init()
{
	ReservePool(kPoolSize);
	cout<<"complete the initialization!"<<endl;
}

bool Capture::ReservePool(int pool_size)
{
	for(int i=curent_pool_size_;i<curent_pool_size_ + pool_size;i++)
	{
		//init capture zmq property from config file
		deque<XML_ZMQ> *cap_zmq_deque = listening_item_.get_cap()->get_zmqdeque();
		for(deque<XML_ZMQ>::iterator iter=cap_zmq_deque->begin();iter!=cap_zmq_deque->end();iter++)
		{
			ZMQItem cap_zmq_item;
			cap_zmq_item.zmqpattern = (*iter).get_zmqpattern();
			cap_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",i);
			cap_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
			AddZMQItem(cap_zmq_item);
		}
		//init cap socket
		zmq::socket_t *sock = new zmq::socket_t (*context_,this->zmqitems_[i].zmqpattern);
		if("bind" == this->zmqitems_[i].zmqsocketaction)
		{
			sock->bind(this->zmqitems_[i].zmqsocketaddr.c_str());
		}
		else if("connect" == this->zmqitems_[i].zmqsocketaction)
		{
			sock->connect(this->zmqitems_[i].zmqsocketaddr.c_str());
		}
		sock_deque_.push_back(sock);
		Sleep(100);
		//init parse zmq property from config file
		Parse* parse = new Parse(context_ , listening_item_);
		deque<XML_ZMQ>* parse_zmq_deque = listening_item_.get_parse()->get_zmqdeque();
		for(deque<XML_ZMQ>::iterator iter = parse_zmq_deque->begin();iter!=parse_zmq_deque->end();iter++)
		{
			ZMQItem parse_zmq_item;
			parse_zmq_item.zmqpattern =(*iter).get_zmqpattern();
			parse_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
			if("inproc://log" == (*iter).get_zmqsocketaddr())
			{
				parse_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
			}
			else
			{
				char buf[16];
				memset(buf,0,sizeof(buf));
				sprintf(buf,"%d",i);
				parse_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
			}
			parse->AddZMQItem(parse_zmq_item);
		}
		parse->Start();
		parse_deque_.push_back(parse);
	}
	curent_pool_size_ += pool_size;
	return true;
}

void * Capture::RunThreadFunc()
{
    pcap_if_t * alldevs ;
    char *errbuf= new char[PCAP_ERRBUF_SIZE];
    int devsnum= 0;
    pcap_if_t *d;
    int i;
    int inum = adapter_id_;
    int len=65535;
    int mode= 1;
    int timeout=1000;
    pcap_addr_t *a;
    struct bpf_program fcode;
    int filtertag = 0;
    pcap_if_t *selecteddev;
    pcap_t *adhandle;
    unsigned int netmask;
    InCapThreadParam ictp;
	if(pcap_findalldevs(&alldevs,errbuf) == -1)
	{
		cout<<"Error in pcap_findalldevs %s "<<errbuf<<endl;
		return ((void *)1);
	}

	for(selecteddev=alldevs,i=0; i<inum-1; i++,selecteddev=selecteddev->next);

	if((adhandle = pcap_open_live(selecteddev->name,len,mode,timeout,errbuf))==NULL)
	{
		cout<<"Unable to open the adapter! "<<selecteddev->name<<" is not supported by libpcap or winpcap!"<<endl;
		pcap_freealldevs(alldevs);
		return ((void *)1);
	}
	else
	{
		/* Check the link layer. We support only Ethernet for simplicity. */
		if(pcap_datalink(adhandle) != DLT_EN10MB)
		{
			cout<<"This program works only on Ethernet networks!"<<endl;
			pcap_freealldevs(alldevs);
			return ((void *)1);
		}

		for(a=selecteddev->addresses; a; a=a->next)
		{
			if(a->addr->sa_family == AF_INET)
			{
				netmask = ((struct sockaddr_in *)a->netmask)->sin_addr.s_addr;
				break;
			}
		}
	}

	//compile the filter
	if(pcap_compile(adhandle,&fcode,listening_item_.get_filter().c_str(),1,netmask) < 0)
	{
		cout<<"Unable to compile the packet filter. Check the syntax."<<endl;
		pcap_freealldevs(alldevs);
		return ((void *)1);
	}

	//set the filter
	if(pcap_setfilter(adhandle, &fcode)<0)
	{
		cout<<"Error setting the filter!"<<endl;
		pcap_freealldevs(alldevs);
		return ((void *)1);
	}
	
	//dump file
	pcap_dumper_t *dumper;
	dumper = pcap_dump_open(adhandle,"dumper.txt");
	if(dumper == NULL)
	{
		cout<<"can't open dumper filer~!"<<endl;
	}

	delete [] errbuf;
	pcap_freealldevs(alldevs);


	if(pcap_loop(adhandle,0,PacketHandler,(unsigned char *)this) < 0)
	{
		cout<<"pcap_loop:"<<pcap_geterr(adhandle)<<endl;
	}
	
	//pcap_loop(adhandle,0,PacketHandler,(u_char *)dumper);
	//pcap_dump_close(dumper);
	return ((void *)0);
}
//
//void Capture::PacketHandler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data)
//{
//	pcap_dump(param,header,pkt_data);
//}


void Capture::PacketHandler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data)
{
	ip_head *ih;
	tcp_head *tcph;
	int iph_len;
	char key_ip_src[32];
	char key_ip_dst[32];
	XML_ListeningItem *listening_item;
	std::string filter;
	int port;
	deque<zmq::socket_t *> *sock_deque;
	map<std::string,zmq::socket_t*>* sock_map;
	static int tcpconntag = 0;
	static int tcpconnstatus = 0;
	static int tcpdisconntag = 0;
	static int tcpdisconnstatus = 0;
	static int fintag = 0;
	static int acktag = 0;

	Capture *cap = (Capture *)param;
	sock_deque = &(cap->sock_deque_);
	sock_map = &(cap->sock_map_);
	listening_item = &(cap->listening_item_);
	filter = listening_item->get_filter();
	int pos = filter.find("port");
	std::string temp_str = (filter.substr(pos+5,filter.size()-pos-5)).c_str();
	port = atoi(temp_str.c_str());

	ih = (ip_head *)(pkt_data + 14);
	iph_len = (ih->ver_ihl & 0xf) * 4;
	tcph = (tcp_head *)((char *)ih + iph_len);

	if(TCP == ih->protocol)
	{//1.ÅÐ¶ÏtcpÁ¬½Ó×´Ì¬ºÍ¶Ï¿ª×´Ì¬
		memset(key_ip_src,0,sizeof(key_ip_src));
		memset(key_ip_dst,0,sizeof(key_ip_dst));
		sprintf(key_ip_src,"%d.%d.%d.%d:%d",ih->saddr.byte1,ih->saddr.byte2,ih->saddr.byte3,ih->saddr.byte4,ntohs(tcph->source));
		sprintf(key_ip_dst,"%d.%d.%d.%d:%d",ih->daddr.byte1,ih->daddr.byte2,ih->daddr.byte3,ih->daddr.byte4,ntohs(tcph->dest));

        if(SYN == tcph->flags)
        {
            tcpconntag = 1;
            tcpconnstatus = 0;
        }
        if(1 == tcpconntag)
        {
            if(SYN == tcph->flags)
            {
                tcpconnstatus |= 0x1;
            }
            else if(SYNACK == tcph->flags)
            {
                tcpconnstatus |= 0x2;
            }
            else if(ACK == tcph->flags)
            {
                tcpconnstatus |= 0x4;
                tcpconntag = 0;
				if(7 == tcpconnstatus)
				{
					cout<<"A new connection was been built! The ip and port is:"<<key_ip_src<<endl;
					tcpconnstatus = 0;
					if(sock_map->end() == sock_map->find(key_ip_src))
					{	
						if(!sock_deque->empty())
						{
							zmq::socket_t * sock = sock_deque->front();
							sock_deque->pop_front();
							sock_map->insert(pair<std::string,zmq::socket_t *>(key_ip_src,sock));
							cout<<"connection:key_ip:"<<key_ip_src<<endl;
						}
						else
						{
							if(cap->ReservePool(kPoolSize))
							{
								zmq::socket_t *sock = sock_deque->front();
								sock_deque->pop_front();
								sock_map->insert(pair<std::string,zmq::socket_t *>(key_ip_src,sock));
								cout<<"connection:key_ip:"<<key_ip_src<<endl;
							}
							else
							{
								return ;
							}
						}
					}
				}
				else
				{
					tcpconnstatus = 0;
				}
            }
            else
            {
                tcpconntag = 0;
                tcpconnstatus = 0;
            }
        }
		
		//if(FINACK == tcph->flags && tcpdisconntag == 0 && port == ntohs(tcph->dest))
		//{
		//	tcpdisconntag = 1;
		//	tcpdisconnstatus = 0;
		//	tcpdisconnstatus |= 0x1;
		//}
		if(FINACK == tcph->flags && port == ntohs(tcph->source))
		{
			map<std::string,zmq::socket_t*>::iterator iter_map;
			cout<<"disconnect:key_ip"<<key_ip_dst<<endl;
			if((iter_map=sock_map->find(key_ip_dst)) != sock_map->end())
			{
				zmq::socket_t * sock =iter_map->second;
				sock_deque->push_back(sock);
				sock_map->erase(iter_map);
				cout<<key_ip_dst<<" was disconnected!"<<endl;
			}
			else
			{
				cout<<"kill threads,but can't find the connection thread!"<<endl;
			}
		}

		map<std::string,zmq::socket_t*>::iterator iter;
		if((iter=sock_map->find(key_ip_dst)) != sock_map->end())
		{
			/*cout<<"key_ip_dst:"<<key_ip_dst<<endl;*/
			zmq::socket_t * sock = iter->second;
			pcap_work_item item;
			item.port_tag = port;
			item.header = *header;
			memcpy(item.data,pkt_data,header->caplen);
			try
			{
				zmq::message_t msg (sizeof(pcap_work_item));
				memcpy((void*)(msg.data()),&item,sizeof(item));
				//sock->send(msg,ZMQ_NOBLOCK);
				int ret = sock->send(msg);
				assert(true == ret);
			}
			catch(zmq::error_t error)
			{
				cout<<"cap: zmq send error!"<<error.what()<<endl;
			}
		}
	//	}
	}
}