#ifndef MONITOR_DID_TEMPLATE_TO_LUA_STRUCT_H
#define MONITOR_DID_TEMPLATE_TO_LUA_STRUCT_H

#include "xml2cfg.h"
#include <map>
#include <iostream>
using namespace std;

class DIDTemplateToLuaStruct
{
public:
	DIDTemplateToLuaStruct(){};
	~DIDTemplateToLuaStruct(){};
	void Transform();
	//void AutoAddToDidTypeMap();
	//vector<std::string>& GetPathFiles();
	
private:
	vector<std::string> did_template_files_;
};

#endif