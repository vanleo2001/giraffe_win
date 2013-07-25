#ifndef MONITOR_UTILS_H_
#define MONITOR_UTILS_H_
#include "flags.h"
#include "structs.h"

class Utils
{
public:
    Utils(){};
    ~Utils(){};
    static char *tcp_flag_to_str(unsigned char flag);
    static int64 ToMword(unsigned long x);
    static unsigned int UINT24to32(unsigned short low,unsigned char high);
	static char * DCTypeToString(int dc_type);
	
	//pthread_t tid;
	//tid =pthread_self();
	//Utils::Print_Thread_ID(tid);
	static void Print_Thread_ID(pthread_t tid);
};

#endif // MONITOR_UTILS_H_
