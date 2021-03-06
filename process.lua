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

typedef struct UINT24
{
	unsigned short m_wLow;
	char m_cHigh;
}UINT24;

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
	DCT_KEEPALIVE,			//<-->±£³ÖÁ¬½Ó1
	DCT_LOGIN,				//<-->µÇÂ½ÒÔ¼°²ÉÊý»úÊôÐÔ	2
	DCT_REQPASS,			//<-->ÒªÇóÓÃ»§ÃûÃÜÂëµÇÂ½3
	DCT_USERnPASS,			//<-->ÓÃ»§ÃûÃÜÂë4
	DCT_READY,				//-->µÇÂ½³É¹¦£¬¿ÉÒÔ½ÓÊÕÊý¾Ý5
	DCT_RESEND,				//-->ÖØ·¢Êý¾Ý°ü6
	DCT_STKSTATIC,			//<--¾²Ì¬Êý¾Ý7
	DCT_STKDYNA,			//<--¶¯Ì¬ÐÐÇé8
	DCT_SHL2_MMPEx,			//<--level2À©Õ¹ÂòÂôÅÌ9
	DCT_SHL2_REPORT,		//<--level2·Ö±Ê³É½»10
	DCT_SHL2_BIG_WD,		//<--level2¼´Ê±×î´ó³·µ¥11
	DCT_SHL2_ACCU_WD,		//<--level2ÀÛ¼Æ×î´ó³·µ¥12
	DCT_HK_STATIC,			//<--¸Û¹É¾²Ì¬13
	DCT_HK_DYNA,			//<--¸Û¹É¶¯Ì¬14
	DCT_XML,				//<--XML¸ñÊ½Êý¾Ý15
	DCT_SHL2_QUEUE,			//<--ÂòÂôÅÌ¶ÓÁÐ£¬level2ÌØÓÐ16
	DCT_GENERAL,			//<--Í¨ÓÃÊý¾Ý17
	DCT_USERSTAT,			//-->ÓÃ»§ÊýÁ¿Í³¼Æ18
	DCT_RAWDATA,			//<-->Ô­Ê¼Êý¾Ý19
	DCT_NEWS,				//<--¹«¸æÐÂÎÅµÈÎÄ±¾ÀàÐÍÊý¾Ý20
	DCT_SZL2_ORDER_QUEUE,	//<--Î¯ÍÐ¶ÓÁÐ£¬ÉîÛÚLevel2ÌØÓÐ21
	DCT_SZL2_ORDER_STAT,	//<--Î¯ÍÐ¶ÓÁÐÍ³¼Æ£¬ÉîÛÚLevel2ÌØÓÐ22

	DCT_SZL2_ORDER_FIVE=100,//<--Î¯ÍÐ¶ÓÁÐ¶à±ÊÃ÷Ï¸£¬ÉîÛÚLevel2ÌØÓÐ
	DCT_SZL2_TRADE_FIVE=101,//<--³É½»,³·µ¥¶ÓÁÐ¶à±ÊÃ÷Ï¸£¬ÉîÛÚLevel2ÌØÓÐ
};

typedef struct DC_STKSTATIC_MY
{
	unsigned long	m_dwVersion;	//¾²Ì¬Êý¾Ý°æ±¾,Ã¿´Î¾²Ì¬Êý¾Ý·¢Éú±ä»¯Ôò±ØÐëÔö¼ÓÊýÖµ,Ã¿¸öÐÐÇéÔ´×îºÃÓÐÒ»¸öÏà¶Ô¶ÀÁ¢µÄ¸ßÎ»£¬ÕâÑù²»Í¬ÐÐÇéÔ´¾Í²»»áÓÐÏàÍ¬µÄ°æ±¾
	unsigned short	m_wAttrib;		//°æ±¾ÊôÐÔ
	unsigned int	m_nDay;			//¾²Ì¬¿âÈÕÆÚ£¬YYYYMMDD
	short	m_nNum;			//m_dataÊýÁ¿
}DC_STKSTATIC_MY;


