/******************************************************
*  CopyRight: 北京中交兴路科技有限公司(2012-2015)
*   FileName: main.cpp
*     Author: liubo  2012-11-26 
*Description:
*******************************************************/
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <list>
#include "jedis.h"
#include "shardinfo.h"
#include "jedis_test.h"
#include "log.h"

using namespace std;

const int iMaxThread = 1;
const int iMaxCount = 1000000;
const int iMaxSubCount = 10;
const int iMaxTestCount = 100000;

const char g_dbname[] = "TESTDB";
const char g_tablename[]="TESTTABLE";

const char * g_c_keystring[]=
{
	"keyStr1_C",
	"keyStr2_C",
	"keyStr3_C",
	"keyStr4_C",
	"keyStr5_C",
	"keyStr6_C",
	"字符串键7_C"
};

const char *g_c_valuestring[]=
{
	"Test_keyStr1",
	"Test_keyStr2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
//	"Test_keyStr2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ￥?\\n\\//",
//	"Test_keyStr2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ??\\n\\//",
	"中交兴路字符串写入测试3，开始测试啦。",
	"",
	"Test_keyStr5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"C长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit",
	"中文键名测试"
};

const char * g_java_keystring[]=
{
	"keyStr1",
	"keyStr2",
	"keyStr3",
	"keyStr4",
	"keyStr5",
	"keyStr6",
	"字符串键7"
};

const char *g_java_valuestring[]=
{
	"Test_keyStr1",
	"Test_keyStr2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
//	"Test_keyStr2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ??\\n\\//",
//	"Test_keyStr2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ￥?\\n\\//",
	"中交兴路字符串写入测试3，开始测试啦。",
	"",
	"Test_keyStr5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"JAVA长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit",
	"中文键名测试"
};

const char * g_c_keyhash[]=
{
	"keyHash1_C",
	"keyHash2_C",
	"keyHash3_C",
	"keyHash4_C",
	"keyHash5_C",
	"keyHash6_C",
	"keyHash7_C",
	"keyHash1_C",
};

const char * g_c_fieldhash[]=
{
	"filed1",
	"filed",
	"filed",
	"",
	"filed",
	"filed",
	"filed",
	"filed8"
};

const char *g_c_valuehash[]=
{
	"Test_keyHash1",
	"Test_keyHash2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路哈希写入测试3，开始测试啦。",
	"",
	"",
	"Test_keyHash6-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"C长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit",
	"中交兴路哈希写入测试8-添加一对键值测试。"	
	
};

const char * g_java_keyhash[]=
{
	"keyHash1",
	"keyHash2",
	"keyHash3",
	"keyHash4",
	"keyHash5",
	"keyHash6",
	"keyHash7",
	"keyHash1"
};

const char *g_java_valuehash[]=
{
	"Test_keyHash1",
	"Test_keyHash2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路哈希写入测试3，开始测试啦。",
	"",
	"",
	"Test_keyHash6-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"JAVA长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit",
	"中交兴路哈希写入测试8-添加一对键值测试。"
};

const char * g_c_keylist[]=
{
	"keyList1_C",
	"keyList2_C",
	"keyList3_C",
	"keyList4_C",
	"keyList5_C",
	"keyList6_C"
};

const char * g_c_valuelist[]=
{
	"Test_keyList1",
	"Test_keyList2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",		
//	"Test_keyList2~!`@#$%^&*()_+=-:;<>?/.,|\'{}[11 22]※λΩβΣ??\\0\\n\\//",
	"中交兴路字符串链表写入测试3，开始测试啦。",
	"",
	"Test_keyList5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"C长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit"
//	"头部添加值1""头部添加值2""头部添加值3"	,
//	"尾部添加值1""尾部添加值2""尾部添加值3"	
};

const char * g_java_keylist[]=
{
	"keyList1",
	"keyList2",
	"keyList3",
	"keyList4",
	"keyList5",
	"keyList6"
};

const char * g_java_valuelist[]=
{
	"Test_keyList1",
	"Test_keyList2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路字符串链表写入测试3，开始测试啦。",
	"",
	"Test_keyList5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"JAVA长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit"
//	"头部添加值1""头部添加值2""头部添加值3"	,
//	"尾部添加值1""尾部添加值2""尾部添加值3"	
};

const char * g_c_keyset[]=
{
	"keySet1_C",
	"keySet2_C",
	"keySet3_C",
	"keySet4_C",
	"keySet5_C",
	"keySet6_C"
};

const char *g_c_valueset[]=
{
	"Test_keySet1",
	"Test_keySet2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路字符集写入测试3，开始测试啦。",
	"",
	"Test_keySet5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"C长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit"
};

const char * g_java_keyset[]=
{
	"keySet1",
	"keySet2",
	"keySet3",
	"keySet4",
	"keySet5",
	"keySet6"
};

const char *g_java_valueset[]=
{
	"Test_keySet1",
	"Test_keySet2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路字符集写入测试3，开始测试啦。",
	"",
	"Test_keySet5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"JAVA长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit"
};

const char * g_c_keysortset[]=
{
	"keySortSet1_C",
	"keySortSet2_C",
	"keySortSet3_C",
	"keySortSet4_C",
	"keySortSet5_C",
	"keySortSet6_C"
};

const char *g_c_valuesortset[]=
{
	"Test_keySortSet1",
	"Test_keySortSet2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路有序字符集写入测试3，开始测试啦。",
	"",
	"Test_keySortSet5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"C长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit"
};

const char * g_java_keysortset[]=
{
	"keySortSet1",
	"keySortSet2",
	"keySortSet3",
	"keySortSet4",
	"keySortSet5",
	"keySortSet6"
};

const char *g_java_valuesortset[]=
{
	"Test_keySortSet1",
	"Test_keySortSet2~!`@#$^&*()_+=-:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//",
	"中交兴路有序字符集写入测试3，开始测试啦。",
	"",
	"Test_keySortSet5-start@#$^&*()+=:;<>?/.,|\\'{}[1122]※λΩβΣ\\n\\//中交兴路有序字符集写入测试3，开始测试啦-end",
	"JAVA长数据-Test_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exitTest_keyStr5~!{12345678901234567890}[1122]+‘中交兴路字符串写入测试3，开始测试啦。’exit"
};

const char *g_testdata[]=
{
	"test.",
	"test~!@#$%^&*()_+=-:;<>?/.,|\',",
	"中交兴路",
	"中交兴路2012~!@#$%^&*()_+=-:;<>?/.,|\'"
};

sem_t sem_quit;

string itostr(int key)
{
    char buffer[32] = {0};
    sprintf(buffer, "%d", key);
    return buffer;
}

string lltostr(long long key)
{
    char buffer[32] = {0};
    sprintf(buffer, "%lld", key);
    return buffer;
}

long long ustime(void)
{
    struct timeval tv;
    long long ust;

    gettimeofday(&tv, NULL);
    ust = ((long)tv.tv_sec)*1000000;
    ust += tv.tv_usec;
    return ust;
}

void StartThread(void * pParam)
{
	JedisTest test;
	test.TestCapability("192.168.5.66", 6381);
	sem_post(&sem_quit);
}

string u2g(string u)
{
	string strResult;
	CodeConverter aCode("utf-8","gbk");
	strResult =  aCode.convert(u);
	return(strResult);
}

string g2u(string g)
{
	CodeConverter aCode("gbk","utf-8");
	return aCode.convert(g);
}

string replace_all(string& str,const string& old_value,const string& new_value) 
{ 
	string::size_type pos(0); 
	while(true) { 
		if((pos=str.find(old_value,pos))!=string::npos) 
		{
			str.replace(pos,old_value.length(),new_value); 
			pos+=new_value.length();
		}
		else break;
	}
	return str; 
} 

string& dress(string& str,bool breplace = true)
{
	if (breplace)
	{
		replace_all(str,string("%"),string("%%"));
	//	replace_all(str,string("'"),string("\\'"));
	}
	
//	string::size_type pos(0); 	
//	if (str.find(string(" "),pos)!=string::npos)
//	{
//		str = string("'")+str+string("'");
//	}
	return str;
}

