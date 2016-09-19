/******************************************************
 *  CopyRight: 北京中交兴路科技有限公司(2012-2015)
 *   FileName: jedis_interface.cpp
 *     Author: liubo  2012-12-24
 *Description:
 *******************************************************/

#include "jedis_interface.h"
#include <stdarg.h>
#include <stdio.h>
#include <sys/socket.h>
#include "hiredis.h"
#include <arpa/inet.h>
#include "log.h"

bool JedisCommand::check_error(redisContext *c)
{
//	if (c->err != 0)
//	{
//		dlog1("error context : %s %s", get_context_addr(c).c_str(), c->errstr);
//	}
//	return c == NULL || c->err != 0;
    return false;
}

bool JedisCommand::check_error(redisReply *reply)
{
	if (reply == NULL)
		return true;

	if (reply->type == REDIS_REPLY_ERROR)
	{
        dlog2("%s", showreply(reply).c_str());
		return true;
	}

	return false;
}
//递归调用显示Reply的结果, 测试使用的
string JedisCommand::showreply(void *reply)
{
	if (reply == NULL)
		return "";

	redisReply *r = (redisReply*) reply;
    char buffer[128] = {0};
	switch (r->type)
	{
	case REDIS_REPLY_STRING:
		snprintf(buffer, sizeof(buffer) - 1, "reply REDIS_REPLY_STRING : %s", r->str);
		break;
	case REDIS_REPLY_ARRAY:
        snprintf(buffer, sizeof(buffer) - 1, "reply REDIS_REPLY_ARRAY ");
		for (int i = 0; i < (int) r->elements; i++)
		{
			showreply(r->element[i]);
		}
		break;
	case REDIS_REPLY_INTEGER:
        snprintf(buffer, sizeof(buffer) - 1, "reply REDIS_REPLY_INTEGER : %lld", r->integer);
		break;
	case REDIS_REPLY_NIL:
        snprintf(buffer, sizeof(buffer) - 1, "reply REDIS_REPLY_NIL");
		break;
	case REDIS_REPLY_STATUS:
        snprintf(buffer, sizeof(buffer) - 1, "reply REDIS_REPLY_STATUS : %s ", r->str);
		break;
	case REDIS_REPLY_ERROR:
        snprintf(buffer, sizeof(buffer) - 1, "reply REDIS_REPLY_ERROR : %s ", r->str);
		break;
	default:
		break;
	}
    return buffer;
}

string JedisCommand::get_context_addr(redisContext *c)
{
	struct sockaddr name;
	int len = sizeof(name);
	char buffer[32] = { 0 };
	if (getpeername(c->fd, &name, (socklen_t*) &len) == 0)
	{
		struct sockaddr_in *addr = (struct sockaddr_in*) &name;
		unsigned short port = ntohs(addr->sin_port);
		const char *ip = inet_ntoa(addr->sin_addr);
		sprintf(buffer, "%s:%d", ip, port);
	}
	return buffer;
}

int JedisCommand::del(const char *key)
{
    long long ret;
    execute(false, getkey(key).c_str(), ret, "DEL %s", getkey(key).c_str());
    return ret;
}

bool JedisCommand::setvalue(const char * key, const char * value)
{
	return execute(true, getkey(key).c_str(), "SET %s %s", getkey(key).c_str(), value);
}

string JedisCommand::get(const char * key)
{
	string value;
	execute(false, getkey(key).c_str(), value, "GET %s", getkey(key).c_str());
	return value;
}

bool JedisCommand::exists(const char * key)
{
	long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "EXISTS %s", getkey(key).c_str());
	return ret == 1;
}

string JedisCommand::type(const char * key)
{
	string type;
	execute(false, getkey(key).c_str(), type, "TYPE %s", getkey(key).c_str());
	return type;
}

int JedisCommand::expire(const char * key, int seconds)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "EXPIRE %s %d", getkey(key).c_str(), seconds);
	return ret;
}

int JedisCommand::expireAt(const char * key, int unixTime)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "EXPIREAT %s %d", getkey(key).c_str(), unixTime);
	return ret;
}

int JedisCommand::ttl(const char * key)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "TTL %s", getkey(key).c_str());
	return ret;
}

bool JedisCommand::setbit(const char * key, int offset, bool value)
{
	return execute(true, getkey(key).c_str(), "SETBIT %s %d %d", getkey(key).c_str(), offset, value);
}

