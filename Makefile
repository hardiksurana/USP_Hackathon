all: server client

client: simpleClient.c clientfuncs.c clientfuncs.h
	gcc simpleClient.c clientfuncs.c -o client

server: simpleServer.c
	gcc simpleServer.c -pthread -o server
