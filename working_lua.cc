#include "working_lua.h"

void Working_Lua::Init(zmq::context_t *context)
{
    context_ = context;
}

void Working_Lua::RegisterToLua(lua_State *L)
{
    lua_newtable(L);
    int itable;
    itable = lua_gettop(L);
    lua_pushliteral(L, "stkid");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"time");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"open");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"high");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"low");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"new");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"volume");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"amount");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"innervol");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"tickcount");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"buyprice");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"buyvol");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"sellprice");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"sellvol");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"openinterest");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    lua_pushliteral(L,"settleprice");
    lua_pushnumber(L, 0);
    lua_rawset(L,itable);

    luaL_newmetatable(L, "stk_dyna");
    int imt;
    imt = lua_gettop(L);
    lua_pushliteral(L,"__index");
    lua_pushvalue(L,itable);
    lua_rawset(L,imt);

    lua_newtable(L);
    int iA;
    iA = lua_gettop(L);
    lua_pushvalue(L,itable);
    lua_setmetatable(L, iA);
    lua_setglobal(L,"stk_dyna");
}

void *Working_Lua::RunThreadFunc()
{
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
//    RegisterToLua(L);
    luaL_dofile(L, "process.jit");

    zmq::context_t *context = context_;
    zmq::socket_t sock_lua (*context, this->zmqitems_[0].zmqpattern);

    if("bind" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_lua.bind(this->zmqitems_[0].zmqsocketaddr.c_str());
    }
    else if("connect" == this->zmqitems_[0].zmqsocketaction)
    {
        sock_lua.connect(this->zmqitems_[0].zmqsocketaddr.c_str());
    }

    zmq::pollitem_t items[] = {sock_lua, 0, ZMQ_POLLIN,0};
    zmq::message_t msg_rcv(sizeof(STK_DYNA)+1);
    STK_DYNA *stkd;
    int countlua = 0;
    long int timebase;
    long int timelive;
    while(true)
    {
        //wait for recv
        try
        {
            int rc = zmq::poll(items, 1, 1000);//timeout = 1s
            if(rc > 0)
            {
                if(items[0].revents & ZMQ_POLLIN)
                {
                    msg_rcv.rebuild();
                    sock_lua.recv(&msg_rcv,ZMQ_NOBLOCK);
                    stkd = static_cast<STK_DYNA *>(msg_rcv.data());

                    lua_getglobal(L,"process");
//                    int itest = lua_gettop(L);
                    lua_pushlightuserdata(L,stkd);

//                    int index = lua_gettop(L);
//                    lua_getfield(L,LUA_REGISTRYINDEX,"stk_dyna");
//
//                    lua_pushliteral(L,"__index");
//                    lua_gettable(L,-2);
//
//                    lua_pushliteral(L,"stkid");
//                    lua_pushnumber(L,stkd->stkid);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"time");
//                    lua_pushnumber(L,stkd->time);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"open");
//                    lua_pushnumber(L,stkd->open);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"high");
//                    lua_pushnumber(L,stkd->high);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"low");
//                    lua_pushnumber(L,stkd->low);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"new");
//                    lua_pushnumber(L,stkd->new_);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"volume");
//                    lua_pushnumber(L,stkd->volume);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"innervol");
//                    lua_pushnumber(L,stkd->innervol);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"tickcount");
//                    lua_pushnumber(L,stkd->tickcount);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"buyprice");
//                    lua_pushnumber(L,stkd->buyprice[0]);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"buyvol");
//                    lua_pushnumber(L,stkd->buyvol[0]);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"sellprice");
//                    lua_pushnumber(L,stkd->sellprice[0]);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"sellvol");
//                    lua_pushnumber(L,stkd->sellvol[0]);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"openinterest");
//                    lua_pushnumber(L,stkd->openinterest);
//                    lua_settable(L,-3);
//
//                    lua_pushliteral(L,"settleprice");
//                    lua_pushnumber(L,stkd->settleprice);
//                    lua_settable(L,-3);
//
//                    lua_pop(L,1);
//                    lua_setmetatable(L, index);

                    if(lua_pcall(L,1,1,0) != 0)
                    {
                        string s = lua_tostring(L,-1);
                        std::cout<<s<<endl;
                        lua_pop(L,-1);
                        lua_close(L);
                    }
                    else
                    {

                        /*if(countlua ==0)
                        {
                            timeval tv;
                            gettimeofday(&tv,NULL);
                            timebase = tv.tv_sec*1000000 +tv.tv_usec;
                        }
                        if(++countlua == 100000)
                        {
                            timeval tv;
                            gettimeofday(&tv,NULL);

                            timelive = tv.tv_sec*1000000 + tv.tv_usec;
                            cout<<"100000 packet time:"<<timelive -timebase<<endl;

                        }*/
//                        std::cout<<lua_tostring(L,-1)<<endl;
                        lua_pop(L,-1);
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
            }
        }
        catch(zmq::error_t error)
        {
            cout<<"zmq recv error:"<<error.what()<<endl;
            continue;
        }
    }
}

