#include "parser.h"

std::list<std::string> keywords = {"norrk\x94ping","norrkoping",
"spongebob","britney","spears", "paris", "hilton","ipad", "kunskap"};

std::map<std::string,std::string> parseHttp(std::string s){
  std::map<std::string, std::string> m;

  //std::transform(s.begin(), s.end(), s.begin(), ::tolower);
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

bool censorURL(std::map<std::string, std::string> m){
  std::string url = m.find("URL")->second;
  std::transform(url.begin(), url.end(), url.begin(), ::tolower);
  for (std::list<std::string>::iterator it=keywords.begin(); it != keywords.end(); ++it){
    std::size_t found = url.find(*it);
    if (found!=std::string::npos){
      return true;
    }
  }
  return false;
}
bool censorContent(std::string content){
  std::transform(content.begin(), content.end(), content.begin(), ::tolower);
  for (std::list<std::string>::iterator it=keywords.begin(); it != keywords.end(); ++it){
    std::size_t found = content.find(*it);
    if (found!=std::string::npos){
      return true;
    }
  }
  return false;
}


std::string removeEnc(std::string content){
  std::istringstream resp(content);
  std::string newContent;
  std::string line;
  std::string::size_type index;
  while (std::getline(resp, line)  && line != "\r") {
    index = line.find("Accept-Encoding:",0);
    if(index != std::string::npos){
      std::cerr << "FOUND ENC___________________________:!";
      newContent.append("Accept-Encoding: \r\n");
      continue;
    }else{
      newContent.append(line);
    }
  }
  return newContent;
}
