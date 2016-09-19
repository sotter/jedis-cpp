/******************************************************
*  CopyRight: 北京中交兴路科技有限公司(2012-2015)
*   FileName: hash.h
*     Author: liubo  2012-11-30 
*Description:
*******************************************************/

#ifndef __HASH_H____
#define __HASH_H____

#include <openssl/md5.h>
#include <string>
#include <string.h>
#include <stdlib.h>

class Hash
{
public:
	static long long hash(const char *value, int len)
	{
		unsigned char md5[16] = { 0 };

		MD5_CTX c;
		MD5_Init(&c);
		MD5_Update(&c, value, len);
		MD5_Final(md5, &c);

		long long ret =    (long long) ((md5[3] & 0xFF) << 24)
				| (long long) ((md5[2] & 0xFF) << 16)
				| (long long) ((md5[1] & 0xFF) << 8)
				| (long long) (md5[0] & 0xFF);

        return ret;
	}
};

#endif /* HASH_H_ */
