#include "parser.h"


std::map<std::string,std::string> parseHttp(std::string s){

  std::cout << "Now Parsing" << std::endl;
  std::map<std::string, std::string> m;

  std::istringstream resp(s);
  std::string header;
  std::string::size_type index;
  while (std::getline(resp, header) && header != "\r") {
    index = header.find(':', 0);
    if(index != std::string::npos) {
      m.insert(std::make_pair(
        boost::algorithm::trim_copy(header.substr(0, index)),
        boost::algorithm::trim_copy(header.substr(index + 1))
      ));
    }
  }
  return m;
}
