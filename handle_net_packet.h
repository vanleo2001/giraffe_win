#ifndef MONITOR_HANDLE_NET_PACKET_H
#define MONITOR_HANDLE_NET_PACKET_H
#include "zmq.hpp"
#include "basethread.h"
#include "xml_class_set.h"
#include "parse.h"
#include "luaroutine.h"


const int kPoolSize = 1;

class HandleNetPacket:public BaseThread
{
public:
	HandleNetPacket(zmq::context_t *context,XML_ListeningItem &listening_item):context_(context),listening_item_(listening_item),curent_pool_size_(0)
	{
		sock_ = NULL;
	}
	virtual ~HandleNetPacket()
	{
		if (NULL != sock_)
		{
			delete sock_;
			sock_ = NULL;
		}
		while(!sock_ex_deque_.empty())
		{
			zmq::socket_t *sock = sock_ex_deque_.front();
			if(NULL != sock)
			{
				delete sock;
				sock = NULL;
			}
			sock_ex_deque_.pop_front();
		}
		while(!parse_deque_.empty())
		{
			Parse *parse = parse_deque_.front();
			if(NULL != parse)
			{
				delete parse;
				parse = NULL;
			}
			parse_deque_.pop_front();
		}
		while(!lua_routine_deque_.empty())
		{
			LuaRoutine *lua_routine = lua_routine_deque_.front();
			if(NULL != lua_routine)
			{
				delete lua_routine;
				lua_routine = NULL;
			}
			lua_routine_deque_.pop_front();
		}
		sock_ex_map_.clear();
	};
	void Init();
	void InitZMQEx(int index);
	inline void AddZMQItemEx(ZMQItem item)
	{
		zmqitems_ex_.push_back(item);
	}
	void AddToZMQDequeEx(int index);
	void RunLuaRoutineThread(int index);
	void RunParseThread(int index);
	bool IncreasePool(int pool_sizse=kPoolSize);
	void *RunThreadFunc();
private:
	zmq::context_t *context_;
	zmq::socket_t *sock_;
	XML_ListeningItem listening_item_;
	deque<Parse *> parse_deque_;
	deque<LuaRoutine*> lua_routine_deque_;
	deque<zmq::socket_t *>sock_ex_deque_;
	map<std::string,zmq::socket_t *> sock_ex_map_;
	int curent_pool_size_;
	std::vector<ZMQItem> zmqitems_ex_;
};

#endif