bool JedisCommand::getbit(const char * key, int offset)
{
	long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "GETBIT %s %d", getkey(key).c_str(), offset);
	return ret == 1;
}

int JedisCommand::setrange(const char * key, int offset, const char * value)
{
	long long ret = -1;
	execute(true, getkey(key).c_str(), ret, "SETRANGE %s %d %s", getkey(key).c_str(), offset, value);
	return ret;
}

string JedisCommand::getrange(const char * key, int startOffset, int endOffset)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "GETRANGE %s %d %d", getkey(key).c_str(), startOffset, endOffset);
	return ret;
}

string JedisCommand::getSet(const char * key, const char * value)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "GETSET %s %s", getkey(key).c_str(), value);
	return ret;
}

int JedisCommand::setnx(const char * key, const char * value)
{
	long long ret = -1;
	execute(true, getkey(key).c_str(), ret, "SETNX %s %s", getkey(key).c_str(), value);
	return ret;
}

bool JedisCommand::setex(const char * key, int seconds, const char * value)
{
	return execute(true, getkey(key).c_str(), "SETEX %s %d %s", getkey(key).c_str(), seconds, value);
}

int JedisCommand::decrBy(const char * key, int integer)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "DECRBY %s %d", getkey(key).c_str(), integer);
	return ret;
}

int JedisCommand::decr(const char * key)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "DECR %s", getkey(key).c_str());
	return ret;
}

int JedisCommand::incrBy(const char * key, int integer)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "INCRBY %s %d", getkey(key).c_str(), integer);
	return ret;
}

int JedisCommand::incr(const char * key)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "INCR %s", getkey(key).c_str());
	return ret;
}

int JedisCommand::append(const char * key, const char * value)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "APPEND %s %s", getkey(key).c_str(), value);
	return ret;
}

string JedisCommand::substr(const char * key, int start, int end)
{
	//todo: 后续研究  2012-12-13
	string ret;
	execute(false, getkey(key).c_str(), ret, "SUBSTR %s %d %d", getkey(key).c_str(), start, end);
	return ret;
}

int JedisCommand::hset(const char * key, const char * field, const char * value)
{
	long long ret = -1;
	execute(true, getkey(key).c_str(), ret, "HSET %s %s %s", getkey(key).c_str(), field, value);
	return ret;
}

string JedisCommand::hget(const char * key, const char * field)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "HGET %s %s", getkey(key).c_str(), field);
	return ret;
}

int JedisCommand::hsetnx(const char * key, const char * field, const char * value)
{
	long long ret = -1;
	execute(true, getkey(key).c_str(), ret, "HSETNX %s %s %s", getkey(key).c_str(), field, value);
	return ret;
}

bool JedisCommand::hmset(const char * key, map<string, string> &hash)
{
	string cmd = "HMSET ";
	cmd += getkey(key);

	map<string, string>::iterator iter = hash.begin();
	for (; iter != hash.end(); ++iter)
	{
		cmd += " " + iter->first + " " + iter->second;
	}

	return execute(true, getkey(key).c_str(), cmd.c_str());
}

list<string> JedisCommand::hmget(const char * key, list<string> &fields)
{
	string cmd = "HMGET ";
	cmd += getkey(key);
	list<string>::iterator iter = fields.begin();
	for (; iter != fields.end(); ++iter)
	{
        if(iter->length() == 0)
        	cmd += " ''";
        else
        	cmd += " " + *iter;
	}

	list<string> l;
	execute(false, getkey(key).c_str(), l, cmd.c_str());

	return l;
}

int JedisCommand::hincrBy(const char * key, const char * field, int value)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "HINCRBY %s %s %d", getkey(key).c_str(), field, value);
	return ret;
}

bool JedisCommand::hexists(const char * key, const char * field)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "HEXISTS %s %s", getkey(key).c_str(), field);

	return ret == 1;
}

int JedisCommand::hdel(const char * key, list<string> &field)
{
	long long ret = 0;

	string cmd = "HDEL ";
	cmd += getkey(key);

	list<string>::iterator iter = field.begin();
	for (; iter != field.end(); ++iter)
	{
        if(iter->length() == 0)
        	cmd += " ''";
        else
        	cmd += " " + *iter;
	}

	execute(false, getkey(key).c_str(), ret, cmd.c_str());

	return ret;
}