int main(int argc, char *argv[])
{
//	{
//		JedisTest test;
//	   	test.TestFunction("192.168.5.66", 6381);
//	}
/*
	{
		JedisTest test;
		test.InitExport("192.168.5.66", 6381);
	//	test.TestChinese("192.168.5.66", 6381);
	//	test.Write1MString("192.168.5.66", 6381);
	//   	test.TestConsistence("192.168.5.66", 6381,"a.txt");
	}
*/
	INIT_LOG(NULL, LOG_FILE, "mytest", 3);

//	string a ="1234%12345%";
//	a = replace_all(a,string("%"),string("%%"));
//	cout << a << endl;

//	string a = g2u("Test_keySortSet2~!`@#$%^&*()_+=-:;<>?/.,|\'{}[11 22]※λΩβΣ??\\0\\n\\//");
//	string b = u2g(a);
//	cout << b << endl;
//	assert(b == "Test_keySortSet2~!`@#$%^&*()_+=-:;<>?/.,|\'{}[11 22]※λΩβΣ??\\0\\n\\//");	

//	while(1)
//	{
//		JedisTest test;
//		test.RunExport("192.168.5.66", 6381);
//	}
//	return(-1);
//	{
//		JedisTest test;
//		test.InitExport("192.168.5.66", 6381);		
//		JedisTest test1;
//		test1.InitExport("192.168.5.66", 6384);		
//	}

	int mode = 1;
	if (argc>=2)
	{
		int i = atoi(argv[1]);
		if (i < 1) i=1;
		if (i>5) i=5;
		mode =i;
	}

	if (mode<=3)
	{
		JedisTest test;
	   	test.TestCommonFunction("192.168.100.113", 6379,mode);		
	//   	test.TestCommonFunction("192.168.5.66", 6381,mode);
	}
	else{
		if (mode==4)
		{
			JedisTest test;
			test.InitExport("192.168.100.113", 6379);		
		}

		if (mode==5)
		{
			while(1)
			{
				JedisTest test;
				test.TestExport("192.168.100.113", 6379);	
			}
		}
	}

	return -1;
	long long starttime = ustime();

	sem_init(&sem_quit,0,0);
	{
		int aMaxThread = iMaxThread;
		for (int i=0;i<aMaxThread;i++)
		{
			pthread_t aThread;
			pthread_attr_t attr_aThread;
			pthread_attr_init(&attr_aThread);
			pthread_attr_setdetachstate(&attr_aThread,PTHREAD_CREATE_DETACHED);
			pthread_create(&aThread,&attr_aThread,(void *(*)(void *))StartThread,NULL);
		}		
	}

	for (int i=0;i<iMaxThread;i++)
		sem_wait(&sem_quit);

	sem_destroy(&sem_quit);

	long long endtime = ustime();

	cout << "Total:" << endtime - starttime << "us" << endl;
	
	return 0;
}

void JedisTest::TestReadString(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value)
{
	string strResult,strValue;
	string strKey = g2u(key);
//	cout << strKey << endl;
	strResult = redistable.get(strKey.c_str());
	strValue = u2g(strResult);
//	cout << value << endl;
//	cout << strResult << endl;
//	cout << strValue << endl;
	assert(strValue==value);
	dlog3("Read String::key:%s,value:%s", key.c_str(),value.c_str());		
}

void JedisTest::TestWriteString(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strKey = g2u(key);
	strWriteValue = g2u(value);
	redistable.setvalue(strKey.c_str(),strWriteValue.c_str());
	if (bCheck)
	{
		strResult = redistable.get(strKey.c_str());
		strReadValue = u2g(strResult);	
		assert(strReadValue==value);
	}
	dlog3("Write String::key:%s,value:%s", key.c_str(),value.c_str());	
}

void JedisTest::TestDelString(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,bool bCheck)
{
	string strKey = g2u(key);
	redistable.del(strKey.c_str());
	if (bCheck)
	{
		bool bResult = redistable.exists(strKey.c_str());
		assert(!bResult);
	}
	dlog3("Del Key::key:%s", key.c_str());	
}

void JedisTest::TestReadHash(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string field,string value)
{
	string strResult,strValue;
	string strKey = g2u(key);
	string strField = g2u(field);
	strResult = redistable.hget(strKey.c_str(),strField.c_str());
	strValue = u2g(strResult);
	assert(strValue==value);
	dlog3("Read Hash::key:%s,field:%s,value:%s", key.c_str(),field.c_str(),strValue.c_str());		
}

void JedisTest::TestReadHash(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,map<string,string> mapValue)
{
	string strResult,strValue,strField;
	string strKey = g2u(key);
	map<string,string>::iterator it;
	for (it=mapValue.begin();it!=mapValue.end();it++)
	{
		strField = it->first;
		strResult = redistable.hget(strKey.c_str(),strField.c_str());
		strValue = u2g(strResult);
		assert(strValue==it->second);
	}
	
	dlog3("Read Hash::%s",show_map_u(mapValue).c_str());
}

void JedisTest::TestWriteHash(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string field,string value,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strKey = g2u(key);
	string strField = g2u(field);
	strWriteValue = g2u(value);
	redistable.hset(strKey.c_str(),strField.c_str(),strWriteValue.c_str());
	if (bCheck)
	{
		strResult = redistable.hget(strKey.c_str(),strField.c_str());
		strReadValue = u2g(strResult);
		assert(strReadValue==value);
	}
	dlog3("Write Hash::key:%s,field:%s,value:%s", key.c_str(),field.c_str(),value.c_str());		
}

void JedisTest::TestWriteHash(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,map<string,string>& mapValue,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strField;
	string strKey = g2u(key);
	
	map<string,string>::iterator it;
	for (it=mapValue.begin();it!=mapValue.end();it++)
	{
		strField = g2u(it->first);
		redistable.hset(strKey.c_str(),strField.c_str(),g2u(it->second).c_str());
		if (bCheck)
		{
			strResult = redistable.hget(strKey.c_str(),strField.c_str());
			strReadValue = u2g(strResult);
			assert(strReadValue==it->second);
		}
	}
	
	dlog3("Write Hash::%s",show_map(mapValue).c_str());
}

void JedisTest::TestDelHash(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string field,bool bCheck)
{
	string strResult,strValue;
	string strKey = g2u(key);
	string strField = g2u(field);
	list<string> listField;
	listField.push_back(strField);
	redistable.hdel(strKey.c_str(),listField);

	if (bCheck)
	{
		bool bResult = redistable.hexists(strKey.c_str(),strField.c_str());
		assert(!bResult);
	}
	
	dlog3("Del Hash::key:%s,field:%s", key.c_str(),field.c_str());		
}


void JedisTest::TestReadList(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,bool bLeft)
{
	string strResult,strValue;
	string strKey = g2u(key);
	if (bLeft)
		strResult = redistable.lpop(strKey.c_str());
	else
		strResult = redistable.rpop(strKey.c_str());
		
	strValue = u2g(strResult);
	cout << value << endl;
	cout << strValue << endl;
	assert(strValue==value);
	dlog3("Read List::key:%s,value:%s", key.c_str(),strValue.c_str());		
}

void JedisTest::TestWriteList(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,bool bLeft,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strKey = g2u(key);
	strWriteValue = g2u(value);
	strWriteValue = dress(strWriteValue);
	
	list<string> listValue;
	listValue.clear();
	listValue.push_back(strWriteValue);
	int ret;
	if (bLeft)
		ret = redistable.lpush(strKey.c_str(),listValue);
	else
		ret = redistable.rpush(strKey.c_str(),listValue);

	if (ret<=0)
	{
		dlog3("Write List failed::%s",show_list(listValue).c_str());
	}
	else
		dlog3("Write List::%s",show_list(listValue).c_str());
	
//	cout << show_list(listValue) << endl;
		
	if (bCheck)
	{
		if (bLeft)
			strResult = redistable.lpop(strKey.c_str());
		else
			strResult = redistable.rpop(strKey.c_str());
		
		strReadValue = u2g(strResult);
		assert(strReadValue==value);
	}
	dlog3("Write List::key:%s,value:%s", key.c_str(),value.c_str());	
}

