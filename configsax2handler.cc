#include "configsax2handler.h"

//ConfigSAX2Handler::ConfigSAX2Handler():countcapzmqs_(0),countparsezmqs_(0),countlogzmqs_(0),threadtagcap_(0),threadtagparse_(0),threadtaglog_(0),threadtagworkinglua_(0)
//{
//
//}
//
//ConfigSAX2Handler::~ConfigSAX2Handler()
//{
//
//}
//
//void ConfigSAX2Handler::startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs)
//{
//    char * msg = XMLString::transcode(qname);
////    cout<<"qname:"<<msg<<endl;
////    cout<<"msg length:"<<strlen(msg)<<endl;
//	if(strcmp(msg, "listeningitem") == 0)
//	{
//		XMLCh *listeningitemkey = XMLString::transcode()
//	}
//    if(strcmp(msg, "thread") == 0)
//    {
//        XMLCh *threadclasskey = XMLString::transcode("threadclass");
//        std::string threadclass = XMLString::transcode(attrs.getValue(threadclasskey));
//        if("Log" == threadclass)
//        {
//            threadtaglog_ = 1;
//            logitem_ = new Log();
//        }
//        else if("Parse" == threadclass)
//        {
//            threadtagparse_ = 1;
//            parseitem_ = new Parse();
//        }
//        else if("Capture" == threadclass)
//        {
//            threadtagcap_ = 1;
//            capitem_ = new Capture();
//        }
//        else if("WorkingLua" == threadclass)
//        {
//            threadtagworkinglua_ = 1;
//            workingluaitem_ = new Working_Lua();
//        }
//    }
//    if(strcmp(msg, "zmq") == 0)
//    {
//        if(1 == threadtagcap_)
//        {
//            countcapzmqs_++;
//        }
//        else if(1 == threadtagparse_)
//        {
//            countparsezmqs_++;
//        }
//        else if(1 == threadtaglog_)
//        {
//            countlogzmqs_++;
//        }
//        else if(1 == threadtagworkinglua_)
//        {
//            countworkingluazmqs_++;
//        }
//    }
//    XMLString::release(&msg);
//
//}
//
//void ConfigSAX2Handler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
//{
//
//    char *msg = XMLString::transcode(qname);
////    cout<<"qname:"<<msg<<endl;
//    stringstream ss;
//    if(strcmp(msg, "portid") == 0)
//    {
//        int portid;
//        ss<<content_;
//        ss>>portid;
//        port_.push_back(portid);
//    }
//    else if(strcmp(msg, "filter") == 0)
//    {
//        filter_.push_back(content_);
//    }
//    else if(strcmp(msg, "zmqpattern") == 0)
//    {
//        if("PUB" == content_)
//            zmqitem_.zmqpattern = ZMQ_PUB;
//        else if("SUB" == content_)
//            zmqitem_.zmqpattern = ZMQ_SUB;
//        else if("PUSH" == content_)
//            zmqitem_.zmqpattern = ZMQ_PUSH;
//        else if("PULL" == content_)
//            zmqitem_.zmqpattern = ZMQ_PULL;
//        else if("PAIR" == content_)
//            zmqitem_.zmqpattern = ZMQ_PAIR;
//    }
//    else if(strcmp(msg, "zmqsocketaction") == 0)
//    {
//        zmqitem_.zmqsocketaction = content_;
//    }
//    else if(strcmp(msg, "zmqsocketaddr") == 0)
//    {
////        cout<<"addr:"<<content_<<endl;
//        zmqitem_.zmqsocketaddr = content_;
//    }
//    else if(strcmp(msg, "zmq") == 0)
//    {
//        if(countcapzmqs_ >= 1)
//        {
//            capitem_->AddZMQItem(zmqitem_);
//        }
//        else if(countparsezmqs_ >= 1)
//        {
//            parseitem_->AddZMQItem(zmqitem_);
//        }
//        else if(countlogzmqs_ >= 1)
//        {
//            logitem_->AddZMQItem(zmqitem_);
//        }
//        else if(countworkingluazmqs_ >= 1)
//        {
//            workingluaitem_->AddZMQItem(zmqitem_);
//        }
//    }
//    else if(strcmp(msg, "thread") == 0)
//    {
//        if(1 == threadtagcap_)
//        {
//            caps_.push_back(*capitem_);
//            delete capitem_;
//            countcapzmqs_ = 0;
//            threadtagcap_ = 0;
//        }
//        else if(1 == threadtagparse_)
//        {
//            parses_.push_back(*parseitem_);
//            delete parseitem_;
//            countparsezmqs_ = 0;
//            threadtagparse_ = 0;
//        }
//        else if(1 == threadtaglog_)
//        {
//            logs_.push_back(*logitem_);
//            delete logitem_;
//            countlogzmqs_ = 0;
//            threadtaglog_ = 0;
//        }
//        else if(1 == threadtagworkinglua_)
//        {
//            working_luas_.push_back(*workingluaitem_);
//            delete workingluaitem_;
//            countworkingluazmqs_ = 0;
//            threadtagworkinglua_  = 0;
//        }
//    }
//}
//
//void ConfigSAX2Handler::characters(const XMLCh* const chars, const XMLSize_t length)
//{
//    content_ = XMLString::transcode(chars);
////    cout<<content_<<endl;
//}
//
//vector<XML_ListeningItem>& ConfigSAX2Handler::get_listeningitem()
//{
//	return listeningitem_;
//}

//vector<int> & ConfigSAX2Handler::getPorts()
//{
//    return port_;
//}

//vector<std::string> & ConfigSAX2Handler::getFilters()
//{
//    return filter_;
//}

//vector <Capture> & ConfigSAX2Handler::getCapInstances()
//{
//    return caps_;
//}
//
//vector <Parse> &ConfigSAX2Handler::getParseInstances()
//{
//    return parses_;
//}
//
//vector<Log> & ConfigSAX2Handler::getLogInstances()
//{
//    return logs_;
//}
//
//vector<Working_Lua> & ConfigSAX2Handler::getWorkingLuaInstances()
//{
//    return working_luas_;
//}
