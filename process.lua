require("os")
--local path =  os.getenv("PWD")
--local lualib_path = path..'/lualib/'

require("base_type")
require("logic_process")
--require("9268_100000")
--require("9268_100001")
--require("9268_100002")
--require("9268_100030")
--require("9268_100032")


local ffi = require("ffi")

--io.write("this is lua script\n")

ffi.cdef [[
#pragma pack(1)

static const int  SHL2_MMP_SIZE=5;

int printf(const char *fmt, ...);

typedef unsigned int time_t;

typedef struct IOPV
{
	float value;
}IOPV;
enum DC_GENERAL_INTYPE
{
	GE_IOPV = 5,
	GE_MATU_YLD = 6,
	GE_HKREL_TXT = 1001,
	GE_STATIC_EX = 10001,
	GE_HKDYNA = 10002,
	GE_BLK_STK = 10003,
};

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
	unsigned short	id;			//本市场内唯一标示,在本市场内的序号
	char	label[10];		//代码
	char	name[32];		//名称
	unsigned char	type;			//STK_TYPE
	unsigned char	price_digit;		//价格最小分辨率，非常重要，每一个unsigned int类型的价格都要除以10^m_nPriceDigit才是真正的价格
	short	 vol_unit;			//成交量单位，每一成交量单位表示多少股
	MWORD	float_issued;		//流通股本
	MWORD	total_issued;		//总股本

	unsigned long	last_close;		//昨收
	unsigned long	adv_stop;		//涨停
	unsigned long	dec_stop;
}STK_STATIC;

typedef struct DC_STKSTATIC
{
	unsigned long	m_dwVersion;	//静态数据版本,每次静态数据发生变化则必须增加数值,每个行情源最好有一个相对独立的高位，这样不同行情源就不会有相同的版本,0x80000000
	unsigned short	m_wAttrib;		//版本属性
	unsigned int	m_nDay;			//静态库日期，YYYYMMDD
	short	 m_nNum;			//m_data数量
	STK_STATIC m_data[1];	//数据
}DC_STKSTATIC;

typedef struct DC_STKDYNA_MY
{
	unsigned short	m_wDynaSeq;		//动态行情序列号，用于客户端转发
	short   m_nNum;
}DC_STKDYNA_MY;

typedef struct STK_DYNA
{
	unsigned short	id;			//股票ID
	time_t	deal_time;				//成交时间
	unsigned long  open;			//开盘
	unsigned long	high;			//最高
	unsigned long	low;			//最低
	unsigned long	last;			//最新
	MWORD	vol;			//成交量
	MWORD	amount;			//成交额
	MWORD	inner_vol;		//内盘成交量,<0表示该笔成交为主动卖，>=0表示主动买,绝对值表示内盘成交量
	unsigned long	tick_count;			//累计成交笔数
	unsigned long	buy_price[5];		//委买价格
	unsigned long	buy_vol[5];			//委买量
	unsigned long	sell_price[5];		//委卖价格
	unsigned long	sell_vol[5];			//委卖量
	unsigned long	open_interest;		//持仓量(期货期指特有)
	unsigned long	settle_price;		//结算价(期货期指现货特有)
}STK_DYNA;

typedef struct DC_STKDYNA
{
	unsigned short	m_wDynaSeq;		//动态行情序列号，用于客户端转发
	short   m_nNum;
	STK_DYNA m_data[1];
}DC_STKDYNA;

typedef struct SH_L2_MMPEX
{
	unsigned short	id;			//股票ID
	unsigned int	avg_buy_price;	//加权平均委买价格
	MWORD	all_buy_vol;		//委买总量
	unsigned int	avg_sell_price;	//加权平均委卖价格
	MWORD	all_sell_vol;		//委卖总量
	unsigned int	buy_price[SHL2_MMP_SIZE];	//委买价6-10
	unsigned int	buy_vol[SHL2_MMP_SIZE];		//委买量6-10
	unsigned int	sell_price[SHL2_MMP_SIZE];	//委卖价6-10
	unsigned int	sell_vol[SHL2_MMP_SIZE];		//委卖量6-10
}SH_L2_MMPEX;

typedef struct SH_L2_Queue
{
	unsigned short mmp;
}SH_L2_Queue;

typedef struct DCS_STKSTATIC_Ex_MY
{
	short	m_nNum;
}DC_STKSTATIC_Ex_MY;


typedef struct STK_STATICEx
{
	enum STK_SUBTYPE
	{
		NILTYPE = 0,
		ASHARE	= 'A',			//A股,仅对STOCK,WARRANT有效
		BSHARE	= 'B',			//B股,仅对STOCK,WARRANT有效
		GOV_BOND = 'G',			//国债,仅对BOND有效
		ENT_BOND = 'O',			//企业债,仅对BOND有效
		FIN_BOND = 'F',			//金融债,仅对BOND有效
	};
	enum STK_SP//股票属性
	{
		NULLSP = 0,
		NSP	= 'N',//正常
		SSP	= 'S',//ST股
		PSP = 'P',//PT股
		TSP = 'T',//代办转让证券
		LSP = 'L',//上市开放型基金（LOF）
		OSP = 'O',//仅揭示净值的开放式基金
		FSP = 'F',//非交易型开放式基金
		ESP = 'E',//ETF
		ZSP = 'Z',//处于退市整理期的证券
	};
	char	m_cType;			//STK_TYPE,由该标志决定联合中使用哪一个结构
	char	m_cSubType;			//STK_SUBTYPE
	union
	{
		struct 		//股票	(STOCK,OTHER_STOCK)
		{
			float	m_fFaceValue;		//	面值
			float	m_fProfit;			//	每股收益
			unsigned short	m_wIndustry;		//	行业'A' -- 'M',参见 industryClasify
			char	m_cTradeStatus;		//	交易状态，'N'=Normal, 'H'=Halt, 'X'=Not trade on this Market
			float	m_fCashDividend;	//	每股红利
			char	m_cSecurityProperties;//特殊分类标记，取值集合STK_SP中的值
			unsigned long	m_dwLastTradeDate;//最后交易日，目前为空，做预留用

		} m_equitySpec;
		 struct 		//基金,ETF,LOF	(FUND,ETF,LOF)
		{
			float	m_fFaceValue;		//	面值
			float	m_fTotalIssued;		//	总股本
			float	m_fIOPV;			//	IOPV净值,仅对ETF,LOF有效，***
		} m_fundSpec;
		 struct		//权证,选择权	(OPTION,WARRANT)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  兑换/行权比例
			float	m_fStrikePrice;		//	行权价格
			unsigned long	m_dwMaturityDate;	//	到期日,YYYYMMDD
			char	m_strUnderLine[12];	//	对应股票,包含市场代码，如SH600000
			float	m_fBalance;			//  流通余额
		} m_warrantSpec;
		 struct 		//债券	(BOND)
		{
			unsigned long	m_dwMaturityDate;	//	到期日,YYYYMMDD
			unsigned long	m_dwIntAccruDate;	//	起息日，YYYYMMDD
			float	m_fIssuePrice;		//	发行价
			float	m_fCouponRate;		//	利率
			float	m_fFaceValue;		//	面值
			float	m_fAccruedInt;		//	应计利息,***
		} m_bondSpec;
		 struct		//可转债	(COV_BOND)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  兑换/行权比例
			float	m_fStrikePrice;		//	行权价格
			unsigned long	m_dwMaturityDate;	//	到期日,YYYYMMDD
			char	m_strUnderLine[12];	//	对应股票,包含市场代码，如SH600000
			float	m_fAccruedInt;		//	应计利息
		} m_CnvtSpec;
	   struct		//期货,商品	(FUTURE,FTR_IDX,COMM)
		{
			unsigned long	last_day_oi;			//昨日持仓量
			float	last_settle_price;		//昨日结算价
		} m_futureSpec;
		 struct	//信托	(TRUST)
		{
			float	m_dwfAsset;			//净资产
			unsigned long	m_dwAssetDate;		//YYYYMMDD
		} m_trustSpec;
	}Spec;
}STK_STATICEx;  

