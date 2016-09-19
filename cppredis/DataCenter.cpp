/******************************************************
 *   FileName: DataCenter.cpp
 *     Author: liubo  2012-12-21 
 *Description:
 *******************************************************/

#include "DataCenter.h"
#include "utility.h"
#include "shardinfo.h"
#include <unistd.h>
#include "log.h"

DataCenter::DataCenter()
{
	_state = normal;
	_sharded = new Sharded;
	_sharded_new = new Sharded;
}

DataCenter::~DataCenter()
{
	_thread_pool.stop(5);
	if (_sharded != NULL) {
		delete _sharded;
		_sharded = NULL;
	}
	if (_sharded_new != NULL) {
		delete _sharded_new;
		_sharded_new = NULL;
	}
}

bool DataCenter::init(const char *cfgip, unsigned short cfgport)
{
	if (!_jedis.init_obj(cfgip, cfgport)) {
		dlog1("init config server fail");
		return false;
	}

	if (!init_sharded_state()) {
		dlog1("get shardchannelstatus fail");
		return false;
	}

	if (_state == syning) {
		if (!init_sharded(_sharded_new, CONFIG_ADDRESS_KEY)) {
			dlog1("init shard_new fail in syning");
			return false;
		}
		if (!init_sharded(_sharded, CONFIG_ADDRESS_OLDKEY)) {
			dlog1("init sharded fail in syning");
			return false;
		}
	} else {
		if (!init_sharded(_sharded, CONFIG_ADDRESS_KEY)) {
			dlog1("init shard fail");
			return false;
		}
	}

	if (!_subs_jedis.init_obj(cfgip, cfgport)) {
		dlog1("init subscribe jedis fail");
		return false;
	}

	_thread_pool.regist(0, this, 1);
	_thread_pool.start();
	return true;
}

int DataCenter::run_thread(int type)
{
	bool running = true;

	string sub_key = CONFIG_SHARDCHANNEL_KEY;

	vector<string> channels;
	channels.push_back(sub_key);

	/* һֱ���ģ�ֱ�����ĳɹ�Ϊֹ  */
	while (!_subs_jedis.subscribe(channels)) {
		dlog1("subscribe fail, try again");
		sleep(1);
	}

	while (running) {
		string channel;
		string value;

		if (_subs_jedis.subscribe_get_reply(channel, value)) {
			handle_subs(channel, value);
		}
	}

	return -1;
}

void DataCenter::handle_subs(string &channel, string &value)
{
	dlog1("subscribe channel : %s value : %s \n", channel.c_str(), value.c_str());

	if (channel == CONFIG_SHARDCHANNEL_KEY) {
		if (value == SHARD_START_STATUS) {
			vos::CAutoGuardWR wrlock(_wrlock);       //дͬ��, ��ʱGetSharded�Ĳ���������ס
			if (init_sharded(_sharded_new, CONFIG_ADDRESS_KEY)) {
				dlog1("state convert to syning");
				_state = syning;
			}
		} else if (value == SHARD_NORMAL_STATUS) {
			vos::CAutoGuardWR wrlock(_wrlock);       //дͬ��, ��ʱGetSharded�Ĳ���������ס
			Sharded *temp = _sharded;
			_sharded->destroy();
			_sharded = _sharded_new;
			_sharded_new = temp;
			_state = normal;
			dlog1("shard<->shard_new convert finish ");
		} else if (value == SHARD_CHANGE_STATUS)        //�滻ĳһ̨����,����������ӻ�
		{
			vos::CAutoGuardWR wrlock(_wrlock);
			init_sharded(_sharded, CONFIG_ADDRESS_KEY);
		}

		return;
	}
}

bool DataCenter::init_sharded_state()
{
	string key = CONFIG_DB;
	key += KEY_SEPARATOR;
	key += CACHE;
	key += KEY_SEPARATOR;
	key += CONFIG_SHARDCHANNELSTATUS_KEY;

	string value = _jedis.get(key.c_str());
	if (value.empty())
		return false;

	_state = (SYN_STATE) atoi(value.c_str());
	return true;
}

bool DataCenter::init_sharded(Sharded *sharded, const char *address)
{
	string address_key = CONFIG_DB;
	address_key += KEY_SEPARATOR;
	address_key += CACHE;
	address_key += KEY_SEPARATOR;
	address_key += address;

	set<string> adds = _jedis.hkeys(address_key.c_str());
	if (adds.empty())
		return false;

	set<string>::iterator iter = adds.begin();
	vector<ShardInfo *> vec_infos;
	for (; iter != adds.end(); ++iter) {
		string addr = _jedis.hget(address_key.c_str(), iter->c_str());
		if (!addr.empty()) {
			vector<string> vec;
			if (utility::split2vec(addr, vec, ":", 2)) {
				vec_infos.push_back(new ShardInfo(vec[0], atoi(vec[1].c_str()), Protocol::DEFAULT_WEIGHT, *iter));
			}
		}
	}

	if (!vec_infos.empty()) {
		sharded->destroy();
		if (!sharded->init(vec_infos)) {
			dlog1("init shard fail");
			return false;
		}
		dlog1("init shard success");
	}
	return true;
}

