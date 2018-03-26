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

#define MSG_START "!OK!"
#define MSG_NOTFOUND "!NF!"
#define MSG_END "!EN!"

// === Prototypes ===
void createSocket(int* socketFD, int port);
void waitForClient(int welcomeFD, int* clientFD);
void getArguments(int argc, char** argv, int* port);
char waitForQuery(int clientFD, char* buffer);
void sendUserInfo(int socket, char flag, char* login);
void sendMessage(int socket, char* msg);
void errorExit(char* msg);


// === Functions ===
int main(int argc, char** argv)
{
	// --- Processing arguments ---
	int port;
	getArguments(argc, argv, &port);
	
	
	// --- Setting up socket ---
	int socketFD;
	createSocket(&socketFD, port);
	
	
	// --- Client connection ---
	int clientFD; // socklen_t
	waitForClient(socketFD, &clientFD);
	
	
	// --- Reading messages ---
	char buffer[1024];
	memset(buffer, '\0', sizeof(buffer)); 	// Earse buffer
	
	char flag;
	flag = waitForQuery(clientFD, buffer);

	
	// --- Sending result ---
	sendUserInfo(clientFD, flag, buffer);


	// --- Ending connection ---
	close(clientFD);
	return 0;
}


void createSocket(int* socketFD, int port)
{
	struct sockaddr_in serv_addr;
	
	
	// --- Creating socket ---
	*socketFD = socket(AF_INET, SOCK_STREAM, 0);

	if(*socketFD < 0)
		errorExit("ERROR: Couldn't create socket");
	
	
	// --- Setting up the socket ---
	memset(&serv_addr, '\0', sizeof(serv_addr));	// Earse adress buffer
	
	serv_addr.sin_family = AF_INET;	// Symbolic constant
	serv_addr.sin_port = htons(port);	// Convert to network byte order
	serv_addr.sin_addr.s_addr = INADDR_ANY;	// localost
	
	
	
	// --- Binding the socket ---
	int binded = bind(*socketFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
	if(binded < 0)
		errorExit("Couldn't bind socket");
}


void waitForClient(int welcomeFD, int* clientFD)
{
	
	listen(welcomeFD, 5);	// Expect client connection
	
	struct sockaddr_in clientAddr;
	socklen_t clientAddrSize = sizeof(clientAddr);
	
	while(1)
	{
		*clientFD = accept(welcomeFD, (struct sockaddr *) &clientAddr, &clientAddrSize);	// Sleep untill connection
		if(*clientFD < 0) 
			continue;	// Ignore unsuccessful connection
			
		int pid = fork();
		if(pid < 0) 
			errorExit("Couldn't fork the process");
			
		if(pid == 0) // Child
		{
			close(welcomeFD);	// Won't accept new connection, just deal with this one
			return; // End endless loop
		}
		else // Parent	
		{
			close(*clientFD);	// Won't work with this client, just waits for new one
		}
	}
}

char waitForQuery(int clientFD, char* buffer)
{
	char internalBuffer[1024];
	
	do
	{
		int n;
		n = read(clientFD,internalBuffer, sizeof(internalBuffer)-1); // Sleep untill message is sent
		if(n < 0)
			errorExit("Couldn't read from socket");
		
		internalBuffer[n] = '\0';	// End of received message
		strncpy(&buffer[strlen(buffer)], internalBuffer, n);
	}
	while(strrchr(internalBuffer, ':') == NULL);
	
	int handshakeLength = strlen(buffer);
	buffer[handshakeLength-2] = '\0';	// Earse flag
	
	return buffer[handshakeLength-1];	// Return flag
}

void sendUserInfo(int socket, char flag, char* login)
{
	FILE *file = fopen("/etc/passwd", "r");
	if(file == NULL)
		errorExit("Couldn't open file");	
	
	int found = 0;	// Found result
	char line[1024];	// Buffer for reading file
	
	if(flag == 'l')
	{
		if(login == NULL)	// No prefix defined
		{
			// -- Browsing file --	
			while(fgets(line, sizeof(line), file) != NULL)
			{	
				char *token = strtok(line, ":");	// Get the first colon
				sendMessage(socket, token);
				found = 1;
			}
		}
		else	// Prefix defined
		{
			// -- Browsing file --	
			while(fgets(line, sizeof(line), file) != NULL)
			{	
				if(!strncmp(login, line, strlen(login)))	// Matches the rule
				{
					char *token = strtok(line, ":");	// Get the first colon
					sendMessage(socket, token);
					found = 1;
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
		else if(flag == 'f')
			skip = 5;
		else
		{
			fclose(file);
			fprintf(stderr,"ERROR: Invalid flag received\n");
			return;
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
				sendMessage(socket, token);
			}
		}		
	}

	fclose(file);

	
	if(found == 0)
	{
		sendMessage(socket, MSG_NOTFOUND);
	}	

	// --- End connectiom ---
	sendMessage(socket, MSG_END);
	close(socket);
}


void sendMessage(int socket, char* msg)
{
	static int handshakeSent = 0;
	int n;
	
	
	// --- Sending handshake before the first message ---
	if(handshakeSent == 0 && strcmp(msg, MSG_NOTFOUND))
	{
		handshakeSent = 1;
		sendMessage(socket, MSG_START);
	}
	
	
	if(!strcmp(msg, MSG_END))
	{
		n = write(socket, msg, strlen(msg));
		handshakeSent = 0;	// Prepare for next client
	}
	else
	{
		// --- Adding \n at the end of message ---
		int msgLength = strlen(msg);
		char msgSent[msgLength+2];
		strcpy(msgSent, msg);
		msgSent[msgLength] = '\n';
		msgSent[msgLength+1] = '\0';
		n = write(socket, msgSent, msgLength+1);
	}
	
	if(n < 0)
		errorExit("Couldn't write to socket");	
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
	
	if(argc != 3 || optind != 3)
		errorExit("Invalid arguments usage");
}


void errorExit(char* msg)
{
	fprintf(stderr,"ERROR: %s\n", msg);
	exit(1);	
}