typedef struct STK_STATIC
{
	unsigned short	id;			//±¾ÊÐ³¡ÄÚÎ¨Ò»±êÊ¾,ÔÚ±¾ÊÐ³¡ÄÚµÄÐòºÅ
	char	label[10];		//´úÂë
	char	name[32];		//Ãû³Æ
	unsigned char	type;			//STK_TYPE
	unsigned char	price_digit;		//¼Û¸ñ×îÐ¡·Ö±æÂÊ£¬·Ç³£ÖØÒª£¬Ã¿Ò»¸öunsigned intÀàÐÍµÄ¼Û¸ñ¶¼Òª³ýÒÔ10^m_nPriceDigit²ÅÊÇÕæÕýµÄ¼Û¸ñ
	short	 vol_unit;			//³É½»Á¿µ¥Î»£¬Ã¿Ò»³É½»Á¿µ¥Î»±íÊ¾¶àÉÙ¹É
	MWORD	float_issued;		//Á÷Í¨¹É±¾
	MWORD	total_issued;		//×Ü¹É±¾

	unsigned long	last_close;		//×òÊÕ
	unsigned long	adv_stop;		//ÕÇÍ£
	unsigned long	dec_stop;
}STK_STATIC;

typedef struct DC_STKSTATIC
{
	unsigned long	m_dwVersion;	//¾²Ì¬Êý¾Ý°æ±¾,Ã¿´Î¾²Ì¬Êý¾Ý·¢Éú±ä»¯Ôò±ØÐëÔö¼ÓÊýÖµ,Ã¿¸öÐÐÇéÔ´×îºÃÓÐÒ»¸öÏà¶Ô¶ÀÁ¢µÄ¸ßÎ»£¬ÕâÑù²»Í¬ÐÐÇéÔ´¾Í²»»áÓÐÏàÍ¬µÄ°æ±¾,0x80000000
	unsigned short	m_wAttrib;		//°æ±¾ÊôÐÔ
	unsigned int	m_nDay;			//¾²Ì¬¿âÈÕÆÚ£¬YYYYMMDD
	short	 m_nNum;			//m_dataÊýÁ¿
	STK_STATIC m_data[1];	//Êý¾Ý
}DC_STKSTATIC;

typedef struct DC_STKDYNA_MY
{
	unsigned short	m_wDynaSeq;		//¶¯Ì¬ÐÐÇéÐòÁÐºÅ£¬ÓÃÓÚ¿Í»§¶Ë×ª·¢
	short   m_nNum;
}DC_STKDYNA_MY;

typedef struct STK_DYNA
{
	unsigned short	id;			//¹ÉÆ±ID
	time_t	deal_time;				//³É½»Ê±¼ä
	unsigned long  open;			//¿ªÅÌ
	unsigned long	high;			//×î¸ß
	unsigned long	low;			//×îµÍ
	unsigned long	last;			//×îÐÂ
	MWORD	vol;			//³É½»Á¿
	MWORD	amount;			//³É½»¶î
	MWORD	inner_vol;		//ÄÚÅÌ³É½»Á¿,<0±íÊ¾¸Ã±Ê³É½»ÎªÖ÷¶¯Âô£¬>=0±íÊ¾Ö÷¶¯Âò,¾ø¶ÔÖµ±íÊ¾ÄÚÅÌ³É½»Á¿
	unsigned long	tick_count;			//ÀÛ¼Æ³É½»±ÊÊý
	unsigned long	buy_price[5];		//Î¯Âò¼Û¸ñ
	unsigned long	buy_vol[5];			//Î¯ÂòÁ¿
	unsigned long	sell_price[5];		//Î¯Âô¼Û¸ñ
	unsigned long	sell_vol[5];			//Î¯ÂôÁ¿
	unsigned long	open_interest;		//³Ö²ÖÁ¿(ÆÚ»õÆÚÖ¸ÌØÓÐ)
	unsigned long	settle_price;		//½áËã¼Û(ÆÚ»õÆÚÖ¸ÏÖ»õÌØÓÐ)
}STK_DYNA;

