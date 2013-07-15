#include "parse.h"
//#include "decrypt_aes.h"
//#include "decrypt_simple.h"
#include "extract_dc.h"
#include "MonitorFileMap.h"
#include "DidUncompress.h"
#include "public.h"
#include "xml_class_set.h"
#include "DIDTemplateToLuaStruct.h"
#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
    #include <lua.hpp>
#endif
#include <sys/types.h>


const int MAX_PACKET_LEN = 409600;
//BOOL Parse::DecryptDataPack(DC_HEAD *pData)
//{
//	BOOL bRet = FALSE;
//	DC_ENCY_TYPE type = (DC_ENCY_TYPE)(pData->m_wAttrib&DC_ENCY_MASK);
//	int len;
//	if(type==DC_ENCY_SIMPLE)
//	{
//	    len = Utils::UINT24to32(pData->m_nLen.m_wLow, pData->m_nLen.m_cHigh);
//	    DecryptSimple decryptsimple;
//		decryptsimple.DecryptData((BYTE*)(pData+1),len);
//		pData->m_wAttrib &= ~DC_ENCY_SIMPLE;
//		bRet = TRUE;
//	}
//	//else if(type==DC_ENCY_AES)
//	//{
// //       len = Utils::UINT24to32(pData->m_nLen.m_wLow, pData->m_nLen.m_cHigh);
// //       DecryptAES decryptaes;
//	//	decryptaes.DecryptData((BYTE*)(pData+1),len);
//	//	pData->m_wAttrib &= ~DC_ENCY_AES;
//	//	bRet = TRUE;
//	//}
//	return bRet;
//}

bool Parse::IsDCHeader(unsigned char * dc_header)
{
	DC_HEAD * dc_head = (DC_HEAD *)dc_header;
	if(DC_TAG == dc_head->m_cTag && IsDCType(dc_head->m_cType) && 0 != dc_head->m_nLen.Get())
		return true;
	else
		return false;
}

BOOL Parse::ExtractDataPack(const DC_HEAD* pOrgHead,DC_HEAD* pHeadBuf,int nBufSize,WORD* pwMarketBuf)
{
    const BYTE *pOrgData = (BYTE *)(pOrgHead + 1);
    BYTE *pDataBuf = (BYTE *)(pHeadBuf + 1);
 	int nRet = 0;
	if(pOrgHead->m_cTag==DC_TAG && !(pOrgHead->m_wAttrib&(DC_ENCY_MASK)) && (pOrgHead->m_wAttrib&DC_CPS_MASK)==DC_STD_CPS)
	{
		if(!pOrgData)
			pOrgData = (BYTE*)(pOrgHead+1);
		if(!pDataBuf)
			pDataBuf = (BYTE*)(pHeadBuf+1);

		*pHeadBuf = *pOrgHead;
		pHeadBuf->m_wAttrib &= ~DC_STD_CPS;

		nRet = extractDC_.ExtractData(pOrgHead->m_cType,pOrgData,(DWORD)pOrgHead->m_nLen.Get(),pDataBuf,nBufSize-sizeof(DC_HEAD),pStkDyna,pStkStatic);
		if(nRet>0)
		{
			pHeadBuf->m_nLen = nRet;
			nRet += sizeof(DC_HEAD);
		}
	}
	return nRet;
}

bool Parse::IsDCType(int dc_type)
{
	if(dc_type < 0 || (dc_type > 28 && dc_type <100) || dc_type > 101)
	{
		return false;
	}
	else
		return true;
}

