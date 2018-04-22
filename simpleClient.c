#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for thread
#include<signal.h>
#include <fcntl.h>

#include "clientfuncs.h"

int is_special(char ch){
       	if(ch >= 'a' && ch <='z' )
                return 0;
        if(ch >= 'A' && ch <= 'Z')
                return 0;
        if(ch >= '0' && ch <= '9')
                return 0;
        if(ch == '.' || ch == '-' )
                return 0;
        return ch;
}


void get_file(char *fileName){
        int fp = open(fileName, O_CREAT|O_WRONLY|O_TRUNC, 0644);
        if(!fp){
                char *msg = "Could not open find File\n";
                printf("%s\n", msg);
        } else {
                int len;
                char buf[100];
                while((len = recv(sock, buf, sizeof(buf), 0)) > 0){
                        write(fp, buf, strlen(buf));
                        printf("RECVD : %d bytes  \nbuff : %s\n",strlen(buf), buf);
                        if (len < sizeof(buf)){
                                break;
                        }
                }
                printf("Came here\n");
                close(fp);
                write(sock, "\n", strlen("\n"));
        }
}

char **get_tokens(char *line){

        char *copyOfLine = strdup(line);
        char **tokens;

        tokens = malloc(sizeof(char *)*10);
        memset(tokens, 0, sizeof(tokens));

        char subbuff[20];

        int llen = strlen(copyOfLine);
        int numTokens = 0;
        int i =0, j=0;
        while(i < llen){
                if(is_special(copyOfLine[j])) {
                        if (copyOfLine[j] == ' ') {
                                memcpy( subbuff, copyOfLine+i, j-i);
                                subbuff[j-i] = '\0';
                                tokens[numTokens] = strdup(subbuff);
                                numTokens++;
                                i=j+1;
                        }
                }
                if(j == llen){
                        memcpy( subbuff, copyOfLine+i, j-i);
                        subbuff[j-i] = '\0';
                        tokens[numTokens] = strdup(subbuff);
                        numTokens++;
                        i=j+1;
                }
                j++;
        }
        tokens[numTokens] = NULL;
        return tokens;
}


int parseInput(char *ip){
        ip[strcspn(ip, "\r\n")] = 0;
        printf("Parse Input : %s SIZE : %d \n", ip, strlen(ip));
        if(strlen(ip) < 2){
                return 0;
        }
        char **tokens = get_tokens(ip);
        if ( strcmp(tokens[0], "get")==0){
                get_file(tokens[1]);
                return 0;
        } else if ( strcmp(tokens[0], "send")==0){

                return 0;
        }
        return 1;
}




int main(int argc , char *argv[])
{

    struct sockaddr_in server;
    char message[1000] , server_reply[2000];
    int len;
    if( signal(SIGINT, sighandler) < 0 ){
            printf("Could Not Set Signal Handler\n");
    }

    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");

    server.sin_addr.s_addr = inet_addr(argv[1]);
    server.sin_family = AF_INET;
    server.sin_port = htons( atoi(argv[2]) );

    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }

    puts("-----------------------Connected-------------------------------\n");

    //keep communicating with server
    while((len = recv(sock, server_reply, sizeof(server_reply), 0)) > 0)
    {
        printf("\n%.*s\n", len, server_reply);

        printf(">> ");
        if (fgets(message, sizeof(message), stdin) == NULL)
            break;

        //Send some data
        if( send(sock , message , strlen(message) , 0) < 0)
        {
            puts("Send failed");
            return 1;
        } else{
                parseInput(message);
        }
    }

    close(sock);

    return 0;
}
