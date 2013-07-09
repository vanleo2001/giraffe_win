#include "log.h"

map<int,log4cpp::Category*> logmapping;

void * Log::RunThreadFunc()
{
//zmq version
{
    int n = ports_.size();
    zmq::context_t * context = context_;

	vector<log4cpp::Appender *> appender(n);
   /* log4cpp::Appender *appender[n];*/
    log4cpp::PatternLayout *playout = new log4cpp::PatternLayout();
    playout->setConversionPattern("%d:%p:%m%n");
    log4cpp::Category &root = log4cpp::Category::getRoot();
	vector<log4cpp::Category *> logarray (n);
    //log4cpp::Category *logarray[n];
    size_t maxfilesize = 1024*1024*1024; //1G

    for(int i=0;i<n;i++)
    {
        string logfile;
        stringstream ss;
        ss<<ports_[i];
        ss>>logfile;
		logfile += ".log";
        appender[i]=  new log4cpp::RollingFileAppender("default",logfile,maxfilesize,true);
        appender[i]->setLayout(playout);

        logarray[i] = &(root.getInstance(logfile));
        logarray[i]->addAppender(appender[i]);
        logarray[i]->setPriority(log4cpp::Priority::INFO);
        logmapping.insert(pair<int,log4cpp::Category *>(ports_[i],logarray[i]));
    }
    bufelement info;
    log4cpp::Category *log;
    map<int, log4cpp::Category *>::iterator it;

    zmq::socket_t socket_log (*context, this->zmqitems_[0].zmqpattern);
    //cout<<"log:pattern:"<<this->zmqitems_[0].zmqpattern<<endl;
    //cout<<"log:socketaction:"<<this->zmqitems_[0].zmqsocketaction<<endl;
    //cout<<"log:socketaddr:"<<this->zmqitems_[0].zmqsocketaddr<<endl;
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        socket_log.bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        socket_log.connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    zmq::pollitem_t items[] = {socket_log, 0, ZMQ_POLLIN, 0};

    zmq::message_t msg_rcv(sizeof(bufelement)+1);
    while(true)
    {
        //wait for recv
        try
        {
            int rc = zmq::poll(items, 1, 1000);//timeout = 1s
            if(rc > 0)
            {
                if(items[0].revents & ZMQ_POLLIN)
                {
                    msg_rcv.rebuild();
                    socket_log.recv(&msg_rcv,ZMQ_NOBLOCK);
                    info = *(static_cast<bufelement *>(msg_rcv.data()));
                    it = logmapping.find(info.port_tag);
                    if(it != logmapping.end())
                    {
                        log = it->second;
                        log->info("%s len:%d %s %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d flags:%s dc_type:%s",
                                info.timestamp,
                                info.len,
                                info.iproto,
                                info.saddrbyte1,
                                info.saddrbyte2,
                                info.saddrbyte3,
                                info.saddrbyte4,
                                info.sport,
                                info.daddrbyte1,
                                info.daddrbyte2,
                                info.daddrbyte3,
                                info.daddrbyte4,
                                info.dport,
                                info.flags,
                                info.dctype);
                    }
                    else
                    {
                        cout<<"can't find mapping!"<<endl;
                    }

                }
            }
            else if (rc ==0 )//timeout
            {
                continue;
            }
            else
            {
                cout<<"zmq poll fail"<<endl;
            }
        }
        catch(zmq::error_t error)
        {
            cout<<"zmq recv error:"<<error.what()<<endl;
    //            cout<<"error context addr:"<<(u_int)context<<endl;
            continue;
        }
    }
}

//deque version
//{
//    int n = *((int *)(args));
//    log4cpp::Appender *appender[n];
//    log4cpp::PatternLayout *playout = new log4cpp::PatternLayout();
//    playout->setConversionPattern("%d:%p:%m%n");
//    log4cpp::Category &root = log4cpp::Category::getRoot();
//    log4cpp::Category *logarray[n];
//
//    for(int i=0;i<n;i++)
//    {
//        string logfile;
//        stringstream ss;
//        ss<<i;
//        ss>>logfile;
//		logfile += ".log";
//        appender[i]=  new log4cpp::FileAppender("default",logfile);
//        appender[i]->setLayout(playout);
//
//        logarray[i] = &(root.getInstance(logfile));
//        logarray[i]->addAppender(appender[i]);
//        logarray[i]->setPriority(log4cpp::Priority::INFO);
//        logmapping.insert(pair<int,log4cpp::Category *>(i,logarray[i]));
//    }
//
//    bufelement info;
//    log4cpp::Category *log;
//    map<int, log4cpp::Category *>::iterator it;
//
//    while(true)
//    {
//        //deque
//        if(!dbuf_log.empty())
//        {
//            pthread_mutex_lock(&mutex);
//            info = dbuf_log.front();
//            cout<<"deque size:"<<dbuf_log.size()<<endl;
//            cout<<"info filtertag:"<<info.filtertag<<endl;
//            it = logmapping.find(info.filtertag);
//            if(it != logmapping.end())
//            {
//                log = it->second;
//                log->info("%s len:%d %s %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d flags:%s dc_type:%s",
//                        info.timestamp,
//                        info.len,
//                        info.iproto,
//                        info.saddrbyte1,
//                        info.saddrbyte2,
//                        info.saddrbyte3,
//                        info.saddrbyte4,
//                        info.sport,
//                        info.daddrbyte1,
//                        info.daddrbyte2,
//                        info.daddrbyte3,
//                        info.daddrbyte4,
//                        info.dport,
//                        info.flags,
//                        info.dctype);
//
//                dbuf_log.pop_front();
//                pthread_mutex_unlock(&mutex);
//            }
//            else
//            {
//                cout<<"can't find mapping!"<<endl;
//            }
//        }
//
//    }
//}
//

}