typedef struct STK_HKDYNA
{
	unsigned long	m_dwIEPrice;	//平衡价
	MWORD	m_mIEVolume;	//平衡量

	//买卖盘笔数
	unsigned short	m_wBuyOrderNum[5];
	unsigned short	m_wSellOrderNum[5];

	//经纪队列
	struct HK_BROKER	
	{
		unsigned short	m_wNumBroker;
		unsigned short	m_wBrokerNo[40];
		char	m_cType[40];		//'B':broker,'R':register trader,'S':number of spread
	}m_buyBroker,m_sellBroker;
}STK_HKDYNA;

#pragma unpack()
]]

local C = ffi.C


function FormatReturnError(dc_type,ret_error)
	local ret_str
	if(ret_error ~= nil) then
		ret_str = string.format("%s:%s", dc_type,ret_error)
	else
		ret_str = nil
	end
	return ret_str
end

local did_template_id_table = {}
function init(did_template_id)
	req_id = require(did_template_id)
	table.insert(did_template_id_table,req_id)	
end

function process(dctype,pdcdata)
	local stk
	local ret_str
    if dctype == C.DCT_STKSTATIC then
        stk = ffi.cast("STK_STATIC *",pdcdata)
		--outstr = string.format("stk_static: stk_label = %s, last = %d",ffi.string(pdata.m_strLabel),pdata.m_dwLastClose)
		ret_error = handle_stk_static(stk)
		dc_type = "static"
		ret_str = FormatReturnError(dc_type,ret_error)
   elseif dctype == C.DCT_STKDYNA then
        stk = ffi.cast("STK_DYNA *",pdcdata)
		ret_error = handle_stk_dyna(stk)
		dc_type = "dyna"
		ret_str = FormatReturnError(dc_type,ret_error)
		--outstr = string.format("[%d]:%d, last = %d, high = %d, low = %d\n",pdata.m_time,pdata.m_wStkID,pdata.m_dwNew,pdata.m_dwHigh,pdata.m_dwLow)
	elseif dctype == C.DCT_SHL2_MMPEx then
		stk = ffi.cast("SH_L2_MMPEX *", pdcdata)
		ret_error = handle_shl2_mmpex(stk)
		dc_type = "sh_l2_mmpex"
		ret_str = FormatReturnError(dc_type, ret_error)
	else
		ret_str =nil
    end
	return stk.id,ret_str
