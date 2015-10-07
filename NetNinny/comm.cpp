#include "comm.h"

// redirect strings
std::string errorUrl1 = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error1.html\r\nConnection: Close\r\n\r\n";
std::string errorUrl2 = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error2.html\r\nConnection: Close\r\n\r\n";


struct comp {
  bool operator() (const std::string& lhs, const std::string& rhs) const {
    return strcasecmp(lhs.c_str(), rhs.c_str()) < 0;
  }
};


Comm::Comm(int allowedConns){
  this->allowedConns = allowedConns;
}

/*
* create socket outward towards webserver
* parseHttp return a map with <key,content> , around ':',
* example: <Host, www.test.se>
* censor the url
* do host lookup
* connect to the websocket
* remove encoding
* change to connection close
* send browser request to webserver
* recieve response from webserver
* read until receiving exactly zero bytes (i.e. connection closed);
* censor incoming data if text.
*
*/
std::string Comm::communicate(std::string content){

  sockaddr_in webserver;
  int webSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(webSocket == -1){
    std::cerr << "Could not create socket to web.\n";
    close(webSocket);
    return "";
  }

  std::map<std::string,std::string,comp> m = parseHttp(content);
  bool cens = censorURL(m);
  if(cens){
    close(webSocket);
    return errorUrl1;
  }

  std::string address =  m.find("Host")->second;
  hostent* host = gethostbyname(address.c_str());
  if(!host)
  {
    std::cerr << "Host not found!\n";
    close(webSocket);
    return "";
  }


  webserver.sin_family = AF_INET;
  webserver.sin_addr.s_addr = reinterpret_cast<in_addr*>(host->h_addr)->s_addr;
  webserver.sin_port = htons(port);
  memset(webserver.sin_zero, 0, sizeof(webserver.sin_zero));



  if(connect(webSocket, (const sockaddr*)&webserver, sizeof(webserver)) < 0){
    std::cerr << "Could not connect to webserver.\n";
    close(webSocket);
    return "";
  }

  content = removeEnc(content);
  content = modifyConn(content);

  int n = 1;
  if((n = send(webSocket, content.c_str(), content.size(), 0)) < 0){
    std::cerr << "Could not send to webserver.\n";
  }

  content = "";

  int buffersize = 4000;
  char buffer[buffersize];

  while( n > 0 )
  {
    n = recv(webSocket, buffer, buffersize, 0);
    if(n <= 0)
    break;

    content.append( std::string(buffer, n) );
  }


  close(webSocket);
  std::map<std::string,std::string,comp> mWeb = parseHttp(content);

  printString(content);
  std::string contentType =  mWeb.find("Content-Type")->second;
  if(contentType == "text/html"){

    cens = censorContent(content);
    std::cerr << cens ;
    if(cens){
      std::cerr <<"-----------BLOCK----------\n";
      return errorUrl2;
    }
  }

  return content;
}

int Comm::getPort(){
  return this->port;
}
