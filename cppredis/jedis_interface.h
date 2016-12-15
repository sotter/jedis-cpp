/******************************************************
 *   FileName: jedis_interface.h
 *     Author: liubo  2012-12-12 
 *Description:
 *******************************************************/

#ifndef JEDIS_INTERFACE_H_
#define JEDIS_INTERFACE_H_

#include <string>
#include <list>
#include <map>
#include <set>
#include <vector>

using namespace std;

struct redisContext;
struct redisReply;
class JedisCommand {
public:
	JedisCommand()
	{
	}
	virtual ~JedisCommand()
	{
	}
	//reply为空的情况认为是错误的，这时候一般c中也会有错误发生
	bool check_error(redisContext *c);
	bool check_error(redisReply *reply);

	virtual string getkey(const char * key) = 0;
	// 做测试使用，获得reply的值
	string showreply(void *reply);

	string get_context_addr(redisContext *c);

	virtual int del(const char *key);

	virtual bool setvalue(const char * key, const char * value);

	virtual string get(const char * key);

	virtual bool exists(const char * key);

	virtual string type(const char * key);

	virtual int expire(const char * key, int seconds);

	virtual int expireAt(const char * key, int unixTime);

	virtual int ttl(const char * key);

	virtual bool setbit(const char * key, int offset, bool value);

	virtual bool getbit(const char * key, int offset);

	virtual int setrange(const char * key, int offset, const char * value);

	virtual string getrange(const char * key, int startOffset, int endOffset);

	virtual string getSet(const char * key, const char * value);

	virtual int setnx(const char * key, const char * value);

	virtual bool setex(const char * key, int seconds, const char * value);

	virtual int decrBy(const char * key, int integer);

	virtual int decr(const char * key);

	virtual int incrBy(const char * key, int integer);

	virtual int incr(const char * key);

	virtual int append(const char * key, const char * value);

	virtual string substr(const char * key, int start, int end);

	virtual int hset(const char * key, const char * field, const char * value);

	virtual string hget(const char * key, const char * field);

	virtual int hsetnx(const char * key, const char * field, const char * value);

	virtual bool hmset(const char * key, map<string, string> &hash);

	virtual list<string> hmget(const char * key, list<string> &fields);

	virtual int hincrBy(const char * key, const char * field, int value);

	virtual bool hexists(const char * key, const char * field);

	virtual int hdel(const char * key, list<string> &field);

	virtual int hlen(const char * key);

	virtual set<string> hkeys(const char * key);

	virtual list<string> hvals(const char * key);

	virtual map<string, string> hgetAll(const char * key);

	virtual int rpush(const char * key, list<string> &s);

	virtual int lpush(const char * key, list<string> &s);

	virtual int llen(const char * key);

	virtual list<string> lrange(const char * key, int start, int end);

	virtual bool ltrim(const char * key, int start, int end);

	virtual string lindex(const char * key, int index);

	virtual bool lset(const char * key, int index, const char * value);

	virtual int lrem(const char * key, int count, const char * value);

	virtual string lpop(const char * key);

	virtual string rpop(const char * key);

	virtual int sadd(const char * key, list<string> &member);

	virtual set<string> smembers(const char * key);

	virtual int srem(const char * key, list<string> &member);

	virtual string spop(const char * key);

	virtual int scard(const char * key);

	virtual bool sismember(const char * key, const char * member);

	virtual string srandmember(const char * key);

	virtual int zadd(const char * key, double score, const char * member);

	virtual int zadd(const char * key, map<string, double> scoreMembers);

	virtual set<string> zrange(const char * key, int start, int end);

	virtual int zrem(const char * key, list<string> &member);

	virtual double zincrby(const char * key, double score, const char * member);

	virtual int zrank(const char * key, const char * member);

	virtual int zrevrank(const char * key, const char * member);

	virtual set<string> zrevrange(const char * key, int start, int end);

	virtual int zcard(const char * key);

	virtual double zscore(const char * key, const char * member);

//	virtual list<string> sort(const char * key);

	virtual int zcount(const char * key, double min, double max);

	virtual int zcount(const char * key, const char * min, const char * max);

	virtual set<string> zrangeByScore(const char * key, double min, double max);

	virtual set<string> zrangeByScore(const char * key, const char * min, const char * max);

	virtual set<string> zrevrangeByScore(const char * key, double max, double min);

	virtual set<string> zrangeByScore(const char * key, double min, double max, int offset, int count);

	virtual set<string> zrevrangeByScore(const char * key, const char * max, const char * min);

	virtual set<string> zrangeByScore(const char * key, const char * min, const char * max, int offset, int count);

	virtual set<string> zrevrangeByScore(const char * key, double max, double min, int offset, int count);

	virtual set<string> zrevrangeByScore(const char * key, const char * max, const char * min, int offset, int count);

	virtual int zremrangeByRank(const char * key, int start, int end);

	virtual int zremrangeByScore(const char * key, double start, double end);

	virtual int zremrangeByScore(const char * key, const char * start, const char * end);

	virtual int lpushx(const char * key, const char * value);

	virtual int rpushx(const char * key, const char * value);

	virtual bool execute(bool iswrite, const char *key, const char *format, ...)
	{
		return false;
	}
	virtual bool execute(bool iswrite, const char *key, long long &result, const char *format, ...)
	{
		return false;
	}
	virtual bool execute(bool iswrite, const char *key, string &result, const char *format, ...)
	{
		return false;
	}
	virtual bool execute(bool iswrite, const char *key, list<string> &result, const char *format, ...)
	{
		return false;
	}
	virtual bool execute(bool iswrite, const char *key, set<string> &result, const char *format, ...)
	{
		return false;
	}

};

#endif /* JEDIS_INTERFACE_H_ */
