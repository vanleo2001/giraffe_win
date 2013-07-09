#pragma once
#include "stdafx.h"
#include "flags.h"
#include "pub_pfloat.h"
#include "dzh_time_t.h"
#include <string>
using namespace std;
#pragma pack(1)
#define  EXCECOUNT   1000
#define  IPBUFCOUNT   1024

//enum Error_Type
//{
//	SYSTERM,
//	BUSINESS
//};
//
//enum Error_Level
//{
//	ERR,
//	WARNING
//};

#pragma pack(push,1)
typedef struct MonitorMsg
{
	dzh_time_t time;
	char stock_label[32];
	char error_type [32];
	char error_level[32];
	char error_info[64];
} MonitorMsg;
#pragma pack(pop)
//typedef struct MonitorDetail
//{
//	char     StockName[32];
//	PFloat   bidAverage;
//	byte     bidCombineFlag;
//	byte     priExFlag;
//	byte     priFreExFlag;
//	byte     m_bBreak;
//}MonitorDetail;

class MonitorFileMap
{
public:
    enum  Type
	{
       BUILD,
       OPEN
	};
public:
	MonitorFileMap(string  MapName, Type  type);
	~MonitorFileMap();
    bool  IsException(string stockName, int& index);
    MonitorMsg*  GetMapMsg();
    /*MonitorDetail*  GetMapDetail();*/
    LPVOID  GetMapPointer();
    void    DeleteClientIP(char*  ip);
private:
    //void  ClearException(string stockName);
private:
    LPVOID   mapMsg;
    HANDLE   myMutex;
};
