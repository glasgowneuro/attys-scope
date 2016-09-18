#ifndef _EXT_DATA_RECEIVE
#define _EXT_DATA_RECEIVE

#define EXTDATABUFFERSIZE 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>


class Ext_data_receive {

public:

	Ext_data_receive(int port, const char* defaultOutputString = NULL);
	int readSocket();
	~Ext_data_receive();
	const char* getData();

private:
	int sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	sockaddr_in* serv_addr;
	sockaddr_in* cli_addr;
	int ourPort;

	char* extDataBuffer;

	char* defaultOutput;

	fd_set read_flags;

	int dataPickedUp;
};


#endif
