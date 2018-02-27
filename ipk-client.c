#include <stdio.h>
#include <stdlib.h> /// atoi
#include <unistd.h> // getopt


int main(int argc, char **argv)
{
	int c;
	char *host;
	int port;
	char *login;
	char flag = '0';


	while((c = getopt (argc, argv, "h:p:n:f:l:")) != -1)
	{
		switch(c)
		{
			case 'h':
				host = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 'n':
			case 'f':
			case 'l':
				if(flag != '0')
				{
					fprintf (stderr, "ERROR: Invalid arguments (-n, -f and -l can't be combinated)\n");
					return 1;
				}
				flag = c;
				login = optarg;
				break;
			case '?':
				fprintf (stderr, "ERROR: Invalid arguments\n");
				return 1;
				break;
		}
	}
	
	printf("Host: %s\nPort: %d\nLogin: %s\n", host, port, login);
	return 0;
}