void Parse	::CombinePacket(unsigned char *pdch, int dc_len)
{
	int temp_len = dc_len;
	unsigned char *temp_pdch = (unsigned char *)pdch;
	DC_HEAD *temp_dch_item = (DC_HEAD *)temp_pdch;
	//int imcomplete_packet_tag = 0;
	//int first_imcomplete_len = 0;
	//int last_imcomplete_len = 0;
	int packet_len = 0;

	while(temp_len > 0)
	{
		//combine dc_header and judge whether the combined dc_header is the real dc_header
		if(0 !=dc_header_last_inner_len_ )
		{
			memcpy(dc_header_ + dc_header_last_inner_len_ , temp_pdch, sizeof(DC_HEAD)-dc_header_last_inner_len_);
			if(IsDCHeader(dc_header_))
			{
				memset(combined_packet_item_.data, 0, sizeof(combined_packet_item_.data));
				memcpy(combined_packet_item_.data,dc_header_,dc_header_last_inner_len_);
				last_temp_len_ = dc_header_last_inner_len_;
				DC_HEAD * p_dc_head = (DC_HEAD *)dc_header_;
				last_pack_len_ = sizeof(DC_HEAD) + p_dc_head->m_nLen.Get() - dc_header_last_inner_len_;
			}
			else
			{
				dc_header_last_inner_len_ = 0;
			}
		}

		if(DC_TAG == temp_dch_item->m_cTag && 0 != temp_dch_item->m_nLen.Get() && IsDCType(temp_dch_item->m_cType))
		{
			//if(DCT_GENERAL == temp_dch_item->m_cType)
			//{
			//	DC_GENERAL_MY * test = (DC_GENERAL_MY *)(temp_dch_item + 1);
			//	cout<<"general totallen:"<<test->m_nTotalLen<<endl;
			//	cout<<"general num:"<<test->m_wNum<<endl;
			//	cout<<"general datasize:"<<test->m_nDataSize<<endl;
			//}
		/*	cout<<"dc_type:"<<(int)(temp_dch_item->m_cType)<<endl;*/
			packet_len = sizeof(DC_HEAD) + temp_dch_item->m_nLen.Get();
			if(packet_len > MAX_PACKET_LEN)
			{
				packet_len = 0;
			}
		}
		else if(last_pack_len_ != 0 && long_pack_tag_ == 1)
		{
			packet_len = last_pack_len_;
			if(packet_len > MAX_PACKET_LEN)
			{
				packet_len = 0;
			}
		}
		else if(last_pack_len_ != 0 && dc_header_last_inner_len_ != 0)
		{
			packet_len = last_pack_len_;
			dc_header_last_inner_len_ = 0;
		}
		else
			packet_len = 0;
		//case 1
		if(!case2_tag_ && temp_len >= packet_len && 0 != packet_len)
		{
			long_pack_tag_ = 0;
			last_pack_len_ = 0;
			last_temp_len_ = 0;
			while(temp_len >= packet_len && 0 != packet_len)
			{
				cout<<"case1:temp_len:"<<temp_len<<" packet_len:"<<packet_len<<endl;
				if( DC_TAG == temp_dch_item->m_cTag && IsDCType(temp_dch_item->m_cType))
				{
					if(temp_len == packet_len)
					{
						memset(combined_packet_item_.data,0,sizeof(combined_packet_item_.data));
						memcpy(combined_packet_item_.data,temp_pdch,packet_len);
						combined_packet_deque_.push_back(combined_packet_item_);
						temp_len = 0;
						packet_len = 0;
						break;
					}
					else //(temp_len > packet_len)
					{
						memset(combined_packet_item_.data, 0,sizeof(combined_packet_item_.data));
						memcpy(combined_packet_item_.data,temp_pdch,packet_len);
						combined_packet_deque_.push_back(combined_packet_item_);
						temp_len -= packet_len;
						temp_pdch += packet_len;
						temp_dch_item = (DC_HEAD *)temp_pdch;

						if(temp_len > 0 && temp_len < sizeof(DC_HEAD))
						{
							memset(dc_header_, 0 ,sizeof(dc_header_));
							memcpy(dc_header_,temp_pdch,temp_len);
							dc_header_last_inner_len_ += temp_len;
							packet_len = 0;
							break;
						}

						if(DC_TAG == temp_dch_item->m_cTag && 0 != temp_dch_item->m_nLen.Get() && IsDCType(temp_dch_item->m_cType))
						{
							last_temp_len_ = 0;
							last_pack_len_ = 0;
							packet_len = sizeof(DC_HEAD) + temp_dch_item->m_nLen.Get();
						}
						else
						{
							packet_len = 0;
							temp_len = 0;
							break;
						}
					}
				}
				else
				{
					packet_len = 0;
					temp_len = 0;
					break;
				}
			}
		}
	    if(0 != packet_len)
		{
			//case 2
			cout<<"case2 templen:"<<temp_len<<" packet_len:"<<packet_len<<endl<<flush;
			if(0 != packet_len && packet_len > temp_len && DC_TAG == temp_dch_item->m_cTag && last_pack_len_ == 0 && IsDCType(temp_dch_item->m_cType))
			{
				case2_tag_ = true;
				long_pack_tag_ = 1;
				memset(combined_packet_item_.data,0,sizeof(combined_packet_item_.data));
				memcpy(combined_packet_item_.data,temp_pdch,temp_len);
				last_pack_len_ = packet_len - temp_len;
				last_temp_len_ += temp_len;
				temp_len = 0;
				break;
			}
			if(long_pack_tag_ == 1 && last_pack_len_ > temp_len)
			{
				memcpy(combined_packet_item_.data+last_temp_len_,temp_pdch,temp_len);
				last_pack_len_ -= temp_len;
				last_temp_len_ += temp_len;
				temp_len = 0;
			}
			else if(long_pack_tag_ ==1 && last_pack_len_<= temp_len )
			{
				memcpy(combined_packet_item_.data + last_temp_len_,temp_pdch,last_pack_len_);
				combined_packet_deque_.push_back(combined_packet_item_);
				temp_pdch += last_pack_len_;
				temp_len -= last_pack_len_;
				temp_dch_item = (DC_HEAD *)temp_pdch;
				
				if(temp_len > 0 && temp_len < sizeof(DC_HEAD))
				{
					memset(dc_header_, 0 ,sizeof(dc_header_));
					memcpy(dc_header_,temp_pdch,temp_len);
					dc_header_last_inner_len_ += temp_len;
					temp_len = 0;
					packet_len = 0;
					last_pack_len_ = 0;
					last_temp_len_ = 0;
					long_pack_tag_ = 0;
					case2_tag_ = 0;
					break;
				}
				
				//if(DC_TAG == temp_dch_item->m_cTag && 0 != temp_dch_item->m_nLen.Get() && IsDCType(temp_dch_item->m_cType))
				//{
				//	packet_len = sizeof(DC_HEAD) + temp_dch_item->m_nLen.Get();
				//}
				//else
				//{
				//	packet_len = 0;
				//	temp_len = 0;
				//}
				last_pack_len_ = 0;
				last_temp_len_ = 0;
				long_pack_tag_ = 0;						
				//imcomplete_packet_tag =0;
				//first_imcomplete_len = 0;
				//last_imcomplete_len = 0;
				case2_tag_ = false;
			}
			else
			{
				//cout<<"-----------"<<endl;
				temp_len = 0;
				packet_len = 0;
				last_pack_len_ = 0;
				last_temp_len_ = 0;
				long_pack_tag_ = 0;
				case2_tag_ = 0;
			}
		}
		else
		{
			temp_len = 0;
			break;
		}
	}
}