void JedisTest::TestWriteList(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,list<string>& listValue,bool bLeft,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strKey = g2u(key);
	list<string> listNew;
	list<string>::iterator iter = listValue.begin();

	for (; iter != listValue.end(); ++iter)
	{
		string strValue = g2u(*iter);
		strValue = dress(strValue);
		listNew.push_back(strValue);
	}

	int ret;	
	if (bLeft)
		ret = redistable.lpush(strKey.c_str(),listNew);
	else
		ret = redistable.rpush(strKey.c_str(),listNew);

	if (ret<=0)
	{
		dlog3("Write List failed::%s",show_list(listValue).c_str());
	}
	else
		dlog3("Write List::%s",show_list(listValue).c_str());
		
	if (bCheck)
	{
	//	if (bLeft)
	//		strResult = redistable.lpop(strKey.c_str());
	//	else
	//		strResult = redistable.rpop(strKey.c_str());
		
	//	strReadValue = u2g(strResult);
	//	assert(strReadValue==value);
	}
	dlog3("Write List::%s",show_list(listValue).c_str());
}

void JedisTest::TestDelList(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,int iCount)
{
	string strResult,strValue;
	string strKey = g2u(key);
	strValue = g2u(value);
		
	redistable.lrem(strKey.c_str(),iCount,strValue.c_str());
	dlog3("Del List::key:%s,value:%s,count:%d", key.c_str(),value.c_str(),iCount);
}

void JedisTest::TestReadSet(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value)
{
	string strResult,strValue;
	string strKey = g2u(key);
	strValue = g2u(value);

	set<string> setValue;
	set<string>::iterator it;
	setValue = redistable.smembers(strKey.c_str());
	it = setValue.find(strValue);
	assert(it!=setValue.end());		
	dlog3("Read Set::key:%s,value:%s", key.c_str(),strValue.c_str());		
}

void JedisTest::TestWriteSet(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strKey = g2u(key);
	strWriteValue = g2u(value);
	string strNewValue = strWriteValue;
	strNewValue = dress(strNewValue);

	cout << "key:" << key << " value:" << value << endl;
	cout << "key:" << strKey << " value:" << strWriteValue << endl;

	list<string> listValue;
	listValue.clear();
	listValue.push_back(strNewValue);
	int ret;
	ret = redistable.sadd(strKey.c_str(),listValue);
	if (ret <=0)
	{
		dlog3("Write Set failed::%s",show_list_u(listValue).c_str());
		return;
	}
	else
		dlog3("Write Set::%s",show_list_u(listValue).c_str());			
	
//	strWriteValue = dress(strWriteValue,false);
	
	if (bCheck)
	{
		set<string> setValue;
		set<string>::iterator it;
		setValue = redistable.smembers(strKey.c_str());
		dlog3("Write Set::SMEMBERS:%s",show_set_u(setValue).c_str());	
		if (strWriteValue.length()==0)
			it = setValue.find("''");	
		else
			it = setValue.find(strWriteValue);
	//	cout << "value:" << u2g(strWriteValue) << endl;

		set<string>::iterator iter;

	//	for ( iter=setValue.begin(); iter != setValue.end(); ++iter)
	//	{
	//		cout << "value1:" << u2g(*iter) << endl;			
	//		cout << "value2:" << dress(value,false) << endl;
	//	}		
		
	//	cout << show_set(setValue) << endl;
		assert(it!=setValue.end());		
	}
	dlog3("Write Set::key:%s,value:%s", key.c_str(),value.c_str());		
}

void JedisTest::TestWriteSet(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,list<string>& listValue,bool bCheck)
{
	string strResult,strWriteValue,strReadValue;
	string strKey = g2u(key);
	
	list<string> listNew;
	list<string>::iterator iter = listValue.begin();

	for (; iter != listValue.end(); ++iter)
	{
		string strValue = g2u(*iter);
		strValue = dress(strValue);
		listNew.push_back(strValue);
	}
	
	int ret;
	ret = redistable.sadd(strKey.c_str(),listValue);
	if (ret <=0)
	{
		dlog3("Write Set failed::%s",show_list(listValue).c_str());
		return;
	}
	else
		dlog3("Write Set::%s",show_list(listValue).c_str());			

	strWriteValue = dress(strWriteValue,false);
	
	if (bCheck)
	{
		set<string> setValue;
		set<string>::iterator it;
		setValue = redistable.smembers(strKey.c_str());

		dlog3("Write Set::SMEMBERS:%s",show_set_u(setValue).c_str());			

		for (listValue.begin(); iter != listValue.end(); ++iter)
		{
			if ((*iter).length()==0)
				it = setValue.find("''");	
			else
				it = setValue.find(*iter);			
			assert(it!=setValue.end());		
		}		
	}
//	dlog3("Write Set::key:%s,value:%s", key.c_str(),value.c_str());		
}


void JedisTest::TestDelSet(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,bool bCheck)
{
	string strResult,strValue;
	string strKey = g2u(key);
	strValue = g2u(value);
	string strNewValue = strValue;
	strNewValue = dress(strNewValue);

	list<string> listValue;
	listValue.push_back(strValue);
	
	redistable.srem(strKey.c_str(),listValue);
	if (bCheck)
	{
		bool bResult = redistable.sismember(strKey.c_str(),strValue.c_str());
		assert(!bResult);
	}
	dlog3("Del Set::key:%s,value:%s", key.c_str(),value.c_str());		
}

void JedisTest::TestReadSortset(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,double score)
{
	string strKey = g2u(key);
	string strValue = g2u(value);
//	string strNewValue = strValue;
//	strNewValue = dress(strNewValue);
	set<string> setValue;
	set<string>::iterator it;	
	setValue = redistable.zrange(strKey.c_str(),0,-1);
	it = setValue.find(strValue);
	assert(it!=setValue.end());
	dlog3("Read Sortset::key:%s,value:%s", key.c_str(),value.c_str());		
}

void JedisTest::TestReadSortset(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,map<string,double>& mapValue)
{
	string strKey = g2u(key);
	set<string> setValue;
	set<string>::iterator it;	
	setValue = redistable.zrange(strKey.c_str(),0,-1);
	map<string,double>::iterator itmap;
	double min;
	if (!mapValue.empty())
	{
		for (itmap=mapValue.begin();itmap!=mapValue.end();itmap++)
		{
			string strValue = g2u(itmap->first);
			assert(setValue.find(strValue)!=setValue.end());
		}
	}

	assert(!setValue.empty());

	if (!setValue.empty())
	{
		it=setValue.begin();
		min = mapValue[u2g(*it)];
		while(it!=setValue.end())
		{
			double score = mapValue[u2g(*it)];
			assert(min<=score);
			min = score;
			it++;
		}
	}

	dlog3("Read Sortset::%s",show_set_u(setValue).c_str());
}

void JedisTest::TestWriteSortset(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,double score,bool bCheck)
{
	string strResult,strWriteValue;
	string strKey = g2u(key);
	strWriteValue = g2u(value);
	string strNewValue = strWriteValue;
	strNewValue = dress(strNewValue);

//	cout << value << endl;
//	cout << strWriteValue << endl;
//	cout << strNewValue << endl;

	map<string, double> scoreMembers;
	scoreMembers[strNewValue]=score;
//	cout << show_map_u(scoreMembers);
	
	redistable.zadd(strKey.c_str(),scoreMembers);
//	redistable.zadd(strKey.c_str(),score,strNewValue.c_str());

	strWriteValue = dress(strWriteValue,false);
	
	if (bCheck)
	{
		set<string> setValue;
		set<string>::iterator it;	
		setValue = redistable.zrange(strKey.c_str(),0,-1);
		cout << show_set_u(setValue) << endl;
		if (strWriteValue.length()!=0)
			it = setValue.find(strWriteValue);
		else
			it = setValue.find(string("''"));
		assert(it!=setValue.end());
	}
	dlog3("Write Sortset::key:%s,value:%s", key.c_str(),value.c_str());		
}

