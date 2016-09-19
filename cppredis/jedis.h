/******************************************************
 *  CopyRight: 北京中交兴路科技有限公司(2012-2015)
 *   FileName: main.cpp
 *     Author: liubo  2012-11-26
 *Description: 怎么样设定JEDIS的重连策略：
 *Description: (1) 执行命令中如果检测到是网络错误，立即重连，如果重连失败放弃这次重连。下一次命令到来，如果相隔的时间大于最小重连时间，便自动重连。
 *Description: 重连的驱动是来自命令执行的线程，不能自己开辟一个线程，这样就涉及到同步的问题。
 *Description: (2) 2012-12-14: 主从链路放在同一对象中，后面的同步，重连，错误判断等相当复杂。修改为一个对象中就只包含一个链路，上层再封装一个
 *Description: 包含两个对象的链路对象, 这个对象与ShardInfo处于一个层次上。
 *******************************************************/

#ifndef __JEDIS_H__
#define __JEDIS_H__

#include <vector>
#include <list>
#include <string>
#include <time.h>
#include <string>
#include <map>
#include <set>
#include "jedis_interface.h"

using namespace std;

// 这里定义对象存活时间为60秒
#define OBJ_LIVE_TIME  120

struct redisContext;
struct redisReply;
class ShardInfo;

// Redis对象
class Jedis : public JedisCommand
{
	typedef vector<string> VecString;
public:
	Jedis();
	virtual ~Jedis();

	bool init_obj(ShardInfo * shard_info);
	// 初始化对象
	bool init_obj(const char *ip, unsigned short port);

    string get_name(){return _name;}

	string get_addr();

	virtual string getkey(const char * key)
	{
		return key;
	}

	virtual bool subscribe(vector<string> &channels);
	virtual bool subscribe_get_reply(string &channel, string &value);

	virtual bool execute(bool iswrite, const char *key, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, long long &result, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, string &result, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, list<string> &result, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, set<string> &result, const char *format, ...);

	virtual bool execute(bool iswrite, const char *key, const char *format, va_list ap);
	virtual bool execute(bool iswrite, const char *key, long long &result, const char *format, va_list ap);
	virtual bool execute(bool iswrite, const char *key, string &result, const char *format, va_list ap);
	virtual bool execute(bool iswrite, const char *key, list<string> &result, const char *format, va_list ap);
	virtual bool execute(bool iswrite, const char *key, set<string> &result, const char *format,  va_list ap);

	virtual bool execute(const char *cmd, long long &result);
private:
	// 连接服务器
	redisContext * connect(const char *ip, int port);
	//设置读写超时时间, seconds = 0, useconds = 0, 表示禁止超时。
    void set_timeout(int seconds, int useconds);

	// 解析订阅信息返回的结果, 把订阅的关键的字和结果，放到result中
    bool parse_subs_reply(void *reply, map<string, string> &result);

public:
    string        _name;
    redisContext  *_redis_context;
    bool          _is_connect;                 //连接状态：true 处于连接状态，false处于断开状态
    int           _reconn_timeval;             //重连的时间间隔
    time_t        _last_reconn_time;           //上次重连的时间
};

#endif /* REDISPOOL_H_ */
