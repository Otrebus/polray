#include "Utils.h"

std::vector<std::string> split(const std::string& s, char delim) {
	std::vector<std::string> ret;
	for (int i = 0, i2 = 0; i <= s.size(); i = i2 + 1) {
		i2 = s.find(delim, i);
		i2 = i2 == -1 ? s.size() : i2;
		ret.push_back(s.substr(i, i2 - i));
	}
	return ret;
}