#pragma once
#include "dscomapi.h"
#include "MSXML_DOMDocument.h"
#include <map>
#include <hash_map>

using namespace std;

typedef hash_map<unsigned int, int> STKIDMAP;

struct TStructForGetBmData 
{
	STKIDMAP* pStkIDMapHash;			//stkid hash映射关系
	char* pFullData;					//全量数据内存地址
	unsigned int iBufferSize;			//全量数据内存大小
	unsigned int iTotalStk;    
	TStructForGetBmData()
	{
		memset(	this, 
				0x0, 
				sizeof(TStructForGetBmData));
	}
	TStructForGetBmData(const TStructForGetBmData& obj)
	{
		memcpy(	this, 
				&obj, 
				sizeof(TStructForGetBmData));
	}
};

class DataBuffer
{
public:
	DataBuffer();
	~DataBuffer();
	int Allocate(int lt);
	void Flush();
	char* GetData();
	int GetLen();
private:
	char* data;
	int len;
};

class DidUncompress
{
public:
	struct TDidInfo 
	{
		int did;
		string strFilePath;				//did 数据描述模板文件路径
		bool bCompress;					//是否向服务端请求压缩后的did 数据
		bool bFull;						//是否想服务端请求完整的did 数据，为false 表示是增量的数据
		bool bServerSupport;			//记录服务端是否支持该did
		bool bNeedSynaDidStructFile;	//是否需要服务端同步数据描述模板文件
		CDidStructApi* pDidStruct;		//数据描述模板
		CDsCompressApi* pUnCompress;	//解压缩对象
		char* pUnCompressBuffer;		//解压缩缓冲区
		int iBufferLen;					//缓冲区大小
		bool bInitialized;				//初始化成功

		TDidInfo(){
			did = 0;
			strFilePath = "";
			bCompress = true;
			bFull = true;
			bServerSupport = false;
			bNeedSynaDidStructFile = true;
			pDidStruct = NULL;
			pUnCompress = NULL;
			pUnCompressBuffer = NULL;
			iBufferLen = 0;
			bInitialized = false;
		}

		TDidInfo(const TDidInfo& obj)
		{
			did = obj.did;
			strFilePath = obj.strFilePath;
			bCompress = obj.bCompress;
			bFull = obj.bFull;
			bServerSupport = obj.bServerSupport;
			bNeedSynaDidStructFile = obj.bNeedSynaDidStructFile;
			pDidStruct = obj.pDidStruct;
			pUnCompress = obj.pUnCompress;
			pUnCompressBuffer = obj.pUnCompressBuffer;
			iBufferLen = obj.iBufferLen;
			bInitialized = obj.bInitialized;
		}
	};

public:
	DidUncompress(string & path);
	~DidUncompress(void);
	int ReadConfig();
	void Initialize();
	//int Uncompress(char* source_data, int source_size, int num, int did, char* des_data, int des_size );
	//int Uncompress();
	//For Test
	int Compress(char* source_data, int source_size, int num, int did, char* des_data, int& des_size );
	int DisassemblePack(DC_HEAD* pPack,DataBuffer& buf);

private:
	void ReadDidStruct(STEP::DOMNode* node);
	int UpdateDSData(TDidInfo* pDidFile,char* pTemplateData,int len);

private:
	string configFilePath;
	map<int, TDidInfo> m_mapDidFile;
	map<int,TStructForGetBmData> m_mapStructForBmData;
};

