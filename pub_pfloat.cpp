/********************************************************************
	created:	2012:10:22   15:51
	filename: 	pub_pfloat.cpp
	author:		fu.changyou
	
	purpose:	pfloat的实现
*********************************************************************/
#include "stdafx.h"
#include "pub_pfloat.h"
#include <string>

#pragma warning(disable:4244)
#pragma warning(disable:4996)

#ifndef MAXVALUE
#define MAXVALUE(x, y) ((x)>(y)?(x):(y))
#endif
#ifndef MINVALUE
#define MINVALUE(x, y) ((x)<(y)?(x):(y))
#endif

//默认构造函数
PFloat::PFloat()
{
    this->_data._value = 0L;
	//do nothing
}

//拷贝构造函数
PFloat::PFloat(const PFloat &obj)
{
    _data._value = obj._data._value;

    //防止出现m_llBase为0而m_llTag为1的情况
    if (this->_data._real_data.m_llBase == 0)
    {
        this->_data._real_data.m_llTag = 0;
    }
}

//按照PFloat结构保存的INT64整数作为构造函数参数
PFloat::PFloat(INT64 fValue)
{
    this->_data._value = fValue;

    //防止出现m_llBase为0而m_llTag为1的情况
    if (this->_data._real_data.m_llBase == 0)
    {
        this->_data._real_data.m_llTag = 0;
    }
}

//传入一个字符串表示的浮点数做构造函数参数
PFloat::PFloat(const char* strValue)
{
    _data._value = 0;
	Str2PFloat(*this, strValue);
}

//传入参数nBase和sPrec
PFloat::PFloat(INT64 nBase, unsigned short sPrec)
{
    _data._value = 0;
    _data._real_data.m_llPrec = sPrec;
    if (nBase < 0)
    {
        _data._real_data.m_llTag = 1;
        _data._real_data.m_llBase = ~nBase+1;
    }
    else
    {
        _data._real_data.m_llBase = nBase;
    }
}

//获取值
double PFloat::GetValue() const
{
    return (_data._real_data.m_llTag == 1 ? -1 : 1) * GetABSValue();
}

//获取绝对值
double PFloat::GetABSValue() const
{
    if (_data._real_data.m_llPrec == 0)
    {
        return _data._real_data.m_llBase;
    }
    double f = 1.0;
    for (int i = 0; i < _data._real_data.m_llPrec; i++)
    {
        f *= 10;
    }
    return _data._real_data.m_llBase / f;
}

 //传入一个参数用作保存该值，并返回该字符串,请确保sValue有足够的空间
const char* PFloat::GetStrValue(char sValue[])
{
	sprintf(sValue, "%lld\0", _data._real_data.m_llBase);

	//加入小数点
	if (_data._real_data.m_llPrec > 0)
	{
		int iLen = strlen(sValue);
		if (iLen <= _data._real_data.m_llPrec)
		{//小数太多
			int iPos = _data._real_data.m_llPrec - iLen;
			memcpy(sValue+iPos+2, sValue, iLen);
			memset(sValue, '0', iPos+2);
			sValue[1] = '.';
			sValue[2+_data._real_data.m_llPrec] = 0;
		}
		else
		{
			int iPos = iLen - _data._real_data.m_llPrec;
			memcpy(sValue+iPos+1, sValue+iPos, _data._real_data.m_llPrec);
			*(sValue+iPos) = '.';
			sValue[iLen+1] = 0;
		}
	}
    if (_data._real_data.m_llTag == 1)
    {//负数，加符号
        sValue[strlen(sValue)+1] = 0;
        memcpy(sValue+1, sValue, strlen(sValue));
        sValue[0] = '-';
    }
	return sValue;
}

