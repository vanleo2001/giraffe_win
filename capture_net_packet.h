#ifndef MONITOR_CAPTURE_NET_PACKET_H
#define MONITOR_CAPTURE_NET_PACKET_H

#include "basethread.h"

class CaptureNetPacket: public BaseThread
{
public:
	CaptureNetPacket(int adapter_id, std::string filter, zmq::context_t *context):adapter_id_(adapter_id),context_(context)
	{
		sock_ = NULL;
		filter_ = filter;
	};
	~CaptureNetPacket()
	{
		if(NULL != sock_)
		{
			delete sock_;
			sock_ = NULL;
		}
	};
	void *RunThreadFunc();
	void Init();
private:
	void InitZMQ();
	static void PacketHandler(unsigned char*param, const struct pcap_pkthdr *header, const unsigned char *pkt_data);
	zmq::context_t *context_;
	zmq::socket_t *sock_;
	int adapter_id_;
	std::string filter_;
};
#endif