void Parse::ExtractPacket(lua_State *L, struct pcap_pkthdr *header, unsigned char *pkt_data,zmq::socket_t * sock, int port_tag)
{
	int extract_ret = 0;
	static int did_sync_module_tag = 0;
	int did_template_id = 0;
	short stknum = 0;
	int struct_size = 0;
	unsigned char * pdcdata = NULL;
	
	char *iproto = NULL;
	unsigned short sport,dport;
	char *netflags = NULL;
	ip_head *ih = NULL;
	int iph_len = 0;
	int tcph_len = 0;
	udp_head *udph = NULL;
	tcp_head *tcph = NULL;
	DC_HEAD *pdch = NULL;
	struct tm *ltime = NULL;
	time_t local_tv_sec;
	bufelement info;

	while(!combined_packet_deque_.empty())
	{
		DataBuffer data_buf;
		CombinedPacketItem &packet_item = combined_packet_deque_.front();
		DC_HEAD * pdch = (DC_HEAD *)packet_item.data;
		cout<<"dctype:"<<(int)(pdch->m_cType)<<endl;
		if(DC_STD_CPS == (pdch->m_wAttrib & DC_CPS_MASK))
		{
			if(NULL == extractbuf)
			{
				extractbuf = new char[EXTRACT_BUF];
			}
			memset(extractbuf,0,EXTRACT_BUF);
			if((extract_ret = (ExtractDataPack(pdch,(DC_HEAD *)extractbuf,EXTRACT_BUF)))>0)
			{
				pdch = (DC_HEAD *)extractbuf;
			}
			else if(extract_ret == 0)
			{
				cout<<"the packet isn't extracted!"<<endl;
			}
			else
			{
				cout<<"extract error! error num is :"<<extract_ret<<endl;
				return ;
			}
		}
		
        if(DCT_STKSTATIC == pdch->m_cType)
        {
			//initial the did related files
			extractDC_.set_static_before_dyna_tag(true);	
			if(1 == did_sync_module_tag)
			{
				DIDTemplateToLuaStruct did_to_lua;
				did_to_lua.Transform();
				did_sync_module_tag = 0;
			}
            DC_STKSTATIC_MY* p = (DC_STKSTATIC_MY*)(pdch+1);
			stknum = p->m_nNum;
			struct_size = sizeof(STK_STATIC);
            pdcdata = (unsigned char  *)(p+1);
            nHqTotal = p->m_nNum;
            if(NULL == pStkDyna)
            {
                pStkDyna = new STK_DYNA[nHqTotal];
            }
            else
            {
                delete [] pStkDyna;
                pStkDyna = new STK_DYNA[nHqTotal];
            }
            if(NULL == pStkMMPEx)
            {
                pStkMMPEx = new SH_L2_MMPEX[nHqTotal];
            }
            else
            {
                delete [] pStkMMPEx;
                pStkMMPEx = new SH_L2_MMPEX[nHqTotal];
            }

            if(NULL == pStkStatic)
            {
                pStkStatic = new STK_STATIC[nHqTotal];
            }
            else
            {
                delete pStkStatic;
                pStkStatic = new STK_STATIC[nHqTotal];
            }

            if(pStkStatic && pStkDyna && pStkMMPEx)
            {
                STK_STATIC *pdata = (STK_STATIC *)(p+1);
                memmove(pStkStatic, pdata, nHqTotal*sizeof(STK_STATIC));
                memset(pStkDyna,   0, sizeof(STK_DYNA)*nHqTotal);
                memset(pStkMMPEx,  0, sizeof(SH_L2_MMPEX)*nHqTotal);
            }
        }
        else if(DCT_STKDYNA == pdch->m_cType)
        {
            DC_STKDYNA_MY* p = (DC_STKDYNA_MY*)(pdch+1);
			stknum = p->m_nNum;
			struct_size = sizeof(STK_DYNA);
            pdcdata = (unsigned char  *)(p+1);
            STK_DYNA* pDyna = (STK_DYNA*)(p+1);
            STK_DYNA* pData = NULL;
            for (int i=0; i<p->m_nNum; i++)
            {
                if (((pDyna+i)->m_wStkID < nHqTotal) )
                {
                    pData = pStkDyna + (pDyna+i)->m_wStkID;
                    memmove(pData, pDyna+i, sizeof(STK_DYNA));
                }
            }
        }
        else if(DCT_SHL2_MMPEx == pdch->m_cType)
        {
            
        }
		else if(DCT_DSDATA == pdch->m_cType)
		{
			did_sync_module_tag = 1;
		}
		else if(DCT_DID == pdch->m_cType)
		{
			string did_config_file = XML_DID::get_did_config_path();
			DidUncompress diducp(did_config_file);
			diducp.ReadConfig();
			diducp.Initialize();
			if(diducp.DisassemblePack(pdch,data_buf))
			{
				cout<<"uncompress success!"<<endl;
				DC_DIDHead *did_head = (DC_DIDHead *)(pdch+1);
				did_template_id = did_head->GetDid();
				stknum = did_head->GetRecNum();
				struct_size = data_buf.GetLen()/stknum;
				pdcdata = (unsigned char *)(data_buf.GetData());
							
				//cout<<"did num?:"<<did_head->GetDid()<<endl;//did num
				//cout<<"did template struct size?:"<<struct_size<<endl;//did_head length
				//cout<<"did stknum?"<<did_head->GetRecNum()<<endl;//stknum
			}
			else
			{
				cout<<"uncompress fail"<<endl;
			}
		}
		else if(DCT_DIDSTATIC == pdch->m_cType)
		{

		}
		
		DispatchToLua(L, pdcdata, pdch->m_cType, stknum, struct_size, did_template_id);
		
		ih = (ip_head *)(pkt_data + 14); //14 bytes is the length of ethernet header
        iph_len = (ih->ver_ihl & 0xf) * 4;//20bytes

		tcph = (tcp_head *) ((unsigned char*)ih + iph_len);
		tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
        iproto = "TCP";
        /* convert from network byte order to host byte order */
        sport = ntohs( tcph->source );
        dport = ntohs( tcph->dest );
        netflags = Utils::tcp_flag_to_str(tcph->flags);
		/* convert the timestamp to readable format */
		local_tv_sec = header->ts.tv_sec;
		ltime=localtime(&local_tv_sec);
		memset(info.timestamp, 0, sizeof(info.timestamp));
		strftime(info.timestamp, sizeof(info.timestamp), "%H:%M:%S", ltime);

		info.port_tag = port_tag;
		info.len = ih->tlen - iph_len - tcph_len;
		info.iproto = iproto;
		info.saddrbyte1 = ih->saddr.byte1;
		info.saddrbyte2 = ih->saddr.byte2;
		info.saddrbyte3 = ih->saddr.byte3;
		info.saddrbyte4 = ih->saddr.byte4;
		info.sport = sport;
		info.daddrbyte1 = ih->daddr.byte1;
		info.daddrbyte2 = ih->daddr.byte2;
		info.daddrbyte3 = ih->daddr.byte3;
		info.daddrbyte4 = ih->daddr.byte4;
		info.dport = dport;
		info.flags = netflags;
                           
		info.dctype = Utils::DCTypeToString(pdch->m_cType);
		//info.seqtag = seqtag;
		DispatchToLog(sock, info);
                            
		combined_packet_deque_.pop_front();
//                                try
//                                {
//                                    zmq::message_t msg_workinglua_send(sizeof(pdch));
//                                    memcpy((void *)msg_workinglua_send.data(),(char *)&stock_dyna,sizeof(stock_dyna));
//                                    socket_workinglua_send.send(msg_workinglua_send,ZMQ_NOBLOCK);
//                                }
//                                catch(zmq::error_t error)
//                                {
//                                    cout<<"zmq send error"<<error.what()<<endl;
//                                }

                //test parse flow rate
//                          if(countnum_parse == 0)
//                          {
//                              timebase_parse = time((time_t)NULL);
//                              cout<<"parse begin counting"<<endl;
//                          }
//                          long int timelive_parse = time((time_t *)NULL);
//                          if(timelive_parse - timebase_parse > 120 && timetag_parse== 0)
//                          {
//                              timetag_parse = 1;
//                              //cout<<"parse filtertag:"<<filtertag<<" !!! 2min packet quantity:"<<countnum_parse<<endl<<flush;
//                          }
//                          else
//                          {
//                              countnum_parse = countnum_parse + 1;
					///*cout<<countnum_parse<<endl;*/
//                          }
    }
}