int JedisCommand::hlen(const char * key)
{
	long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "HLEN %s", getkey(key).c_str());
	return ret;
}

set<string> JedisCommand::hkeys(const char * key)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "HKEYS %s", getkey(key).c_str());
	return ret;
}

list<string> JedisCommand::hvals(const char * key)
{
	list<string> ret;
	execute(false, getkey(key).c_str(), ret, "HVALS %s", getkey(key).c_str());
	return ret;
}

map<string, string> JedisCommand::hgetAll(const char * key)
{
	map<string, string> ret;

	list<string> l;
	if (execute(false, getkey(key).c_str(), l, "HGETALL %s", getkey(key).c_str()))
	{
        list<string>::iterator iter = l.begin();
        while(iter != l.end())
        {
        	string first = *iter;
        	string second = *(++iter);
        	ret.insert(make_pair(first, second));
            if(iter != l.end())
                ++iter;
            else
            	break;
        }
	}
	return ret;
}

int JedisCommand::rpush(const char * key, list<string> &s)
{
	long long ret = 0;

	string cmd = "RPUSH ";
	cmd += getkey(key);

	list<string>::iterator iter = s.begin();
	for (; iter != s.end(); ++iter)
	{
        if(iter->length() == 0)
        	cmd += " ''";
        else
        	cmd += " " + *iter;
	}

	execute(true, getkey(key).c_str(), ret, cmd.c_str());

	return ret;
}

int JedisCommand::lpush(const char * key, list<string> &strings)
{
	long long ret = 0;

	string cmd = "LPUSH ";
	cmd += getkey(key);

	list<string>::iterator iter = strings.begin();
	for (; iter != strings.end(); ++iter)
	{
        if(iter->length() == 0)
        	cmd += " ''";
        else
		    cmd += " " + *iter;
	}

	execute(true, getkey(key).c_str(), ret, cmd.c_str());
//	execute(cmd.c_str(), ret);

	return ret;
}

int JedisCommand::llen(const char * key)
{
	long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "LLEN %s", getkey(key).c_str());
	return ret;
}

list<string> JedisCommand::lrange(const char * key, int start, int end)
{
	list<string> ret;
	execute(false, getkey(key).c_str(), ret, "LRANGE %s %d %d", getkey(key).c_str(), start, end);
	return ret;
}

//status const char *类型，改为bool类型
bool JedisCommand::ltrim(const char * key, int start, int end)
{
	return execute(false, getkey(key).c_str(), "LTRIM %s %d %d", getkey(key).c_str(), start, end);
}

string JedisCommand::lindex(const char * key, int index)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "LINDEX %s %d", getkey(key).c_str(), index);
	return ret;
}

//status const char *类型，改为bool类型
bool JedisCommand::lset(const char * key, int index, const char * value)
{
	return execute(true, getkey(key).c_str(), "LSET %s %d %s", getkey(key).c_str(), index, value);
}

int JedisCommand::lrem(const char * key, int count, const char * value)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "LREM %s %d %s", getkey(key).c_str(), count, value);
	return ret;
}

string JedisCommand::lpop(const char * key)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "LPOP %s", getkey(key).c_str());
	return ret;
}

string JedisCommand::rpop(const char * key)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "RPOP %s", getkey(key).c_str());
	return ret;
}

int JedisCommand::sadd(const char * key, list<string> &member)
{
	long long ret = 0;
	string m = "SADD ";
//    string m ;
	m += getkey(key);
	list<string>::iterator iter = member.begin();
	for (; iter != member.end(); ++iter)
	{
        if(iter->length() == 0)
        	m += " ''";
        else
		    m += " " + *iter;
	}

	execute(true, getkey(key).c_str(), ret, m.c_str());
	return ret;
}

set<string> JedisCommand::smembers(const char * key)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "SMEMBERS %s", getkey(key).c_str());
	return ret;
}

int JedisCommand::srem(const char * key, list<string> &member)
{
	long long ret = 0;
	string m = "SREM ";
	m += getkey(key);
	list<string>::iterator iter = member.begin();
	for (; iter != member.end(); ++iter)
	{
        if(iter->length() == 0)
        	m += " ''";
        else
		    m += " " + *iter;
	}
	execute(false, getkey(key).c_str(), ret, m.c_str());

	return ret;
}