void JedisTest::TestWriteSortset(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,map<string,double>& mapValue,bool bCheck)
{
	string strResult,strWriteValue;
	string strKey = g2u(key);

	map<string,double>::iterator mapIt;
	map<string,double> mapNew;
	
	for (mapIt=mapValue.begin();mapIt!=mapValue.end();mapIt++)
	{
		string strNewValue = g2u(mapIt->first);
		strNewValue = dress(strNewValue);
		mapNew[strNewValue]=mapIt->second;
	}
	
	redistable.zadd(strKey.c_str(),mapNew);
	
	if (bCheck)
	{
		set<string> setValue;
		set<string>::iterator it;	
		setValue = redistable.zrange(strKey.c_str(),0,-1);
		for (mapIt=mapValue.begin();mapIt!=mapValue.end();mapIt++)
		{
			it = setValue.find(g2u(mapIt->first));
			assert(it!=setValue.end());
		}
	}
	dlog3("Write Sortset::%s",show_map(mapValue).c_str());
}

void JedisTest::TestDelSortset(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,string value,bool bCheck)
{
	string strResult,strValue;
	string strKey = g2u(key);
	strValue = g2u(value);
	string strNewValue = strValue;
	strNewValue = replace_all(strNewValue,string("%"),string("%%"));

	list<string> listValue;
	listValue.push_back(strValue);
	
	redistable.zrem(strKey.c_str(),listValue);
	if (bCheck)
	{
		set<string> setValue;
		set<string>::iterator it;	
		setValue = redistable.zrange(strKey.c_str(),0,-1);
		it = setValue.find(strValue);
		assert(it==setValue.end());
	}
	dlog3("Del Sortset::key:%s,value:%s", key.c_str(),value.c_str());		
}

void JedisTest::TestInc(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,bool bCheck)
{
	string strResult;
	string strKey = g2u(key);
	int iResult = redistable.incr(strKey.c_str());
	if (bCheck)
	{
		int iReadResult = redistable.incrBy(strKey.c_str(),0);
		assert(iResult==iReadResult);
	}
	dlog3("Inc::key:%s,Result:%d", key.c_str(),iResult);		
}

void JedisTest::TestDec(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,bool bCheck)
{
	string strResult;
	string strKey = g2u(key);
	int iResult = redistable.decr(strKey.c_str());
	if (bCheck)
	{
		int iReadResult = redistable.incrBy(strKey.c_str(),0);
		assert(iResult==iReadResult);
	}
	dlog3("Dec::key:%s,Result:%d", key.c_str(),iResult);		
}

void JedisTest::TestIncBy(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,int iStep,bool bCheck)
{
	string strResult;
	string strKey = g2u(key);
	int iResult = redistable.incrBy(strKey.c_str(),iStep);
	if (bCheck)
	{
		int iReadResult = redistable.decrBy(strKey.c_str(),0);
		assert(iResult==iReadResult);
	}
	dlog3("Inc by::key:%s,step:%d,Result:%d", key.c_str(),iStep,iResult);		
}

void JedisTest::TestDecBy(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,
	string key,int iStep,bool bCheck)
{
	string strResult;
	string strKey = g2u(key);
	int iResult = redistable.decrBy(strKey.c_str(),iStep);
	if (bCheck)
	{
		int iReadResult = redistable.incrBy(strKey.c_str(),0);
		assert(iResult==iReadResult);
	}
	dlog3("Dec by::key:%s,step:%d,Result:%d", key.c_str(),iStep,iResult);		
}

void JedisTest::TestString(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,int mode)
{
	switch(mode)
	{
		case 1:	//	读
		{
			TestReadString(datacenter,redisdb,redistable,g_java_keystring[1], g_java_valuestring[1]);
			for (int i=0;i<7;i++)
			{
				TestReadString(datacenter,redisdb,redistable,g_java_keystring[i], g_java_valuestring[i]);
			}			
			break;
		}
		case 2:	//	写
		{
			for (int i=0;i<7;i++)
			{
				TestWriteString(datacenter,redisdb,redistable,g_c_keystring[i], g_c_valuestring[i],true);
			}			
			break;
		}
	}
	dlog3("TestString Finish");	
	cout << "TestString Finish" << endl;
}

void JedisTest::TestList(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,int mode)
{
	switch(mode)
	{
		case 1:	//	读
		{
			for (int i=0;i<6;i++)
			{
				TestReadList(datacenter,redisdb,redistable,g_java_keylist[i], g_java_valuelist[i],true);
			}			
			TestReadList(datacenter,redisdb,redistable,"keyList7", "头部添加值3",true);
			TestReadList(datacenter,redisdb,redistable,"keyList7", "头部添加值2",true);
			TestReadList(datacenter,redisdb,redistable,"keyList7", "头部添加值1",true);

			TestReadList(datacenter,redisdb,redistable,"keyList8", "尾部添加值3",false);
			TestReadList(datacenter,redisdb,redistable,"keyList8", "尾部添加值2",false);
			TestReadList(datacenter,redisdb,redistable,"keyList8", "尾部添加值1",false);			
			break;
		}
		case 2:	//	写
		{
		//	list<string> listValue;
		//	listValue.clear();
		//	listValue.push_back(g2u(string(g_c_valuelist[1])));
		//	redistable.lpush(g2u(g_c_keylist[1]).c_str(),listValue);

//			TestDelString(datacenter,redisdb,redistable,"tan",true);
//			TestWriteList(datacenter,redisdb,redistable,g_c_keylist[i], g_c_valuelist[i],true,false);
			
			for (int i=0;i<6;i++)
			{
				TestDelString(datacenter,redisdb,redistable,g_c_keylist[i],true);
				TestWriteList(datacenter,redisdb,redistable,g_c_keylist[i], g_c_valuelist[i],true,false);
			}

			TestDelString(datacenter,redisdb,redistable,"keyList7_C",true);
			list<string> listValue;
			listValue.clear();
			listValue.push_back(string("头部添加值1"));
			listValue.push_back(string("头部添加值2"));
			listValue.push_back(string("头部添加值3"));
			TestWriteList(datacenter,redisdb,redistable,"keyList7_C",listValue,true,false);

			TestDelString(datacenter,redisdb,redistable,"keyList8_C",true);
			listValue.clear();
			listValue.push_back(string("尾部添加值1"));
			listValue.push_back(string("尾部添加值2"));
			listValue.push_back(string("尾部添加值3"));
			TestWriteList(datacenter,redisdb,redistable,"keyList8_C",listValue,false,false);
			
			break;
		}
	}
	dlog3("TestList Finish");	
	cout << "TestList Finish" << endl;
}

void JedisTest::TestSet(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,int mode)
{
	switch(mode)
	{
		case 1:	//	读
		{
			for (int i=0;i<6;i++)
			{
				TestReadSet(datacenter,redisdb,redistable,g_java_keyset[i], g_java_valueset[i]);
			}			
			TestReadSet(datacenter,redisdb,redistable,"keySet7", "Test_keySet71");
			TestReadSet(datacenter,redisdb,redistable,"keySet7", "Test_keySet72");
			TestReadSet(datacenter,redisdb,redistable,"keySet7", "Test_keySet73");
			break;
		}
		case 2:	//	写
		{
			for (int i=0;i<6;i++)
			{
				TestDelString(datacenter,redisdb,redistable,g_c_keyset[i],true);
				TestWriteSet(datacenter,redisdb,redistable,g_c_keyset[i], g_c_valueset[i],true);
			}

			TestDelString(datacenter,redisdb,redistable,"keySet7_C",true);
			list<string> listValue;
			listValue.clear();
			listValue.push_back(string("Test_keySet71"));
			listValue.push_back(string("Test_keySet72"));
			listValue.push_back(string("Test_keySet73"));
			
			TestWriteSet(datacenter,redisdb,redistable,"keySet7_C",listValue,true);
			break;
		}
	}
	dlog3("TestSet Finish");	
	cout << "TestSet Finish" << endl;
}