RedisDB DataCenter::get_redisdb(const char *name)
{
	RedisDB redisdb(name, this);
	return redisdb;
}

string DataCenter::get_key(const char *key)
{
	string ret;
	ret = ret + CONFIG_DB + KEY_SEPARATOR + CACHE + KEY_SEPARATOR + key;
	return ret;
}

set<string> DataCenter::get_database_names()
{
	return _jedis.smembers(get_key(CONFIG_DB_KEY).c_str());
}

bool DataCenter::existdb(const char *dbname)
{
	return _jedis.sismember(get_key(CONFIG_DB_KEY).c_str(), dbname);

}

Sharded *DataCenter::get_sharded()
{
	return _sharded;
}

Sharded *DataCenter::get_shardednew()
{
	return _sharded_new;
}

Jedis & DataCenter::get_cfg_jedis()
{
	return _jedis;
}

RedisTable RedisDB::get_redis_table(const char *name, int timeout)
{
	return RedisTable(name, this, timeout);
}

set<string> RedisDB::get_table_names()
{
	return _data_center->get_cfg_jedis().hkeys(_data_center->get_key(get_db_name().c_str()).c_str());
}

bool RedisDB::existtable(const char *table)
{
	string key = _data_center->get_key(get_db_name().c_str());
	string field = get_db_name() + KEY_SEPARATOR + table;
	return _data_center->get_cfg_jedis().hexists(key.c_str(), field.c_str());
}

string RedisTable::get_jedis_addr(const char *key)
{
	string addr;
	Jedis *jedis = _data_center->get_sharded()->getShard(getkey(key).c_str());
	if (jedis != NULL) {
		addr = jedis->get_addr();
	}

	return addr;
}

string RedisTable::getkey(const char * key)
{
	return _redis_db->get_db_name() + KEY_SEPARATOR + _tablename + KEY_SEPARATOR + key;
}

int RedisTable::decrBy(const char * key, int integer)
{
	return _data_center->get_cfg_jedis().decrBy(getkey(key).c_str(), integer);
}

int RedisTable::decr(const char * key)
{
	return _data_center->get_cfg_jedis().decr(getkey(key).c_str());
}

int RedisTable::incrBy(const char * key, int integer)
{
	return _data_center->get_cfg_jedis().incrBy(getkey(key).c_str(), integer);
}

int RedisTable::incr(const char * key)
{
	return _data_center->get_cfg_jedis().incr(getkey(key).c_str());
}

bool RedisTable::do_jedis_execute(Jedis *jedis, bool iswrite, const char *key, const char *format, va_list ap)
{
	bool ret = false;
	if (jedis != NULL) {
		ret = jedis->execute(iswrite, key, format, ap);
		if (iswrite)
			set_expire(jedis, key);
	} else
		ret = false;
	return ret;

}

bool RedisTable::do_jedis_new_execute(Jedis *jedis, bool iswrite, const char *key, const char *format, va_list ap)
{
	bool ret = true;
	if (_data_center->get_state() == DataCenter::syning && iswrite) {
		Jedis *j = _data_center->get_shardednew()->getShard(key);
		if (j != NULL && j->get_name() != jedis->get_name()) //������������»��;ɻ��϶�������ͬһ��Jedis����ô�����κβ�����
				{
			printf("call j->execute \n");
			ret = j->execute(iswrite, key, format, ap);
			set_expire(j, key);
		}
	}

	return ret;
}

bool RedisTable::execute(bool iswrite, const char *key, const char *format, ...)
{
	vos::CAutoGuardRD rdlock(_data_center->get_wrlock());
	Jedis *jedis = NULL;

	bool ret = false;
	jedis = _data_center->get_sharded()->getShard(key);
	if (jedis != NULL) {
		va_list ap;
		va_start(ap, format);
		ret = jedis->execute(iswrite, key, format, ap);
		va_end(ap);

		if (iswrite)
			set_expire(jedis, key);
	} else
		ret = false;

	if (_data_center->get_state() == DataCenter::syning && iswrite) {
		Jedis *j = _data_center->get_shardednew()->getShard(key);
		if (j != NULL && j->get_name() != jedis->get_name()) //������������»��;ɻ��϶�������ͬһ��Jedis����ô�����κβ�����
				{
			va_list ap;
			va_start(ap, format);
			ret = jedis->execute(iswrite, key, format, ap);
			va_end(ap);

			set_expire(j, key);
		}
	}

	return ret;
}

