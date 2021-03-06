#include "capture_net_packet.h"
#include "structs.h"
#include "flags.h"

void DispatchCapData(zmq::socket_t * sock, void * data, int size)
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

void CaptureNetPacket::Init()
{
	
	InitZMQ();
}

void CaptureNetPacket::InitZMQ()
{
	assert(-1 != this->zmqitems_[0].zmqpattern);
	sock_ = new zmq::socket_t(*context_,this->zmqitems_[0].zmqpattern);
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
}

void * CaptureNetPacket::RunThreadFunc()
{
    pcap_if_t * alldevs ;
    char *errbuf= new char[PCAP_ERRBUF_SIZE];
    int devsnum= 0;
    pcap_if_t *d;
    int i;
    int inum = adapter_id_;
    int len=10240;
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
	if(pcap_compile(adhandle,&fcode,filter_.c_str(),1,netmask) < 0)
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

	if(pcap_loop(adhandle,0,PacketHandler,(unsigned char*)sock_) < 0)
	{
		cout<<"pcap_loop:"<<pcap_geterr(adhandle)<<endl;
	}
	
	//pcap_loop(adhandle,0,PacketHandler,(u_char *)dumper);
	//pcap_dump_close(dumper);
	return ((void *)0);
}

void CaptureNetPacket::PacketHandler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data)
{
	zmq::socket_t *sock = (zmq::socket_t *)param;
	CapNetPacketItem item;
	item.header = *header;
	unsigned char *pdata = new unsigned char[CAP_PACK_BUF_SIZE];
	assert(NULL != pdata);
	memset(pdata, 0, CAP_PACK_BUF_SIZE);
	assert(CAP_PACK_BUF_SIZE >= header->caplen);
	memcpy(pdata,pkt_data,header->caplen);
	item.data = pdata;
	DispatchCapData(sock, &item, sizeof(item));
}

