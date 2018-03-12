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

#define MSG_START "!OK!\n"
#define MSG_NOTFOUND "!NF!\n"
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
	char buffer[1024];
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

	
	
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	int validHandshake = 0;
	int validFarewell = 0;
	char* bufferPrintable;	// Printable part of the buffer (excluding handshake)

	while((n = read(sockfd, buffer, sizeof(buffer)-1)) > 0)
    {
		// --- Checking reading error ---
		if(n < 0) 
			errorExit("Couldn't read from socket");


		// --- Checking handshake ---
        if(validHandshake == 0)
        {
			int handshakeLength = strlen(MSG_START);
			
			if(strncmp(buffer, MSG_START, handshakeLength))	// Check if beggining of the buffer is handshake (!OK!)
			{
				if(!strncmp(buffer, MSG_NOTFOUND, strlen(MSG_NOTFOUND)))
					errorExit("No result to be shown");
				else
					errorExit("Unexpected handshake from server");
			}
			else
			{
				//validHandshake = 1;
				bufferPrintable = &buffer[handshakeLength];
			}
		}
		else
			buffer[n] = '\0';	// End of received message
        
        
		// --- Searching farewell ---
		if(validFarewell == 0)
		{		
			if(!strncmp(&buffer[n-4], MSG_END, strlen(MSG_END)))	// Comapre the end of packet
			{
				validFarewell = 1;
				buffer[n-4] = '\0';	// Shorten the message (earse farewell)
			}
		}
		else
			validFarewell = 0;	// Received some content after the farewell*/
		
		
		// --- Printing the message ---
		printf("%s",bufferPrintable);
		fflush(stdout);
		
		
		// --- Restoring buffer after handshake ---
		if(validHandshake == 0)
		{
			bufferPrintable = buffer;
			validHandshake = 1;
		}
	}


	// --- Checking farewell ---
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
    {
        errorExit("Couldn't find such host");
	}
	
	
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
				
				if(c != 'l')
					*login = optarg; // Save login parameter (option -l is processed later)
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
			*login = "";
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
