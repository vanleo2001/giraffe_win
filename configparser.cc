#include "configparser.h"
#include <assert.h>
#include <iostream>

using namespace std;

std::string ConfigParser::SplitDidTemplateId(std::string &did_template_path)
{
	int pre_pos = did_template_path.find_last_of("/\\");
	int last_pos = did_template_path.find_last_not_of(".xml");
	std::string did_template_id = did_template_path.substr(pre_pos+1 , last_pos-pre_pos);
	cout<<did_template_id<<endl;
	assert(!did_template_id.empty());
	return did_template_id;
}

void ConfigParser::Parse()
{
	xml2cfg config;
	deque<XML_ZMQ> *log_zmqdeque = xml_log_.get_zmqdeque();
	config.open("test.xml");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"listening");
	
	xmlNodeSetPtr log_nodeset = config.locate(NULL,"/listening/log");
	for(int i=0;i<log_nodeset->nodeNr;i++)
	{
		xmlNodePtr log_node = config.get_node(log_nodeset,i);
		xmlNodeSetPtr log_zmq_nodeset = config.locate(log_node,"zmqs/zmq");
		for(int i=0;i<log_zmq_nodeset->nodeNr;i++)
		{
			xmlNodePtr log_zmq_node = config.get_node(log_zmq_nodeset,i);
			std::string zmqpattern;
			std::string zmqsocketaction;
			std::string zmqsocketaddr;
			config.get_prop_string(log_zmq_node,"zmqpattern",zmqpattern,NULL);
			config.get_prop_string(log_zmq_node,"zmqsocketaction",zmqsocketaction,NULL);
			config.get_prop_string(log_zmq_node,"zmqsocketaddr",zmqsocketaddr,NULL);
			XML_ZMQ xml_zmq(zmqpattern,zmqsocketaction,zmqsocketaddr);
			log_zmqdeque->push_back(xml_zmq);
		}
	}
	xmlNodeSetPtr listeningitem_nodeset = config.locate(root_node,"//listeningitem");
	cout<<listeningitem_nodeset->nodeNr<<endl;
	for(int i=0; i<listeningitem_nodeset->nodeNr; i++)
	{
		xmlNodePtr listeningitem_node =config.get_node(listeningitem_nodeset,i);
		int port = config.get_prop_int(listeningitem_node,"port",NULL);
		ports_.push_back(port);
		std::string filter;
		config.get_prop_string(listeningitem_node,"filter",filter,NULL);
		cout<<filter<<endl;
		XML_ListeningItem listening_item(port, filter);

		xmlNodeSetPtr didtemplate_nodeset = config.locate(listeningitem_node,"did_templates/did_template");
		if(didtemplate_nodeset->nodeNr > 0)
		{
			for(int i=0;i<didtemplate_nodeset->nodeNr;i++)
			{
				xmlNodePtr didtemplate_node = config.get_node(didtemplate_nodeset,i);
				std::string did_template_path;
				config.get_prop_string(didtemplate_node,"did_template_path",did_template_path,NULL);
				listening_item.push_did_template_paths(did_template_path);
				std::string did_template_id = SplitDidTemplateId(did_template_path);
				listening_item.push_did_template_ids(did_template_id);
				listening_item.insert_did_filepath_map(std::pair<int, std::string>(atoi(did_template_id.c_str()), did_template_path));
			}
		}

		xmlNodeSetPtr thread_nodeset = config.locate(listeningitem_node,"thread");
		cout<<"thread_nodeset:"<<thread_nodeset->nodeNr<<endl;
		for(int i=0;i<thread_nodeset->nodeNr;i++)
		{
			xmlNodePtr thread_node = config.get_node(thread_nodeset,i);
			std::string threadclass;
			config.get_prop_string(thread_node,"threadclass",threadclass,NULL);
			cout<<threadclass<<endl;
			deque<XML_ZMQ> *zmqdeque;
			if("CaptureNetPacket" == threadclass)
			{
				zmqdeque = listening_item.get_capture_net_packet()->get_zmqdeque();
			}
			else if("HandleNetPacket" == threadclass)
			{
				zmqdeque = listening_item.get_handle_net_packet()->get_zmqdeque();
			}
			else if("Parse" == threadclass)
			{
				zmqdeque = listening_item.get_parse()->get_zmqdeque();
			}
			else if("LuaRoutine" == threadclass)
			{
				zmqdeque = listening_item.get_lua_routine()->get_zmqdeque();
			}

			xmlNodeSetPtr zmq_nodeset = config.locate(thread_node,"zmqs/zmq");
			cout<<"zmqnodeset:"<<zmq_nodeset->nodeNr<<endl;
			for(int i=0;i<zmq_nodeset->nodeNr;i++)
			{
				xmlNodePtr zmq_node = config.get_node(zmq_nodeset,i);
				std::string zmqpattern;
				std::string zmqsocketaction;
				std::string zmqsocketaddr;
				config.get_prop_string(zmq_node,"zmqpattern",zmqpattern,NULL);
				config.get_prop_string(zmq_node,"zmqsocketaction",zmqsocketaction,NULL);
				config.get_prop_string(zmq_node,"zmqsocketaddr",zmqsocketaddr,NULL);
				XML_ZMQ zmq_item(zmqpattern,zmqsocketaction,zmqsocketaddr);
				zmqdeque->push_back(zmq_item);
			}
		}
		listeningitem_deque_.push_back(listening_item);
	}
}