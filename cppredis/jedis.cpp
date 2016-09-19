
#include "hiredis.h"
#include "jedis.h"
#include <assert.h>
#include <string>
#include "shardinfo.h"
#include "log.h"

using namespace std;

#define FREE_REPLY(reply) if(reply != NULL) {freeReplyObject(reply); reply = NULL;}

#define FREE_REDIS(p)  if ( p != NULL ) { redisFree( p) ; p = NULL ; }

Jedis::Jedis()
{
	_redis_context = NULL;
	_is_connect = false;
	_reconn_timeval = 10;
	_last_reconn_time = time(0);
}

Jedis::~Jedis()
{
    dlog1("jedis : %s free", _name.c_str());
	FREE_REDIS(_redis_context);
}

bool Jedis::init_obj(ShardInfo * shard_info)
{
	_name = shard_info->getName();
    if(_name.empty())
    {
    	char buffer[32] = {0};
    	snprintf(buffer, sizeof(buffer) - 1, "%s:%d", shard_info->getHost().c_str(), shard_info->getPort());
    	_name = buffer;
    }
	return init_obj(shard_info->getHost().c_str(), shard_info->getPort());
}

// 初始化对象
bool Jedis::init_obj(const char *ip, unsigned short port)
{
	_redis_context = connect((char*) ip, port);
	if (_redis_context != NULL && _redis_context->err == REDIS_OK)
	{
		dlog1("connect redis %s : %d\n", ip, port);
		_is_connect = true;
		set_timeout(10, 0);
    	char buffer[32] = {0};
    	snprintf(buffer, sizeof(buffer) - 1, "%s:%d", ip, port);
    	_name = buffer;
		return true;
	}

	return false;
}

string Jedis::get_addr()
{
    return get_context_addr(_redis_context);
}

// 连接服务器
redisContext * Jedis::connect(const char *ip, int port)
{
	struct timeval tv = { 10, 1000 };
	struct timeval timeout = { 0, 500000 }; // 0.5 seconds
	redisContext *c = redisConnectWithTimeout((char*) ip, port, timeout);
	if (c->err)
	{
		dlog1("%s:%d ,Connection error: %s\n", ip, port, c->errstr);
		FREE_REDIS( c);
		return NULL;
	}
	assert(redisSetTimeout(c,tv) == REDIS_OK);
	return c;
}

void Jedis::set_timeout(int seconds, int useconds)
{
	struct timeval tv;
	tv.tv_sec = seconds;
	tv.tv_usec = useconds;
	redisSetTimeout(_redis_context, tv);

	return;
}

bool Jedis::subscribe(vector<string> &channels)
{
    set_timeout(0, 0);   //将本Jedis做成阻塞式的。
	string subs_channels = "SUBSCRIBE";
	for (int i = 0; i < (int)channels.size(); ++i)
	{
		subs_channels += " " + channels[i];
	}

	if (!execute(false, " ", subs_channels.c_str()))
		return false;

	dlog1("%s success \n", subs_channels.c_str());
	return true;
}

bool Jedis::subscribe_get_reply(string &channel, string &value)
{
	//必须设置
	set_timeout(10, 0);

	redisReply *subs_reply = NULL;
	redisGetReply(_redis_context, (void**) &subs_reply);

	if (check_error(_redis_context) || check_error(subs_reply))
	{
		return false;
	}

	showreply((void*) subs_reply);

	bool ret = false;
	if (strncmp(subs_reply->element[0]->str, "message", strlen("message")) == 0)
	{
		if (subs_reply->elements == 3)
		{
			channel.assign(subs_reply->element[1]->str, subs_reply->element[1]->len);
			value.assign(subs_reply->element[2]->str, subs_reply->element[2]->len);
			ret = true;
		}
		else if (subs_reply->elements == 4)
		{
			channel.assign(subs_reply->element[2]->str, subs_reply->element[2]->len);
			value.assign(subs_reply->element[3]->str, subs_reply->element[3]->len);
			ret = true;
		}
	}

	freeReplyObject(subs_reply);
	return true;
}

//--------------------------------------------------------------------------------//
bool Jedis::execute(bool iswrite, const char *key, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	bool ret = execute(iswrite, key, format, ap);
	ret = execute(iswrite, key, format, ap);
	va_end(ap);

	return ret;
}

bool Jedis::execute(bool iswrite, const char *key, long long &result, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	bool ret = execute(iswrite, key, result, format, ap);
	va_end(ap);

	return ret;
}

//下面几个execute由于重载的原因，代码都一样，能否通过模板的方式实现？
bool Jedis::execute(bool iswrite, const char *key, string &result, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	bool ret = execute(iswrite, key, result, format, ap);
	va_end(ap);

	return ret;
}

