#include <map>
#include "capture.h"


pthread_key_t key_cap;
pthread_once_t once_control_cap = PTHREAD_ONCE_INIT;

void CreateThreadKey()
{
	pthread_key_create(&key_cap, NULL);
}

void Capture::Init()
{
	IncreasePool(kPoolSize);
	cout<<"complete the initialization!"<<endl;
}

void Capture::InitCaptureZMQProperty(int index)
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
		sprintf(buf,"%d",index);
		cap_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		AddZMQItem(cap_zmq_item);
	}
}

void Capture::AddToZMQDeque(int index)
{
	//init cap socket
	zmq::socket_t *sock = new zmq::socket_t (*context_,this->zmqitems_[index].zmqpattern);
	if("bind" == this->zmqitems_[index].zmqsocketaction)
	{
		sock->bind(this->zmqitems_[index].zmqsocketaddr.c_str());
	}
	else if("connect" == this->zmqitems_[index].zmqsocketaction)
	{
		sock->connect(this->zmqitems_[index].zmqsocketaddr.c_str());
	}
	sock_deque_.push_back(sock);
}

void Capture::RunLuaRoutineThread(int index)
{
	//init lua routine zmq property from config file
	LuaRoutine* lua_routine = new LuaRoutine(context_,listening_item_);
	deque<XML_ZMQ>* lua_routine_zmq_deque = listening_item_.get_lua_routine()->get_zmqdeque();
	for(deque<XML_ZMQ>::iterator iter = lua_routine_zmq_deque->begin();iter!=lua_routine_zmq_deque->end();iter++)
	{
		ZMQItem lua_routine_zmq_item;
		lua_routine_zmq_item.zmqpattern = (*iter).get_zmqpattern();
		lua_routine_zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		char buf[16];
		memset(buf,0,sizeof(buf));
		sprintf(buf,"%d",index);
		lua_routine_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		lua_routine->AddZMQItem(lua_routine_zmq_item);
	}
	lua_routine->Init();
	lua_routine->Start();
	lua_routine_deque_.push_back(lua_routine);
}

void Capture::RunParseThread(int index)
{
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
			sprintf(buf,"%d",index);
			parse_zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr() + buf;
		}
		parse->AddZMQItem(parse_zmq_item);
	}
	parse->Init();
	parse->Start();
	parse_deque_.push_back(parse);
}

void set_inner_thread_params(const void * value)
{
	pthread_once(&once_control_cap, CreateThreadKey);
	pthread_setspecific(key_cap, value);
}

void * get_inner_thread_params()
{
	return pthread_getspecific(key_cap);
}

//void Capture::ListenTcpConnection()
//{
//
//}
//
//void Capture::ListenTcpDisconnction()
//{
//
//}

bool Capture::IncreasePool(int pool_size)
{
	for(int i=curent_pool_size_;i<curent_pool_size_ + pool_size;i++)
	{
		InitCaptureZMQProperty(i);
		AddToZMQDeque(i);
#ifndef __linux
		Sleep(100);
#else
		sleep(1);
#endif
		RunLuaRoutineThread(i);
		RunParseThread(i);
	}
	curent_pool_size_ += pool_size;
	return true;
}

void CreateDidConfContent(vector<DidStruct> & did_structs, char * out_str)
{
	assert(NULL != out_str);
	char did_content[256] = {0};
	strcat(out_str, "<?xml version=\"1.0\" encoding=\"gb2312\" ?>\n <DidStruct>\n");
	for(vector<DidStruct>::iterator iter = did_structs.begin();iter != did_structs.end();iter++)
	{
		memset(did_content, 0,sizeof(did_content));
		sprintf(did_content, "\t <did id=\"%d\" file=\"%s\" whole=\"%u\" compress=\"%u\" /> \n", iter->id ,iter->file_path.c_str(),iter->whole_tag,iter->compress_tag);
		strcat(out_str, did_content);
	}
	strcat(out_str, "</DidStruct>\n");
}

//void WriteIntoFile(const char *file_name, const char *mode, const void* data , size_t length)
//{
//	FILE * fp = fopen(file_name, mode);
//	if(NULL != fp)
//	{
//		fwrite(data, 1, length, fp);
//		fclose(fp);
//	}
//	else
//	{
//		cout<<"open file error!"<<endl;
//		assert(0);
//	}
//}

void WriteDidConfFile(const char * file_name, vector<DidStruct> &did_structs)
{
	char file_content[2048] = {0};
	CreateDidConfContent(did_structs, file_content);
	Utils::WriteIntoFile(file_name, "wb", file_content, strlen(file_content));
}

