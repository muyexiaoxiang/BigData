#pragma once
#include <string>
#include <iostream>
#include <cassert>
using namespace std;

#define UN_INT 0xcccccccccccccccc  
#define MAX_INT64 0x7FFFFFFFFFFFFFFF  
#define MIN_INT64 0x8000000000000000  

typedef long long INT64;

class BigData
{
public:
	BigData(INT64 value);
	BigData(const char* str);


	BigData operator+(const BigData& bg);
	BigData operator-(const BigData& bg);
	BigData operator*(const BigData& bg);
	BigData operator/(const BigData& bg);

private:
	bool IsINT64Overflow()const;  //是否溢出

	string Add(string strLeft, string strRight);
	string Sub(string strLeft, string strRight);
	string MUL(string strLeft, string strRight);
	string Div(string strLeft, string strRight);

	void INT64tostring();

	bool IsLeftBig(char* pLeft, size_t LSize, char* pRight, size_t RSize);
	char SubLoop(char* &pLeft, size_t &LSize, char* pRight, size_t RSize);

	friend ostream& operator<<(ostream& _cout, const BigData& bg);
private:
	string _strData;// char*
	INT64 _value;
};



#include "BigData.h"

//值构造
BigData::BigData(INT64 value)
:_value(value)
{
	INT64tostring();
}

void BigData::INT64tostring()
{
	char symbol = '+';
	if (_value < 0)
		symbol = '-';
	_strData.append(1, symbol);

	INT64 tmp = _value;
	int i = 0;
	while (tmp)
	{
		int num = tmp % 10;
		if (_value < 0)
		{
			num = 0 - num;
		}
		_strData.append(1, num + '0');
		tmp /= 10;
		i++;
	}

	//字符串逆置
	char* pleft = (char*)_strData.c_str() + 1;
	char* pright = pleft + _strData.size() - 2;
	while (pleft < pright)
	{
		char temp = *pleft;
		*pleft = *pright;
		*pright = temp;
		pleft++;
		pright--;
	}
}

BigData::BigData(const char* str)
{
	if (str == NULL)
		return;

	char* src = (char*)str;

	while (isspace(*src)) //跳过空白字符
		src++;

	char symbol = *src; //记录符号位置
	if (symbol == '+' || symbol == '-')
		src++;
	else if (symbol >= '0'&&symbol <= '9')
		symbol = '+';
	else
		return;

	while (*src == '0') //跳过零位
		src++;

	_strData.resize(strlen(str) + 1);
	_strData[0] = symbol;

	_value = 0;
	int count = 1;
	while (*src >= '0' && *src <= '9')
	{
		_strData[count++] = *src;
		_value = _value * 10 + (*src - '0');
		src++;
	}
	_strData.resize(count);
	if (symbol == '-')
		_value = 0 - _value;
}


ostream& operator<<(ostream& _cout, const BigData& bg)
{
	if (bg.IsINT64Overflow())
	{
		_cout << bg._value << endl;
		return _cout;
	}
	else
	{
		char* ptr = (char*)bg._strData.c_str();
		if (ptr[0] == '+')
			ptr++;
		_cout << ptr << endl;
		return _cout;
	}
}

//相加  溢出  不溢出
BigData BigData::operator+(const BigData& bg)
{
	//都没有溢出
	if (bg.IsINT64Overflow() && IsINT64Overflow())
	{
		if (bg._value >= 0 && bg._value <= 0)
		{
			return BigData(_value + bg._value);
		}
		else if ((_value >= 0 && MAX_INT64 - _value >= bg._value)
			|| (_value < 0 && MIN_INT64 - _value <= bg._value))
		{
			return BigData(_value + bg._value);
		}
	}

	return BigData(Add(_strData, bg._strData).c_str());
}