//void Log::StartThread(zmq::context_t *context)
//{
//    logp_.context = context;
//    pthread_create(&tid_,NULL,ThreadFunc,&logp_);
//    cout<<"start log thread"<<endl;
//}

//void Log::JoinThread()
//{
//    int err;
//    void * tret;
//    if(err = pthread_join(tid_,&tret))
//    {
//        cout<<"can't join log thread!"<<endl;
//        return ;
//    }
//}

//void Log::init()
//{
//    int n = filenum_;
////    string tempfilter;
////	map<string, int>::iterator it_tag;
//    log4cpp::Appender *appender[n];
//    log4cpp::PatternLayout *playout = new log4cpp::PatternLayout();
//    playout->setConversionPattern("%d:%p:%m%n");
//    log4cpp::Category &root = log4cpp::Category::getRoot();
//    log4cpp::Category *logarray[n];
//
//    for(int i=0;i<n;i++)
//    {
//        string logfile;
//        stringstream ss;
//        ss<<i;
//        ss>>logfile;
//		logfile += ".log";
//        appender[i]=  new log4cpp::FileAppender("default",logfile);
//        appender[i]->setLayout(playout);
//
//        logarray[i] = &(root.getInstance(logfile));
//        logarray[i]->addAppender(appender[i]);
//        logarray[i]->setPriority(log4cpp::Priority::INFO);
//        logmapping.insert(pair<int,log4cpp::Category *>(i,logarray[i]));
//    }
//}


