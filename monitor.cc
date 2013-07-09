#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
//#include <zmq.hpp>
#include "configsax2handler.h"
#include "FriendlyExceptionHandle.h"
#include "xml2cfg.h"
#include "xml_class_set.h"
#include "capture.h"

using namespace std;

#define CONFIGFILE "test.xml"

extern map<string, int> logtagmap;

std::string XML_DID::did_config_path_ = "";
vector<std::string> XML_DID::did_templates_path_;

bool Event_Routine(DWORD event_type)
{
	switch(event_type)
	{
	case CTRL_CLOSE_EVENT:
		return true;
	}
}

int main()
{
#ifndef __linux
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)Event_Routine,true);
#else
#endif
	//parse config file
	xml2cfg config;
	deque<XML_ListeningItem> listeningitem_deque;
	vector <int> ports;
	XML_Log xml_log;
	deque<XML_ZMQ> *log_zmqdeque = xml_log.get_zmqdeque();
	config.open("test.xml");
	xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"listening");
	
	xmlNodeSetPtr didconfig_nodeset = config.locate(NULL,"/listening/did_config");
	xmlNodePtr didconfig_node = config.get_node(didconfig_nodeset,0);
	std::string did_config_path;	
	config.get_prop_string(didconfig_node,"configpath",did_config_path,NULL);
	XML_DID::set_did_config_path(did_config_path);
	xmlNodeSetPtr didtemplate_nodeset = config.locate(NULL,"//did_template");
	for(int i=0;i<didtemplate_nodeset->nodeNr;i++)
	{
		xmlNodePtr didtemplate_node = config.get_node(didtemplate_nodeset,i);
		std::string did_template_path;
		config.get_prop_string(didtemplate_node,"did_template_path",did_template_path,NULL);
		XML_DID::push_did_templates_path(did_template_path);
	}
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
		ports.push_back(port);
		std::string filter;
		config.get_prop_string(listeningitem_node,"filter",filter,NULL);
		cout<<filter<<endl;
		XML_ListeningItem listening_item(port, filter);
		xmlNodeSetPtr thread_nodeset = config.locate(listeningitem_node,"thread");
		cout<<"thread_nodeset:"<<thread_nodeset->nodeNr<<endl;
		for(int i=0;i<thread_nodeset->nodeNr;i++)
		{
			xmlNodePtr thread_node = config.get_node(thread_nodeset,i);
			std::string threadclass;
			config.get_prop_string(thread_node,"threadclass",threadclass,NULL);
			cout<<threadclass<<endl;
			deque<XML_ZMQ> *zmqdeque;
			if("Capture" == threadclass)
			{
				zmqdeque = listening_item.get_cap()->get_zmqdeque();
			}
			else if("Parse" == threadclass)
			{
				zmqdeque = listening_item.get_parse()->get_zmqdeque();
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
		listeningitem_deque.push_back(listening_item);
	}
	zmq::context_t context(1);
	//init log zmq property based on the config file	
	Log log(&context,ports);
	for(deque<XML_ZMQ>::iterator iter = log_zmqdeque->begin();iter!=log_zmqdeque->end();iter++)
	{
		ZMQItem zmq_item;
		zmq_item.zmqpattern = (*iter).get_zmqpattern();
		zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
		log.AddZMQItem(zmq_item);
	}
	log.Start();

	//create capture thread based on the config file

	deque<Capture> capture_deque;
	for(deque<XML_ListeningItem>::iterator item=listeningitem_deque.begin();item != listeningitem_deque.end();item++)
	{
		Capture capture_item(&context, *item);
		capture_deque.push_back(capture_item);
	}

	for(deque<Capture>::iterator iter_capture=capture_deque.begin();iter_capture!=capture_deque.end();iter_capture++)
	{
		iter_capture->Init();
		iter_capture->Start();
		Sleep(5000);
	}

	for(deque<Capture>::iterator iter=capture_deque.begin();iter!=capture_deque.end();iter++)
	{
		iter->Join();
	}

	pthread_exit(NULL);
	context.close();
	//EnableFriendllyExceptionHandle( true );
    //int n;
    //int i =0;

    //try
    //{
    //    XMLPlatformUtils::Initialize();
    //}
    //catch(const XMLException &err)
    //{
    //    char *msg = XMLString::transcode(err.getMessage());
    //    cout<<"initial error:"<<msg<<endl;
    //    XMLString::release(&msg);
    //    return 1;
    //}

    //char *xmlfile = CONFIGFILE;
    //SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    //reader->setFeature(XMLUni::fgSAX2CoreValidation,true);

    //ConfigSAX2Handler * handler = new ConfigSAX2Handler();
    //reader->setContentHandler(handler);
    //reader->setErrorHandler(handler);

    //try
    //{
        //reader->parse(xmlfile);

        //zmq::context_t context(1);

//        n = ports.size();
//        for(i=0;i<n;i++)
//        {
//            logtagmap.insert(pair<string,int>(filters[i],ports[i]));
//        }
//
//        for(i=0;i<logs.size();i++)
//        {
//            logs[i].init(&context,ports);
//            logs[i].Start();
//        }
//
//		//for(i=0;i<working_luas.size();i++)
//		//{
//		//	working_luas[i].Init(&context);
//		//	working_luas[i].Start();
//		//}
//
//        for(i=0;i<n;i++)
//        {
//            cout<<ports[i]<<endl;
//            cout<<filters[i]<<endl;
//            cout<<caps[i].zmqitems_[0].zmqsocketaddr<<endl;
//            caps[i].init(&context, filters[i]);
//            caps[i].Start();
//#ifdef __linux
//            sleep(1);
//#else 
//			Sleep(1000);
//#endif
//        }
//
//        for(i=0;i<n;i++)
//        {
//            parses[i].init(&context);
//            parses[i].Start();
//        }
//
//        for(i=0;i<logs.size();i++)
//        {
//            logs[i].Join();
//        }
//        //for(i=0;i<n;i++)
//        //{
//        //    caps[i].Join();
//        //    parses[i].Join();
//        //    //working_luas[i].Join();
//        //}

        //context.close();

    //}
    //catch(const XMLException &err)
    //{
    //    char *msg = XMLString::transcode(err.getMessage());
    //    cout<<"error:xml exceptiion"<<endl;
    //    XMLString::release(&msg);
    //    return -1;
    //}
    //catch(const SAXParseException &err)
    //{
    //    char *msg = XMLString::transcode(err.getMessage());
    //    cout<<"error:saxpase"<<endl;
    //    XMLString::release(&msg);
    //    return -1;
    //}
    //delete reader;
    //delete handler;

    return 0;
}
