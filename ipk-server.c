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
void getUserInfo(char flag, char* login);


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
	
	
	
	
	///////////////////////////////////

/*
	getUserInfo('n', "furdys");
	getUserInfo('h', "furdys");
	getUserInfo('l', "sys");
	getUserInfo('l', NULL);
*/

	return 0;
}

void getUserInfo(char flag, char* login)
{
	FILE *file = fopen("/etc/passwd", "r");
	if(file == NULL)
		errorExit("Couldn't open file");	
	
	int found = 0;	// Found result
	char line[1024];	// Buffer for reading file
	
	if(flag == 'l')
	{
		if(login == NULL)
		{
			// -- Browsing file --	
			while(fgets(line, sizeof(line), file) != NULL)
			{	
				char *token = strtok(line, ":");	// Get the first colon
				printf("%s\n", token);
			}
		}
		else
		{
			// -- Browsing file --	
			while(fgets(line, sizeof(line), file) != NULL)
			{	
				if(!strncmp(login, line, strlen(login)))	// Matches the rule
				{
					char *token = strtok(line, ":");	// Get the first colon
					printf("%s\n", token);
				}
			}			
		}
	}
	else
	{
		// -- Resolving flag --
		int skip; // How many colons should be skipped
		if(flag == 'n')
			skip = 4;
		else if(flag == 'h')
			skip = 5;
		else
		{
			fclose(file);
			errorExit("Invalid flag");
		}
		
		
		// -- Preparing login variable --
		char newLogin[strlen(login)+2];
		char suffix[2] = ":";
		strcpy(newLogin, login);
		strcat(newLogin, suffix);
		
		
		// -- Searching in file --	
		char line[1024];
		while(fgets(line, sizeof(line), file) != NULL)
		{
			if(!strncmp(newLogin, line, strlen(newLogin)))	// Found the specified login
			{
				found = 1;
				
				// - Skipping colons -
				char *token = strtok(line, ":");
				for(int i = 0; i != skip; i++)
					token = strtok(NULL, ":");
					
				// - Print result -
				printf( "%s\n", token );
			}
		}		
	}

	fclose(file);

	if(found == 0)
	{
		// @todo Return not found and print in on client's STDERR
	}
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
