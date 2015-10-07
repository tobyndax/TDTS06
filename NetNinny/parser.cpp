#include "parser.h"


//The list of keywords to be blocked.
std::list<std::string> keywords = {"norrk\x94ping","norrkoping",
"spongebob","britney","spears","hilton","eller"};

/*
* Custom comparator to map to handle lower/upper-case discrepancies.
*/


struct comp {
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

/*
* prints a string with special characters visible.
*/

void printString(std::string s){

  for(char& c : s) {
    printchar((unsigned char)c);
  }
  std::cout << std::endl;
}

//
// Thanks to:
// http://stackoverflow.com/questions/1079748/how-to-print-n-instead-of-a-newline
//

void printchar(unsigned char theChar){

  switch (theChar) {

    case '\n':
    printf("\\n\n");
    break;
    case '\r':
    printf("\\r");
    break;
    case '\t':
    printf("\\t");
    break;
    default:
    if ((theChar < 0x20) || (theChar > 0x7f)) {
      printf("\\%03o", (unsigned char)theChar);
    } else {
      printf("%c", theChar);
    }
    break;
  }
}

/*
* save the first line as URL
* extract every colon seperated data into key and content.
*/

std::map<std::string,std::string,comp> parseHttp(std::string s){
  std::map<std::string, std::string,comp> m;

  std::istringstream resp(s);
  std::string header;
  std::string::size_type index;
  bool firstLine = true;
  while (std::getline(resp, header) && header != "\r") {
    if(firstLine){
      m.insert(std::make_pair("URL",header.substr(4,header.length())));
      firstLine = false;
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

/*
* Check URL for inapropriate substrings.
*/

bool censorURL(std::map<std::string, std::string,comp> m){
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

/*
* Check complete content for inappropriate substrings.
*/

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

/*
* Remove line about encoding in header to ensure unencoded messages.
*/

std::string removeEnc(std::string content){
  std::istringstream resp(content);
  std::string newContent;
  std::string line;
  std::string::size_type index;
  while (std::getline(resp, line)) {
    boost::algorithm::to_lower(line);
    index = line.find("accept-encoding:",0);
    if(index != std::string::npos){
      continue;
    }else if(line == "\r"){
    	newContent.append(line+"\n");
    	continue;
    }
    else{
      newContent.append(line+"\n");
    }
  }
  return newContent;
}

/*
* change connection to connection: close, to work faster with our recv.
*/

std::string modifyConn(std::string content){
  std::istringstream resp(content);
  std::string newContent;
  std::string line;
  std::string::size_type index;
  while (std::getline(resp, line)) {
    index = line.find("onnection:",0);
    if(index != std::string::npos){
      newContent.append("connection: close \r\n");
      continue;
    }else if(line == "\r"){
    	newContent.append(line+"\n");
    	continue;
    }
    else{
      newContent.append(line+"\n");
    }
  }
  return newContent;
}
