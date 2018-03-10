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

#define MSG_START "!OK!"
#define MSG_NOTFOUND "!NF!"
#define MSG_END "!EN!"

// === Prototypes ===
void connectToServer(int* sockfd, char* host, int port);
void getArguments(int argc, char** argv, char** host, int* port, char** login, char* flag);
void errorExit(char* msg);


// === Functions ===
int main(int argc, char** argv)
{
	// --- Processing arguments ---
	char *host;
	int port;
	char *login = "";
	char flag ;
	getArguments(argc, argv, &host, &port, &login, &flag);

	
	// --- Connecting to server ---
	int sockfd;
	connectToServer(&sockfd, host, port);
	
	
	// --- Communicating with server ---
	char buffer[256];
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	
	
	int loginLength = strlen(login);
	char handshake[loginLength+3];
	strcpy(handshake, login);
	handshake[loginLength] = ':';
	handshake[loginLength+1] = flag;
	handshake[loginLength+2] = '\0';
	
	
	int n;
	n = write(sockfd, handshake, loginLength+2);	// Send input
	if(n < 0) 
		errorExit("Couldn't write to socket");	
	/*
	n = write(sockfd, buffer, strlen(buffer));	// Send input
	if(n < 0) 
		errorExit("Couldn't write to socket");
	*/
	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	
	int validHandshake = 0;
	int validFarewell = 0;
	
    while((n = read(sockfd, buffer, sizeof(buffer)-1)) > 0)
    {		
		if(n < 0) 
			errorExit("Couldn't read from socket");
        
        if(validHandshake == 0)
        {
			if(strncmp(buffer, MSG_START, strlen(MSG_START)))
				errorExit("Unexpected handshake from server");
			else
			{
				validHandshake = 1;
				//bufferPrintable = &buffer[strlen(MSG_START)+1]
			}
		}
        
        buffer[n] = '\0';
        
		if(validFarewell == 0)
		{
			char* lastRow = strrchr(buffer, '\n');
			
			if(!strcmp(&lastRow[1], MSG_END))
			{
				validFarewell = 1;
				lastRow[1] = '\0';
			}
		}
		else
			validFarewell = 0;
        
        printf("%s",buffer);
    } 
    
    if(validFarewell == 0)
		errorExit("Farewell from server not received");
	
	
	return 0;
}

void connectToServer(int* sockfd, char* host, int port)
{
    struct sockaddr_in serv_addr;
    struct hostent *server;	
	
	// --- Creating socket ---
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	
	if(*sockfd < 0) 
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
    int connected = connect(*sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if(connected < 0) 
        errorExit("Couldn't connect");
}


/**
 *
 * @todo "./ipk-client -h host -p port -l" should be legit
 */
void getArguments(int argc, char** argv, char** host, int* port, char** login, char* flag)
{
	int c;
	*flag = '0';
	int hFlag, pFlag; // Required options
	
	
	// --- Loading arguments ---
	while((c = getopt(argc, argv, "h:p:n:f:l")) != -1)
	{
		switch(c)
		{
			case 'h':
				*host = optarg;
				hFlag = 1;
				break;
			case 'p':
				*port = atoi(optarg);
				pFlag = 1;
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
				break;
			case '?':
				errorExit("Invalid arguments");
				break;
			default:
				errorExit("Strange error while loading arguments");
				break;
		}
	}


	// --- Checking arguments ---
	if(*flag == 'l')	// Optional argument with option -p
	{
		if(argc == 7 && optind == 6) // -l <login>
			*login = argv[optind];
		else if(argc == 6 && optind == 6) // -l
			*login = NULL;
		else
			errorExit("Invalid arguments usage");
	}
	else // Argument count check
	{
		if(argc != 7 || optind != 7)
			errorExit("Invalid arguments usage");
	}
	
	// Required options check
	if(hFlag != 1 || pFlag != 1 || *flag == '0')
		errorExit("Invalid arguments usage (Possibly multiple use of one option)");
}


void errorExit(char* msg)
{
	fprintf(stderr,"ERROR: %s\n", msg);
	exit(1);	
}
