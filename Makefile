all: server client

client: simpleClient.c clientfuncs.c clientfuncs.h
	gcc -g simpleClient.c clientfuncs.c -o client

server: simpleServer.c
	gcc -g simpleServer.c -pthread -o server
