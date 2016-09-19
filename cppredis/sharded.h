/******************************************************
 *  CopyRight: 北京中交兴路科技有限公司(2012-2015)
 *   FileName: sharded.h
 *     Author: liubo  2012-12-5 
 *Description:
 *******************************************************/

#ifndef SHARDED_H_
#define SHARDED_H_

#include <map>
#include <vector>

class Jedis;
class ShardInfo;

class Sharded
{
public:
	Sharded();
	virtual ~Sharded();

	bool init(std::vector<ShardInfo*> &vec);

	Jedis *getShard(const char *key);

	bool empty(){return _nodes.empty();}

    void destroy();

private:
	ShardInfo *getShardInfo(const char *key);
private:

	std::map<long long, ShardInfo*> _nodes;
	std::map<ShardInfo *, Jedis*> _resources;
};

#endif /* SHARDED_H_ */
