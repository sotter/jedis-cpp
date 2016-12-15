/******************************************************
 *   FileName: utility.cpp
 *     Author: liubo  2012-12-21 
 *Description:
 *******************************************************/
#include "utility.h"

// 自动拆分前多个分割符处理
bool utility::split2vec(const string &str, std::vector<std::string> &vec, const std::string split, const int count)
{
	if (str.empty())
		return false;

	std::size_t end = 0;
	std::size_t len = split.length();
	std::size_t pos = str.find(split.c_str());

	int index = 0;
	while (pos != std::string::npos) {
		vec.push_back(str.substr(end, pos - end));

		end = pos + len;
		if (count > 0 && ++index == count) {
			break;
		}
		pos = str.find(split.c_str(), end);
	}

	if (end < str.length()) {
		vec.push_back(str.substr(end));
		if (index != count && count > 0) {
			++index;
		}
	}

	return (count == 0 || index == count);
}