//加减乘除等基本操作
PFloat PFloat::operator+(const PFloat& f) const
{
	short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
	INT64 llValue = (_data._real_data.m_llTag == 1 ? -1 : 1) * this->_data._real_data.m_llBase * Power10(sMax-this->_data._real_data.m_llPrec)
			+ (f._data._real_data.m_llTag == 1 ? -1 : 1) * f._data._real_data.m_llBase * Power10(sMax - f._data._real_data.m_llPrec);
	PFloat f1(llValue, sMax);
	return f1;
}

PFloat PFloat::operator-(const PFloat& f) const
{
	short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
	INT64 llValue = (_data._real_data.m_llTag == 1 ? -1 : 1) * this->_data._real_data.m_llBase * Power10(sMax-this->_data._real_data.m_llPrec)
		- (f._data._real_data.m_llTag == 1 ? -1 : 1) * f._data._real_data.m_llBase * Power10(sMax - f._data._real_data.m_llPrec);
	PFloat f1(llValue, sMax);
	return f1;
}

PFloat PFloat::operator*(const PFloat& f) const
{
	short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
	INT64 llValue =this->_data._real_data.m_llBase / Power10(this->_data._real_data.m_llPrec + f._data._real_data.m_llPrec - sMax) * f._data._real_data.m_llBase;
    if (this->_data._real_data.m_llTag != f._data._real_data.m_llTag)
    {
        llValue *= -1;
    }
	PFloat f1(llValue, sMax);
	return f1;
}

PFloat PFloat::operator/(const PFloat& f) const
{
	short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
	INT64 llValue = 0;
	if (f._data._real_data.m_llBase == 0)
	{
		llValue =(this->_data._real_data.m_llTag == 1 ? -1 : 1) * this->_data._real_data.m_llBase * Power10(sMax-this->_data._real_data.m_llPrec);
	}
	else
	{
		llValue =this->_data._real_data.m_llBase / f._data._real_data.m_llBase * Power10(f._data._real_data.m_llPrec+sMax-this->_data._real_data.m_llPrec);	
        if (this->_data._real_data.m_llTag != f._data._real_data.m_llTag)
        {
            llValue *= -1;
        }		
	}
	PFloat f1(llValue, sMax);
	return f1;
}

/////////////////////////////////string/////////////////////////////////////////
PFloat PFloat::operator+(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return (*this) + f1;
}

PFloat PFloat::operator-(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return (*this) - f1;
}

PFloat PFloat::operator*(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return (*this) * f1;
}

PFloat PFloat::operator/(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return (*this) / f1;
}

/////////////////////////////////Int64/////////////////////////////////////////
//Int64内存块的加减乘除，切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
PFloat PFloat::operator+(INT64 f) const
{
    PFloat f1(f);
    return (*this) + f1;
}

PFloat PFloat::operator-(INT64 f) const
{
    PFloat f1(f);
    return (*this) - f1;
}

PFloat PFloat::operator*(INT64 f) const
{
    PFloat f1(f);
    return (*this) * f1;
}

PFloat PFloat::operator/(INT64 f) const
{
    PFloat f1(f);
    return (*this) / f1;
}

///////////////////////////////+=\-=\*=\/= PFloat///////////////////////////////////////////
const PFloat& PFloat::operator+=(const PFloat& f) const
{
	(*this) = (*this) + f;
	return (*this);
}
const PFloat& PFloat::operator-=(const PFloat& f) const
{
	(*this) = (*this) - f;
	return (*this);
}

const PFloat& PFloat::operator*=(const PFloat& f) const
{
	(*this) = (*this) * f;
	return (*this);
}

const PFloat& PFloat::operator/=(const PFloat& f) const
{
	(*this) = (*this) / f;
	return (*this);
}

///////////////////////////////+=\-=\*=\/= string///////////////////////////////////////////
const PFloat& PFloat::operator+=(const char* f) const
{
	(*this) = (*this) + f;
	return (*this);
}

const PFloat& PFloat::operator-=(const char* f) const
{
	(*this) = (*this) - f;
	return (*this);
}

const PFloat& PFloat::operator*=(const char* f) const
{
	(*this) = (*this) * f;
	return (*this);
}

