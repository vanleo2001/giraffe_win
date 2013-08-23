#ifndef MONITOR_ITHREAD_H_
#define MONITOR_ITHREAD_H_

#include <pthread.h>
#include "structs.h"

typedef void * (*WorkThread)(void *);

class BaseThread;

class IThread
{
public:
    BaseThread *pbasethread_;

    IThread(){};
    ~IThread(){};
    bool Start(WorkThread workthread, BaseThread *basethread)
    {
        pbasethread_ = basethread;
		//pthread_attr_t attr;
		//pthread_attr_init(&attr);
		//pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
        int err;
        if(err= pthread_create(&tid_,NULL,workthread,this))
        {
            cout<<"can't create thread!"<<endl;
            return false;
        }
//#ifdef __linux
//		sleep(1);
//#else
//		Sleep(1000);
//#endif
//		pthread_attr_destroy(&attr);
        return true;
    }

    bool Join()
    {
        int err;
        void * tret;
        if(err = pthread_join(tid_,&tret))
        {
            cout<<"can't join thread!"<<endl;
            return false;
        }
        return true;
    }

	bool Cancel()
	{
		int err;
		if(err = pthread_cancel(tid_))
		{
			cout<<"can't cancel thread"<<endl;
			return false;
		}
		return true;
	}
protected:
private:
    pthread_t tid_;

};

class BaseThread
{
public:
	vector<ZMQItem > zmqitems_;
    BaseThread(){};
    virtual ~BaseThread(){};
    bool Start()
    {
        return ithread_.Start(ThreadFunc,this);
    }
    bool Join()
    {
        return ithread_.Join();
    }
	bool Cancel()
	{
		return ithread_.Cancel();
	}
    void AddZMQItem(ZMQItem zmqitem)
    {
        zmqitems_.push_back(zmqitem);
    }
    virtual void* RunThreadFunc() = 0;
protected:
private:
    IThread ithread_;
    static void* ThreadFunc(void *args)
    {
        IThread * ithread = (IThread *)args;
        ithread->pbasethread_->Run();
        return (void *)0;
    }
    void Run()
    {
        RunThreadFunc();
    }
};


#endif // MONITOR_ITHREAD_H_

