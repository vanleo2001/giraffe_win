require("base_type")
--io.write("this is lua script\n")
--[[
function process(a)
    return "haha"
end
]]--

local ffi = require("ffi")

--io.write("this is lua script\n")

ffi.cdef [[
#pragma pack(1)

static const int  SHL2_MMP_SIZE=5;

int printf(const char *fmt, ...);

typedef unsigned int time_t;

typedef struct MWORD
{
	enum
	{
		MAXBASE = 0x1FFFFFFF,
	};

	int m_nBase:30;
	unsigned int m_nMul:2;
}MWORD;

enum DC_TYPE
{
	DCT_NUL = 0,
	DCT_KEEPALIVE,			//<-->��������1
	DCT_LOGIN,				//<-->��½�Լ�����������	2
	DCT_REQPASS,			//<-->Ҫ���û��������½3
	DCT_USERnPASS,			//<-->�û�������4
	DCT_READY,				//-->��½�ɹ������Խ�������5
	DCT_RESEND,				//-->�ط����ݰ�6
	DCT_STKSTATIC,			//<--��̬����7
	DCT_STKDYNA,			//<--��̬����8
	DCT_SHL2_MMPEx,			//<--level2��չ������9
	DCT_SHL2_REPORT,		//<--level2�ֱʳɽ�10
	DCT_SHL2_BIG_WD,		//<--level2��ʱ��󳷵�11
	DCT_SHL2_ACCU_WD,		//<--level2�ۼ���󳷵�12
	DCT_HK_STATIC,			//<--�۹ɾ�̬13
	DCT_HK_DYNA,			//<--�۹ɶ�̬14
	DCT_XML,				//<--XML��ʽ����15
	DCT_SHL2_QUEUE,			//<--�����̶��У�level2����16
	DCT_GENERAL,			//<--ͨ������17
	DCT_USERSTAT,			//-->�û�����ͳ��18
	DCT_RAWDATA,			//<-->ԭʼ����19
	DCT_NEWS,				//<--�������ŵ��ı���������20
	DCT_SZL2_ORDER_QUEUE,	//<--ί�ж��У�����Level2����21
	DCT_SZL2_ORDER_STAT,	//<--ί�ж���ͳ�ƣ�����Level2����22

	DCT_SZL2_FULL_ORDER=100,//<--ί�ж��ж����ϸ������Level2����
	DCT_SZL2_FULL_TRADE=101,//<--�ɽ�,�������ж����ϸ������Level2����
};

typedef struct DC_STKSTATIC_MY
{
	unsigned long	m_dwVersion;	//��̬���ݰ汾,ÿ�ξ�̬���ݷ����仯�����������ֵ,ÿ������Դ�����һ����Զ����ĸ�λ��������ͬ����Դ�Ͳ�������ͬ�İ汾
	unsigned short	m_wAttrib;		//�汾����
	unsigned int	m_nDay;			//��̬�����ڣ�YYYYMMDD
	short	m_nNum;			//m_data����
}DC_STKSTATIC_MY;

typedef struct STK_STATIC
{
	unsigned short	m_wStkID;			//���г���Ψһ��ʾ,�ڱ��г��ڵ����
	char	m_strLabel[10];		//����
	char	m_strName[32];		//����
	unsigned char	m_cType;			//STK_TYPE
	unsigned char	m_nPriceDigit;		//�۸���С�ֱ��ʣ��ǳ���Ҫ��ÿһ��unsigned int���͵ļ۸�Ҫ����10^m_nPriceDigit���������ļ۸�
	short	m_nVolUnit;			//�ɽ�����λ��ÿһ�ɽ�����λ��ʾ���ٹ�
	MWORD	m_mFloatIssued;		//��ͨ�ɱ�
	MWORD	m_mTotalIssued;		//�ܹɱ�

	unsigned long	m_dwLastClose;		//����
	unsigned long	m_dwAdvStop;		//��ͣ
	unsigned long	m_dwDecStop;
}STK_STATIC;

typedef struct DC_STKSTATIC
{
	unsigned long	m_dwVersion;	//��̬���ݰ汾,ÿ�ξ�̬���ݷ����仯�����������ֵ,ÿ������Դ�����һ����Զ����ĸ�λ��������ͬ����Դ�Ͳ�������ͬ�İ汾,0x80000000
	unsigned short	m_wAttrib;		//�汾����
	unsigned int	m_nDay;			//��̬�����ڣ�YYYYMMDD
	short	m_nNum;			//m_data����
	STK_STATIC m_data[1];	//����
}DC_STKSTATIC;

typedef struct DC_STKDYNA_MY
{
	unsigned short	m_wDynaSeq;		//��̬�������кţ����ڿͻ���ת��
	short   m_nNum;
}DC_STKDYNA_MY;

typedef struct STK_DYNA
{
	unsigned short	m_wStkID;			//��ƱID
	time_t	m_time;				//�ɽ�ʱ��
	unsigned long	m_dwOpen;			//����
	unsigned long	m_dwHigh;			//���
	unsigned long	m_dwLow;			//���
	unsigned long	m_dwNew;			//����
	MWORD	m_mVolume;			//�ɽ���
	MWORD	m_mAmount;			//�ɽ���
	MWORD	m_mInnerVol;		//���̳ɽ���,<0��ʾ�ñʳɽ�Ϊ��������>=0��ʾ������,����ֵ��ʾ���̳ɽ���
	unsigned long	m_dwTickCount;			//�ۼƳɽ�����
	unsigned long	m_dwBuyPrice[5];		//ί��۸�
	unsigned long	m_dwBuyVol[5];			//ί����
	unsigned long	m_dwSellPrice[5];		//ί���۸�
	unsigned long	m_dwSellVol[5];			//ί����
	unsigned long	m_dwOpenInterest;		//�ֲ���(�ڻ���ָ����)
	unsigned long	m_dwSettlePrice;		//�����(�ڻ���ָ�ֻ�����)
}STK_DYNA;

typedef struct DC_STKDYNA
{
	unsigned short	m_wDynaSeq;		//��̬�������кţ����ڿͻ���ת��
	short   m_nNum;
	STK_DYNA m_data[1];
}DC_STKDYNA;

typedef struct SH_L2_MMPEX
{
	unsigned short	m_wStkID;			//��ƱID
	unsigned int	m_dwAvgBuyPrice;	//��Ȩƽ��ί��۸�
	MWORD	m_mAllBuyVol;		//ί������
	unsigned int	m_dwAvgSellPrice;	//��Ȩƽ��ί���۸�
	MWORD	m_mAllSellVol;		//ί������
	unsigned int	m_dwBuyPrice[SHL2_MMP_SIZE];	//ί���6-10
	unsigned int	m_dwBuyVol[SHL2_MMP_SIZE];		//ί����6-10
	unsigned int	m_dwSellPrice[SHL2_MMP_SIZE];	//ί����6-10
	unsigned int	m_dwSellVol[SHL2_MMP_SIZE];		//ί����6-10
}SH_L2_MMPEX;

typedef struct DC_STKSTATIC_Ex_MY
{
	short	m_nNum;
}DC_STKSTATIC_Ex_MY;

typedef struct DC_SHL2_MMPEx
{
	short	m_nNum;
	SH_L2_MMPEX m_data[1];
}DC_SHL2_MMPEx;


#pragma unpack()
]]