void JedisTest::TestSortset(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,int mode)
{
	switch(mode)
	{
		case 1:	//	读
		{
			for (int i=0;i<6;i++)
			{
				TestReadSortset(datacenter,redisdb,redistable,g_java_keysortset[i], g_java_valuesortset[i],1);
			}			
			TestReadSortset(datacenter,redisdb,redistable,"keySortSet7", "keySortSet1",2);
			TestReadSortset(datacenter,redisdb,redistable,"keySortSet7", "keySortSet2",3);
			TestReadSortset(datacenter,redisdb,redistable,"keySortSet7", "keySortSet3",1);
			break;
		}
		case 2:	//	写
		{
	//		TestDelString(datacenter,redisdb,redistable,g_c_keysortset[5],true);			
	//		TestWriteSortset(datacenter,redisdb,redistable,g_c_keysortset[5], g_c_valuesortset[5],1,true);

			for (int i=0;i<6;i++)
			{
				TestDelString(datacenter,redisdb,redistable,g_c_keysortset[i],true);			
				TestWriteSortset(datacenter,redisdb,redistable,g_c_keysortset[i], g_c_valuesortset[i],1,true);
			}

			TestDelString(datacenter,redisdb,redistable,"keySortSet7_C",true);
			TestDelString(datacenter,redisdb,redistable,"keySortSet8_C",true);
			TestDelString(datacenter,redisdb,redistable,"keySortSet9_C",true);
			
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet7_C", "keySortSet1",2,true);
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet7_C", "keySortSet2",3,true);
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet7_C", "keySortSet3",1,true);

			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet8_C", "keySortSet1",-2,true);
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet8_C", "keySortSet2",-3,true);
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet8_C", "keySortSet3",1,true);

			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet9_C", "keySortSet1",0.78,true);
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet9_C", "keySortSet2",1.587,true);
			TestWriteSortset(datacenter,redisdb,redistable,"keySortSet9_C", "keySortSet3",1.2565,true);

			break;
		}
	}
	dlog3("TestSortset Finish");
	cout << "TestSortset Finish" << endl;	
}

void JedisTest::TestHash(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable,int mode)
{
	switch(mode)
	{
		case 1:	//	读
		{
			for (int i=1;i<8;i++)
			{
				TestReadHash(datacenter,redisdb,redistable,g_java_keyhash[i], g_c_fieldhash[i], g_java_valuehash[i]);
			}
			map<string,string> mapValue;
			mapValue["filed3"]=string("Test_keyHash91");
			mapValue["filed4"]=string("Test_keyHash92");
			mapValue["filed5"]=string("Test_keyHash93");
			mapValue["filed6"]=string("Test_keyHash94");
			
			TestReadHash(datacenter,redisdb,redistable,"keyHash1",mapValue);
			break;
		}
		case 2:	//	写
		{
			TestDelString(datacenter,redisdb,redistable,"keyHash1_C",true);
			TestDelString(datacenter,redisdb,redistable,"keyHash2_C",true);
			TestDelString(datacenter,redisdb,redistable,"keyHash3_C",true);
			TestDelString(datacenter,redisdb,redistable,"keyHash4_C",true);
			TestDelString(datacenter,redisdb,redistable,"keyHash5_C",true);
			TestDelString(datacenter,redisdb,redistable,"keyHash6_C",true);
			TestDelString(datacenter,redisdb,redistable,"keyHash7_C",true);

			TestWriteHash(datacenter,redisdb,redistable,g_c_keyhash[0], "filed1","Test_keyHash1",true);
			TestWriteHash(datacenter,redisdb,redistable,g_c_keyhash[0], "filed2","Test_keyHash2",true);
			
			for (int i=1;i<8;i++)
			{
				TestWriteHash(datacenter,redisdb,redistable,g_c_keyhash[i], g_c_fieldhash[i],g_c_valuehash[i],true);
			}
			
			map<string,string> mapValue;
			mapValue["filed3"]=string("Test_keyHash91");
			mapValue["filed4"]=string("Test_keyHash92");
			mapValue["filed5"]=string("Test_keyHash93");
			mapValue["filed6"]=string("Test_keyHash94");
			
			TestWriteHash(datacenter,redisdb,redistable,"keyHash1_C", mapValue,true);
			break;
		}
	}
	dlog3("TestHash Finish");	
	cout << "TestHash Finish" << endl;	
}

void JedisTest::TestVar(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable)
{
	TestInc(datacenter,redisdb,redistable,"count1",true);
	TestIncBy(datacenter,redisdb,redistable,"count2",12,true);
	TestInc(datacenter,redisdb,redistable,"count2",true);
	TestIncBy(datacenter,redisdb,redistable,"count1",20,true);
	TestIncBy(datacenter,redisdb,redistable,"count1",-20,true);

	TestDec(datacenter,redisdb,redistable,"count3",true);
	TestDecBy(datacenter,redisdb,redistable,"count4",12,true);
	TestDec(datacenter,redisdb,redistable,"count4",true);
	TestDecBy(datacenter,redisdb,redistable,"count3",20,true);
	TestDecBy(datacenter,redisdb,redistable,"count3",-20,true);		
	dlog3("TestVar Finish");	
	cout << "TestVar Finish" << endl;	
}

void JedisTest::TestDel(DataCenter& datacenter,
	RedisDB& redisdb,RedisTable& redistable)
{
	TestDelString(datacenter,redisdb,redistable,"keyStr2",true);
	TestDelString(datacenter,redisdb,redistable,"keyList2",true);
	TestDelList(datacenter,redisdb,redistable,"keyList7", "头部添加值2", 2);
	TestDelString(datacenter,redisdb,redistable,"keySet2",true);
	TestDelSet(datacenter,redisdb,redistable, "keySet7", "Test_keySet72", true);
	TestDelSet(datacenter,redisdb,redistable, "keySet7", "Test_keySet71", true);
	TestDelString(datacenter,redisdb,redistable,"keySortSet2",true);
	TestDelSortset(datacenter,redisdb,redistable, "keySet7", "keySortSet2", true);	
	TestDelString(datacenter,redisdb,redistable,"keyHash2",true);
	TestDelHash(datacenter,redisdb,redistable,"keyHash1","filed4",true);
	TestDelHash(datacenter,redisdb,redistable,"keyHash1","filed5",true);	
	dlog3("TestDel Finish");	
	cout << "TestDel Finish" << endl;	
}

void JedisTest::TestCommonFunction(const char *ip, unsigned short port,int mode)
{

	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb(g_dbname);
	RedisTable redistable = redisdb.get_redis_table(g_tablename);

	string strResult;

	//	查询数据库是否存在
//	assert(datacenter.existdb(g_dbname));

//	set<string> sResult = redisdb.get_table_names();
//	set<string>::iterator setIt;
//	setIt = sResult.find(g_tablename);
//	assert(setIt!=sResult.end());
//	sResult.clear();

	switch(mode)
	{
		case 1:	//	只读
		{
			TestString(datacenter,redisdb,redistable,1);
			TestList(datacenter,redisdb,redistable,1);
			TestSet(datacenter,redisdb,redistable,1);
			TestSortset(datacenter,redisdb,redistable,1);
			TestHash(datacenter,redisdb,redistable,1);
		}
		case 2:	//	只写
		{
			TestString(datacenter,redisdb,redistable,2);
			TestList(datacenter,redisdb,redistable,2);
			TestSet(datacenter,redisdb,redistable,2);
			TestSortset(datacenter,redisdb,redistable,2);
			TestHash(datacenter,redisdb,redistable,2);	
			TestVar(datacenter,redisdb,redistable);
			break;
		}
		case 3:	//	删除
		{
			TestDel(datacenter,redisdb,redistable);
			break;
		}		
	}

	cout << "TestCommonFunction Finish" << endl;
}

