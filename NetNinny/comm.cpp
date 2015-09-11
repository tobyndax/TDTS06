#include "comm.h"

std::string errorUrl1 = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error1.html\r\nConnection: Close\r\n\r\n";
std::string errorUrl2 = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error2.html\r\nConnection: Close\r\n\r\n";



Comm::Comm(int allowedConns){
  this->allowedConns = allowedConns;

}

std::string Comm::communicate(std::string content){
  sockaddr_in webserver;
  int webSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(webSocket == -1){
    std::cerr << "Could not create socket to web.\n";
    close(webSocket);
    return "";
  }

  std::map<std::string,std::string> m = parseHttp(content);
  bool cens = censorURL(m);
  if(cens){
    close(webSocket);
    return errorUrl1;
  }

  std::map<std::string,std::string>::iterator it;

  if(m.find("Host")==m.end()){
    std::cerr << "Host not found in map! " << content.size() <<std::endl;
    //std::cerr << content << std::endl;
    close(webSocket);
    return "";
  }

  std::string address =  m.find("Host")->second;

  hostent* host = gethostbyname(address.c_str());
  if(!host)
  {
    std::cerr << "Host not found!\n";
    //std::string out = "Proxy could not find address. :S";
    //send(browser_socket, out.c_str(), out.size(), 0);
    close(webSocket);
    return "";
  }

  webserver.sin_family = AF_INET;

  //This is a bit of magic. (atleast for me)
  webserver.sin_addr.s_addr = reinterpret_cast<in_addr*>(host->h_addr)->s_addr;
  webserver.sin_port = htons(port);
  memset(webserver.sin_zero, 0, sizeof(webserver.sin_zero));

  if(connect(webSocket, (const sockaddr*)&webserver, sizeof(webserver)) < 0){
    std::cerr << "Could not connect to webserver.\n";
    close(webSocket);
    return "";
  }
  
  content = removeEnc(content);

  int n;
  if((n = send(webSocket, content.c_str(), content.size(), 0)) < 0){
    std::cerr << "Could not send to webserver.\n";
  }

  content = "";

  int buffersize = 4000;
  char buffer[buffersize];

  while(canRead(webSocket, 500))
  {
    n = recv(webSocket, buffer, buffersize, 0);
    if(n <= 0)
    break;

    content.append( std::string(buffer, n) );

  }
  close(webSocket);
  std::map<std::string,std::string> mWeb = parseHttp(content);

  std::string contentType =  mWeb.find("Content-Type")->second;
  if(contentType == "text/html"){
    std::cerr <<"-----------Found text type----------\n";
    cens = censorContent(content);
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

bool Comm::canRead(int socket, unsigned int timeout)
{
  timeval time;
  fd_set fd;
  FD_ZERO(&fd);
  FD_SET(socket, &fd);
  time.tv_sec = timeout / 1000;
  time.tv_usec = (timeout % 1000) * 1000;
  return ( select(socket+1, &fd, NULL, NULL, &time) > 0 );
}
