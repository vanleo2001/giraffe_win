#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
//#include <zmq.hpp>
//#include "configsax2handler.h"
//#include "FriendlyExceptionHandle.h"
#include "xml2cfg.h"
#include "xml_class_set.h"
//#include "capture.h"
#include "log.h"
#include "capture_net_packet.h"
#include "handle_net_packet.h"
#include "DIDTemplateToLuaStruct.h"
#include "configparser.h"

using namespace std;

#define CONFIGFILE "test.xml"

extern map<string, int> logtagmap;

#ifndef __linux
bool Event_Routine(DWORD event_type)
{
	switch(event_type)
	{
	case CTRL_CLOSE_EVENT:
		return true;
	}
}
#else
#endif

int get_network_adapter()
{
	pcap_if_t *alldevs;
    char *errbuf= new char[PCAP_ERRBUF_SIZE];
	int devsnum = 0;
	int inum = 0;
    pcap_if_t *d;
	
	if(pcap_findalldevs(&alldevs,errbuf) == -1)
	{
		cout<<"Error in pcap_findalldevs %s "<<errbuf<<endl;
		return -1;
	}
	if(NULL != errbuf)
	{
		delete [] errbuf;
		errbuf = NULL;
	}

	for(d=alldevs; d; d=d->next)
	{
		cout<<++devsnum<<". "<<d->name;
		if(d->description)
			cout<<" ("<<d->description<<")"<<endl;
		else
			cout<<" (No description)"<<endl;
	}

	cout<<"Please input the number of adapter you choose:"<<endl;
	cin>>inum;
	if(inum<1||inum>devsnum)
	{
		cout<<"Adapter number out of range!"<<endl;
		pcap_freealldevs(alldevs);
		return  -2;
	}
	else
		return inum;
}

void RunLogThread(zmq::context_t &context, Log &log ,XML_Log & xml_log)
{
	deque<XML_ZMQ> * log_zmqdeque = xml_log.get_zmqdeque();
	//init log zmq property based on the config file
	for(deque<XML_ZMQ>::iterator iter = log_zmqdeque->begin();iter!=log_zmqdeque->end();iter++)
	{
		ZMQItem zmq_item;
		zmq_item.zmqpattern = (*iter).get_zmqpattern();
		zmq_item.zmqsocketaction = (*iter).get_zmqsocketaction();
		zmq_item.zmqsocketaddr = (*iter).get_zmqsocketaddr();
		log.AddZMQItem(zmq_item);
	}
	log.Start();
}

void TransformDidTemplateToLua(XML_ListeningItem & xml_listening_item)
{
	DIDTemplateToLuaStruct did_to_lua;
	did_to_lua.Transform(xml_listening_item);
}

void RunCaptureThread(zmq::context_t &context, int adapter_id, deque<CaptureNetPacket> & cap_net_packet_deque, deque<HandleNetPacket> & handle_net_packet_deque, deque<XML_ListeningItem> &listeningitem_deque)
{
	//create capture thread based on the config file
	int index = 0;
	for(deque<XML_ListeningItem>::iterator item=listeningitem_deque.begin();item != listeningitem_deque.end();item++)
	{
		TransformDidTemplateToLua(*item);
		CaptureNetPacket capture_net_packet(adapter_id,item->get_filter(), &context);
		HandleNetPacket handle_net_packet(&context, *item);

		deque<XML_ZMQ> *capture_net_packet_zmq_deque = (*item).get_capture_net_packet()->get_zmqdeque();
		for(deque<XML_ZMQ>::iterator iter_cap=capture_net_packet_zmq_deque->begin();iter_cap!=capture_net_packet_zmq_deque->end();iter_cap++)
		{
			ZMQItem capture_net_packet_zmq_item;
			capture_net_packet_zmq_item.zmqpattern = (*iter_cap).get_zmqpattern();
			capture_net_packet_zmq_item.zmqsocketaction = (*iter_cap).get_zmqsocketaction();
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",index);
			capture_net_packet_zmq_item.zmqsocketaddr = (*iter_cap).get_zmqsocketaddr() + buf;
			capture_net_packet.AddZMQItem(capture_net_packet_zmq_item);
		}

		deque<XML_ZMQ> *handle_net_packet_zmq_deque = (*item).get_handle_net_packet()->get_zmqdeque();
		for(deque<XML_ZMQ>::iterator iter_handle=handle_net_packet_zmq_deque->begin();iter_handle!=handle_net_packet_zmq_deque->end();iter_handle++)
		{
			ZMQItem handle_net_packet_zmq_item;
			handle_net_packet_zmq_item.zmqpattern = (*iter_handle).get_zmqpattern();
			handle_net_packet_zmq_item.zmqsocketaction = iter_handle->get_zmqsocketaction();
			char buf[16];
			memset(buf,0,sizeof(buf));
			sprintf(buf,"%d",index);
			handle_net_packet_zmq_item.zmqsocketaddr = iter_handle->get_zmqsocketaddr() + buf;
			handle_net_packet.AddZMQItem(handle_net_packet_zmq_item);	
			break;
		}

		index++;
		cap_net_packet_deque.push_back(capture_net_packet);
		handle_net_packet_deque.push_back(handle_net_packet);
	}

	for(deque<CaptureNetPacket>::iterator iter=cap_net_packet_deque.begin();iter!=cap_net_packet_deque.end();iter++)
	{
		iter->Init();
		iter->Start();
	}
	for(deque<HandleNetPacket>::iterator iter=handle_net_packet_deque.begin();iter!=handle_net_packet_deque.end();iter++)
	{
		iter->Init();
		Utils::SleepUsec(1000000);
		iter->Start();
	}
}

void JoinCaptureThread(deque<CaptureNetPacket> & capture_net_packet_deque, deque<HandleNetPacket> & handle_net_packet_deque)
{
	for(deque<CaptureNetPacket>::iterator iter=capture_net_packet_deque.begin();iter!=capture_net_packet_deque.end();iter++)
	{
		iter->Join();
	}
	for(deque<HandleNetPacket>::iterator iter=handle_net_packet_deque.begin();iter!=handle_net_packet_deque.end();iter++)
	{
		iter->Join();
	}
}

int main()
{
#ifndef __linux
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)Event_Routine,true);
#else
#endif

	int adapter_id = get_network_adapter();
	assert(adapter_id >= 0);
	ConfigParser config_parser("test.xml");
	config_parser.Parse();

	zmq::context_t context(1);

	vector<int>& ports = config_parser.get_ports();
	XML_Log & xml_log = config_parser.get_log_obj();
	Log log(&context,ports);
	RunLogThread(context, log, xml_log);

	deque<CaptureNetPacket> cap_net_packet_deque;
	deque<HandleNetPacket> handle_net_packet_deque;
	deque<XML_ListeningItem> & listeningitem_deque = config_parser.get_listeningitems();
	RunCaptureThread(context, adapter_id, cap_net_packet_deque, handle_net_packet_deque, listeningitem_deque);
	JoinCaptureThread(cap_net_packet_deque, handle_net_packet_deque);

	pthread_exit(NULL);
	context.close();

	//////////////////////////////////////////////////////////////////////////////
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