void JedisTest::TestFunction(const char *ip, unsigned short port)
{
	int iTimeout = 2;
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");

	cout.setf(ios::right);
	
	int iResult = -1;
	bool bResult = false;
	string strResult;

	//	测试数据库

	//	字符串操作检测
	//	设置字符串
	bResult = redistable.setvalue("setkey", "setvalue");	
    	cout<<  "set : "<< bResult << endl;
	assert(bResult);
	
	//	读取字符串
	strResult = redistable.get("setkey");		
	cout << "get : " << strResult << endl;
	assert(strResult=="setvalue");
	
	//	判断关键字是否存在
	cout << "exists : " << redistable.exists("setkey") << endl;
	assert(redistable.exists("setkey") == 1);

	//	获取关键字类型
	strResult = redistable.type("setkey");	
	cout << "type : " << strResult << endl;
	assert(strResult=="string");

	//	设置关键字满期值
	iResult = redistable.expire("setkey", iTimeout);
	cout << "expire : " << iResult << endl;
	assert(iResult == 1);

	//	获取关键字有效时间
	iResult = redistable.ttl("setkey");
	cout << "ttl : " << iResult << endl;
	assert(iResult <= iTimeout && iResult>=0);

	assert(redistable.exists("setkey"));

	//	检测关键字满期值的有效性
	sleep(iTimeout+1);
	cout << "setkey: " << redistable.get_jedis_addr("setkey") << endl;
//	cout << redistable.getkey("setkey") << endl;
	assert(!redistable.exists("setkey"));
	
	//	设置字符串
	bResult = redistable.setvalue("setkey", "setvalue");	
    	cout<<  "set : "<< bResult << endl;
	assert(bResult);

	//	设置关键字满期值(绝对时间)
	iResult = redistable.expireAt("setkey", time(0) + iTimeout);
	cout << "expireAt : " << iResult << endl;
	assert(iResult == 1);		

	//	获取关键字有效时间
	iResult = redistable.ttl("setkey");
	cout << "ttl : " << iResult << endl;
	assert(iResult <= iTimeout);

	bool bExist = (iResult > 0);

	bResult = redistable.setbit("setkey", 1, false);		
	cout << "setbit : " << bResult << endl;
	assert(bResult);

	bResult = redistable.getbit("setkey", 1);
	cout << "getbit : " << bResult << endl;
	assert(!bResult);

	iResult = redistable.setrange("setkey", 0, "1000");
	cout << "setrange : " << iResult << endl;
	if (bExist) assert(iResult==8);
	else assert(iResult == 4);

	strResult = redistable.getrange("setkey", 0, 3);
	cout << "getrange : " << strResult << endl;
	assert(strResult == "1000");

	strResult = redistable.getSet("setkey", "newkey");
	cout << "getset : " << strResult << endl;
	if (bExist)
		assert(strResult=="1000alue");
	else 
		assert(strResult == "1000");

	strResult = redistable.get("setkey");
	cout << "get : " << strResult << endl;
	assert(strResult == "newkey");

	//	对setnx命令进行检测
	iResult = redistable.setnx("setkey", "newkey1");
	cout << "setnx : " << iResult << endl;
	assert(iResult == 0);
	
	iResult = redistable.del("setkey");
	cout << "del : " << iResult << endl;
	assert(iResult == 1);

	iResult = redistable.setnx("setkey", "newkey1");
	cout << "setnx : " << iResult << endl;
	assert(iResult == 1);

	//	对setex命令进行检测	
	bResult = redistable.setex("setkey", iTimeout, "newkey1");
	cout << "setex : " << bResult << endl;
	assert(bResult);
	sleep(iTimeout+1);
	assert(!redistable.exists("setkey"));
	
	redistable.setvalue("setkey", "10000000");

	//	共享变量测试
	string key = redistable.getkey("setkey");
	datacenter.get_cfg_jedis().setvalue(key.c_str(), "10000000");
	//	对decrby命令进行检测
	iResult = redistable.decrBy("setkey", 1000);
	cout << "decrBy : " << iResult << endl;
	assert(iResult == 9999000);

	//	对decr命令进行检测
	iResult = redistable.decr("setkey");
	cout << "decr : " << iResult << endl;
	assert(iResult == 9998999);

	//	对incrby命令进行检测
	iResult = redistable.incrBy("setkey", 1000);
	cout << "incrBy : " <<  iResult << endl;
	assert(iResult == 9999999);

	//	对incr命令进行检测
	iResult = redistable.incr("setkey");
	cout << "incr : " << iResult << endl;
	assert(iResult == 10000000);

	//	对append命令进行检测
	iResult = redistable.append("setkey", "1");
	cout << "append : " << iResult << endl;
	assert(iResult == 9);
	strResult = redistable.get("setkey");		
	assert(strResult=="100000001");

	//	对substr命令进行检测
	strResult = redistable.substr("setkey", 0, 3);
	cout << "substr : " << strResult << endl;
	assert(strResult == "1000");

	///=================================================================

	//	对HASH进行检测
	redistable.del("hkey");
	//	对HSET命令进行测试
	iResult = -1;
	iResult = redistable.hset("hkey", "field-0", "value");
	cout << "hset : " << iResult << endl;
	assert(iResult == 1);

	//	对HGET命令进行测试
	strResult = redistable.hget("hkey", "field-0");
	cout << "hget : " <<  strResult << endl;
	assert(strResult == "value");

	//	对HSETNX命令进行测试
	iResult = redistable.hsetnx("hkey", "field-0", "value");
	cout << "hsetnx : " << iResult << endl;
	assert(iResult == 0);

	bResult = redistable.hexists("hkey", "field-0");
	cout << "hexists : " << bResult << endl;
	assert(bResult);

	list<string> f;
	f.push_back("field-0");
	iResult = redistable.hdel("hkey",f);
	cout << "hdel : " << bResult << endl;
	assert(iResult);

	iResult = redistable.hsetnx("hkey", "field-0", "value");
	cout << "hsetnx : " << iResult << endl;
	assert(iResult == 1);
	strResult = redistable.hget("hkey", "field-0");
	cout << "hget : " <<  strResult << endl;
	assert(strResult == "value");		

	map<string, string> hash;
	hash["field-1"] = "value-1";
	hash["field-2"] = "value-2";
	hash["field-3"] = "value-3";
	bResult = redistable.hmset("hkey", hash);
	cout << "hmset : " << bResult << endl;
	assert(bResult);
	strResult = redistable.hget("hkey", "field-1");
	cout << "hget : " <<  strResult << endl;
	assert(strResult == "value-1");		
	strResult = redistable.hget("hkey", "field-2");
	cout << "hget : " <<  strResult << endl;
	assert(strResult == "value-2");		
	strResult = redistable.hget("hkey", "field-3");
	cout << "hget : " <<  strResult << endl;
	assert(strResult == "value-3");		
	
	list<string> fields;
	fields.push_back("field-1");
	fields.push_back("field-2");
	fields.push_back("field-3");
	list<string> l = redistable.hmget("hkey", fields);
	cout << "hmget : " << show_list(l) << endl;
	list<string>::iterator listIt;
	listIt = find(l.begin(),l.end(),"value-1");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"value-2");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"value-3");
	assert(listIt != l.end());

	redistable.hset("hkey", "field-0", "100000");
	
	iResult = redistable.hincrBy("hkey", "field-0", 1000);
	cout << "hincrBy : " << iResult << endl;
	assert(iResult == 101000);

	iResult = redistable.hlen("hkey");
	cout << "hlen : " << iResult << endl;
	assert(iResult>=4);

	set<string>::iterator setIt;
	set<string> s = redistable.hkeys("hkey");
	cout << "hkeys : " << show_set(s) << endl;
	setIt = s.find("field-0");
	assert(setIt!=s.end());
	setIt = s.find("field-1");
	assert(setIt!=s.end());
	setIt = s.find("field-2");
	assert(setIt!=s.end());
	setIt = s.find("field-3");
	assert(setIt!=s.end());

	l.clear();
	l = redistable.hvals("hkey");
	cout << "hvals : " << show_list(l) << endl;
	listIt = find(l.begin(),l.end(),"101000");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"value-1");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"value-2");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"value-3");
	assert(listIt != l.end());
	
	map<string, string> m = redistable.hgetAll("hkey");
	cout << "hgetAll : " << show_map(m) << endl;
	assert(m["field-0"]=="101000");
	assert(m["field-1"]=="value-1");
	assert(m["field-2"]=="value-2");
	assert(m["field-3"]=="value-3");

	///=================================================================

	//	List检测
	redistable.del("lkey");
	list<string> strings;
	strings.push_back("lvalue-0");
	strings.push_back("lvalue-1");
	strings.push_back("lvalue-2");
	strings.push_back("lvalue-3");
	iResult = redistable.rpush("lkey", strings);
	cout << "rpush : " << iResult << endl;
	assert(iResult>=4);

	list<string> strings1;
	strings1.push_back("lvalue-5");
	strings1.push_back("lvalue-6");
	iResult = redistable.lpush("lkey", strings1);
	cout << "lpush : " << iResult << endl;
	assert(iResult>=6);

	iResult = redistable.llen("lkey");
	cout << "llen : " << iResult << endl;
	assert(iResult>=6);

	l.clear();
	l = redistable.lrange("lkey", 0, 10);
	cout << "lrange : " << show_list(l) << endl;
	listIt = find(l.begin(),l.end(),"lvalue-6");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-5");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-0");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-1");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-2");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-3");
	assert(listIt != l.end());

	bResult = redistable.ltrim("lkey", 0, 3) ;
	cout << "ltrim : " << bResult << endl;
	assert(bResult);

	l.clear();
	l = redistable.lrange("lkey", 0, 4);
	cout << "lrange : " << show_list(l) << endl;
	listIt = find(l.begin(),l.end(),"lvalue-6");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-5");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-0");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-1");
	assert(listIt != l.end());
	listIt = find(l.begin(),l.end(),"lvalue-3");
	assert(listIt == l.end());
	listIt = find(l.begin(),l.end(),"lvalue-4");
	assert(listIt == l.end());

	strResult = redistable.lindex("lkey", 1);
	cout << "lindex : " << strResult << endl;
	assert(strResult == "lvalue-5");

	bResult = redistable.lset("lkey", 1, "new-lvalue-5");		
	cout << "lset : " << bResult << endl;
	assert(bResult);

	l = redistable.lrange("lkey", 0, 4);
	cout << "lrange : " << show_list(l) << endl;
	listIt = find(l.begin(),l.end(),"lvalue-5");
	assert(listIt == l.end());
	listIt = find(l.begin(),l.end(),"new-lvalue-5");
	assert(listIt != l.end());

