#ifndef MONITOR_PROTOCOL_H_
#define MONITOR_PROTOCOL_H_

#include <pthread.h>
#include <iostream>
#include <deque>
#include <zmq.hpp>
#include "pcap/pcap.h"
#include "log4cpp/Category.hh"
#include "flags.h"
#include "mword.h"

using namespace std;

const int PCAPTOPARSE_BUF_SIZE = 1024*100;
const int COMBINED_PACKET_ITEM_SIZE = 1024*100;

#pragma pack(push,1)

typedef struct combined_packet_item
{
	char data[COMBINED_PACKET_ITEM_SIZE];
}combined_packet_item;

/* 4 bytes IP address */
typedef struct ip_address
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address;

typedef struct IpHead
{
    u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service
	u_short tlen;			// Total length
	u_short identification; // Identification
	u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	protocol;			// Protocol
	u_short crc;			// Header checksum
	ip_address	saddr;		// Source address
	ip_address	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_head;

typedef struct TcpHead
{
    u_short source;  //source port
	u_short dest; //destination port
	u_long seq;  //sequence number
	u_long ack_seq;  //acknowledge number
	u_char dataoffset;  //data offset
	u_char flags;  // flags
	u_short window;  // window length
	u_short check;  //checksum
	u_short urg_ptr;  //urgent pointer
}tcp_head;

typedef struct UdpHead
{
    u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_head;

struct CPS_STATIC_HEAD
{
	enum ATTR
	{
		SEQ_ID = 1,				//˳��ID
		FULLDATA = 2,			//����ѹ��/����ѹ��
	};
	WORD	m_wMarket;		//�г�
	WORD	m_wNum;			//��������
	BYTE	m_cAttrib;
};

struct CPS_DYNA_HEAD
{
	enum ATTR
	{
		SEQ_CPS =		1,			//���ѹ��
		HAS_TIME_BIAS = 2,			//ӵ��ʱ��ƫ��(������������ӵ����ͬʱ��)
		HAS_TICK_COUNT= 0x40,		//ӵ�гɽ�����
		HAS_AMOUNT =	0x80,		//�������޳ɽ���

		EQUITY =		0,			//��Ʊ����������Ʒ��
		FUTURE =		0x8,		//��Ʒ�ڻ����ڻ�Ʒ��
		FOREIGN_EXG	=	0x10,		//����ֻ�п��ߵ�������һ�۵�Ʒ��
		HKSEC =			0x18,		//�۹ɵ������̼۸���������
		FTR_IDX =		0x20,		//�ڻ�ָ������5�������̺ͳֲ���������۵�Ʒ��
		TYPEMASK =		0x38,
	};
	BYTE	m_cCRC;			//CRCУ���
	WORD	m_wMarket;		//�г�
	WORD	m_wNum;			//��������
	time_t	m_baseTime;		//��׼ʱ�䣬���е�ʱ��ƫ�������ǻ��ڸ�ʱ��
	BYTE	m_cAttrib;		//����
	BYTE	m_cVer;			//ѹ���汾
};


//////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct bufelement
{
    int port_tag;
    char *timestamp;
    int len;
    char * iproto;
    int saddrbyte1;
    int saddrbyte2;
    int saddrbyte3;
    int saddrbyte4;
    int sport;
    int daddrbyte1;
    int daddrbyte2;
    int daddrbyte3;
    int daddrbyte4;
    int dport;
    char *flags;
    char *dctype;
    long long seqtag;
}bufelement;

typedef struct pcap_work_item
{
    int port_tag;
    long long seqtag;
    struct pcap_pkthdr header;
    unsigned char data[PCAPTOPARSE_BUF_SIZE];
}pcap_work_item;

typedef struct ThreadFuncParams
{
    string filter;
    int threadtag;
//    pthread_mutex_t *mutex_cp;
//    deque<pcap_work_item> *dbuf;
    zmq::context_t *context;
//    zmq::socket_t * sock;
    void *args;
}ThreadFuncParams;

typedef struct InCapThreadParam
{
    int filtertag;
    zmq::socket_t *sock;
    unsigned int countnum;
    long int timebase;
    int timetag;
    long long seqtag;
}InCapThreadParam;



typedef struct LogParam
{
    int logfilenum;
//    deque<pcap_work_item> *dbuf_cp;
//    deque<bufelement> *dbuf_pl;
    zmq::context_t *context;
}LogParam;

typedef struct ParseParam
{
    int threadtag;
    zmq::context_t *context;
}ParseParam;

typedef  struct ThreadFuncLogParam
{
    deque<bufelement> *dbuf_pl;
    log4cpp::Category *log;
}ThreadFuncLogParam;

typedef struct ZMQItem
{
	int zmqpattern;
	std::string zmqsocketaction;
	std::string zmqsocketaddr;
	ZMQItem(){};
	~ZMQItem(){};
	ZMQItem(const ZMQItem & zmqitem)
	{
		zmqpattern = zmqitem.zmqpattern;
		zmqsocketaction = zmqitem.zmqsocketaction;
		zmqsocketaddr = zmqitem.zmqsocketaddr;
	}

}ZMQItem;

#pragma pack(pop)

#endif // MONITOR_PROTOCOL_H_