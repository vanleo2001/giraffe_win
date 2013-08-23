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
	unsigned short	id;			//���г���Ψһ��ʾ,�ڱ��г��ڵ����
	char	label[10];		//����
	char	name[32];		//����
	unsigned char	type;			//STK_TYPE
	unsigned char	price_digit;		//�۸���С�ֱ��ʣ��ǳ���Ҫ��ÿһ��unsigned int���͵ļ۸�Ҫ����10^m_nPriceDigit���������ļ۸�
	short	 vol_unit;			//�ɽ�����λ��ÿһ�ɽ�����λ��ʾ���ٹ�
	MWORD	float_issued;		//��ͨ�ɱ�
	MWORD	total_issued;		//�ܹɱ�

	unsigned long	last_close;		//����
	unsigned long	adv_stop;		//��ͣ
	unsigned long	dec_stop;
}STK_STATIC;

typedef struct DC_STKSTATIC
{
	unsigned long	m_dwVersion;	//��̬���ݰ汾,ÿ�ξ�̬���ݷ����仯�����������ֵ,ÿ������Դ�����һ����Զ����ĸ�λ��������ͬ����Դ�Ͳ�������ͬ�İ汾,0x80000000
	unsigned short	m_wAttrib;		//�汾����
	unsigned int	m_nDay;			//��̬�����ڣ�YYYYMMDD
	short	 m_nNum;			//m_data����
	STK_STATIC m_data[1];	//����
}DC_STKSTATIC;

typedef struct DC_STKDYNA_MY
{
	unsigned short	m_wDynaSeq;		//��̬�������кţ����ڿͻ���ת��
	short   m_nNum;
}DC_STKDYNA_MY;

typedef struct STK_DYNA
{
	unsigned short	id;			//��ƱID
	time_t	deal_time;				//�ɽ�ʱ��
	unsigned long  open;			//����
	unsigned long	high;			//���
	unsigned long	low;			//���
	unsigned long	last;			//����
	MWORD	vol;			//�ɽ���
	MWORD	amount;			//�ɽ���
	MWORD	inner_vol;		//���̳ɽ���,<0��ʾ�ñʳɽ�Ϊ��������>=0��ʾ������,����ֵ��ʾ���̳ɽ���
	unsigned long	tick_count;			//�ۼƳɽ�����
	unsigned long	buy_price[5];		//ί��۸�
	unsigned long	buy_vol[5];			//ί����
	unsigned long	sell_price[5];		//ί���۸�
	unsigned long	sell_vol[5];			//ί����
	unsigned long	open_interest;		//�ֲ���(�ڻ���ָ����)
	unsigned long	settle_price;		//�����(�ڻ���ָ�ֻ�����)
}STK_DYNA;

typedef struct DC_STKDYNA
{
	unsigned short	m_wDynaSeq;		//��̬�������кţ����ڿͻ���ת��
	short   m_nNum;
	STK_DYNA m_data[1];
}DC_STKDYNA;

