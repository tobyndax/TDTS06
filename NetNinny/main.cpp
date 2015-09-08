#include "proxy.h"
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
using namespace std;

//forward declaration
int validate(int,char* argv[]);

int main(int argc, char* argv[]){
  int proxyPort = validate(argc,argv);
  Proxy* proxy = new Proxy(proxyPort,10);

  return 0;
}
void printHelp(char* scriptname){
  cout << scriptname << " [portnumber]" << endl;
  exit(-1);
}
/*--------------------------------------------------------------
*   Validation of input arguments, how many, and that
*   the argument passed is in fact a number.
*--------------------------------------------------------------*/
int validate(int argc, char* argv[]){
  if(argc != 2){
    cerr << "incorrect amount of arguments" << endl;
    printHelp(argv[0]);
  }

  istringstream ss(argv[1]);
  int port;

  if (!(ss >> port)){
    cerr << "Invalid (non-integer) input: " << argv[1] << '\n';
    printHelp(argv[0]);
  }
  return port;
}