typedef struct DC_STKDYNA
{
	unsigned short	m_wDynaSeq;		//¶¯Ì¬ÐÐÇéÐòÁÐºÅ£¬ÓÃÓÚ¿Í»§¶Ë×ª·¢
	short   m_nNum;
	STK_DYNA m_data[1];
}DC_STKDYNA;

typedef struct SHL2_MMPEX
{
	unsigned short	id;			//¹ÉÆ±ID
	unsigned int	avg_buy_price;	//¼ÓÈ¨Æ½¾ùÎ¯Âò¼Û¸ñ
	MWORD	all_buy_vol;		//Î¯Âò×ÜÁ¿
	unsigned int	avg_sell_price;	//¼ÓÈ¨Æ½¾ùÎ¯Âô¼Û¸ñ
	MWORD	all_sell_vol;		//Î¯Âô×ÜÁ¿
	unsigned int	buy_price[SHL2_MMP_SIZE];	//Î¯Âò¼Û6-10
	unsigned int	buy_vol[SHL2_MMP_SIZE];		//Î¯ÂòÁ¿6-10
	unsigned int	sell_price[SHL2_MMP_SIZE];	//Î¯Âô¼Û6-10
	unsigned int	sell_vol[SHL2_MMP_SIZE];		//Î¯ÂôÁ¿6-10
}SHL2_MMPEX;

typedef struct SHL2_Queue
{
	unsigned short mmp;
}SHL2_Queue;

typedef struct DCS_STKSTATIC_Ex_MY
{
	short	m_nNum;
}DC_STKSTATIC_Ex_MY;


