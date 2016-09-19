/******************************************************
 *   FileName: DataCenter.h
 *     Author: liubo  2012-12-21 
 *Description:
 *******************************************************/

#ifndef DATACENTER_H_
#define DATACENTER_H_

#include <string>
#include "vos_thread.h"
#include "vos_lock.h"
#include "jedis_interface.h"
#include "sharded.h"
#include "jedis.h"

using namespace std;

#define CACHE                       "cache"
#define CONFIG_DB                   "config"
#define CONFIG_ADDRESS_KEY          "address"
#define CONFIG_ADDRESS_OLDKEY       "oldaddress"
#define CONFIG_DB_KEY               "db"

#define CONFIG_SHARDCHANNEL_KEY       "shardchannel"
#define CONFIG_SHARDCHANNELSTATUS_KEY "shardchannelstatus"

#define KEY_SEPARATOR               "-"
#define ADDRESS_SEPARATOR           ":"
#define SHARD_NORMAL_STATUS         "0"
#define SHARD_START_STATUS          "1"
#define SHARD_END_STATUS            "2"
#define SHARD_CHANGE_STATUS         "3"

#define POOL_MAXACTIVE              500
#define POOL_MAXIDLE                1000 * 60
#define POOL_MAXWAIT                1000 * 10
#define SHARDPOOL_MAXACTIVE         500
#define SHARDPOOL_MAXIDLE           1000 * 60
#define SHARDPOOL_MAXWAIT           1000 * 10

class DataCenter;
class RedisDB;
class RedisTable;

class DataCenter: public vos::RunThread {
public:
	enum SYN_STATE {
		normal/* ��״*/, syning/* ����ͬ�� */, toconvert/* ͬ���Ѿ����׼���л����л���ɺ���toconvert״ */, change = 3
	};

	DataCenter();
	virtual ~DataCenter();

	bool init(const char *cfgip, unsigned short cfgport);

	virtual int run_thread(int type);

	RedisDB get_redisdb(const char *name);

	set<string> get_database_names();

	bool existdb(const char *dbname);

	SYN_STATE get_state()
	{
		return _state;
	}
	string get_key(const char *key);
	Sharded * get_sharded();
	Sharded * get_shardednew();
	Jedis & get_cfg_jedis();

	vos::CRWLock &get_wrlock()
	{
		return _wrlock;
	}
private:
	bool init_sharded(Sharded *sharded, const char *address);
	bool init_sharded_state();
	void handle_subs(string &channel, string &value);
private:
	vos::ThreadPool _thread_pool;
	vos::CRWLock _wrlock;
	Sharded *_sharded;
	//ͬ����ʱ��ʹ�õ�?
	Sharded *_sharded_new;
	SYN_STATE _state;
	Jedis _jedis; //�����÷��������ӵ�jedis����
	Jedis _subs_jedis;
};

class RedisDB {
public:
	RedisDB(string name, DataCenter *d = NULL) :
			_dbname(name), _data_center(d)
	{
	}
	~RedisDB()
	{
	}

	const string &get_db_name()
	{
		return _dbname;
	}

	RedisTable get_redis_table(const char *name, int timeout = 0);

	set<string> get_table_names();

	bool existtable(const char *table);

	DataCenter *get_data_center()
	{
		return _data_center;
	}

	void set_data_center(DataCenter *d)
	{
		_data_center = d;
	}

private:
	string _dbname;
	DataCenter *_data_center;
};

class RedisTable: public JedisCommand {
public:
	RedisTable(string name, RedisDB *db, int timeout = 0) :
			_tablename(name), _redis_db(db), _timeout(timeout)
	{
		_data_center = db->get_data_center();
	}

	~RedisTable()
	{
	}

	string get_jedis_addr(const char *key);

	virtual string getkey(const char * key);

	//���������ĸ��ӿڣ���Ҫ��д���������ļӼ������÷�������ʵ�x
	virtual int decrBy(const char * key, int integer);

	virtual int decr(const char * key);

	virtual int incrBy(const char * key, int integer);

	virtual int incr(const char * key);

	virtual bool execute(bool iswrite, const char *key, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, long long &result, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, string &result, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, list<string> &result, const char *format, ...);
	virtual bool execute(bool iswrite, const char *key, set<string> &result, const char *format, ...);

	bool do_jedis_execute(Jedis *jedis, bool iswrite, const char *key, const char *format, va_list ap);
	bool do_jedis_new_execute(Jedis *jedis, bool iswrite, const char *key, const char *format, va_list ap);

private:
	void set_expire(Jedis *jedis, const char *key);
private:
	string _tablename;
	RedisDB *_redis_db;
	DataCenter *_data_center;
	int _timeout;
};

#endif /* DATACENTER_H_ */
