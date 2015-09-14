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

  if(m.find("host")==m.end()){
    //std::cerr << "Host not found in map! " << content.size() <<std::endl;
    //std::cerr << content << std::endl;
    close(webSocket);
    return "";
  }

  std::string address =  m.find("host")->second;
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

  //Hur ser sista datan ut?. (alltså /r/n /r/n + content)
  //när slutar vi läsa?
  // Plan: Plocka ut content length, trigga på \r\n\r\n och läs sedan c
  //content length till bytes.
  bool count = false;
  bool recieving = true;
  int dataRec = 0;
  int contentLen = 0;
  while(canRead(webSocket,1000)){
    n = recv(webSocket, buffer, buffersize, 0);

    content.append(std::string(buffer, n));
    std::size_t found = content.find("\r\n\r\n");

    //End sequence found, however in this case there might be more data
    // after the sequence which we need to count.
    // so extract position of end seq. and see if we have more data there.
    // since we now have the header resp we can extract the content length.
    if (found!=std::string::npos && !count){
      count = true;
      std::map<std::string,std::string> mWeb = parseHttp2(content);
      contentLen = atoi(mWeb.find("Content-Length")->second.c_str());
      int rem = content.size() - found - 4;
      dataRec += rem;
      //extract content length here. If it doesn't exist end read here.
      continue;
    }
    if(count){
      dataRec += n;
      //printf("%i of %i \n", dataRec, contentLen);
      if (dataRec == contentLen)
        break;
    }
  }


  close(webSocket);
  std::map<std::string,std::string> mWeb = parseHttp(content);

  std::string contentType =  mWeb.find("content-type")->second;
  if (contentType.find("text") != std::string::npos) {
    std::cerr <<"-----------"+contentType+"----------\n";
    cens = censorContent(content);
    if(cens){
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
