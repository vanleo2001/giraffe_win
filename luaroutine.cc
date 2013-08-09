#include "luaroutine.h"

const char load_file[] = "process.lua";

void LuaRoutine::Init()
{
	InitZMQ();
	InitLua();
}

void LuaRoutine::InitLua()
{
	lua_state_ = luaL_newstate();
	assert(NULL != lua_state_);
	luaL_openlibs(lua_state_);
	luaL_dofile(lua_state_ , load_file);
	vector<std::string> & did_template_ids = listening_item_.get_did_template_ids();
	for(vector<std::string>::iterator iter = did_template_ids.begin();iter != did_template_ids.end();iter++)
	{
		lua_getglobal(lua_state_, "init");
		lua_pushstring(lua_state_, (*iter).c_str());
		
		if(lua_pcall(lua_state_,1,0,0) != 0)
		{
			string s = lua_tostring(lua_state_,-1);
			std::cout<<s<<endl;
			lua_pop(lua_state_,-1);
			lua_close(lua_state_);
		}
		else
		{
			lua_pop(lua_state_, -1);
		}
	}
}

void LuaRoutine::InitZMQ()
{
	assert(-1 != this->zmqitems_[0].zmqpattern);
	sock_ = new zmq::socket_t(*context_,this->zmqitems_[0].zmqpattern);
    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_->connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
}

struct STK_STATIC * LuaRoutine::GetStkByID(int stk_id)
{
	assert(NULL != stk_static_);
	return stk_static_ + stk_id;	
}

void LuaRoutine::DispatchToMonitor(int stk_id, const char *value)
{
	assert(NULL != value);
	STK_STATIC * pstkstaticitem = GetStkByID(stk_id);
	MonitorMsg *monitor_msg  = (MonitorMsg *)(monitor_mapping_file_->GetMapMsg());
	time_t t;
	t = time(&t);
	dzh_time_t current_time(t);
	monitor_msg->time = current_time;
	strcpy(monitor_msg->error_type,"BUSINESS");
	strcpy(monitor_msg->error_level,"WARNING");
	strcpy(monitor_msg->stock_label, pstkstaticitem->m_strLabel);
	strcpy(monitor_msg->error_info, value);
}

void LuaRoutine::DispatchToLua(unsigned char * pdcdata, int dc_type,int dc_general_intype, int stk_num, int struct_size, int did_template_id)
{
	assert(NULL != pdcdata);
	//did
	if(DCT_DID == dc_type)
	{
		for(int i=0;i<stk_num;i++)
		{
			//cout<<i<<endl;
			lua_getglobal(lua_state_,"process_did");
			lua_pushinteger(lua_state_,listening_item_.get_port());
			lua_pushinteger(lua_state_, did_template_id);
			lua_pushlightuserdata(lua_state_, pdcdata+i*struct_size);
			if(lua_pcall(lua_state_,3,1,0) != 0)
			{
				//cout<<lua_tostring(lua_state_,-1)<<endl;
				lua_pop(lua_state_,-1);
				lua_close(lua_state_);
			}
			else
			{
				//cout<<"did:"<<lua_tostring(lua_state_,-1)<<endl;
				lua_pop(lua_state_,-1);
			}
		}
	}
	//static or dyna
	if (DCT_STKSTATIC == dc_type || DCT_STKDYNA == dc_type)
	{
		//working_lua
		int countlua = 0;
		for(int i=0;i<stk_num;i++)
		{
			lua_getglobal(lua_state_,"process");
			lua_pushinteger(lua_state_, dc_type);
			lua_pushlightuserdata(lua_state_,pdcdata+struct_size * i);
			//Sleep(50);
			if(lua_pcall(lua_state_,2,2,0) != 0)
			{
				string s = lua_tostring(lua_state_,-1);
				std::cout<<s<<endl;
				lua_pop(lua_state_,-1);
				lua_close(lua_state_);
			}
			else
			{
				const char * lua_ret = lua_tostring(lua_state_,-1);
				int stkid = lua_tonumber(lua_state_, -2);
				if(NULL != lua_ret)
				{
					//cout<<"lua stkid:"<<stkid<<"  lua_ret:"<<lua_ret<<endl;
					DispatchToMonitor(stkid, lua_ret);
				}
				lua_pop(lua_state_,-1);
			}
		}
	}
	if(DCT_GENERAL == dc_type)
	{
		unsigned short *pstk_id = (unsigned short *)pdcdata;
		unsigned char *pdata = pdcdata + stk_num *sizeof(WORD);
		for(int i=0;i<stk_num;i++)
		{
			lua_getglobal(lua_state_,"process_general");
			lua_pushinteger(lua_state_,dc_general_intype);
			lua_pushlightuserdata(lua_state_,pdata + struct_size * i);
			
			if(lua_pcall(lua_state_,2,1,0) != 0)
			{
				string s = lua_tostring(lua_state_,-1);
				std::cout<<s<<endl;
				lua_pop(lua_state_,-1);
				lua_close(lua_state_);
			}
			else
			{
				const char * lua_ret = lua_tostring(lua_state_,-1);
				if(NULL != lua_ret)
				{
					//cout<<"general stkid:"<<*(pstk_id+i)<<" lua_ret:"<<lua_ret<<endl;
					DispatchToMonitor(*(pstk_id+i),lua_ret);
				}
				lua_pop(lua_state_, -1);
			}
		}
	}
	free(pdcdata);
	pdcdata = NULL;
}

void * LuaRoutine::RunThreadFunc()
{
	zmq::message_t msg_rcv(sizeof(Lua_ZMQ_MSG_Item));
	while(true)
	{
		msg_rcv.rebuild();
		sock_->recv(&msg_rcv);
		Lua_ZMQ_MSG_Item *msg_item = (Lua_ZMQ_MSG_Item*)(msg_rcv.data());
		stk_static_ = msg_item->stk_static;
		DispatchToLua(msg_item->pdcdata, msg_item->dc_type, msg_item->dc_general_intype, msg_item->stk_num, msg_item->struct_size, msg_item->did_template_id);
	}
}