#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for thread
#include<signal.h>

#include "clientfuncs.h"

void sighandler(int signo) {
        if(signo == SIGINT) {
                char *message = "ctrl+c";
                if (send(sock , message , strlen(message) , 0) < 0){
                        printf("Could not send SIGINT\n");
                }
                // close(sock);
                printf("\n");
                exit(0);

        }
}
