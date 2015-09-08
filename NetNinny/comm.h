#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <string>
#include <cctype>

#include "parser.h"


class Comm
{
public:
    Comm(int);
    int getPort();
    std::string communicate(std::string content);

protected:
    int port = 80;
    int allowedConns;
    bool canRead(int,unsigned int);
};
