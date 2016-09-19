/******************************************************
 *   FileName: shardinfo.h
 *     Author: liubo  2012-11-29
 *Description:
 *******************************************************/
#ifndef ___SHARDINFO_H___
#define ___SHARDINFO_H___

#include <list>
#include <map>
#include <set>
#include <list>
#include <stdio.h>
#include <string>
#include <iostream>
#include "jedis.h"

using namespace std;

class Jedis;
class Protocol
{
public:
	static const int DEFAULT_PORT = 6379;
	static const int DEFAULT_TIMEOUT = 2000;
	static const int DEFAULT_DATABASE = 0;

//	static const char* CHARSET = "UTF-8";
	static const char DOLLAR_BYTE = '$';
	static const char ASTERISK_BYTE = '*';
	static const char PLUS_BYTE = '+';
	static const char MINUS_BYTE = '-';
	static const char COLON_BYTE = ':';

	static const int DEFAULT_WEIGHT = 1;
};

class ShardInfo
{
public:

	ShardInfo(string host, int port = Protocol::DEFAULT_PORT, int weight = Protocol::DEFAULT_WEIGHT, string name =
			string(""), string password = string(""), int timeout = Protocol::DEFAULT_TIMEOUT)
	{
		_host = host;
		_port = port;
		_name = name;
		_password = password;
		_timeout = timeout;
		_weight = weight;

	    if(_name.empty())
	    {
	    	char buffer[32] = {0};
	    	snprintf(buffer, sizeof(buffer) - 1, "%s:%d", host.c_str(), port);
	    	_name = buffer;
	    }
	}

	string getPassword()
	{
		return _password;
	}

	int getWeight()
	{
		return _weight;
	}

	string toString()
	{
		char out[128] = { 0 };
		snprintf(out, sizeof(out) - 1, "%s:%d*%d", _host.c_str(), _port, _weight);
		return out;
	}

	string getHost()
	{
		return _host;
	}

	int getPort()
	{
		return _port;
	}

	void setPassword(string auth)
	{
		_password = auth;
	}

	int getTimeout()
	{
		return _timeout;
	}

	void setTimeout(int timeout)
	{
		_timeout = timeout;
	}

	string getName()
	{
		return _name;
	}

	Jedis *createResource()
	{
		Jedis *j = new Jedis;
		if (!j->init_obj(this))
		{
			delete j;
			return NULL;
		}
		return j;
	}

private:
	int _timeout;
	string _host;
	int _port;
	int _weight;
	string _password;
	string _name;
};

#endif /* SHARDINFO_H_ */