bool IsTcpConnection(unsigned char flags, int &tcpconntag, int &tcpconnstatus)
{
	if(SYN == flags)
	{
		tcpconntag = 1;
		tcpconnstatus = 0;
	}
	if(1 == tcpconntag)
	{
		if(SYN == flags)
		{
			tcpconnstatus |= 0x1;
		}
		else if(SYNACK == flags)
		{
			tcpconnstatus |= 0x2;
		}
		else if(ACK == flags)
		{
			tcpconnstatus |= 0x4;
			tcpconntag = 0;
			if(7 == tcpconnstatus)
			{
				tcpconnstatus = 0;
				return true;		
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
	return false;
}

bool IsTcpDisConnection(unsigned char flags, int port, unsigned short src_port)
{
	if(FINACK == flags && port == ntohs(src_port))
	{
		return true;
	}
	return false;
}

void DispatchData(zmq::socket_t * sock, void * data, int size)
{
	assert(NULL != sock && NULL != data);
	try
	{
		zmq::message_t msg(size);
		memcpy((void*)(msg.data()), data, size); 
		//sock->send(msg,ZMQ_NOBLOCK);
		int ret = sock->send(msg);
	}
	catch(zmq::error_t error)
	{
		cout<<"cap: zmq send error! error content:"<<error.what()<<endl;
		assert(0);
	}
}

void * Capture::RunThreadFunc()
{
	remove("test_data.txt");
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
    pcap_if_t *selecteddev;
    pcap_t *adhandle;
    unsigned int netmask;
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
	//pcap_dumper_t *dumper;
	//dumper = pcap_dump_open(adhandle,"dumper.txt");
	//if(dumper == NULL)
	//{
	//	cout<<"can't open dumper filer~!"<<endl;
	//}

	delete [] errbuf;
	errbuf = NULL;
	pcap_freealldevs(alldevs);

	CapInnerThreadParam in_thread_param;
	in_thread_param.acktag = 0;
	in_thread_param.fintag = 0;
	in_thread_param.tcpconnstatus = 0;
	in_thread_param.tcpconntag = 0;
	in_thread_param.tcpdisconnstatus = 0;
	in_thread_param.tcpdisconntag = 0;

	set_inner_thread_params(&in_thread_param);


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
	int tcph_len = 0;
	int head_len = 0;
	unsigned long tcp_current_seq = 0;
	unsigned long tcp_data_len = 0;
	char key_ip_src[32];
	char key_ip_dst[32];
	XML_ListeningItem *listening_item;
	std::string filter;
	int port;
	deque<zmq::socket_t *> *sock_deque;
	map<std::string,zmq::socket_t*>* sock_map;

	CapInnerThreadParam * in_thread_params = static_cast<CapInnerThreadParam *>(get_inner_thread_params());
	int &acktag = in_thread_params->acktag;
	int &fintag = in_thread_params->fintag;
	int & tcpconnstatus = in_thread_params->tcpconnstatus;
	int & tcpconntag = in_thread_params->tcpconntag;
	int & tcpdisconnstatus = in_thread_params->tcpdisconnstatus;
	int & tcpdisconntag = in_thread_params->tcpdisconntag;

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
	tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
	head_len = iph_len + tcph_len;

	memset(key_ip_src,0,sizeof(key_ip_src));
	memset(key_ip_dst,0,sizeof(key_ip_dst));
	sprintf(key_ip_src,"%d.%d.%d.%d:%d",ih->saddr.byte1,ih->saddr.byte2,ih->saddr.byte3,ih->saddr.byte4,ntohs(tcph->source));
	sprintf(key_ip_dst,"%d.%d.%d.%d:%d",ih->daddr.byte1,ih->daddr.byte2,ih->daddr.byte3,ih->daddr.byte4,ntohs(tcph->dest));

	if(TCP == ih->protocol)
	{//1.ÅÐ¶ÏtcpÁ¬½Ó×´Ì¬ºÍ¶Ï¿ª×´Ì¬
		if(IsTcpConnection(tcph->flags, tcpconntag, tcpconnstatus))
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
					if(cap->IncreasePool(kPoolSize))
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
		if(IsTcpDisConnection(tcph->flags, port, tcph->source))
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
			tcp_data_len = ntohs(ih->tlen) - head_len;//must use ih->tlen, because sometime it will have supplement package.
			tcp_current_seq = ntohl(tcph->seq);
			cout<<"cap:current_seq:"<<tcp_current_seq<<" data_len:"<<tcp_data_len<<endl;
		//caishu  --> zhongzhuan
		if((iter=sock_map->find(key_ip_dst)) != sock_map->end())
		{

			/*cout<<"key_ip_dst:"<<key_ip_dst<<endl;*/
			pcap_work_item item;
			item.port_tag = port;
			item.header = *header;
			memcpy(item.data,pkt_data,header->caplen);
			DispatchData(iter->second, &item, sizeof(item));
		}
		else //zhongzhuan  --> caishu
		{
			DC_HEAD * pdch = (DC_HEAD *)(pkt_data + 54);//	54= 14+20+20 ethernet head:14bytes, ip head:20bytes, tcp head:20bytes
			if(DC_TAG == pdch->m_cTag && DCT_DSDID == pdch->m_cType)
			{
				DC_DSDID *pdsdid =  (DC_DSDID *)(pdch + 1);
				int port = listening_item->get_port();
				vector<DidStruct> did_structs;	
				for(int i=0;i<pdch->m_nLen/sizeof(DC_DSDID);i++)
				{
					DidStruct did_struct;
					map<int, std::string> & did_filepath_map = listening_item->get_did_filepath_map();
					map<int, std::string>::iterator iter = did_filepath_map.find(pdsdid->m_dwDid);
					if(iter != did_filepath_map.end())
					{
						did_struct.id = pdsdid->m_dwDid;
						did_struct.whole_tag = pdsdid->m_bFull;
						did_struct.compress_tag = pdsdid->m_bNoCompress ? 0 : 1;
						did_struct.file_path = iter->second;
						did_structs.push_back(did_struct);
						pdsdid += 1;
					}
				}

				char did_conf_file[64] = {0};
				sprintf(did_conf_file, "%d_did_config.xml", port);
				WriteDidConfFile(did_conf_file, did_structs);
			}
		}
	}
}
