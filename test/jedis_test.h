/******************************************************
 *  CopyRight: 北京中交兴路科技有限公司(2012-2015)
 *   FileName: redis_test.cpp
 *     Author: liubo  2012-12-14
 *Description:
 *******************************************************/
#ifndef __redistable_TEST__
#define __redistable_TEST__

#include <map>
#include <unistd.h>
#include <assert.h>
#include <string>
#include <list>
#include <set>
#include <algorithm>
#include <iconv.h>
using namespace std;

#include "DataCenter.h"

class CodeConverter
{
private:
	iconv_t cd;
public:
 
	// 构造
	CodeConverter(const char *from_charset,const char *to_charset) 
	{
		cd = iconv_open(to_charset,from_charset);
		assert(cd!=(iconv_t)(-1) && cd!=(iconv_t)(0));
	}
  
	// 析构
	~CodeConverter() 
	{
		iconv_close(cd);
	}
 
	string convert(string& inbuf)
	{
		string strResult;

		size_t inlen = inbuf.length();
		size_t outlen = inlen * 4;
		if (inlen==0)
			return strResult;

		char * outbuf = new char[outlen];
		memset(outbuf,0,outlen);
		char * p1 = (char *)inbuf.c_str();
		char * p2 = outbuf;
		char **pin = &p1;
		char **pout = &p2;
		int iLen = iconv(cd,pin,&inlen,pout,&outlen);
		if (iLen>=0)
			strResult = string(outbuf);

	//	cout << strResult << endl;
		delete []outbuf;
		return strResult;
	}	
};

string u2g(string u);
string g2u(string g);

class JedisTest
{
public:
	string show_map(map<string, double>& m)
	{
		string str = "map(";
		map<string, double>::iterator iter = m.begin();

		for (; iter != m.end(); ++iter)
		{
			str += "first:" + iter->first + ",second:";
			char buf[128];
			memset(buf,0,sizeof(buf));
			gcvt(iter->second, 10, buf);
			str += string(buf) + ";";
		}
		str += ")";
		return str;
	}

	string show_map_u(map<string, double>& m)
	{
		string str = "map(";
		map<string, double>::iterator iter = m.begin();

		for (; iter != m.end(); ++iter)
		{
			str += "first:" + u2g(iter->first) + ",second:";
			char buf[128];
			memset(buf,0,sizeof(buf));
			gcvt(iter->second, 10, buf);
			str += string(buf) + ";";
		}
		str += ")";
		return str;
	}

	string show_map(map<string, string>& m)
	{
		string str = "map(";
		map<string, string>::iterator iter = m.begin();

		for (; iter != m.end(); ++iter)
		{
			str += "first:" + iter->first + ",second:" + iter->second + ";";
		}
		str += ")";
		return str;
	}
	
	string show_map_u(map<string, string>& m)
	{
		string str = "map(";
		map<string, string>::iterator iter = m.begin();

		for (; iter != m.end(); ++iter)
		{
			str += "first:" + u2g(iter->first) + ",second:" + u2g(iter->second) + ";";
		}
		str += ")";
		return str;
	}

	string show_set(set<string>& s)
	{
		string str = "set(";
		set<string>::iterator iter = s.begin();

		for (; iter != s.end(); ++iter)
		{
			str += " " + *iter;
		}
		str += ")";
		return str;
	}
	
	string show_set_u(set<string>& s)
	{
		string str = "set(";
		set<string>::iterator iter = s.begin();

		for (; iter != s.end(); ++iter)
		{
			str += " " + u2g(*iter);
		}
		str += ")";
		return str;
	}
	
	string show_list(list<string>& l)
	{
		string str = "list(";
		list<string>::iterator iter = l.begin();

		for (; iter != l.end(); ++iter)
		{
			str += " " + *iter;
		}
		str += ")";
		return str;
	}

	string show_list_u(list<string>& l)
	{
		string str = "list(";
		list<string>::iterator iter = l.begin();

		for (; iter != l.end(); ++iter)
		{
			str += " " + u2g(*iter);
		}
		str += ")";
		return str;
	}

	void TestCommonFunction(const char *ip, unsigned short port,int mode);
	
	void TestFunction(const char *ip, unsigned short port);

	void TestCapability(const char *ip, unsigned short port);

	void TestConsistence(const char *ip, unsigned short port,const char * pFileName);

	void Read1MString(const char *ip, unsigned short port);

	void Write1MString(const char *ip, unsigned short port);

	void TestChinese(const char *ip, unsigned short port);

	void InitExport(const char *ip, unsigned short port);	

	void TestExport(const char *ip, unsigned short port);	

	void RunExport(const char *ip, unsigned short port);	

private:
	void TestReadString(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value);

	void TestWriteString(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,bool bCheck);

	void TestDelString(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,bool bCheck);

	void TestReadHash(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string field,string value);

	void TestReadHash(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,map<string,string> mapValue);
	
	void TestWriteHash(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string field,string value,bool bCheck);

	void TestWriteHash(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,map<string,string>& mapValue,bool bCheck);
	
	void TestDelHash(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string field,bool bCheck);

	void TestReadList(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,bool bLeft);

	void TestWriteList(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,bool bLeft,bool bCheck);

	void TestWriteList(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,list<string>& listValue,bool bLeft,bool bCheck);

	void TestDelList(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,int iCount);

	void TestReadSet(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value);

	void TestWriteSet(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,bool bCheck);

	void TestWriteSet(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,list<string>& listValue,bool bCheck);	

	void TestDelSet(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,bool bCheck);

	void TestReadSortset(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string field,double score);

	void TestReadSortset(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,map<string,double>& mapValue);
	
	void TestWriteSortset(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,double score,bool bCheck);

	void TestWriteSortset(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,map<string,double>& mapValue,bool bCheck);

	void TestDelSortset(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,string value,bool bCheck);

	void TestInc(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,bool bCheck);

	void TestDec(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,bool bCheck);

	void TestIncBy(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,int iStep,bool bCheck);

	void TestDecBy(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,
		string key,int iStep,bool bCheck);

	void TestString(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,int mode);

	void TestList(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,int mode);
	
	void TestSet(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,int mode);

	void TestSortset(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,int mode);
	
	void TestHash(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable,int mode);

	void TestVar(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable);

	void TestDel(DataCenter& datacenter,
		RedisDB& redisdb,RedisTable& redistable);	
	
//	redistable redistable;
};

#endif
