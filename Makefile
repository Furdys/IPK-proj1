CLIENT=ipk-client
SERVER=ipk-server

CC=gcc
CFLAGS=-Wall -pedantic -lm

all: $(CLIENT) $(SERVER)

$(CLIENT): $(CLIENT).c
	$(CC) $(CFLAGS) -o $(CLIENT) $(CLIENT).c 

$(SERVER): $(SERVER).c
	$(CC) $(CFLAGS) -o $(SERVER) $(SERVER).c

clean:
	rm -f *.o $(CLIENT) $(SERVER)
