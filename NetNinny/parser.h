#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>


std::map<std::string,std::string> parseHttp(std::string s);
std::string censor(std::string content, std::map<std::string, std::string> m);

