#include "parser.h"

std::list<std::string> keywords = {"norrk\x94ping","norrkoping",
	"spongebob","britney","spears", "paris", "hilton"};
std::string errorUrl1 ="";
std::map<std::string,std::string> parseHttp(std::string s){
  std::map<std::string, std::string> m;

  std::istringstream resp(s);
  std::string header;
  std::string::size_type index;
  bool firstLine = true;
  while (std::getline(resp, header) && header != "\r") {
  	if(firstLine){
  		m.insert(std::make_pair("URL",header.substr(4,header.length())));
  		firstLine = false;
  		//std::cerr << m.find("URL")->second << std::endl;
      continue;
  	}
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

std::string censor(std::string content, std::map<std::string, std::string> m){
	std::string url = m.find("URL")->second;
	std::transform(url.begin(), url.end(), url.begin(), ::tolower);
	for (std::list<std::string>::iterator it=keywords.begin(); it != keywords.end(); ++it){
		std::size_t found = url.find(*it);
  		if (found!=std::string::npos){
  			return errorUrl1;
  		}
	}
	return "";
}
