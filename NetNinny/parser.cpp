#include "parser.h"



std::vector<std::string> keywords = {"norrk\x94ping","norrkoping",
	"spongebob","britney","spears", "paris", "hilton"};
  
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

bool censorURL(std::string content, std::map<std::string, std::string> m){
	std::string url = m.find("URL")->second;
	for(size_t i =0; i<=keywords.size();i++){
		std::transform(url.begin(), url.end(), url.begin(), ::tolower);
		std::size_t found = url.find(keywords.at(i));
  		if (found!=std::string::npos){
  			return true;
  		}
	}
	return false;
}