local C = ffi.C
local pdata
local outstr
function process(dctype,pdcdata)
    if dctype == C.DCT_STKSTATIC then
        --print ("lua:static")
        pdata = ffi.cast("STK_STATIC *",pdcdata)
        --print(ffi.string(pdata.m_strLabel))
        --print(ffi.string(pdata.m_strName))
		outstr = string.format("stk_static: stk_label = %s, last = %d",ffi.string(pdata.m_strLabel),pdata.m_dwLastClose)
    elseif dctype == C.DCT_STKDYNA then
        --print ("lua:dyna")
        pdata = ffi.cast("STK_DYNA *",pdcdata)
		outstr = string.format("[%d]:%d, last = %d, high = %d, low = %d\n",pdata.m_time,pdata.m_wStkID,pdata.m_dwNew,pdata.m_dwHigh,pdata.m_dwLow)
	else
		outstr =""
    end
    
	return pdata.m_wStkID,outstr
end

function process_did(template_id,data)
	local template
	if template_id == 100000 then
		template = require("100000")
		pdata = ffi.cast("T_BUY_SELL_INFO *",data)
		--print(pdata.STKID)
		--print(pdata.BuyCount[0])
		--print(pdata.SellCount[0])
		outstr = "100000"
	elseif template_id == 100001 then
		template = require("100001")
		pdata = ffi.cast("T_BUY_SELL_TICK_INFO *",data)
		--print(pdata.STKID)
		--print(pdata.BuyOrderId)
		outstr = "100001"
	elseif template_id == 100002 then
		template = require("100002")
		pdata = ffi.cast("T_IOPV_INFO *",data)
		--print(pdata.STKID)
		outstr = "100002"
	elseif template_id == 100012 then
		template = require("100012")
		pdata = ffi.cast("T_CBT_MARKET *",data)
		--print(pdata.STKID)
		outstr = "100012"
	elseif tempalte_id == 100030 then
		template = require("100030")
		pdata = ffi.cast("T_ETF_INFO *",data)
		--print(pdata.STKID)
		outstr = "100030"
	elseif template_id == 100032 then
		template = require("100032")
		pdata = ffi.cast("T_MMP_INFO *",data)
		--print(pdata.STKID)
		outstr = "100032"
	end
	return outstr
end