//字符串相加
string BigData::Add(string strLeft, string strRight)
{
	int Lsize = strLeft.size();
	int Rsize = strRight.size();

	if (Lsize < Rsize)
	{
		swap(strLeft, strRight);
		swap(Lsize, Rsize);
	}

	string ret;
	ret.resize(Lsize + 1);
	ret[0] = strLeft[0]; //符号位

	char tmep = '0';  //保存进位
	int i = 0;
	for (i = 1; i < Lsize; i++)
	{
		int src = strLeft[Lsize - i] + tmep - '0';
		if (i < Rsize)
		{
			src += strRight[Rsize - i] - '0';
		}
		ret[Lsize - i + 1] = src % 10 + '0';
		tmep = src / 10;
	}
	ret[1] = tmep + '0';
	return ret;
}

//相减
BigData BigData::operator-(const BigData& bg)
{
	if (bg.IsINT64Overflow() && IsINT64Overflow())
	{
		if (_strData[0] == bg._strData[0])
			return BigData(_value - bg._value);
		// 8-x >= -10 => 8 + -10 <= x
		// -8 - x <= 10 => -8 - 10 <= x
		else
		{
			if ((_value >= 0 && MIN_INT64 + _value >= bg._value)
				|| (_value < 0 && _value - MAX_INT64 <= bg._value))
			{
				return BigData(_value - bg._value);
			}
		}
	}
	return BigData(Sub(_strData, bg._strData).c_str());
}

