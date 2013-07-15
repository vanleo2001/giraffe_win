#ifndef MONITOR_PACKETPARSE_H_
#define MONITOR_PACKETPARSE_H_

#ifdef __linux
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include "winsock2.h"
#endif

#include <pthread.h>
#include <deque>
#include <iostream>

#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
	#include <lua.hpp>
#endif

#include "data.h"
#include "structs.h"
#include "datacolect.h"
#include "flags.h"
#include "utils.h"
#include "basethread.h"
#include "extract_dc.h"
#include "dzh_time_t.h"
#include "MonitorFileMap.h"

using namespace std;

#define EXTRACT_BUF (2*1024*2014)

class Parse:public BaseThread
{
public:
    Parse(zmq::context_t * context)
    {
        context_ = context;
        nHqTotal = 0;
        extractbuf = NULL;
        pStkStatic = NULL;
        pStkDyna = NULL;
        pStkMMPEx = NULL;
		file_ = new MonitorFileMap("FX_191111.dat",MonitorFileMap::BUILD);
		count_rc_ = 0;
		last_pack_len_ = 0;
		last_temp_len_ = 0;
		long_pack_tag_ = 0;
		case2_tag_  = false;
		dc_header_last_inner_len_ = 0;
		last_tcp_seq_ = 0;
    };
    virtual ~Parse()
    {
		if(extractbuf != NULL)
		{
			delete[] extractbuf;
			extractbuf = NULL;
		}
		if(pStkStatic != NULL)
		{
			delete[] pStkStatic;
			pStkStatic = NULL;
		}
		if(pStkDyna != NULL)
		{
			delete[] pStkDyna;
			pStkDyna = NULL;
		}
		if(pStkMMPEx != NULL)
		{
			delete[] pStkMMPEx;
			pStkMMPEx = NULL;
		}
		if(file_ !=NULL)
		{
			delete file_;
			file_=NULL;
		}
	};
	void *RunThreadFunc();
    BOOL DecryptDataPack(DC_HEAD * pData);
    BOOL ExtractDataPack(const DC_HEAD* pOrgHead,DC_HEAD* pHeadBuf,int nBufSize,WORD* pwMarketBuf=NULL);
	bool IsDCType(int dc_type);
	bool IsDCHeader(unsigned char * dc_header);
	void CombinePacket(unsigned char * pdch, int dc_len);
	void ExtractPacket(lua_State *L, struct pcap_pkthdr *header, unsigned char *pkt_data,zmq::socket_t * sock, int port_tag);
	void DispatchToLua(lua_State *L, unsigned char * pdcdata, int dc_type, int stk_num, int stuct_size, int did_template_id=0);
	void DispatchToLog(zmq::socket_t *sock, bufelement &info);
	void DispatchToMonitor(int stk_id, std::string &value);
    int nHqTotal;
    char*  extractbuf;
    STK_STATIC *pStkStatic;
    STK_DYNA *pStkDyna;
    SH_L2_MMPEX *pStkMMPEx;
protected:
private:
    zmq::context_t *context_;
	ExtractDC extractDC_;
	MonitorFileMap * file_;
	deque<CombinedPacketItem> combined_packet_deque_;
	int count_rc_;
	int long_pack_tag_;
	int last_pack_len_;
	int last_temp_len_;
	bool case2_tag_;
	CombinedPacketItem combined_packet_item_;
	unsigned char dc_header_[10];//10 is the length of dc_header
	int dc_header_last_inner_len_;
	unsigned long last_tcp_seq_;
};


//class Parse
//{
//public:
//    Parse()
//    {
//    };
//    ~Parse(){};
//    void Parsing(int, struct pcap_pkthdr *, unsigned char *,zmq::socket_t *);
//    void StartThread(zmq::context_t *, int);
//    void JoinThread();
//
//    static void *ThreadFunc(void * args);
//protected:
//private:
//    pthread_t tid_;
//    ParseParam pp_;
//};

#endif // MONITOR_PACKETPARSE_H_
