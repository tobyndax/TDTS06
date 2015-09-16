#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdio.h>
#include <boost/algorithm/string.hpp>


struct comp;

std::map<std::string,std::string,comp> parseHttp(std::string s);
bool censorURL(std::map<std::string, std::string, comp> m);
bool censorContent(std::string content);
std::string removeEnc(std::string content);
void printchar(unsigned char theChar);
void printString(std::string s);
std::string modifyConn(std::string content);