const PFloat& PFloat::operator/=(const char* f) const
{
	(*this) = (*this) / f;
	return (*this);
}

///////////////////////////////+=\-=\*=\/= Int64///////////////////////////////////////////
//Int64内存块的加减乘除并赋值，切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
const PFloat& PFloat::operator+=(INT64 f) const
{
    (*this) = (*this) + f;
    return (*this);
}

const PFloat& PFloat::operator-=(INT64 f) const
{
    (*this) = (*this) - f;
    return (*this);
}

const PFloat& PFloat::operator*=(INT64 f) const
{
    (*this) = (*this) * f;
    return (*this);
}

const PFloat& PFloat::operator/=(INT64 f) const
{
    (*this) = (*this) / f;
    return (*this);
}

///////////////////////////////=赋值///////////////////////////////////////////
const PFloat& PFloat::operator=(const PFloat& f) const
{
	this->_data._value = f._data._value;
	return (*this);
}

const PFloat& PFloat::operator=(const char* f) const
{
    PFloat f1(f);
    this->_data._value = f1._data._value;
	return (*this);
}

const PFloat& PFloat::operator=(INT64 f) const
{//切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
    this->_data._value = f;

    //防止出现m_llBase为0而m_llTag为1的情况
    if (this->_data._real_data.m_llBase == 0)
    {
        this->_data._real_data.m_llTag = 0;
    }
    return (*this);
}

////////////////////////////////相等判断//////////////////////////////////////////
bool   PFloat::operator==(const PFloat& f) const
{
    if (this->_data._real_data.m_llTag != f._data._real_data.m_llTag)
    {
        return false;
    }

	if (this->_data._real_data.m_llPrec == f._data._real_data.m_llPrec)
	{
		return this->_data._real_data.m_llBase == f._data._real_data.m_llBase;
	}
	else
	{
		short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
		return (this->_data._real_data.m_llBase * Power10(sMax-this->_data._real_data.m_llPrec))
			== (f._data._real_data.m_llBase * Power10(sMax - f._data._real_data.m_llPrec));
	}
}

bool   PFloat::operator==(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return *this == f1;
}

bool   PFloat::operator==(INT64 f) const
{//切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
    return *this == PFloat(f);
}

///////////////////////////////不等于判断///////////////////////////////////////////
bool   PFloat::operator!=(const PFloat& f) const
{
	return !(*this == f);
}

bool   PFloat::operator!=(const char* f) const
{
	return !(*this == f);
}

bool   PFloat::operator!=(INT64 f) const
{//切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
    return *this != PFloat(f);
}

///////////////////////////////大于判断///////////////////////////////////////////
bool   PFloat::operator>(const PFloat& f) const
{
    if (this->_data._real_data.m_llTag != f._data._real_data.m_llTag)
    {
        return this->_data._real_data.m_llTag < f._data._real_data.m_llTag;
    }

	if (this->_data._real_data.m_llPrec == f._data._real_data.m_llPrec)
	{
		return this->_data._real_data.m_llBase > f._data._real_data.m_llBase;
	}
	else
	{
		short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
		return (this->_data._real_data.m_llBase * Power10(sMax-this->_data._real_data.m_llPrec))
			> (f._data._real_data.m_llBase * Power10(sMax - f._data._real_data.m_llPrec));
	}
}

bool   PFloat::operator>(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return *this > f1;
}

bool   PFloat::operator>(INT64 f) const
{//切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
    PFloat f1(f);
    return *this > f1;
}

//小于判断
bool   PFloat::operator<(const PFloat& f) const
{
    if (this->_data._real_data.m_llTag != f._data._real_data.m_llTag)
    {
        return this->_data._real_data.m_llTag > f._data._real_data.m_llTag;
    }

	if (this->_data._real_data.m_llPrec == f._data._real_data.m_llPrec)
	{
		return this->_data._real_data.m_llBase < f._data._real_data.m_llBase;
	}
	else
	{
		short sMax = MAXVALUE(this->_data._real_data.m_llPrec, f._data._real_data.m_llPrec);
		return (this->_data._real_data.m_llBase * Power10(sMax-this->_data._real_data.m_llPrec))
			< (f._data._real_data.m_llBase * Power10(sMax - f._data._real_data.m_llPrec));
	}
}