end

function process_did(port,template_id,data)
	local oustr
	--local	lua_lib = string.format("%d_%d",port,template_id)
	--local template = require(lua_lib)
	if template_id == 100000 then
		pdata = ffi.cast("T_BUY_SELL_INFO *",data)
		--print(pdata.STKID)
		--print(pdata.BuyCount[0])
		--print(pdata.SellCount[0])
		outstr = "100000"
	elseif template_id == 100001 then
		pdata = ffi.cast("T_BUY_SELL_TICK_INFO *",data)
		--print(pdata.STKID)
		--print(pdata.BuyOrderId)
		outstr = "100001"
	elseif template_id == 100002 then
		pdata = ffi.cast("T_IOPV_INFO *",data)
		--print(pdata.STKID)
		outstr = "100002"
	elseif template_id == 100012 then
		pdata = ffi.cast("T_CBT_MARKET *",data)
		--print(pdata.STKID)
		outstr = "100012"
	elseif tempalte_id == 100030 then
		pdata = ffi.cast("T_ETF_INFO *",data)
		--print(pdata.STKID)
		outstr = "100030"
	elseif template_id == 100032 then
		pdata = ffi.cast("T_MMP_INFO *",data)
		--print(pdata.STKID)
		outstr = "100032"
	end
	return outstr
end

function process_general(intype,data)
	local stk
	local ret_str
		if(intype == C.GE_STATIC_EX) then
			stk = ffi.cast("STK_STATICEx *" ,data)
			--print(stk.m_cType)
			--print(stk.m_cSubType)
			if(stk.m_cType == 1) then
				ret_str = nil
				--print("equity")
				--print(stk.Spec.m_equitySpec.m_fFaceValue)
				--print(stk.Spec.m_equitySpec.m_fProfit)
				--print(stk.Spec.m_equitySpec.m_wIndustry)
			elseif(stk.m_cType == 2) then
				ret_str = nil
			elseif(stk.m_cType == 3) then
				ret_str = nil
				--print("warrantSpec")
				--print(stk.Spec.m_warrantSpec.m_cStyle)
				--print(stk.Spec.m_warrantSpec.m_cCP)
				--print(stk.Spec.m_warrantSpec.m_fStrikePrice)
			elseif(stk.m_cType == 4) then
				ret_str = nil
				--print("bondSpec")
				--print(stk.Spec.m_bondSpec.m_dwMaturityDate)
				--print(stk.Spec.m_bondSpec.m_dwIntAccruDate)
				--print(stk.Spec.m_bondSpec.m_fIssuePrice)
				--print(stk.Spec.m_bondSpec.m_fFaceValue)
			elseif(stk.m_cType == 5) then
				ret_str = nil
			elseif(stk.m_cType == 6) then
				dc_type = "staticex future"
				ret_error = handle_future(stk.Spec.m_futureSpec)
				ret_str = FormatReturnError(dc_type,ret_error)
			elseif(stk.m_cType == 7) then
				ret_str = nil
			else				 
				ret_str = nil
			end 
		elseif(intype == C.GE_HKDYNA) then
			stk = ffi.cast("STK_HKDYNA *",data)
			ret_str = nil
		elseif(intype == C.GE_IOPV) then
			iopv = ffi.cast("IOPV *",data)
			print("iopv")
			dc_type = "iopv"
			ret_error = handle_iopv(iopv.value)
			ret_str = FormatReturnError(dc_type,ret_error) 
		else 
			ret_str = nil
		end
		return ret_str 
end

function process_shl2_queue(dctype, pdcdata)
	local stk
	if dctype == C.DCT_SHL2_QUEUE then
		stk = ffi.cast("SH_L2_Queue *", pdcdata)
		ret_error = handle_shl2_mmp(stk)
		dc_type = "sh_l2_queue"
		ret_str = FormatReturnError(dc_type, ret_error)
	end
	return ret_str
end