typedef struct SH_L2_MMPEX
{
	unsigned short	id;			//��ƱID
	unsigned int	avg_buy_price;	//��Ȩƽ��ί��۸�
	MWORD	all_buy_vol;		//ί������
	unsigned int	avg_sell_price;	//��Ȩƽ��ί���۸�
	MWORD	all_sell_vol;		//ί������
	unsigned int	buy_price[SHL2_MMP_SIZE];	//ί���6-10
	unsigned int	buy_vol[SHL2_MMP_SIZE];		//ί����6-10
	unsigned int	sell_price[SHL2_MMP_SIZE];	//ί����6-10
	unsigned int	sell_vol[SHL2_MMP_SIZE];		//ί����6-10
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
		ASHARE	= 'A',			//A��,����STOCK,WARRANT��Ч
		BSHARE	= 'B',			//B��,����STOCK,WARRANT��Ч
		GOV_BOND = 'G',			//��ծ,����BOND��Ч
		ENT_BOND = 'O',			//��ҵծ,����BOND��Ч
		FIN_BOND = 'F',			//����ծ,����BOND��Ч
	};
	enum STK_SP//��Ʊ����
	{
		NULLSP = 0,
		NSP	= 'N',//����
		SSP	= 'S',//ST��
		PSP = 'P',//PT��
		TSP = 'T',//����ת��֤ȯ
		LSP = 'L',//���п����ͻ���LOF��
		OSP = 'O',//����ʾ��ֵ�Ŀ���ʽ����
		FSP = 'F',//�ǽ����Ϳ���ʽ����
		ESP = 'E',//ETF
		ZSP = 'Z',//�������������ڵ�֤ȯ
	};
	char	m_cType;			//STK_TYPE,�ɸñ�־����������ʹ����һ���ṹ
	char	m_cSubType;			//STK_SUBTYPE
	union
	{
		struct 		//��Ʊ	(STOCK,OTHER_STOCK)
		{
			float	m_fFaceValue;		//	��ֵ
			float	m_fProfit;			//	ÿ������
			unsigned short	m_wIndustry;		//	��ҵ'A' -- 'M',�μ� industryClasify
			char	m_cTradeStatus;		//	����״̬��'N'=Normal, 'H'=Halt, 'X'=Not trade on this Market
			float	m_fCashDividend;	//	ÿ�ɺ���
			char	m_cSecurityProperties;//��������ǣ�ȡֵ����STK_SP�е�ֵ
			unsigned long	m_dwLastTradeDate;//������գ�ĿǰΪ�գ���Ԥ����

		} m_equitySpec;
		 struct 		//����,ETF,LOF	(FUND,ETF,LOF)
		{
			float	m_fFaceValue;		//	��ֵ
			float	m_fTotalIssued;		//	�ܹɱ�
			float	m_fIOPV;			//	IOPV��ֵ,����ETF,LOF��Ч��***
		} m_fundSpec;
		 struct		//Ȩ֤,ѡ��Ȩ	(OPTION,WARRANT)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  �һ�/��Ȩ����
			float	m_fStrikePrice;		//	��Ȩ�۸�
			unsigned long	m_dwMaturityDate;	//	������,YYYYMMDD
			char	m_strUnderLine[12];	//	��Ӧ��Ʊ,�����г����룬��SH600000
			float	m_fBalance;			//  ��ͨ���
		} m_warrantSpec;
		 struct 		//ծȯ	(BOND)
		{
			unsigned long	m_dwMaturityDate;	//	������,YYYYMMDD
			unsigned long	m_dwIntAccruDate;	//	��Ϣ�գ�YYYYMMDD
			float	m_fIssuePrice;		//	���м�
			float	m_fCouponRate;		//	����
			float	m_fFaceValue;		//	��ֵ
			float	m_fAccruedInt;		//	Ӧ����Ϣ,***
		} m_bondSpec;
		 struct		//��תծ	(COV_BOND)
		{
			char	m_cStyle;			//  'A' or 'E'	American or European Style
			char	m_cCP;				//	'C' or 'P' Call or Put
			float	m_fCnvtRatio;		//  �һ�/��Ȩ����
			float	m_fStrikePrice;		//	��Ȩ�۸�
			unsigned long	m_dwMaturityDate;	//	������,YYYYMMDD
			char	m_strUnderLine[12];	//	��Ӧ��Ʊ,�����г����룬��SH600000
			float	m_fAccruedInt;		//	Ӧ����Ϣ
		} m_CnvtSpec;
	   struct		//�ڻ�,��Ʒ	(FUTURE,FTR_IDX,COMM)
		{
			unsigned long	last_day_oi;			//���ճֲ���
			float	last_settle_price;		//���ս����
		} m_futureSpec;
		 struct	//����	(TRUST)
		{
			float	m_dwfAsset;			//���ʲ�
			unsigned long	m_dwAssetDate;		//YYYYMMDD
		} m_trustSpec;
	}Spec;
}STK_STATICEx;  

typedef struct STK_HKDYNA
{
	unsigned long	m_dwIEPrice;	//ƽ���
	MWORD	m_mIEVolume;	//ƽ����

	//�����̱���
	unsigned short	m_wBuyOrderNum[5];
	unsigned short	m_wSellOrderNum[5];

	//���Ͷ���
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