//void *Log::ThreadFunc(void * args)
//{
////deque version
////{
////    int n = *((int *)(args));
////    log4cpp::Appender *appender[n];
////    log4cpp::PatternLayout *playout = new log4cpp::PatternLayout();
////    playout->setConversionPattern("%d:%p:%m%n");
////    log4cpp::Category &root = log4cpp::Category::getRoot();
////    log4cpp::Category *logarray[n];
////
////    for(int i=0;i<n;i++)
////    {
////        string logfile;
////        stringstream ss;
////        ss<<i;
////        ss>>logfile;
////		logfile += ".log";
////        appender[i]=  new log4cpp::FileAppender("default",logfile);
////        appender[i]->setLayout(playout);
////
////        logarray[i] = &(root.getInstance(logfile));
////        logarray[i]->addAppender(appender[i]);
////        logarray[i]->setPriority(log4cpp::Priority::INFO);
////        logmapping.insert(pair<int,log4cpp::Category *>(i,logarray[i]));
////    }
////
////    bufelement info;
////    log4cpp::Category *log;
////    map<int, log4cpp::Category *>::iterator it;
////
////    while(true)
////    {
////        //deque
////        if(!dbuf_log.empty())
////        {
////            pthread_mutex_lock(&mutex);
////            info = dbuf_log.front();
////            cout<<"deque size:"<<dbuf_log.size()<<endl;
////            cout<<"info filtertag:"<<info.filtertag<<endl;
////            it = logmapping.find(info.filtertag);
////            if(it != logmapping.end())
////            {
////                log = it->second;
////                log->info("%s len:%d %s %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d flags:%s dc_type:%s",
////                        info.timestamp,
////                        info.len,
////                        info.iproto,
////                        info.saddrbyte1,
////                        info.saddrbyte2,
////                        info.saddrbyte3,
////                        info.saddrbyte4,
////                        info.sport,
////                        info.daddrbyte1,
////                        info.daddrbyte2,
////                        info.daddrbyte3,
////                        info.daddrbyte4,
////                        info.dport,
////                        info.flags,
////                        info.dctype);
////
////                dbuf_log.pop_front();
////                pthread_mutex_unlock(&mutex);
////            }
////            else
////            {
////                cout<<"can't find mapping!"<<endl;
////            }
////        }
////
////    }
////}
////
//
////zmq version
//{
//    LogParam *lp = (LogParam *)(args);
//    int n = lp->logfilenum;
//    zmq::context_t * context = lp->context;
//
//    log4cpp::Appender *appender[n];
//    log4cpp::PatternLayout *playout = new log4cpp::PatternLayout();
//    playout->setConversionPattern("%d:%p:%m%n");
//    log4cpp::Category &root = log4cpp::Category::getRoot();
//    log4cpp::Category *logarray[n];
//
//    for(int i=0;i<n;i++)
//    {
//        string logfile;
//        stringstream ss;
//        ss<<i;
//        ss>>logfile;
//		logfile += ".log";
//        appender[i]=  new log4cpp::FileAppender("default",logfile);
//        appender[i]->setLayout(playout);
//
//        logarray[i] = &(root.getInstance(logfile));
//        logarray[i]->addAppender(appender[i]);
//        logarray[i]->setPriority(log4cpp::Priority::INFO);
//        logmapping.insert(pair<int,log4cpp::Category *>(i,logarray[i]));
//    }
//    bufelement info;
//    log4cpp::Category *log;
//    map<int, log4cpp::Category *>::iterator it;
//
//
////    cout<<"log context addr:"<<(u_int)context<<endl;
//    zmq::socket_t socket_log (*context, ZMQ_PULL);
//    socket_log.bind("inproc://log");
//    zmq::pollitem_t items[] = {socket_log, 0, ZMQ_POLLIN, 0};
//
//    zmq::message_t msg_rcv(sizeof(bufelement)+1);
//    while(true)
//    {
//        //wait for recv
//        try
//        {
//            int rc = zmq::poll(items, 1, 1000);//timeout = 1s
//            if(rc > 0)
//            {
//                if(items[0].revents & ZMQ_POLLIN)
//                {
//                    msg_rcv.rebuild();
//                    socket_log.recv(&msg_rcv,ZMQ_NOBLOCK);
//                    info = *(static_cast<bufelement *>(msg_rcv.data()));
//    //                    info = *((bufelement *)(const_cast<char *>(str.c_str())));
////                    cout<<"info filtertag:"<<info.filtertag<<endl;
//                    it = logmapping.find(info.filtertag);
//                    if(it != logmapping.end())
//                    {
//                        log = it->second;
//                        log->info("%s len:%d %s %d.%d.%d.%d:%d -> %d.%d.%d.%d:%d flags:%s dc_type:%s seqNO.:%lld",
//                                info.timestamp,
//                                info.len,
//                                info.iproto,
//                                info.saddrbyte1,
//                                info.saddrbyte2,
//                                info.saddrbyte3,
//                                info.saddrbyte4,
//                                info.sport,
//                                info.daddrbyte1,
//                                info.daddrbyte2,
//                                info.daddrbyte3,
//                                info.daddrbyte4,
//                                info.dport,
//                                info.flags,
//                                info.dctype,
//                                info.seqtag);
////                        sleep(1);
//                    }
//                    else
//                    {
//                        cout<<"can't find mapping!"<<endl;
//                    }
//
//                }
//            }
//            else if (rc ==0 )//timeout
//            {
//                continue;
//            }
//            else
//            {
//                cout<<"zmq poll fail"<<endl;
//            }
//        }
//        catch(zmq::error_t error)
//        {
//            cout<<"zmq recv error:"<<error.what()<<endl;
//    //            cout<<"error context addr:"<<(u_int)context<<endl;
//            continue;
//        }
//    }
//
//}
//}
//
