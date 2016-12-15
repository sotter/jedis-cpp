/******************************************************
 *   FileName: sharded.cpp
 *     Author: liubo  2012-12-5 
 *Description:
 *******************************************************/

#include "sharded.h"
#include "shardinfo.h"
#include "hash.h"
#include <string.h>
#include <stdlib.h>
#include "log.h"

using namespace std;

Sharded::Sharded()
{

}

Sharded::~Sharded()
{
	dlog1("call Sharded::~Sharded()");
	destroy();
}

bool Sharded::init(vector<ShardInfo*> &shard_infos)
{
	char key[128] = { 0 };

	Jedis *j = NULL;
	for (int i = 0; i < (int) shard_infos.size(); i++) {
		/* 如果这个shard_infos中的信息不能创建一个有效的Redis连接，那么继续尝试下一个 */
		if ((j = shard_infos[i]->createResource()) == NULL) {
			delete shard_infos[i];
			shard_infos[i] = NULL;
			continue;
		}

		if (shard_infos[i]->getName().empty()) {
			for (int n = 0; n < 160 * shard_infos[i]->getWeight(); n++) {
				memset(key, 0, sizeof(key));
				snprintf(key, sizeof(key) - 1, "SHARD-%d-NODE-%d", i, n);
				_nodes.insert(make_pair(Hash::hash(key, strlen(key)), shard_infos[i]));
			}
		} else {
			for (int n = 0; n < 160 * shard_infos[i]->getWeight(); n++) {
				memset(key, 0, sizeof(key));
				snprintf(key, sizeof(key) - 1, "%s*%d%d", shard_infos[i]->getName().c_str(),
						shard_infos[i]->getWeight(), n);
				_nodes.insert(make_pair(Hash::hash(key, strlen(key)), shard_infos[i]));
			}
		}

		_resources.insert(make_pair(shard_infos[i], j));
	}

	return !_resources.empty();
}

void Sharded::destroy()
{
	map<ShardInfo *, Jedis*>::iterator iter = _resources.begin();
	for (; iter != _resources.end(); ++iter) {
		delete iter->first;
		delete iter->second;
	}

	_nodes.clear();
	_resources.clear();
}

Jedis *Sharded::getShard(const char *key)
{
	ShardInfo *shard_info = getShardInfo(key);
	if (shard_info == NULL)
		return NULL;
	else
		return _resources[shard_info];
}

ShardInfo *Sharded::getShardInfo(const char *key)
{
	if (_nodes.empty())
		return NULL;

	long long hash_key = Hash::hash(key, strlen(key));

	map<long long, ShardInfo*>::iterator iter = _nodes.find(hash_key);

	if (iter != _nodes.end()) {
		return iter->second;
	} else {
		iter = _nodes.upper_bound(hash_key);
		if (iter != _nodes.end()) {
			return iter->second;
		} else {
			return _nodes.begin()->second;
		}
	}
}