void Parse::DispatchToLua(lua_State * L, unsigned char * pdcdata, int dc_type, int stk_num, int struct_size, int did_template_id)
{
	//did
	if(DCT_DID == dc_type)
	{
		for(int i=0;i<stk_num;i++)
		{
			//cout<<i<<endl;
			lua_getglobal(L,"process_did");
			lua_pushinteger(L, did_template_id);
			lua_pushlightuserdata(L,pdcdata+i*struct_size);
			if(lua_pcall(L,2,1,0) != 0)
			{
				cout<<lua_tostring(L,-1)<<endl;
				lua_pop(L,-1);
				lua_close(L);
			}
			else
			{
		/*		cout<<lua_tostring(L,-1)<<endl;*/
				lua_pop(L,-1);
			}
		}
	}
	//old
	if (DCT_STKSTATIC == dc_type || DCT_STKDYNA == dc_type)
	{
		//working_lua
		int countlua = 0;
		for(int i=0;i<stk_num;i++)
		{
			lua_getglobal(L,"process");
			lua_pushinteger(L, dc_type);
			lua_pushlightuserdata(L,pdcdata+struct_size * i);
			//Sleep(50);
			if(lua_pcall(L,2,2,0) != 0)
			{
				string s = lua_tostring(L,-1);
				std::cout<<s<<endl;
				lua_pop(L,-1);
				lua_close(L);
			}
			else
			{
				string lua_ret = lua_tostring(L,-1);
				int stkid = lua_tonumber(L, -2);
				//cout<<"lua stkid:"<<stkid<<endl;
				//cout<<"lua_ret:"<<lua_ret<<endl;
				DispatchToMonitor(stkid, lua_ret);
				lua_pop(L,-1);
			}
		}
	}
}