typedef struct STK_STATICEx
{
	enum STK_SUBTYPE
	{
		NILTYPE = 0,
		ASHARE	= 'A',			//A¹É,½ö¶ÔSTOCK,WARRANTÓÐÐ§
		BSHARE	= 'B',			//B¹É,½ö¶ÔSTOCK,WARRANTÓÐÐ§
		GOV_BOND = 'G',			//¹úÕ®,½ö¶ÔBONDÓÐÐ§
		ENT_BOND = 'O',			//ÆóÒµÕ®,½ö¶ÔBONDÓÐÐ§
		FIN_BOND = 'F',			//½ðÈÚÕ®,½ö¶ÔBONDÓÐÐ§
	};
	enum STK_SP//¹ÉÆ±ÊôÐÔ
	{
		NULLSP = 0,
		NSP	= 'N',//Õý³£
		SSP	= 'S',//ST¹É
		PSP = 'P',//PT¹É
		TSP = 'T',//´ú°ì×ªÈÃÖ¤È¯
		LSP = 'L',//ÉÏÊÐ¿ª·ÅÐÍ»ù½ð£¨LOF£©
		OSP = 'O',//½ö½ÒÊ¾¾»ÖµµÄ¿ª·ÅÊ½»ù½ð
		FSP = 'F',//·Ç½»Ò×ÐÍ¿ª·ÅÊ½»ù½ð
		ESP = 'E',//ETF
		ZSP = 'Z',//´¦ÓÚÍËÊÐÕûÀíÆÚµÄÖ¤È¯
	};
	char	m_cType;			//STK_TYPE,ÓÉ¸Ã±êÖ¾¾ö¶¨ÁªºÏÖÐÊ¹ÓÃÄÄÒ»¸ö½á¹¹
	char	m_cSubType;			//STK_SUBTYPE
	union
	{
		struct 		//¹ÉÆ±	(STOCK,OTHER_STOCK)
		{
			float	m_fFaceValue;		//	ÃæÖµ
			float	m_fProfit;			//	Ã¿¹ÉÊÕÒæ
			unsigned short	m_wIndustry;		//	ÐÐÒµ'A' -- 'M',²Î¼û industryClasify
			char	m_cTradeStatus;		//	½»Ò××´Ì¬£¬'N'=Normal, 'H'=Halt, 'X'=Not trade on this Market
			float	m_fCashDividend;	//	Ã¿¹ÉºìÀû
			char	m_cSecurityProperties;//ÌØÊâ·ÖÀà±ê¼Ç£¬È¡Öµ¼¯ºÏSTK_SPÖÐµÄÖµ
			unsigned long	m_dwLastTradeDate;//×îºó½»Ò×ÈÕ£¬Ä¿Ç°Îª¿Õ£¬×öÔ¤ÁôÓÃ

		} m_equitySpec;
		 struct 		//»ù½ð,ETF,LOF	(FUND,ETF,LOF)
		{
			float	m_fFaceValue;		//	ÃæÖµ
			float	m_fTotalIssued;		//	×Ü¹É±¾
			float	m_fIOPV;			//	IOPV¾»Öµ,½ö¶ÔETF,LOFÓÐÐ§£¬***
		} m_fundSpec;
		 struct		//È¨Ö¤,Ñ¡ÔñÈ¨	(OPTION,WARRANT)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  ¶Ò»»/ÐÐÈ¨±ÈÀý
			float	m_fStrikePrice;		//	ÐÐÈ¨¼Û¸ñ
			unsigned long	m_dwMaturityDate;	//	µ½ÆÚÈÕ,YYYYMMDD
			char	m_strUnderLine[12];	//	¶ÔÓ¦¹ÉÆ±,°üº¬ÊÐ³¡´úÂë£¬ÈçSH600000
			float	m_fBalance;			//  Á÷Í¨Óà¶î
		} m_warrantSpec;
		 struct 		//Õ®È¯	(BOND)
		{
			unsigned long	m_dwMaturityDate;	//	µ½ÆÚÈÕ,YYYYMMDD
			unsigned long	m_dwIntAccruDate;	//	ÆðÏ¢ÈÕ£¬YYYYMMDD
			float	m_fIssuePrice;		//	·¢ÐÐ¼Û
			float	m_fCouponRate;		//	ÀûÂÊ
			float	m_fFaceValue;		//	ÃæÖµ
			float	m_fAccruedInt;		//	Ó¦¼ÆÀûÏ¢,***
		} m_bondSpec;
		 struct		//¿É×ªÕ®	(COV_BOND)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  ¶Ò»»/ÐÐÈ¨±ÈÀý
			float	m_fStrikePrice;		//	ÐÐÈ¨¼Û¸ñ
			unsigned long	m_dwMaturityDate;	//	µ½ÆÚÈÕ,YYYYMMDD
			char	m_strUnderLine[12];	//	¶ÔÓ¦¹ÉÆ±,°üº¬ÊÐ³¡´úÂë£¬ÈçSH600000
			float	m_fAccruedInt;		//	Ó¦¼ÆÀûÏ¢
		} m_CnvtSpec;
	   struct		//ÆÚ»õ,ÉÌÆ·	(FUTURE,FTR_IDX,COMM)
		{
			unsigned long	last_day_oi;			//×òÈÕ³Ö²ÖÁ¿
			float	last_settle_price;		//×òÈÕ½áËã¼Û
		} m_futureSpec;
		 struct	//ÐÅÍÐ	(TRUST)
		{
			float	m_dwfAsset;			//¾»×Ê²ú
			unsigned long	m_dwAssetDate;		//YYYYMMDD
		} m_trustSpec;
	}Spec;
}STK_STATICEx;  