bool Jedis::execute(bool iswrite, const char *key, list<string> &result, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	bool ret = execute(iswrite, key, result, format, ap);
	va_end(ap);

	return ret;
}

bool Jedis::execute(bool iswrite, const char *key, set<string> &result, const char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	bool ret = execute(iswrite, key, result, format, ap);
	va_end(ap);

	return ret;
}

bool Jedis::execute(bool iswrite, const char *key, const char *format, va_list ap)
{
	if (!_is_connect)
		return false;

	bool ret = false;

	redisReply *reply = (redisReply *) redisvCommand(_redis_context, format, ap);

	if (!check_error(_redis_context) && !check_error(reply))
	{
		ret = true;
	}
	else
	{
		showreply(reply);
	}

	FREE_REPLY(reply);
	return ret;
}

bool Jedis::execute(bool iswrite, const char *key, long long &result, const char *format, va_list ap)
{
	if (!_is_connect)
		return false;

	redisReply *reply = (redisReply *) redisvCommand(_redis_context, format, ap);
    if(reply == NULL)
    {
    	printf("reply == NULL \n");
    }

	if (check_error(_redis_context) || check_error(reply))
	{
		goto error;
	}

	showreply((void*)reply);

	if (reply->type == REDIS_REPLY_INTEGER)
	{
		result = reply->integer;
	}

	FREE_REPLY(reply);
	return true;

	error: showreply(reply);
	FREE_REPLY(reply);
	return false;
}

bool Jedis::execute(bool iswrite, const char *key, string &result, const char *format, va_list ap)
{
	if (!_is_connect)
		return false;

	redisReply *reply = (redisReply *) redisvCommand(_redis_context, format, ap);

	//这个地方先判断reply == NULL ，所以不会产生coredump
	if (check_error(_redis_context) || check_error(reply))
	{
		goto error;
	}

	if (reply->type == REDIS_REPLY_STRING || reply->type == REDIS_REPLY_STATUS)
	{
		result.assign(reply->str, 0, reply->len);
	}

	FREE_REPLY(reply);
	return true;
	error: showreply(reply);
	FREE_REPLY(reply);
	return false;
}

bool Jedis::execute(bool iswrite, const char *key, list<string> &result, const char *format, va_list ap)
{
	if (!_is_connect)
		return false;

	redisReply *reply = (redisReply *) redisvCommand(_redis_context, format, ap);

	//这个地方先判断reply == NULL ，所以不会产生coredump
	if (check_error(_redis_context) || check_error(reply))
	{
		goto error;
	}

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (int i = 0; i < (int) reply->elements; i++)
		{
			if (reply->element[i]->type == REDIS_REPLY_STRING)
			{
				string str(reply->element[i]->str, reply->element[i]->len);
				result.push_back(str);
			}
		}
	}

	FREE_REPLY(reply);
	return true;

	error: showreply(reply);
	FREE_REPLY(reply);
	return false;
}

bool Jedis::execute(bool iswrite, const char *key, set<string> &result, const char *format, va_list ap)
{
	if (!_is_connect)
		return false;
	redisReply *reply = (redisReply *) redisvCommand(_redis_context, format, ap);

	//这个地方先判断reply == NULL ，所以不会产生coredump
	if (check_error(_redis_context) || check_error(reply))
	{
		goto error;
	}

	if (reply->type == REDIS_REPLY_ARRAY)
	{
		for (int i = 0; i < (int)reply->elements; i++)
		{
			if (reply->element[i]->type == REDIS_REPLY_STRING)
			{
				string str(reply->element[i]->str, reply->element[i]->len);
				result.insert(str);
			}
		}
	}

	FREE_REPLY(reply);
	return true;
	error: showreply(reply);
	FREE_REPLY(reply);
	return false;
}

bool Jedis::execute(const char *cmd, long long &result)
{
	int argc = 1;
	const char *argv[1];
	argv[0] = cmd;

	size_t argvlen[1];
	argvlen[0] = strlen(cmd);

	if (!_is_connect)
		return false;

	redisReply *reply = (redisReply *) redisCommandArgv(_redis_context, argc, argv, argvlen);
	if (check_error(_redis_context) || check_error(reply))
	{
		goto error;
	}

	showreply((void*)reply);

	if (reply->type == REDIS_REPLY_INTEGER)
	{
		result = reply->integer;
	}

	FREE_REPLY(reply);
	return true;

	error: showreply(reply);
	FREE_REPLY(reply);
	return false;

    return true;
}
