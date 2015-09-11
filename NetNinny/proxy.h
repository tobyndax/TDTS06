#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>
#include "comm.h"
#include <signal.h>
#include "parser.h"

class Proxy
{
public:
  Proxy(int,int);
  int getPort();
  std::string sniff();

protected:
  void task_1();
  void connectBrowser();
  int proxyPort;
  int allowedConns;
  bool canRead(int,unsigned int);
  int serverSocket;
  int browserSocket;
  Comm* comm;
};
