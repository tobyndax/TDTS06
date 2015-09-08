#include "proxy.h"

Proxy::Proxy(int proxyPort,int allowedConns){
  this->proxyPort = proxyPort;
  this->allowedConns = allowedConns;

  connectBrowser();
}

void Proxy::connectBrowser(){

  this->serverSocket = socket(AF_INET,SOCK_STREAM,0);
  if(this->serverSocket == -1){
    std::cout << "Could not create the server socket.\n";
    exit(0);
  }
  sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(proxyPort);
  server.sin_addr.s_addr = INADDR_ANY;

  if(bind(this->serverSocket, (const sockaddr*)&server, sizeof(server)) == -1){
    std::cout << "Could not bind the socket to the port.\n";
    exit(0);
  }
  if(listen(this->serverSocket, this->allowedConns) == -1){
    std::cout << "Could not listen on server socket.\n";
    exit(0);
  }
  bool listening = true;
  Comm* comm = new Comm(allowedConns);
  while(listening){
    comm->communicate(this->sniff());
  }
}

int Proxy::getPort(){
  return this->proxyPort;
}

bool Proxy::canRead(int socket, unsigned int timeout)
{
  timeval time;
  fd_set fd;
  FD_ZERO(&fd);
  FD_SET(socket, &fd);
  time.tv_sec = timeout / 1000;
  time.tv_usec = (timeout % 1000) * 1000;
  return ( select(socket+1, &fd, NULL, NULL, &time) > 0 );
}

std::string Proxy::sniff(){

  sockaddr_in browAddr;
  socklen_t browAddrLen;

  this->browserSocket = accept(this->serverSocket, (sockaddr*)&browAddr, &browAddrLen);
  if(!fork()){
    std::cout << "New child created" << std::endl;
    int n = 0, buffersize = 4000;
    char buffer[buffersize];
    int empty = 0;
    std::string content = "";
    while(canRead(this->browserSocket, 1000)){

      n = recv(this->browserSocket, buffer, buffersize, 0);
      if(n > 0)
      std::cerr << "Got " << n << " bytes from browser.\n";
      else if(n < 0 || empty++ > 3)
      break;
      content.append(std::string(buffer, n));

    }
    //std::cout << content << std::endl;
    //std::string out = "HTTP/1.1 301 Moved Permanently\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error2.html\r\nConnection: Close\r\n\r\n";
    //send(this->browserSocket, out.c_str(), out.size(), 0);
    if (this->browserSocket){
      close(this->browserSocket);
    }
    return content;
  }else{
    close(this->browserSocket);
    return "";

  }

}