bool   PFloat::operator<(const char* f) const
{
	PFloat f1;
	Str2PFloat(f1, f);
	return *this < f1;
}

bool   PFloat::operator<(INT64 f) const
{//切忌本Int64是指的与PFloat内存块对应的内存值，并非真实值
    PFloat f1(f);
    return *this < f1;
}

//检查strValue是否位有效的小数表示形式
bool   PFloat::CheckValid(const char* strValue) const
{
	if (NULL == strValue)
	{
		return false;
	}

	bool bFindPoint = false;  //小数点

	//第一个数只能是0-9的数字或者+，-正负号或者.小数点
	if (*strValue != '-' 
		&& *strValue != '+'
		&& (*strValue > '9' || *strValue < '0'))
	{
		if (*strValue == '.')
		{
			bFindPoint = true;
		}
		else
		{
			return false;
		}
	}

	char* pCheck = (char*)(strValue+1);
	while (*pCheck != '\0')
	{
		if (*pCheck > '9' && *pCheck < '0')
		{//非数字
			if (*pCheck == '.')
			{//小数点只能出现一次
				if (!bFindPoint)
				{//第一次出现
					bFindPoint = true;
				}
				else
				{
					return false;
				}
			}
			else
			{//其他非法字符
				return false;
			}
		}
		++pCheck;
	}

	return true;
}

//计算10的n次方
int    PFloat::Power10(short n) const
{
	int i = 1;
	for (short s = 0; s < n; s++)
	{
		i *= 10;
	}
	return i;
}

//str转换成PFloat
PFloat    PFloat::Str2PFloat(PFloat& f, const char* str) const
{
	char sBuffer[260] = {0};
	int iTmp = strlen(str) - (sizeof(sBuffer) - 1);
	if (iTmp > 0)
	{//太长，要作数据截断
		strcpy(sBuffer, str+iTmp);
	}
	else
	{
		strcpy(sBuffer, str);
	}

	if (!CheckValid(str))
	{
		f._data._real_data.m_llBase = 0;
		f._data._real_data.m_llPrec = 0;
		return f;
	}

	//如果第一个字符为+、-符号
	bool bNegative = false;  //负数标识
	char* pTmp = sBuffer;
	if (sBuffer[0] == '+')
	{
		++pTmp;
	}
	else if (sBuffer[0] == '-')
	{
		bNegative = true;
		++pTmp;
	}

	char* p = strchr(pTmp, '.');
	if (NULL == p)
	{
		f._data._real_data.m_llPrec = 0;
	}
	else
	{
		f._data._real_data.m_llPrec = strlen(pTmp) - (p - pTmp) - 1;
		memcpy(p, p+1, f._data._real_data.m_llPrec);
		*(p+f._data._real_data.m_llPrec) = 0;
	}
	long long llTmp = 0L;
	sscanf(pTmp, "%lld", &llTmp);
	f._data._real_data.m_llBase = llTmp;
    f._data._real_data.m_llTag = (bNegative ? 1 : 0);

    //防止出现m_llBase为0而m_llTag为1的情况
    if (f._data._real_data.m_llBase == 0)
    {
        f._data._real_data.m_llTag = 0;
    }

	return f;
}

//改变精度，对应的Base采用四舍五入处理，只对精度缩小的进行处理，放大的不处理
void   PFloat::ChangePrec(short prec)
{
    if (_data._real_data.m_llPrec > prec && prec >= 0)
    {
        int iTmp = Power10(_data._real_data.m_llPrec - prec);
        _data._real_data.m_llBase = (_data._real_data.m_llBase + 0.5*iTmp) / iTmp;
        _data._real_data.m_llPrec = prec;
    }
}