typedef struct STK_HKDYNA
{
	unsigned long	m_dwIEPrice;	//Æ½ºâ¼Û
	MWORD	m_mIEVolume;	//Æ½ºâÁ¿

	//ÂòÂôÅÌ±ÊÊý
	unsigned short	m_wBuyOrderNum[5];
	unsigned short	m_wSellOrderNum[5];

	//¾­¼Í¶ÓÁÐ
	struct HK_BROKER	
	{
		unsigned short	m_wNumBroker;
		unsigned short	m_wBrokerNo[40];
		char	m_cType[40];		//'B':broker,'R':register trader,'S':number of spread
	}m_buyBroker,m_sellBroker;
}STK_HKDYNA;

typedef struct SZL2_ORDER_STAT
{
	unsigned short m_wStkID;				//¹ÉÆ±ID
	MWORD	m_nBuyOrderVol[4];		//ÂòÈëµ¥Á¿£¬Ð¡¡¢ÖÐ¡¢´ó¡¢ÌØ´ó
	MWORD	m_nSellOrderVol[4];		//Âô³öµ¥Á¿£¬Ð¡¡¢ÖÐ¡¢´ó¡¢ÌØ´ó
	UINT24	m_dwOrderNum[2];		//Î¯ÍÐµ¥ÊýÁ¿£¬ÂòÈë/Âô³ö
	MWORD	m_nWDVol[2];			
}SZL2_ORDER_STAT;

typedef struct SZL2_ORDER_FIVE
{
	unsigned short nIndex;
	char strOrderKind;
	char strFunCode;
	unsigned long dwPrice;
	unsigned long dwAmount;
	unsigned long dwRecNO;
	int   nSetNO;
	unsigned long nRecTime;
}SZL2_ORDER_FIVE;

typedef struct SZL2_TRADE_FIVE
{
	unsigned short nIndex;
	char strOrderKind;
	char strFunCode;
	unsigned long dwPrice;
	unsigned long dwAmount;
	unsigned long dwRecNO;
	int   nSetNO;
	unsigned long   nRecTime;
}SZL2_TRADE_FIVE;

#pragma unpack()
]]

local C = ffi.C
local ffi_cast = ffi.cast 
local str_format = string.format

function FormatReturnError(dc_type,ret_error)
	local ret_str
	if(ret_error ~= nil) then
		ret_str = str_format("%s:%s", dc_type,ret_error)
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

function test_process( dctype,num,pdcdata )
	local stk 
	local dc_type
	local ret_error
	local ret_str

	if dctype == C.DCT_STKSTATIC then
		stk = ffi_cast("STK_STATIC *", pdcdata)
		dc_type = "static"
		for i=1,num do
			--print(stk.id)
			ret_error = handle_stk_static(stk)			
			ret_str = FormatReturnError(dc_type, ret_error)
			stk = stk + 1
		end
	elseif dctype == C.DCT_STKDYNA then
		stk = ffi_cast("STK_DYNA *", pdcdata)
		dc_type = "dyna"
		for i=1,num do
			ret_error = handle_stk_dyna(stk)
			ret_str = FormatReturnError(dc_type, ret_error)
			stk = stk + 1
		end
	elseif dctype == C.DCT_SZL2_ORDER_STAT then
		stk = ffi_cast("SZL2_ORDER_STAT *", pdcdata)
		dc_type = "szl2_order_stat"
		for i=1,num do
			ret_error = handle_szl2_order_stat(stk) 
			ret_str = FormatReturnError(dc_type, ret_error)
			stk = stk + 1
		end
	elseif dctype == C.DCT_SZL2_TRADE_FIVE then
		stk = ffi_cast("SZL2_TRADE_FIVE*", pdcdata)
		dc_type = "szl2_trade_five"
		for i=1,num do
			ret_error = handle_szl2_trade_five(stk)
			ret_str	= FormatReturnError(dc_type, ret_error)
			stk = stk + 1
		end
	elseif dctype == C.DCT_SZL2_ORDER_FIVE then
		stk = ffi_cast("SZL2_ORDER_FIVE*", pdcdata)
		dc_type = "szl2_order_five"
		for i=1,num do
			ret_error = handle_szl2_order_five(stk)
			ret_str = FormatReturnError(dc_type, ret_error)
			stk = stk + 1
		end
	elseif dctype == C.DCT_SHL2_MMPEx then
		stk = ffi_cast("SHL2_MMPEX *", pdcdata)
		dc_type = "shl2_mmpex"
		for i=1,num do
			ret_error = handle_shl2_mmpex(stk)
			ret_str = FormatReturnError(dc_type, ret_error)
			stk = stk + 1
		end
	else
		ret_str = nil
	end
