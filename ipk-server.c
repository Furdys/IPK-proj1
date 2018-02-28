/**
 * @file ipk-server.c
 * @brief
 * @author Jiri Furda (xfurda00)
 */ 


#include <stdio.h>
#include <stdlib.h> // atoi

#include <unistd.h> // getopt
#include <string.h>	// bzero

#include <sys/socket.h> // socket
#include <netinet/in.h> // internet domain adresses

// --- Prototypes ---
void getArguments(int argc, char** argv, int* port);
void errorExit(char* msg);


// --- Functions ---
int main(int argc, char** argv)
{
	int port;

	getArguments(argc, argv, &port);
	
	printf("Port: %d\n", port);
	
	
	///////////////////
	int sockfd;
	socklen_t newsockfd, clilen;	// Because of gcc warnings
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int  n;
	
	
	// --- Creating socket ---
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(sockfd < 0)
		errorExit("ERROR: Couldn't create socket");
	
	
	// --- Setting up the socket ---
	memset(&serv_addr, '\0', sizeof(serv_addr));	// Earse adress buffer
	
	serv_addr.sin_family = AF_INET;	// Symbolic constant
	serv_addr.sin_port = htons(port);	// Convert to network byte order
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// localost
	
	
	
	// --- Binding the socket ---
	int binded = bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(binded < 0)
		errorExit("Couldn't bind socket");
	
	
	// --- Client connection ---
	printf("Waiting for client\n");
	
	listen(sockfd,5);	// Listen for connection
	
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);	// Sleep untill connection
	if(newsockfd < 0) 
		errorExit("Couldn't accept connection");
	
	printf("Client is connected\n");
	
	
	// --- Reading messages ---
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	n = read(newsockfd, buffer, sizeof(buffer)-1);	// Sleep untill message is sent
	if(n < 0)
		errorExit("Couldn't read from socket");
	
	printf("Received: %s", buffer);
	
	
	// --- Send reply ---
	char* reply = "Roger that!";
	n = write(newsockfd, reply, strlen(reply));
	if(n < 0)
		errorExit("Couldn't write socket");
	
	return 0;
}


void getArguments(int argc, char** argv, int* port)
{
	int c;
	
	while((c = getopt (argc, argv, "p:")) != -1)
	{
		switch(c)
		{
			case 'p':
				*port = atoi(optarg);
				break;
			case '?':
				errorExit("Invalid arguments");
				break;
			default:
				errorExit("Strange error while loading arguments");
				break;
		}
	}
}


void errorExit(char* msg)
{
	fprintf(stderr,"ERROR: %s\n", msg);
	exit(1);	
}
