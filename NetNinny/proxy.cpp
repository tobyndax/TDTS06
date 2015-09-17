#include "proxy.h"

//function for reaping zombie processes

void sigchld_handler(int s)
{
	// waitpid() might overwrite errno, so we save and restore it:
	int saved_errno = errno;

	while(waitpid(-1, NULL, WNOHANG) > 0);

	errno = saved_errno;
}

//construct the proxy with the specified port.
Proxy::Proxy(int proxyPort,int allowedConns){
	this->proxyPort = proxyPort;
	this->allowedConns = allowedConns;

	connectBrowser();
}

/*
*	create Socket
* bind socket
* listen to socket
* reap zombie processes
* accept connection
* fork out communcation upon accept.
*	recieve browser data in sniff()
* send data to Comm class with communicate(string)
* send response from communicate back to browser
*
*/

void Proxy::connectBrowser(){

	struct sigaction sa;

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


	sa.sa_handler = sigchld_handler; // reap all dead processes
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}


	while(1){


		sockaddr_in browAddr;
		socklen_t browAddrLen;

		this->browserSocket = accept(this->serverSocket, (sockaddr*)&browAddr, &browAddrLen);
		if (this->browserSocket == -1) {
			perror("accept");
			continue;
		}

		if(!fork()){

			Comm* comm = new Comm(allowedConns);
			std::string s = comm->communicate(this->sniff());
			if (s.size() > 0 )
			send(browserSocket,s.c_str(),s.size(),0);
			delete comm;
			exit(0);
		}
		close(browserSocket);
	}
}

int Proxy::getPort(){
	return this->proxyPort;
}

/*
* construct buffer,
* recieve data until the end sequence is found.
*/

std::string Proxy::sniff(){

	int n = 0, buffersize = 4000;
	char buffer[buffersize];
	std::string content = "";

	while(true){
		n = recv(this->browserSocket, buffer, buffersize, 0);
		if(n <= 0)
			break;
		content.append(std::string(buffer, n));
		std::size_t found = content.find("\r\n\r\n");
    if (found!=std::string::npos){
			break;
    }
	}

	return content;
}
