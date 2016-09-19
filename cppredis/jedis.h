/******************************************************
 *   FileName: main.cpp
 *     Author: liubo  2012-11-26
 *Description: ��ô���趨JEDIS���������ԣ�
 *Description: (1) ִ������������⵽��������������������������ʧ�ܷ��������������һ�����������������ʱ�������С����ʱ�䣬���Զ�������
 *Description: ������������������ִ�е��̣߳������Լ�����һ���̣߳�������漰��ͬ�������⡣
 *Description: (2) 2012-12-14: ������·����ͬһ�����У������ͬ���������������жϵ��൱���ӡ��޸�Ϊһ�������о�ֻ��һ����·���ϲ��ٷ�װһ��
 *Description: �������������·����, ���������ShardInfo����һ������ϡ�
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

// ���ﶨ�������ʱ��Ϊ60��
#define OBJ_LIVE_TIME  120

struct redisContext;
struct redisReply;
class ShardInfo;

// Redis����
class Jedis: public JedisCommand {
	typedef vector<string> VecString;
public:
	Jedis();
	virtual ~Jedis();

	bool init_obj(ShardInfo * shard_info);
	// ��ʼ������
	bool init_obj(const char *ip, unsigned short port);

	string get_name()
	{
		return _name;
	}

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
	virtual bool execute(bool iswrite, const char *key, set<string> &result, const char *format, va_list ap);

	virtual bool execute(const char *cmd, long long &result);
private:
	// ���ӷ�����
	redisContext * connect(const char *ip, int port);
	//���ö�д��ʱʱ��, seconds = 0, useconds = 0, ��ʾ��ֹ��ʱ��
	void set_timeout(int seconds, int useconds);

	// ����������Ϣ���صĽ��, �Ѷ��ĵĹؼ���ֺͽ��ŵ�result��
	bool parse_subs_reply(void *reply, map<string, string> &result);

public:
	string _name;
	redisContext *_redis_context;
	bool _is_connect;                 //����״̬��true ��������״̬��false���ڶϿ�״̬
	int _reconn_timeval;             //������ʱ����
	time_t _last_reconn_time;           //�ϴ�������ʱ��
};

#endif /* REDISPOOL_H_ */