//字符串相减
string BigData::Sub(string strLeft, string strRight)
{
	int Lsize = strLeft.size();
	int Rsize = strRight.size();
	char symbol;
	if (((Lsize == Rsize) && strLeft<strRight || (Lsize < Rsize))
	{
		swap(strLeft, strRight);
		swap(Lsize, Rsize);
		if (strLeft[0] == '-')
			symbol = '+';
		else
			symbol = '-';
	}

	string ret;
	ret.resize(Lsize);
	ret[0] = symbol;

	char tmep;
	int i = 0;
	for (i = 1; i < Lsize; i++)
	{
		int src = strLeft[Lsize - i] - '0';
		if (i < Rsize)
		{
			src = src - (strRight[Rsize - i] - '0');
		}
		if (src < 0)
		{
			strLeft[Lsize - i - 1] -= 1;
			src += 10;
		}
		ret[Lsize - i] = src + '0';
	}
	return ret;
}

//相乘
BigData BigData::operator*(const BigData& bg)
{
	if (bg._value == 0 || _value == 0)
		return BigData((INT64)0);

	if (IsINT64Overflow() && bg.IsINT64Overflow()) //都没溢出
	{
		if (_strData[0] == bg._strData[0]) //同号
		{
			//10 / 5 = 2  1 不溢出 3 溢出   2>1
			//10 / -5 = -2 -1不溢出 -3 溢出 2<-1
			if ((_value > 0 && MAX_INT64 / _value >= bg._value)
				|| (_value < 0 && MAX_INT64 / _value <= bg._value))
				return BigData(_value*bg._value);
		}
		else
		{
			// -10 / 5 = -2 -1不溢出
			//-10/-5 = 2  1不溢出
			if ((_value>0 && MIN_INT64 / _value <= bg._value)
				|| (_value < 0 && MIN_INT64 / _value >= bg._value))
				return BigData(_value*bg._value);
		}
	}
	return BigData(MUL(_strData, bg._strData).c_str());
}

string BigData::MUL(string strLeft, string strRight)
{
	int Lsize = strLeft.size();
	int Rsize = strRight.size();
	if (Lsize < Rsize)
	{
		swap(strLeft, strRight);
		swap(Lsize, Rsize);
	}
	char symbol = '+';
	if (strLeft[0] != strRight[0])
		symbol = '-';

	string ret;
	ret.assign(Lsize + Rsize - 1, 0); //开辟空间 并初始化为全0
	ret[0] = symbol;
	int len = ret.size();
	int num = 0;
	for (int i = 1; i < Lsize; i++)
	{
		char src = strLeft[Lsize - i];
		char dst = 0;

		if (src == 0)
			num++;

		for (int j = 1; j < Rsize; j++)
		{
			char ptr = src * (strRight[Rsize - j] - '0') + dst;
			ptr += ret[len - j - num] - '0';
			ret[len - j - num] = ptr % 10 + '0';
			dst = ptr / 10;
		}
		ret[len - Lsize - num] += dst;
		num++;
	}
	return ret;
}

//相除
//除法  除数不为零   无溢出直接除  
//left < right    0  
//right = +/-1;  
//left = right    数值相等为+、-1；  
BigData BigData::operator/(const BigData& bg)
{
	if (bg._strData[1] == '0')
		return BigData(INT64(0));
	if (IsINT64Overflow() && bg.IsINT64Overflow())
		return BigData(_value / bg._value);

	int Lsize = _strData.size();
	int Rsize = bg._strData.size();

	if (Lsize < Rsize || (Lsize == Rsize && strcmp(_strData.c_str() + 1, bg._strData.c_str() + 1)<0))
		return BigData(INT64(0));

	if (bg._strData.c_str == '+1' || bg._strData.c_str == '-1')
	{
		string ret = _strData;
		if (ret[0] == bg._strData[0])
			ret[0] = '+';
		else
			ret[0] = '-';
		return BigData(ret.c_str());
	}

	return BigData(Div(_strData, bg._strData).c_str());
}
string BigData::Div(string strLeft, string strRight)
{
	string ret;
	ret.resize(1, '+');
	if (strLeft[0] != strRight[0])       //确定商的符号  
	{
		ret[0] = '-';
	}
	char* pleft = (char*)(strLeft.c_str() + 1);
	char* pright = (char*)(strRight.c_str() + 1);
	size_t len = strRight.size() - 1;        //因为有符号位  
	for (int i = 0; i < strLeft.size() - 1;)
	{

		if (!(IsLeftBig(pleft, len, pright, strRight.size() - 1)))
		{
			ret.append(1, '0');
			len++;
		}
		else
		{
			ret.append(1, SubLoop(pleft, len, pright, strRight.size() - 1));
			while (*pleft == '0' && len > 0)
			{
				pleft++;
				i++;
				len--;
			}
			len++;
		}
		if (len > strRight.size())//pLeft比pRight大一位结果为0，则pLeft中含有0  
		{
			pleft++;
			len--;
			i++;
		}
		if (len + i > strLeft.size() - 1)
		{
			break;
		}
	}
	return ret;
}
bool IsLeftBig(char* pLeft, size_t LSize, char* pRight, size_t RSize)
{
	if (LSize > RSize || (LSize == RSize && strcmp(pLeft, pRight) >= 0))
	{
		return true;
	}
	return false;
}
char SubLoop(char* &pLeft, size_t &LSize, char* pRight, size_t RSize)
{
	assert(pLeft != NULL && pRight != NULL);
	char pret = '0';
	while (1)   //被除数>除数  
	{
		if (!IsLeftBig(pLeft, LSize, pRight, RSize))
		{
			break;
		}
		for (int i = 0; i < RSize; i++)
		{
			char ret = pLeft[LSize - i - 1] - '0';
			ret -= pRight[RSize - i - 1] - '0';
			if (ret < 0)
			{
				pLeft[LSize - i - 2] -= 1;
				ret += 10;
			}
			pLeft[LSize - i - 1] = ret + '0';
		}
		while (*pLeft == '0' && LSize > 0)
		{
			pLeft++;
			LSize--;
		}
		pret++;
	}
	return pret;
}

//没有溢出返回true
bool BigData::IsINT64Overflow() const
{
	string temp = "+9223372036854775807";
	if (_strData[0] == '-')
		temp = "- 9223372036854775808";

	if (_strData.size() < temp.size())
		return true;
	else if ((_strData.size() == temp.size()) && (_strData <= temp))
		return true;
	else
		return false;
}