bool RedisTable::execute(bool iswrite, const char *key, long long &result, const char *format, ...)
{
	vos::CAutoGuardRD rdlock(_data_center->get_wrlock());
	Jedis *jedis = NULL;

	bool ret = false;
	jedis = _data_center->get_sharded()->getShard(key);
	if (jedis != NULL) {
		va_list ap;
		va_start(ap, format);
		ret = jedis->execute(iswrite, key, result, format, ap);
		va_end(ap);

		if (iswrite)
			set_expire(jedis, key);
	} else
		ret = false;

	if (_data_center->get_state() == DataCenter::syning && iswrite) {
		Jedis *j = _data_center->get_shardednew()->getShard(key);
		if (j != NULL && j->get_name() != jedis->get_name()) //������������»��;ɻ��϶�������ͬһ��Jedis����ô�����κβ�����
				{
			va_list ap;
			va_start(ap, format);
			ret = jedis->execute(iswrite, key, result, format, ap);
			va_end(ap);

			set_expire(j, key);
		}
	}

	return ret;
}

bool RedisTable::execute(bool iswrite, const char *key, string &result, const char *format, ...)
{
	vos::CAutoGuardRD rdlock(_data_center->get_wrlock());
	Jedis *jedis = NULL;

	bool ret = false;
	jedis = _data_center->get_sharded()->getShard(key);
	if (jedis != NULL) {
		va_list ap;
		va_start(ap, format);
		ret = jedis->execute(iswrite, key, result, format, ap);
		va_end(ap);

		if (iswrite)
			set_expire(jedis, key);
	} else
		ret = false;

	if (_data_center->get_state() == DataCenter::syning && iswrite) {
		Jedis *j = _data_center->get_shardednew()->getShard(key);
		if (j != NULL && j->get_name() != jedis->get_name()) //������������»��;ɻ��϶�������ͬһ��Jedis����ô�����κβ�����
				{
			va_list ap;
			va_start(ap, format);
			ret = jedis->execute(iswrite, key, result, format, ap);
			va_end(ap);

			set_expire(j, key);
		}
	}

	return ret;
}

bool RedisTable::execute(bool iswrite, const char *key, list<string> &result, const char *format, ...)
{
	vos::CAutoGuardRD rdlock(_data_center->get_wrlock());
	Jedis *jedis = NULL;

	bool ret = false;
	jedis = _data_center->get_sharded()->getShard(key);
	if (jedis != NULL) {
		va_list ap;
		va_start(ap, format);
		ret = jedis->execute(iswrite, key, result, format, ap);
		va_end(ap);

		if (iswrite)
			set_expire(jedis, key);
	} else
		ret = false;

	if (_data_center->get_state() == DataCenter::syning && iswrite) {
		Jedis *j = _data_center->get_shardednew()->getShard(key);
		if (j != NULL && j->get_name() != jedis->get_name()) //������������»��;ɻ��϶�������ͬһ��Jedis����ô�����κβ�����
				{
			va_list ap;
			va_start(ap, format);
			ret = jedis->execute(iswrite, key, result, format, ap);
			va_end(ap);

			set_expire(j, key);
		}
	}

	return ret;
}

bool RedisTable::execute(bool iswrite, const char *key, set<string> &result, const char *format, ...)
{
	vos::CAutoGuardRD rdlock(_data_center->get_wrlock());
	Jedis *jedis = NULL;

	bool ret = false;
	jedis = _data_center->get_sharded()->getShard(key);
	if (jedis != NULL) {
		va_list ap;
		va_start(ap, format);
		ret = jedis->execute(iswrite, key, result, format, ap);
		va_end(ap);

		if (iswrite)
			set_expire(jedis, key);
	} else
		ret = false;

	if (_data_center->get_state() == DataCenter::syning && iswrite) {
		Jedis *j = _data_center->get_shardednew()->getShard(key);
		if (j != NULL && j->get_name() != jedis->get_name()) //������������»��;ɻ��϶�������ͬһ��Jedis����ô�����κβ�����
				{
			va_list ap;
			va_start(ap, format);
			ret = jedis->execute(iswrite, key, result, format, ap);
			va_end(ap);

			set_expire(j, key);
		}
	}

	return ret;
}

void RedisTable::set_expire(Jedis *jedis, const char *key)
{
	if (_timeout > 0) {
		jedis->expire(key, _timeout);
	}
}

