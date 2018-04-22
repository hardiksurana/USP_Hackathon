#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for thread
#include<signal.h>


 #define MAX_CLIENTS 5


//the thread function
void *new_connection_handler(void *);
int parseInput(char *);
char **get_tokens(char *);
int is_special(char);
void get_file(char *);


int sock;
int main(int argc , char *argv[])
{
    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;


    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }


    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8888 );
    bzero (&server.sin_zero, 8);



    //Bind
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }


    //Listen
    listen(socket_desc , MAX_CLIENTS);



    //Accept and incoming connection
    printf("Waiting for incoming connections\n");




    c = sizeof(struct sockaddr_in);
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        printf("Connection accepted");



        pthread_t thread_id;




        if( pthread_create( &thread_id , NULL ,  new_connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }



     printf("Handler assigned\n");
    }


    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }

    return 0;
}


void *new_connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    sock = *(int*)socket_desc;
    int read_size;
    char *message , client_message[2000];



    //Send some messages to the client
    message = "This is connection handler\n";
    write(sock , message , strlen(message));


    message = "Type something \n";
    write(sock , message , strlen(message));

    //Receive a message from client
    while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
    {
        //Send the message back to client
        printf("Client CMD: %s\n", client_message);
        int paresedMessage = parseInput(client_message);
        if(paresedMessage == 1){
                char data[100000];
                char buf[100];
                memset(data, 0, sizeof(data));
                memset(buf, 0, sizeof(buf));
                FILE *stream = popen(client_message, "r");
                while (fgets(buf, sizeof(buf), stream) != 0) {
                        strcat(data, buf);
                }
                pclose(stream);
                write(sock , data , strlen(data));

        }
        write(sock , "\n" , strlen("\n"));

    }

    if(read_size == 0)
    {
        printf("Client disconnected\n");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    //Free the socket pointer
    free(socket_desc);

    return 0;
}

int parseInput(char *ip){
        ip[strcspn(ip, "\r\n")] = 0;
        printf("Parse Input : %s SIZE : %d \n", ip, strlen(ip));
        if(strlen(ip) < 2){
                return 0;
        }
        char **tokens = get_tokens(ip);
        if (strcmp(tokens[0], "cd")==0){
                chdir(tokens[1]);
                return 0;
        } else if ( strcmp(tokens[0], "get")==0){
                get_file(tokens[1]);
                return 0;
        } else if ( strcmp(tokens[0], "send")==0){

                return 0;
        } else if( strcmp(tokens[0], "ctrl+c")==0){
                printf("Clinet disconnected\n");
                return 0;
        }
        return 1;
}

void get_file(char *fileName){
        FILE *fp = fopen(fileName, "r");
        if(!fp){
                char *msg = "Could not find File\n";
                printf("%s\n", msg);
                write(sock , msg , strlen(msg));
        } else {
                char buf[100];
                while(fread(buf, 1, sizeof buf, fp)>0){
                        printf("SENT : %d bytes\n",strlen(buf));
                        write(sock , buf , strlen(buf));
                }
                fclose(fp);
                write(sock , "\n" , strlen("\n"));
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
