/******************************************************
 *   FileName: utility.h
 *     Author: liubo  2012-12-21
 *Description:
 *******************************************************/

#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include <vector>
#include <list>

using namespace std;

class utility
{
public:
    static bool split2vec(const string &str, std::vector<std::string> &vec, const std::string split, const int count);
};
#endif /* UTILITY_H_ */