//	l = redistable.sort("lkey");
//	cout << "sort : " << show_list(l) << endl;

	int iCount = 2;
	iResult = redistable.lrem("lkey", iCount, "lvalue-0");
	cout << "lrem : " << iResult << endl;
	assert(iResult == 1);

	strResult = redistable.lpop("lkey");
	cout << "lpop : " << strResult << endl;
	assert(strResult == "lvalue-6");

	strResult = redistable.rpop("lkey");
	cout << "rpop : " << strResult << endl;
	assert(strResult == "lvalue-1");

	iResult = redistable.lpushx("hkey", "zvalue-7");
	cout << "lpushx : " << iResult << endl;
	assert(iResult==0);

	iResult = redistable.rpushx("hkey", "zvalue-8");
	cout << "rpushx : " << iResult << endl;
	assert(iResult==0);

	

	///=================================================================

	//	SET检测
	redistable.del("skey");
	list<string> member;
	member.push_back("svalue-0");
	member.push_back("svalue-1");
	member.push_back("svalue-2");
	member.push_back("svalue-3");

	iResult = redistable.sadd("skey", member);
	cout << "sadd : " << iResult << endl;

	s = redistable.smembers("skey");
	cout << "smembers : " << show_set(s) << endl;
	setIt = s.find("svalue-0");
	assert(setIt!=s.end());
	setIt = s.find("svalue-1");
	assert(setIt!=s.end());
	setIt = s.find("svalue-2");
	assert(setIt!=s.end());
	setIt = s.find("svalue-3");
	assert(setIt!=s.end());

	member.clear();
	member.push_back("svalue-3");

	iResult = redistable.srem("skey", member);
	cout << "srem : " << iResult << endl;
	assert(iResult==1);
	setIt = s.find("svalue-3");
	assert(setIt!=s.end());
	s.erase(setIt);

	strResult = redistable.spop("skey");
	cout << "spop : " << strResult << endl;
	setIt = s.find(strResult.c_str());
	assert(setIt!=s.end());
	s.erase(setIt);

	iResult = redistable.scard("skey");
	cout << "scard : " << iResult << endl;
	assert(iResult>=2);

	bResult = redistable.sismember("skey", strResult.c_str());
	cout << "sismember : " << bResult << endl;
	assert(!bResult);

	strResult = redistable.srandmember("skey");
	cout << "srandmember : " << strResult << endl;
	setIt = s.find(strResult.c_str());		
	assert(setIt!=s.end());

	///=================================================================

	//	测试zset
	redistable.del("zkey");
	iResult = redistable.zadd("zkey", 1, "zvalue1");
	cout << "zadd : " << iResult <<endl;
	assert(iResult==1);
//
	map<string,double> scoreMembers;
	scoreMembers["zvalue2"]=2.1;
	scoreMembers["zvalue3"]=2.2;
	scoreMembers["zvalue4"]=2.3;
	scoreMembers["zvalue5"]=2.4;		
//
	iResult = redistable.zadd("zkey", scoreMembers);
	cout << "zadd : " << iResult << endl;
	assert(iResult==4);

	s = redistable.zrange("zkey", 1, 2);
	cout << "zrange : " << show_set(s) << endl;
	setIt = s.find("zvalue2");
	assert(setIt!=s.end());
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());

	member.clear();
	member.push_back("zvalue1");
	member.push_back("zvalue2");
	member.push_back("zvalue7");		
	iResult = redistable.zrem("zkey", member);
	cout << "zrem : " << iResult << endl;
	assert(iResult == 2);

	double dResult;
	dResult = redistable.zincrby("zkey", 2, "zvalue3");
	cout << "zincrby : " << dResult << endl;
	assert(dResult >= 4.2 && dResult < 4.4);

	iResult = redistable.zrank("zkey", "zvalue4");
	cout << "zrank : " << iResult << endl;
	assert(iResult==0);

	iResult = redistable.zrevrank("zkey", "zvalue4");
	cout << "zrevrank : " << iResult << endl;
	assert(iResult==2);

	s = redistable.zrevrange("zkey", 0, 5);
	cout << "zrevrange : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());
	setIt = s.find("zvalue5");
	assert(setIt!=s.end());

	//	set<Tuple> zrangeWithScores("zkey", int start, int end);
	//
	//	set<Tuple> zrevrangeWithScores("zkey", int start, int end);

	iResult = redistable.zcard("zkey");
	cout << "zcard : " << iResult << endl;
	assert(iResult>=3);

	dResult = redistable.zscore("zkey", "zvalue4");
	cout << "zscore : " << dResult << endl;
	assert(dResult>(double)2.2 && dResult<(double)2.4);

	/***
	todo: member的类型必须全部是整形
	 */
//		cout << "sort : " << show_list(redistable.sort("zkey")) << endl;

	//list<string> sort("zkey", SortingParams sortingParameters);
	iResult = redistable.zcount("zkey", 0, 5);
	cout << "zcount : " << iResult << endl;
	assert(iResult==3);

	iResult = redistable.zcount("zkey", 1, 4);
	cout << "zcount : " << iResult << endl;
	assert(iResult==2);

	s = redistable.zrangeByScore("zkey", 0, 5);
	cout << "zrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());
	setIt = s.find("zvalue5");
	assert(setIt!=s.end());	

	s = redistable.zrangeByScore("zkey", "0", "4");
	cout << "zrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());
	setIt = s.find("zvalue5");
	assert(setIt!=s.end());	

	s = redistable.zrevrangeByScore("zkey", 5, 1);
	cout << "zrevrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue5");
	assert(setIt!=s.end());	

	s = redistable.zrangeByScore("zkey", 0, 5, 1, 2);
	cout << "zrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue4");
	assert(setIt==s.end());	
	setIt = s.find("zvalue5");
	assert(setIt!=s.end());			

	s = redistable.zrevrangeByScore("zkey", "5", "1");
	cout << "zrevrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue5");
	assert(setIt!=s.end());	

	s = redistable.zrangeByScore("zkey", "1", "5", 2, 5);
	cout << "zrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue4");
	assert(setIt==s.end());	
	setIt = s.find("zvalue5");
	assert(setIt==s.end());		
	
	s = redistable.zrevrangeByScore("zkey", 5, 1, 2, 4);
	cout << "zrevrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt==s.end());	
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue5");
	assert(setIt==s.end());	

	//	set<Tuple> zrangeByScoreWithScores("zkey", double min, double max);
	//
	//	set<Tuple> zrevrangeByScoreWithScores("zkey", double max, double min);
	//
	//	set<Tuple> zrangeByScoreWithScores("zkey", double min, double max, int offset, int count);

	s = redistable.zrevrangeByScore("zkey", "5", "1", 2, 4);
	cout << "zrevrangeByScore : " << show_set(s) << endl;
	setIt = s.find("zvalue3");
	assert(setIt==s.end());	
	setIt = s.find("zvalue4");
	assert(setIt!=s.end());	
	setIt = s.find("zvalue5");
	assert(setIt==s.end());

	//	set<Tuple> zrangeByScoreWithScores("zkey", string min, string max);
	//
	//	set<Tuple> zrevrangeByScoreWithScores("zkey", string max, string min);
	//
	//	set<Tuple> zrangeByScoreWithScores("zkey", string min, string max, int offset, int count);
	//
	//	set<Tuple> zrevrangeByScoreWithScores("zkey", double max, double min, int offset, int count);
	//
	//	set<Tuple> zrevrangeByScoreWithScores("zkey", string max, string min, int offset, int count);

	iResult = redistable.zremrangeByRank("zkey", 1, 2);
	cout << "zremrangeByRank : " << iResult << endl;
	assert(iResult==2);

	iResult = redistable.zremrangeByScore("zkey", 0, 5);
	cout << "zremrangeByScore : " << iResult << endl;
	assert(iResult==1);

	iResult = redistable.zremrangeByScore("zkey", 0, 5);
	cout << "zremrangeByScore : " << iResult << endl;
	assert(iResult==0);

	//	int linsert("zkey", Client.list_POSITION where, string pivot, string value);

	cout << "test ok!" << endl;

}

