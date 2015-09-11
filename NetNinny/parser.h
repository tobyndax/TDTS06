#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>


std::map<std::string,std::string> parseHttp(std::string s);
bool censorURL(std::map<std::string, std::string> m);
bool censorContent(std::string content);
std::string removeEnc(std::string content);