end

function process(dctype,pdcdata)
	local stk
	local ret_error
	local ret_str
    if dctype == C.DCT_STKSTATIC then
        stk = ffi_cast("STK_STATIC *",pdcdata)
		--outstr = string.format("stk_static: stk_label = %s, last = %d",ffi.string(pdata.m_strLabel),pdata.m_dwLastClose)
		ret_error = handle_stk_static(stk)
		dc_type = "static"
		ret_str = FormatReturnError(dc_type,ret_error)
   elseif dctype == C.DCT_STKDYNA then
        stk = ffi_cast("STK_DYNA *",pdcdata)
		ret_error = handle_stk_dyna(stk)
		dc_type = "dyna"
		ret_str = FormatReturnError(dc_type,ret_error)
		--outstr = string.format("[%d]:%d, last = %d, high = %d, low = %d\n",pdata.m_time,pdata.m_wStkID,pdata.m_dwNew,pdata.m_dwHigh,pdata.m_dwLow)
	elseif dctype == C.DCT_SHL2_MMPEx then
		stk = ffi_cast("SHL2_MMPEX *", pdcdata)
		ret_error = handle_shl2_mmpex(stk)
		dc_type = "shl2_mmpex"
		ret_str = FormatReturnError(dc_type, ret_error)
	else
		ret_str =nil
    end
	return stk.id,ret_str
end

function process_did(port,template_id,data)
	local oustr
	local pdata
	--local	lua_lib = string.format("%d_%d",port,template_id)
	--local template = require(lua_lib)
	if template_id == 100000 then
		pdata = ffi_cast("T_BUY_SELL_INFO *",data)
		--print(pdata.STKID)
		--print(pdata.BuyCount[0])
		--print(pdata.SellCount[0])
		outstr = "100000"
	elseif template_id == 100001 then
		pdata = ffi_cast("T_BUY_SELL_TICK_INFO *",data)
		--print(pdata.STKID)
		--print(pdata.BuyOrderId)
		outstr = "100001"
	elseif template_id == 100002 then
		pdata = ffi_cast("T_IOPV_INFO *",data)
		--print(pdata.STKID)
		outstr = "100002"
	elseif template_id == 100012 then
		pdata = ffi_cast("T_CBT_MARKET *",data)
		--print(pdata.STKID)
		outstr = "100012"
	elseif tempalte_id == 100030 then
		pdata = ffi_cast("T_ETF_INFO *",data)
		--print(pdata.STKID)
		outstr = "100030"
	elseif template_id == 100032 then
		pdata = ffi_cast("T_MMP_INFO *",data)
		--print(pdata.STKID)
		outstr = "100032"
	end
	return outstr
end

function process_general(intype,data)
	local stk
	local ret_error
	local ret_str
		if(intype == C.GE_STATIC_EX) then
			stk = ffi_cast("STK_STATICEx *" ,data)
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
			stk = ffi_cast("STK_HKDYNA *",data)
			ret_str = nil
		elseif(intype == C.GE_IOPV) then
			local iopv = ffi_cast("IOPV *",data)
			--print("iopv")
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
		stk = ffi_cast("SHL2_Queue *", pdcdata)
		local ret_error = handle_shl2_mmp(stk)
		dc_type = "shl2_queue"
		local ret_str = FormatReturnError(dc_type, ret_error)
	end
	return ret_str
end
