#include "ext_data_receive.h"
#include <error.h>
#include <unistd.h>
#include <fcntl.h>


#define DEBUG

Ext_data_receive::Ext_data_receive( int portno, const char *defaultOutputString ) {

     extDataBuffer = new char[EXTDATABUFFERSIZE];
     bzero(extDataBuffer,EXTDATABUFFERSIZE);
     dataPickedUp = 1;
     
     defaultOutput = new char[EXTDATABUFFERSIZE];
     if (defaultOutputString) {
	     strcpy(defaultOutput,defaultOutputString);
	     // fprintf(stderr,"Default output: %s\n",defaultOutputString);
     } else {
	     strcpy(defaultOutput,"");
     }
	
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) {
	     fprintf(stderr,"ERROR opening socket");
	     exit(1);
     }

     serv_addr = new sockaddr_in;
     cli_addr = new sockaddr_in;

     bzero((char *) serv_addr, sizeof(sockaddr_in));
     serv_addr->sin_family = AF_INET;
     serv_addr->sin_addr.s_addr = INADDR_ANY;
     serv_addr->sin_port = htons(portno);
     if (bind(sockfd,(sockaddr*)serv_addr,
	      sizeof(sockaddr)) < 0) {
	     fprintf(stderr,"ERROR on binding");
	     exit(1);
     }
     listen(sockfd,5);
     clilen = sizeof(sockaddr);
     newsockfd = accept(sockfd, 
			(sockaddr*)cli_addr, 
			&clilen);
     if (newsockfd < 0) {
	     fprintf(stderr,"ERROR on accept");
	     exit(1);
     }

}


int Ext_data_receive::readSocket() {
	int n = 0;
	timeval waitd;
	waitd.tv_sec = 0;
	waitd.tv_usec = 0;

	FD_ZERO(&read_flags);
	FD_SET(newsockfd,&read_flags);

	int err=select(newsockfd+1,&read_flags,NULL,NULL,&waitd);
	if(err < 0) {
		fprintf(stderr,"select failed\n");
		exit(1);
	}

	if (err>0) {
		if(FD_ISSET(newsockfd, &read_flags)) { //Socket ready for reading
			n = read(newsockfd,extDataBuffer,255);
			if (n < 0) {
				fprintf(stderr,"ERROR reading from socket");
				exit(1);
			}
			dataPickedUp = 0;
		}
	}

	for(unsigned int i=0;i<strlen(extDataBuffer);i++) {
		char c=extDataBuffer[i];
		if (c<' ') {
			extDataBuffer[i] = 0;
			break;
		}
		if (i>=EXTDATABUFFERSIZE) {
			extDataBuffer[0] = 0;
			break;
		}
	}
	
	return n;
}

const char* Ext_data_receive::getData() {
	if (dataPickedUp) {
		return defaultOutput;
	} else {
		dataPickedUp = 1;
		return extDataBuffer;
	}
}


Ext_data_receive::~Ext_data_receive() {
     close(newsockfd);
     close(sockfd);
     delete serv_addr;
     delete cli_addr;
}
