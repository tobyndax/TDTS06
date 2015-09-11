#include "comm.h"

Comm::Comm(int allowedConns){
  this->allowedConns = allowedConns;

}

std::string Comm::communicate(std::string content){
  sockaddr_in webserver;
  int webSocket = socket(AF_INET, SOCK_STREAM, 0);
  if(webSocket == -1){
    std::cerr << "Could not create socket to web.\n";
    return "";
  }

  std::map<std::string,std::string> m = parseHttp(content);
  std::string testa = censor(content,m);
  std::map<std::string,std::string>::iterator it;

  if(m.find("Host")==m.end()){
  	std::cerr << "Host not found in map! " << content.size() <<std::endl;
  	//std::cerr << content << std::endl;
  	return "";
  }

  std::string address =  m.find("Host")->second;

  hostent* host = gethostbyname(address.c_str());
  if(!host)
  {
    std::cerr << "Host not found!\n";
    //std::string out = "Proxy could not find address. :S";
    //send(browser_socket, out.c_str(), out.size(), 0);
    return "";
  }

  webserver.sin_family = AF_INET;

  //This is a bit of magic. (atleast for me)
  webserver.sin_addr.s_addr = reinterpret_cast<in_addr*>(host->h_addr)->s_addr;
  webserver.sin_port = htons(port);
  memset(webserver.sin_zero, 0, sizeof(webserver.sin_zero));

  if(connect(webSocket, (const sockaddr*)&webserver, sizeof(webserver)) < 0){
    std::cerr << "Could not connect to webserver.\n";
    return "";
  }
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
