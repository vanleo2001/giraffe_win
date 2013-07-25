#ifndef MONITOR_CAPTURE_H
#define  MONITOR_CAPTURE_H
#include <stack>
#include <zmq.hpp>
#include "basethread.h"
#include "xml_class_set.h"
#include "parse.h"
#include "log.h"
using namespace std;

const int kPoolSize = 8;

class Capture:public BaseThread
{
public:
	Capture(int adapter_id,zmq::context_t *context,XML_ListeningItem &listening_item):adapter_id_(adapter_id),context_(context),listening_item_(listening_item),curent_pool_size_(0)
	{
	}
	virtual ~Capture()
	{
		while(!sock_deque_.empty())
		{
			zmq::socket_t *sock = sock_deque_.front();
			if(sock != NULL)
			{
				delete sock;
				sock = NULL;
			}
			sock_deque_.pop_front();
		}
		while(!parse_deque_.empty())
		{
			Parse *parse = parse_deque_.front();
			if(parse != NULL)
			{
				delete parse;
				parse = NULL;
			}
			parse_deque_.pop_front();
		}
		sock_map_.clear();
	};
	void Init();
	bool ReservePool(int pool_sizse=kPoolSize);
	void *RunThreadFunc();
private:
	pcap_t *adhandle;
	int adapter_id_;
	zmq::context_t *context_;
	XML_ListeningItem listening_item_;
	deque<Parse *> parse_deque_;
	deque<zmq::socket_t *>sock_deque_;
	map<std::string,zmq::socket_t *> sock_map_;
	int curent_pool_size_;
	static void PacketHandler(unsigned char *param, const struct pcap_pkthdr *header, const unsigned char *pkt_data);
};


#endif