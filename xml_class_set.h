#ifndef MONITOR_XML_CLASS_SET_H
#define MONITOR_XML_CLASS_SET_H
#include <zmq.h>
#include <deque>
using namespace std;

class XML_ZMQ
{
public:
	XML_ZMQ(std::string zmqpattern,std::string zmqsocketaction,std::string zmqsocketaddr):zmqpattern_(zmqpattern),\
																																	zmqsocketaction_(zmqsocketaction),\
																																	zmqsocketaddr_(zmqsocketaddr){};
	~XML_ZMQ(){};
	inline int get_zmqpattern()
	{
		if("PAIR" == zmqpattern_)
			return ZMQ_PAIR;
		else if("PUB" == zmqpattern_)
			return ZMQ_PUB;
		else if("SUB" == zmqpattern_)
			return ZMQ_SUB;
		else if("REQ" == zmqpattern_)
			return ZMQ_REQ;
		else if("REP" == zmqpattern_)
			return ZMQ_REP;
		else if("DEALER" == zmqpattern_)
			return ZMQ_DEALER;
		else if("ROUTER" == zmqpattern_)
			return ZMQ_ROUTER;
		else if("PULL" == zmqpattern_)
			return ZMQ_PULL;
		else if("PUSH" == zmqpattern_)
			return ZMQ_PUSH;
		else if("XPUB" == zmqpattern_)
			return ZMQ_XPUB;
		else if("XSUB" == zmqpattern_)
			return ZMQ_XSUB;
	}
	inline std::string& get_zmqsocketaction()
	{
		return zmqsocketaction_;
	}
	inline std::string& get_zmqsocketaddr()
	{
		return zmqsocketaddr_;
	}
private:
	std::string zmqpattern_;
	std::string zmqsocketaction_;
	std::string zmqsocketaddr_;
};

class XML_Capture
{
public:
	XML_Capture()
	{
	}
	~XML_Capture()
	{
	}
    inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_Parse
{
public:
	XML_Parse()
	{
		
	}
	~XML_Parse()
	{
		
	}
	inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class XML_Log
{
public:
	XML_Log()
	{
		
	}
	~XML_Log()
	{
		
	}
	inline deque<XML_ZMQ>* get_zmqdeque()
	{
		return &zmqdeque_;
	}
private:
	deque<XML_ZMQ> zmqdeque_;
};

class	XML_DID
{
public:
	XML_DID(){};
	~XML_DID(){};
	static void set_did_config_path(std::string did_config_path)
	{
		did_config_path_ = did_config_path;
	}
	static  std::string get_did_config_path()
	{
		return did_config_path_;
	}
	static void push_did_templates_path(std::string & did_template_path)
	{
		did_templates_path_.push_back(did_template_path);
	}
	static vector<std::string>& get_did_templates_path()
	{
		return did_templates_path_;
	}
private:
	static std::string did_config_path_;
	static vector<std::string> did_templates_path_;
};

class XML_ListeningItem
{
public:
	XML_ListeningItem(int port,std::string filter):port_(port),filter_(filter)
	{
	}
	~XML_ListeningItem()
	{

	}
	inline int& get_port()
	{
		return port_;
	}
	inline std::string& get_filter()
	{
		return filter_;
	}
	inline XML_Capture *get_cap()
	{
		return &cap_;
	}
	inline XML_Parse *get_parse()
	{
		return &parse_;
	}
protected:
private:
	int port_;
	std::string filter_;
	XML_Capture cap_;
	XML_Parse parse_;
};

#endif