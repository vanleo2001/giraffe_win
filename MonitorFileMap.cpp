#include "stdafx.h"
#include "MonitorFileMap.h"

MonitorFileMap::MonitorFileMap(string  MapName, Type  type)
{

    if(type == BUILD)
    {
        HANDLE  hFile = NULL;
	    hFile = CreateFile(MapName.c_str(), GENERIC_READ|GENERIC_WRITE,
           FILE_SHARE_READ|FILE_SHARE_WRITE,
           NULL, CREATE_ALWAYS,
           FILE_FLAG_OVERLAPPED|FILE_FLAG_SEQUENTIAL_SCAN,
           NULL);
        HANDLE  hFileMap = CreateFileMapping(hFile, NULL,
           PAGE_READWRITE,0, 0x4000000, MapName.c_str());
        __int64 qwFileOffset = 0;

	    mapMsg = MapViewOfFile(hFileMap, FILE_MAP_ALL_ACCESS,
           (DWORD)(qwFileOffset>>32), (DWORD)(qwFileOffset&0xFFFFFFFF)
		   ,  sizeof(MonitorMsg));
        memset(mapMsg,0, sizeof(MonitorMsg));
		/*((MonitorHead*)(mapHead))->m_ClientIpBuffCount = IPBUFCOUNT;
		((MonitorHead*)(mapHead))->m_nExcepBuffCount = EXCECOUNT;*/
//		LOG(1)("Exception cout %d and Client count %d\n",
 //          ((MonitorHead*)mapHead)->m_nException,
  //         ((MonitorHead*)mapHead)->m_ClientIPCount);
    }
	else if(type == OPEN)
	{
        __int64 qwFileOffset = 0;
        HANDLE  hMapFile = OpenFileMapping(FILE_MAP_READ, false, MapName.c_str());

        mapMsg = MapViewOfFile(hMapFile, FILE_MAP_READ,
           (DWORD)(qwFileOffset>>32), (DWORD)(qwFileOffset&0xFFFFFFFF),
            sizeof(MonitorMsg));
	}
    myMutex = CreateMutex(NULL, false, "FX_191111_Mutex");
}
MonitorFileMap::~MonitorFileMap()
{
	UnmapViewOfFile(mapMsg);
    CloseHandle(myMutex);
}
MonitorMsg*  MonitorFileMap::GetMapMsg()
{
    return (MonitorMsg*)mapMsg;
}
//MonitorDetail*  MonitorFileMap::GetMapDetail()
//{
//return (MonitorDetail*)(((MonitorHead*)(mapHead))+1);
//}
//void  MonitorFileMap::DeleteClientIP(char*  ip)
//{
//	int nClient = GetMapHead()->m_ClientIPCount;
//	char*  pClientBase = GetMapHead()->ClientIP;
//    int Count = 1;
//    DWORD dwWaitResult;
//    bool  bFound = false;
////    LOG(1)("Client cout is %d\n", nClient);
//    for(int i =0; i < nClient; i++)
//	{
//        if(!strncmp((pClientBase+i*32), ip, strlen(ip)))
//		{
//            bFound = true;
//            break;
//		}
//        Count++;
//	}
//    if(bFound)
//	{
//        dwWaitResult = WaitForSingleObject(
//              myMutex,         // 句柄
//              INFINITE);      // 无限等待
//        for(int j = Count + 1; j < nClient; j++)
//	    {
//            memcpy((pClientBase+(j-1)*32),(pClientBase+j*32), 32);
//	    }
//        memset((pClientBase+(GetMapHead()->m_ClientIPCount - 1)*32),0,32);
//        GetMapHead()->m_ClientIPCount--;
//        ReleaseMutex(myMutex);
//	}
//}
//bool  MonitorFileMap::IsException(string stockName, int& index)
//{
//    bool bRet = false;
//    int i;
//    index = 0;
////    LOG(1)("Exception count %d\n", GetMapHead()->m_nException);
//    if(GetMapMsg()->m_nException > 0)
//	{
//	    for(i = 0; i < GetMapHead()->m_nException; i++)
//	    {
//		    if(!strcmp((GetMapDetail()+i)->StockName, stockName.c_str()))
//		    {
//			    if((GetMapDetail()+i)->bidCombineFlag || (GetMapDetail()+i)->priExFlag
//				    || (GetMapDetail()+i)->priFreExFlag ||
//					(GetMapDetail()+i)->m_bBreak)
//			    {
//                    bRet = true;
//                    index = i;
//			    }
//                break;
//		    }
//	    }
//        if(!bRet && i != GetMapHead()->m_nException)
//	    {
//           DWORD dwWaitResult;
//           dwWaitResult = WaitForSingleObject(
//              myMutex,         // 句柄
//              INFINITE);      // 无限等待
//           if(i == GetMapHead()->m_nException - 1)
//		   {
//              memset((GetMapDetail() + i), 0, sizeof(MonitorDetail));
//		   }
//		   else
//		   {
//              for(int j = i+1 ; j < GetMapHead()->m_nException; j++)
//		      {
//                 memcpy((GetMapDetail()+j - 1),(GetMapDetail()+j), sizeof(MonitorDetail));
//		      }
//		   }
//           GetMapHead()->m_nException--;
//           ReleaseMutex(myMutex);
//	     }
//	}
////    LOG(1)("Exit IsException function");
//    return bRet;
//}
LPVOID  MonitorFileMap::GetMapPointer()
{
    return mapMsg;
}
