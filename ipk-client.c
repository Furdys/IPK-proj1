/**
 * @file ipk-client.c
 * @brief
 * @author Jiri Furda (xfurda00)
 */ 


#include <stdio.h>
#include <stdlib.h> // atoi

#include <unistd.h> // getopt
#include <string.h>	// bzero

#include <sys/socket.h> // socket
#include <netinet/in.h> // internet domain adresses
#include <netdb.h>	// gethostbyname



// --- Prototypes ---
void getArguments(int argc, char** argv, char** host, int* port, char** login, char* flag);
void errorExit(char* msg);


// --- Functions ---
int main(int argc, char** argv)
{
	
	char *host;
	int port;
	char *login = "";
	char flag ;

	getArguments(argc, argv, &host, &port, &login, &flag);

	
	printf("Host: %s\nPort: %d\nLogin: %s\n", host, port, login);
	
	///////////////
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;	
	
	char buffer[256];
	
	
	// --- Creating socket ---
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(sockfd < 0) 
        errorExit("Couldn't create socket");	

		
	// --- Resolving host name ---
    server = gethostbyname(host);
    if(server == NULL)
        errorExit("Couldn't find such host");	
	
	
	// --- Setting up server adress ---
	memset(&serv_addr, '\0', sizeof(serv_addr));	// Earse adress buffer
	
    serv_addr.sin_family = AF_INET;	// Symbolic constant
    serv_addr.sin_port = htons(port);	// Convert to network byte order
    memcpy((char *)&serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);	// Set server ip
    
    
    // --- Connecting to server ---
    int connected = connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(connected < 0) 
        errorExit("Couldn't connect");
	
	
	// --- Communicating with server ---
	printf("Write message: ");
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	
	fgets(buffer, sizeof(buffer)-1, stdin);	// Load input
	
	n = write(sockfd, buffer, strlen(buffer));	// Send input
	if(n < 0) 
		errorExit("Couldn't write to socket");
	
	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	n = read(sockfd, buffer, sizeof(buffer)-1);
	if(n < 0) 
		errorExit("Couldn't read from socket");
		
	printf("Received: %s\n",buffer);
	
	
	return 0;
}


/**
 *
 * @todo "./ipk-client -h host -p port -l" should be legit
 */
void getArguments(int argc, char** argv, char** host, int* port, char** login, char* flag)
{
	int c;
	*flag = '0';
	
	while((c = getopt(argc, argv, "h:p:n:f:l:")) != -1)
	{
		switch(c)
		{
			case 'h':
				*host = optarg;
				break;
			case 'p':
				*port = atoi(optarg);
				break;
			case 'n':
			case 'f':
			case 'l':
				if(*flag != '0')
				{
					errorExit("Invalid arguments (-n, -f and -l can't be combinated)");
					exit(1);
				}
				*flag = c;
				*login = optarg;
				break;
			case '?':
				errorExit("Invalid arguments");
				exit(1);
			default:
				errorExit("Strange error while loading arguments");
				exit(1);
		}
	}
}


void errorExit(char* msg)
{
	fprintf(stderr,"ERROR: %s\n", msg);
	exit(1);	
}
