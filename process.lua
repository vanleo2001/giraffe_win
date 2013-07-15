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
	DCT_KEEPALIVE,			//<-->保持连接1
	DCT_LOGIN,				//<-->登陆以及采数机属性	2
	DCT_REQPASS,			//<-->要求用户名密码登陆3
	DCT_USERnPASS,			//<-->用户名密码4
	DCT_READY,				//-->登陆成功，可以接收数据5
	DCT_RESEND,				//-->重发数据包6
	DCT_STKSTATIC,			//<--静态数据7
	DCT_STKDYNA,			//<--动态行情8
	DCT_SHL2_MMPEx,			//<--level2扩展买卖盘9
	DCT_SHL2_REPORT,		//<--level2分笔成交10
	DCT_SHL2_BIG_WD,		//<--level2即时最大撤单11
	DCT_SHL2_ACCU_WD,		//<--level2累计最大撤单12
	DCT_HK_STATIC,			//<--港股静态13
	DCT_HK_DYNA,			//<--港股动态14
	DCT_XML,				//<--XML格式数据15
	DCT_SHL2_QUEUE,			//<--买卖盘队列，level2特有16
	DCT_GENERAL,			//<--通用数据17
	DCT_USERSTAT,			//-->用户数量统计18
	DCT_RAWDATA,			//<-->原始数据19
	DCT_NEWS,				//<--公告新闻等文本类型数据20
	DCT_SZL2_ORDER_QUEUE,	//<--委托队列，深圳Level2特有21
	DCT_SZL2_ORDER_STAT,	//<--委托队列统计，深圳Level2特有22

	DCT_SZL2_FULL_ORDER=100,//<--委托队列多笔明细，深圳Level2特有
	DCT_SZL2_FULL_TRADE=101,//<--成交,撤单队列多笔明细，深圳Level2特有
};

typedef struct DC_STKSTATIC_MY
{
	unsigned long	m_dwVersion;	//静态数据版本,每次静态数据发生变化则必须增加数值,每个行情源最好有一个相对独立的高位，这样不同行情源就不会有相同的版本
	unsigned short	m_wAttrib;		//版本属性
	unsigned int	m_nDay;			//静态库日期，YYYYMMDD
	short	m_nNum;			//m_data数量
}DC_STKSTATIC_MY;

typedef struct STK_STATIC
{
	unsigned short	m_wStkID;			//本市场内唯一标示,在本市场内的序号
	char	m_strLabel[10];		//代码
	char	m_strName[32];		//名称
	unsigned char	m_cType;			//STK_TYPE
	unsigned char	m_nPriceDigit;		//价格最小分辨率，非常重要，每一个unsigned int类型的价格都要除以10^m_nPriceDigit才是真正的价格
	short	m_nVolUnit;			//成交量单位，每一成交量单位表示多少股
	MWORD	m_mFloatIssued;		//流通股本
	MWORD	m_mTotalIssued;		//总股本

	unsigned long	m_dwLastClose;		//昨收
	unsigned long	m_dwAdvStop;		//涨停
	unsigned long	m_dwDecStop;
}STK_STATIC;

typedef struct DC_STKSTATIC
{
	unsigned long	m_dwVersion;	//静态数据版本,每次静态数据发生变化则必须增加数值,每个行情源最好有一个相对独立的高位，这样不同行情源就不会有相同的版本,0x80000000
	unsigned short	m_wAttrib;		//版本属性
	unsigned int	m_nDay;			//静态库日期，YYYYMMDD
	short	m_nNum;			//m_data数量
	STK_STATIC m_data[1];	//数据
}DC_STKSTATIC;

typedef struct DC_STKDYNA_MY
{
	unsigned short	m_wDynaSeq;		//动态行情序列号，用于客户端转发
	short   m_nNum;
}DC_STKDYNA_MY;

typedef struct STK_DYNA
{
	unsigned short	m_wStkID;			//股票ID
	time_t	m_time;				//成交时间
	unsigned long	m_dwOpen;			//开盘
	unsigned long	m_dwHigh;			//最高
	unsigned long	m_dwLow;			//最低
	unsigned long	m_dwNew;			//最新
	MWORD	m_mVolume;			//成交量
	MWORD	m_mAmount;			//成交额
	MWORD	m_mInnerVol;		//内盘成交量,<0表示该笔成交为主动卖，>=0表示主动买,绝对值表示内盘成交量
	unsigned long	m_dwTickCount;			//累计成交笔数
	unsigned long	m_dwBuyPrice[5];		//委买价格
	unsigned long	m_dwBuyVol[5];			//委买量
	unsigned long	m_dwSellPrice[5];		//委卖价格
	unsigned long	m_dwSellVol[5];			//委卖量
	unsigned long	m_dwOpenInterest;		//持仓量(期货期指特有)
	unsigned long	m_dwSettlePrice;		//结算价(期货期指现货特有)
}STK_DYNA;

typedef struct DC_STKDYNA
{
	unsigned short	m_wDynaSeq;		//动态行情序列号，用于客户端转发
	short   m_nNum;
	STK_DYNA m_data[1];
}DC_STKDYNA;

typedef struct SH_L2_MMPEX
{
	unsigned short	m_wStkID;			//股票ID
	unsigned int	m_dwAvgBuyPrice;	//加权平均委买价格
	MWORD	m_mAllBuyVol;		//委买总量
	unsigned int	m_dwAvgSellPrice;	//加权平均委卖价格
	MWORD	m_mAllSellVol;		//委卖总量
	unsigned int	m_dwBuyPrice[SHL2_MMP_SIZE];	//委买价6-10
	unsigned int	m_dwBuyVol[SHL2_MMP_SIZE];		//委买量6-10
	unsigned int	m_dwSellPrice[SHL2_MMP_SIZE];	//委卖价6-10
	unsigned int	m_dwSellVol[SHL2_MMP_SIZE];		//委卖量6-10
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