string JedisCommand::spop(const char * key)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "SPOP %s", getkey(key).c_str());
	return ret;
}

int JedisCommand::scard(const char * key)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "SCARD %s", getkey(key).c_str());
	return ret;
}

bool JedisCommand::sismember(const char * key, const char * member)
{
    long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "SISMEMBER %s %s", getkey(key).c_str(), member);
	return ret == 1;
}

string JedisCommand::srandmember(const char * key)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "SRANDMEMBER %s", getkey(key).c_str());
	return ret;
}

int JedisCommand::zadd(const char * key, double score, const char * member)
{
	long long ret = -1;
	if (member==NULL || strlen(member)==0)
		execute(true, getkey(key).c_str(), ret, "zadd %s %f ''", getkey(key).c_str(), score, member);
	else
		execute(true, getkey(key).c_str(), ret, "zadd %s %f %s", getkey(key).c_str(), score, member);
	return ret;
}

int JedisCommand::zadd(const char * key, map<string, double> scoreMembers)
{
	long long ret = -1;

	//=============================================
	string m = "zadd ";
	m += getkey(key);
	map<string, double>::iterator iter = scoreMembers.begin();
	for (; iter != scoreMembers.end(); ++iter)
	{
		char buffer[256] = { 0 };
        	snprintf(buffer, sizeof(buffer) - 1, " %f ", iter->second);
		m+=buffer;
		if (iter->first.length() == 0)
		{
			m+=string("''");
		}
		else
		{
			m+=iter->first;
		}
	}
	//=============================================
	execute(true, getkey(key).c_str(), ret, m.c_str());
	return ret;
}

set<string> JedisCommand::zrange(const char * key, int start, int end)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZRANGE %s %d %d", getkey(key).c_str(), start, end);
	return ret;
}

int JedisCommand::zrem(const char * key, list<string> &member)
{
	long long ret = -1;

	string m = "ZREM ";
	m += getkey(key);
	list<string>::iterator iter = member.begin();
	for (; iter != member.end(); ++iter)
	{
		m += " " + *iter;
	}

	execute(false, getkey(key).c_str(), ret, m.c_str());
	return ret;
}

//todo: 需测试验证
double JedisCommand::zincrby(const char * key, double score, const char * member)
{
	string ret;
	execute(false, getkey(key).c_str(), ret, "zincrby %s %f %s", getkey(key).c_str(), score, member);
	return atof(ret.c_str());
}

int JedisCommand::zrank(const char * key, const char * member)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "ZRANK %s %s", getkey(key).c_str(), member);
	return ret;
}

int JedisCommand::zrevrank(const char * key, const char * member)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "ZREVRANK %s %s", getkey(key).c_str(), member);
	return ret;
}

set<string> JedisCommand::zrevrange(const char * key, int start, int end)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZREVRANGE %s %d %d", getkey(key).c_str(), start, end);
	return ret;
}

int JedisCommand::zcard(const char * key)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "ZCARD %s", getkey(key).c_str());
	return ret;
}

//todo: 需测试验证
double JedisCommand::zscore(const char * key, const char * member)
{
//	long long ret = -1;
	string ret;
	execute(false, getkey(key).c_str(), ret, "ZSCORE %s %s", getkey(key).c_str(), member);

	if (!ret.empty())
	{
		return strtof(ret.c_str(), NULL);
	}
	else
	{
		return -1;
	}
}

//list<string> JedisCommand::sort(const char * key)
//{
//	list<string> ret;
//	execute(getkey(key).c_str(), ret, "SORT %s", getkey(key).c_str());
//	return ret;
//}

int JedisCommand::zcount(const char * key, double min, double max)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "ZCOUNT %s %f %f", getkey(key).c_str(), min, max);
	return ret;
}

int JedisCommand::zcount(const char * key, const char * min, const char * max)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "ZCOUNT %s %s %s", getkey(key).c_str(), min, max);
	return ret;
}

set<string> JedisCommand::zrangeByScore(const char * key, double min, double max)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZRANGEBYSCORE %s %f %f", getkey(key).c_str(), min, max);
	return ret;
}