void JedisTest::TestCapability(const char *ip, unsigned short port)
{
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");
	long long starttime = ustime();
	int i=0;
	char buf[32];
	int iErrorCount = 0;
	for (i=0;i<iMaxTestCount;i++)
	{
		sprintf(buf,"%d",i);
		if (!redistable.setvalue(buf,buf))
		{
			iErrorCount++;
			cout << "set :" << buf << " failed!" << endl;
		}
	}
	long long endtime = ustime();

	cout << "set : " << endtime - starttime << "us" << endl;
	cout << "set error count:" << iErrorCount << endl;
}

void JedisTest::TestConsistence(const char *ip, unsigned short port,const char * pFileName)
{
	int fd = open(pFileName,O_CREAT|O_TRUNC|O_RDWR,0644);
	DataCenter datacenter;
	datacenter.init(ip, port);
	char buffer[1024];
	char buf[32];
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");
	for (int i=0;i<=1000000;i++)
	{
		sprintf(buf,"%d",i);
		sprintf(buffer,"%s %s\n",redistable.getkey(buf).c_str(),redistable.get_jedis_addr(buf).c_str());
	//	fwrite(buffer,strlen(buffer), sizeof(char), stdout);
		write(fd,buffer,strlen(buffer));
	}
	close(fd);
}

void JedisTest::Read1MString(const char *ip, unsigned short port)
{
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");
	int i=0;
	char buf[32];
//	int iErrorCount = 0;
	string strResult;
	for (i=0;i<1000000;i++)
	{
		sprintf(buf,"%d",i);
		while(1)
		{
			strResult = redistable.get(buf);
			if (strResult.length()!=0)
				break;
			cout << redistable.get_jedis_addr(buf) << endl;
			cout << "result:" << strResult << ";" << i << endl;			
		}
		
		if (atoi(strResult.c_str())!=i)
		{
			cout << redistable.get_jedis_addr(buf) << endl;
			cout << "result:" << strResult << ";" << i << endl;
			break;
		}
	}	
}

void JedisTest::Write1MString(const char *ip, unsigned short port)
{
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");
	int i=0;
	char buf[32];
	int iErrorCount = 0;
	for (i=0;i<1000000;i++)
	{
		sprintf(buf,"%d",i);
		if (!redistable.setvalue(buf,buf))
		{
			iErrorCount++;
			cout << "set :" << buf << " failed!" << endl;
		}
	}
}

void JedisTest::TestChinese(const char *ip, unsigned short port)
{
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");
//	redistable.setvalue("test",g2u("中国").c_str());
//	string strResult1 = redistable.get("test");
	string strResult = u2g(redistable.get("test"));
	cout << redistable.get_jedis_addr("test") << endl;
	cout << strResult << endl;
	assert(strResult == "中国");
}

void JedisTest::InitExport(const char *ip, unsigned short port)
{
	char buf[32];
	int i = 0;
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");

	//	初始化string
	for (i=0;i<iMaxCount;i++)
	{
		sprintf(buf,"%d",i);
		if (!redistable.setvalue(buf,buf))
		{
			cout << "set :" << buf << " failed!" << endl;
		}		
	}
/*	
	//	初始化hash
	for (i=0;i<iMaxCount;i++)
	{
		string value = itostr(i);
		string key = "h"+value;
		for (int j=0;j<iMaxSubCount;j++)
		{
			string field = key + "_" + itostr(j);
			redistable.hset(key.c_str(),field.c_str(),value.c_str());
		}
	}
	
	//	初始化list
	list<string> l;
	for (i=0;i<iMaxCount;i++)
	{
		string value = itostr(i);
		string key = "l"+value;
		l.clear();
		for (int j=0;j<iMaxSubCount;j++)
		{
			string lValue = itostr(j) + "_" + value;
			l.push_back(lValue);
		}
		redistable.lpush(key.c_str(),l);		
	}
	
	//	初始化set
	for (i=0;i<iMaxCount;i++)
	{
		string value = itostr(i);
		string key = "s"+value;
		l.clear();
		for (int j=0;j<iMaxSubCount;j++)
		{
			string lValue = itostr(j) + "_" + value;
			l.push_back(lValue);
		}
		redistable.sadd(key.c_str(),l);
	}
	
	//	初始化zset
	map<string,double> m;
	for (i=0;i<iMaxCount;i++)
	{
		string value = itostr(i);
		string key = "z"+value;
		m.clear();
		for (int j=0;j<iMaxSubCount;j++)
		{
			string offset = key+ "_" + itostr(j);
			m[offset]=(double)j;
		}
		redistable.zadd(key.c_str(),m);
	}
*/	
}

void JedisTest::RunExport(const char *ip, unsigned short port)
{
	char buf[32];
	int i = 0;
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");

	//	初始化string

	while (true)
	{
		for (i=0;i<iMaxCount;i++)
		{
			sprintf(buf,"%d",i);
			if (!redistable.setvalue(buf,buf))
			{
				cout << "set :" << buf << " failed!" << endl;
			}		
		}
	}
}

void JedisTest::TestExport(const char *ip, unsigned short port)
{
	char buf[32];
	string strResult;
	int i=0;
	DataCenter datacenter;
	datacenter.init(ip, port);
	RedisDB redisdb = datacenter.get_redisdb("TEST");
	RedisTable redistable = redisdb.get_redis_table("TABLE");

	//	验证字符串是否正常
	for (i=0;i<iMaxCount;i++)
	{
		sprintf(buf,"%d",i);
		while(1)
		{
			strResult = redistable.get(buf);
			if (strResult.length()!=0)
				break;
			cout << redistable.get_jedis_addr(buf) << endl;
			cout << "result:" << strResult << ";" << i << endl;			
		}
		assert(atoi(strResult.c_str())==i);
	}
/*
	//	验证hash 是否正常
	for (i=0;i<iMaxCount;i++)
	{		
		for (int j=0;j<iMaxSubCount;j++)
		{
			string value = itostr(i);
			string key = "h"+value;
			for (int j=0;j<iMaxSubCount;j++)
			{
				string field = key + "_" + itostr(j);
				string strResult = redistable.hget(key.c_str(),field.c_str());
				assert(atoi(strResult.c_str())==i);
			}
		}
	}

	//	验证list 是否正常
//	for (i=0;i<iMaxCount;i++)
//	{
//		string key = "l" + itostr(i);
//		string value = redistable.lpop(key.c_str());
//		assert(atoi(value.c_str())==i);		
//	}

	//	验证set 是否正常
	for (i=0;i<iMaxCount;i++)
	{
		string key = "s" + itostr(i);
		
	}
	
	//	验证zset 是否正常
*/	
}