void Parse::DispatchToMonitor(int stk_id, std::string & value)
{
	STK_STATIC * pstkstaticitem = const_cast<STK_STATIC *>(extractDC_.GetStaticByID(pStkStatic,stk_id));
	MonitorMsg *monitor_msg  = (MonitorMsg *)file_->GetMapMsg();
	time_t t;
	t = time(&t);
	dzh_time_t current_time(t);
	monitor_msg->time = current_time;
	strcpy(monitor_msg->error_type,"BUSINESS");
	strcpy(monitor_msg->error_level,"WARNING");
	strcpy( monitor_msg->stock_label, pstkstaticitem->m_strLabel );
	strcpy( monitor_msg->error_info, value.c_str() );
}

void Parse::DispatchToLog(zmq::socket_t *sock, bufelement &info)
{
    try
    {
        zmq::message_t msg_send(sizeof(info));
        memcpy((void *)msg_send.data(),(char *)&info,sizeof(info));
        sock->send(msg_send,ZMQ_NOBLOCK);
		/*sock->send(msg_send);*/
    }
    catch(zmq::error_t error)
    {
        cout<<"zmq send error"<<error.what()<<endl;
    }
}

void *Parse::RunThreadFunc()
{
	pthread_detach(pthread_self());
    zmq::context_t * context = context_;
    zmq::socket_t socket_parse_rcv (*context, this->zmqitems_[0].zmqpattern);
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        socket_parse_rcv.bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        socket_parse_rcv.connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    socket_parse_rcv.setsockopt(ZMQ_SUBSCRIBE,"",0);

    zmq::socket_t socket_parse_send(*context, this->zmqitems_[1].zmqpattern);
    if("bind" == this->zmqitems_[1].zmqsocketaction)
    {
        socket_parse_send.bind(this->zmqitems_[1].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[1].zmqsocketaction)
    {
        socket_parse_send.connect(this->zmqitems_[1].zmqsocketaddr.c_str());
    }

    //zmq::socket_t socket_workinglua_send(*context, this->zmqitems_[2].zmqpattern);
    //if("bind" == this->zmqitems_[2].zmqsocketaction)
    //{
    //    socket_workinglua_send.bind(this->zmqitems_[2].zmqsocketaddr.c_str());
    //}
    //else if("connect" == this->zmqitems_[2].zmqsocketaction)
    //{
    //    socket_workinglua_send.connect(this->zmqitems_[2].zmqsocketaddr.c_str());
    //}

    zmq::pollitem_t items[] = {socket_parse_rcv, 0, ZMQ_POLLIN, 0};
 
    pcap_work_item *pw_item_ptr;
	int port_tag;
	//long long seqtag;
	struct pcap_pkthdr *header = NULL;
	unsigned char *pkt_data = NULL;
    unsigned int countnum = 0;
    long int timebase = 0;
    int timetag = 0;
	DC_HEAD *pdch;
    //static int tcpconntag = 0;
    //static int tagscount = 0;

	
    unsigned int countnum_parse = 0;
    long int timebase_parse = 0;
    long int timelive_parse = 0;
    int timetag_parse = 0;

	char * pfinalpacket = NULL;
	int dc_len = 0;
	ip_head *ih = NULL;
	tcp_head *tcph = NULL;
	int head_len = 0;
	int iph_len = 0;
	int tcph_len = 0;

	zmq::message_t msg_rcv(sizeof(pcap_work_item));
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);
	luaL_dofile(L, "process.lua");

	
	//pthread_t tid;
	//tid =pthread_self();
	//Utils::Print_Thread_ID(tid);
    while(true)
    {
        int rc = zmq_poll(items, 1, 10000);//timeout = 1s

        //int rc = zmq::poll(items, 1, -1);//block
		if(rc > 0)
		{
            if(items[0].revents & ZMQ_POLLIN)
            {
				//pthread_t tid;
				//tid =pthread_self();
				//Utils::Print_Thread_ID(tid);
              
				//if(count_rc_ == 1)
				/*cout<<"count:"<<count_rc_++<<endl;*/
				memset((void*)(msg_rcv.data()),0,sizeof(pcap_work_item));
                socket_parse_rcv.recv(&msg_rcv,ZMQ_NOBLOCK);
				//socket_parse_rcv.recv(&msg_rcv);
                pw_item_ptr = static_cast<pcap_work_item*>(msg_rcv.data());

				port_tag = pw_item_ptr->port_tag;
				//seqtag = pw_item_ptr->seqtag;
				header = &(pw_item_ptr->header);
				pkt_data = pw_item_ptr->data;
					
				ih = (ip_head *)(pkt_data + 14); //14 bytes is the length of ethernet header	
				iph_len = (ih->ver_ihl & 0xf) * 4;//20bytes
				tcph = (tcp_head *) ((unsigned char*)ih + iph_len);
				tcph_len = 4*((tcph->dataoffset)>>4&0x0f);
				head_len = iph_len + tcph_len;
				switch(ih->protocol)
				{
				case TCP:
					pdch = (DC_HEAD*)((u_char*)pkt_data +14 + head_len);//54= 14+20+20 ethernet head:14bytes, ip head:20bytes, tcp head:20bytes
					dc_len = ntohs(ih->tlen) - head_len;
					//cout<<"dc_len:"<<dc_len<<"  tcp seq(net char seq):"<<tcph->seq<<endl<<flush;

					if(dc_len > 0)
					{
						if(last_tcp_seq_ != tcph->seq)
						{
							//cout<<"dc_len:"<<dc_len<<"  tcp seq(net char seq):"<<ntohl(tcph->seq)<<endl<<flush;
							cout<<"dc_len"<<dc_len<<endl<<flush;
							last_tcp_seq_ = tcph->seq;
							CombinePacket((unsigned char *)pdch,dc_len);
							ExtractPacket(L, header,pkt_data,&socket_parse_send,port_tag);
						}
					}
					break;
				case UDP:
					break;
				default:
					break;
				}
			}
        }
        else if (rc ==0 )//timeout
        {
            continue;
        }
        else
        {
            cout<<"zmq poll fail"<<endl;
			throw zmq_errno();
		}
    }
}


		//}
  //      catch(zmq::error_t error)
  //      {
  //          cout<<"zmq recv error:"<<error.what()<<endl;
		//	pw_item_ptr = NULL;
  //          continue;
  //      }