set<string> JedisCommand::zrangeByScore(const char * key, const char * min, const char * max)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZRANGEBYSCORE %s %s %s", getkey(key).c_str(), min, max);
	return ret;
}

set<string> JedisCommand::zrevrangeByScore(const char * key, double max, double min)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZREVRANGEBYSCORE %s %f %f", getkey(key).c_str(), max, min);
	return ret;
}

set<string> JedisCommand::zrangeByScore(const char * key, double min, double max, int offset, int count)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZRANGEBYSCORE %s %f %f LIMIT %d %d", getkey(key).c_str(), min, max, offset,
			count);
	return ret;
}

set<string> JedisCommand::zrevrangeByScore(const char * key, const char * max, const char * min)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZREVRANGEBYSCORE %s %s %s", getkey(key).c_str(), max, min);
	return ret;
}

set<string> JedisCommand::zrangeByScore(const char * key, const char * min, const char * max, int offset, int count)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZRANGEBYSCORE %s %s %s LIMIT %d %d", getkey(key).c_str(), min, max, offset,
			count);
	return ret;
}

set<string> JedisCommand::zrevrangeByScore(const char * key, double max, double min, int offset, int count)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZREVRANGEBYSCORE %s %f %f LIMIT %d %d", getkey(key).c_str(), max, min, offset,
			count);
	return ret;
}

set<string> JedisCommand::zrevrangeByScore(const char * key, const char * max, const char * min, int offset, int count)
{
	set<string> ret;
	execute(false, getkey(key).c_str(), ret, "ZREVRANGEBYSCORE %s %s %s LIMIT %d %d", getkey(key).c_str(), max, min, offset,
			count);
	return ret;
}

int JedisCommand::zremrangeByRank(const char * key, int start, int end)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "zremrangeByRank %s %d %d", getkey(key).c_str(), start, end);
	return ret;
}

int JedisCommand::zremrangeByScore(const char * key, double start, double end)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "zremrangeByScore %s %f %f", getkey(key).c_str(), start, end);
	return ret;
}

int JedisCommand::zremrangeByScore(const char * key, const char * start, const char * end)
{
	long long ret = -1;
	execute(false, getkey(key).c_str(), ret, "zremrangeByScore %s %s %s", getkey(key).c_str(), start, end);
	return ret;
}

//	int linsert(const char * key, Client.list_POSITION where, const char * pivot, const char * value){}

int JedisCommand::lpushx(const char * key, const char * value)
{
	long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "LPUSHX %s %s", getkey(key).c_str(), value);
	return ret;
}

int JedisCommand::rpushx(const char * key, const char * value)
{
	long long ret = 0;
	execute(false, getkey(key).c_str(), ret, "RPUSHX %s %s", getkey(key).c_str(), value);
	return ret;
}

//--------------------------------------------------------------------------------//
//bool JedisCommand::execute(bool iswrite, const char *key, const char *format, ...)
//{
//	va_list ap;
//	va_start(ap, format);
//	bool ret = execute(iswrite, key, format, ap);
//	ret = execute(iswrite, key, format, ap);
//	va_end(ap);
//
//	return ret;
//}
//
//bool JedisCommand::execute(bool iswrite, const char *key, long long &result, const char *format, ...)
//{
//	va_list ap;
//	va_start(ap, format);
//	bool ret = execute(iswrite, key, result, format, ap);
//	va_end(ap);
//
//	return ret;
//}
//
////下面几个execute由于重载的原因，代码都一样，能否通过模板的方式实现？
//bool JedisCommand::execute(bool iswrite, const char *key, string &result, const char *format, ...)
//{
//	va_list ap;
//	va_start(ap, format);
//	bool ret = execute(iswrite, key, result, format, ap);
//	va_end(ap);
//
//	return ret;
//}
//
//bool JedisCommand::execute(bool iswrite, const char *key, list<string> &result, const char *format, ...)
//{
//	va_list ap;
//	va_start(ap, format);
//	bool ret = execute(iswrite, key, result, format, ap);
//	va_end(ap);
//
//	return ret;
//}
//
//bool JedisCommand::execute(bool iswrite, const char *key, set<string> &result, const char *format, ...)
//{
//	va_list ap;
//	va_start(ap, format);
//	bool ret = execute(iswrite, key, result, format, ap);
//	va_end(ap);
//
//	return ret;
//}
