#ifdef __linux
    #include <luajit-2.0/lua.hpp>
#else
    #include <lua.hpp>
#endif
#include "datacolect.h"
#include <cstring>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

const char load_file[] = "process.lua";
unsigned long count_pack = 0;
struct itimerval tick;
lua_State *lua_state = NULL;

void PrintCountInfo(int signo)
{
    switch(signo)
    {
        case SIGALRM:
            cout<<"pack count:"<<count_pack<<endl;
            count_pack= 0;
            break;
        default:
            break;
    }
    return ;
}

void DispatchToLua(unsigned char * pdcdata, int dc_type,int dc_general_intype, int stk_num, int struct_size, int did_template_id)
{
	if (DCT_STKSTATIC == dc_type || DCT_STKDYNA == dc_type || DCT_SHL2_MMPEx == dc_type )
    {
        //working_lua
        int countlua = 0;

        count_pack += 1;
        lua_getglobal(lua_state,"test_process");
        lua_pushinteger(lua_state, dc_type);
        lua_pushinteger(lua_state, stk_num);
        lua_pushlightuserdata(lua_state,pdcdata);
        //Sleep(50);
        if(lua_pcall(lua_state,3,0,0) != 0)
        {
            string s = lua_tostring(lua_state,-1);
            std::cout<<s<<endl;
            lua_pop(lua_state,-1);
            lua_close(lua_state);
        }
        else
        {
            //const char * lua_ret = lua_tostring(lua_state,-1);
            //int stkid = lua_tonumber(lua_state, -2);
            //if(NULL != lua_ret)
            {
              //cout<<"lua stkid:"<<stkid<<"  lua_ret:"<<lua_ret<<endl;
              //DispatchToMonitor(stkid, lua_ret);
            }
            lua_pop(lua_state,-1);
        }
        
    }
}
int main(void)
{
	unsigned char * pdata = (unsigned char *)malloc(2000*sizeof(struct STK_STATIC));
    memset(pdata, 0, 2000*sizeof(struct STK_STATIC));
    struct STK_STATIC stk_static;
	stk_static.m_wStkID = 1;
    memset(stk_static.m_strLabel, 0, sizeof(stk_static.m_strLabel));
    memcpy(stk_static.m_strLabel, "fdafsdf",5);
    memset(stk_static.m_strName, 0, sizeof(stk_static.m_strName));
    memcpy(stk_static.m_strName,"hhhhh",3);
    stk_static.m_cType = DCT_STKSTATIC;
    stk_static.m_nPriceDigit = '1';
    stk_static.m_nVolUnit = 321;
    stk_static.m_mFloatIssued = 134;
    stk_static.m_mTotalIssued = 321;
    stk_static.m_dwLastClose = 4324;
    stk_static.m_dwAdvStop = 432;
    stk_static.m_dwDecStop = 23423;
    struct STK_STATIC *p = (struct STK_STATIC *)pdata;	

    for(int i=0;i<2000;i++)
    {
        memcpy(p+i,(unsigned char *)&stk_static,sizeof(stk_static));
    }

   	signal(SIGALRM, PrintCountInfo);
    tick.it_value.tv_sec = 10;
    tick.it_value.tv_usec = 0;

    tick.it_interval.tv_sec = 60;
    tick.it_interval.tv_usec = 0;

    setitimer(ITIMER_REAL,&tick,NULL);
	lua_state = luaL_newstate();
    assert(NULL != lua_state);
    luaL_openlibs(lua_state);
    luaL_dofile(lua_state , load_file);

	while(true)
	{
		DispatchToLua(pdata, DCT_STKSTATIC, 0, 2000, sizeof(stk_static), 0);
	}
}
