#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 3490

int main(){
    signal(SIGPIPE, SIG_IGN); // on linux to prevent crash on closing socket
    int listeningSocket = -1, data_len = 0, clientSocket = -1;
    char buf[256];
    int flag = 0;
    socklen_t len = sizeof(buf);
    int sock = socket(AF_INET, SOCK_STREAM, 0);//create socket
    if (sock == -1) {
        perror("socket");
        return -1;
    }   
	 
    if((listeningSocket = socket(AF_INET , SOCK_STREAM , 0 )) == -1)
    {
        printf("Could not create listening socket : %d" ,errno);
    }
    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT);  //network order
      
    // Bind the socket to the port with any IP at this port
    if (bind(listeningSocket, (struct sockaddr *)&serverAddress , sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code : %d" ,errno);
	// TODO: close the socket
        return -1;
    } 
    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    if (listen(listeningSocket, 1) == -1) //1 is a Maximum size of queue connection requests
											//number of concurrent connections 
    {
	printf("listen() failed with error code : %d",errno);
	// TODO: close the socket
        return -1;
    }
    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");
      
    struct sockaddr_in clientAddress;  
    socklen_t clientAddressLen = sizeof(clientAddress);

    while(1){
        if (flag == 0){
            memset(&clientAddress, 0, sizeof(clientAddress));
            clientAddressLen = sizeof(clientAddress);
            clientSocket = accept(listeningSocket, (struct sockaddr *)&clientAddress, &clientAddressLen);
            if (clientSocket == -1)
            {
                printf("listen failed with error code : %d",errno);
                // TODO: close the sockets
                return -1;
            }
            printf("A new client connection accepted\n");
            flag = 1;
        }
        if (flag == 1){
            memset(buf, 0, len);
            data_len = recv(clientSocket, buf, len, 0);
            if(data_len<0){
                printf("%d", errno);
            }
            else{
                if(strcmp(buf, "EXIT\n") == 0){
                    flag = 0;
                    close(clientSocket);
                    printf("client disconnected\n");
                    continue;
                }
                puts(buf);
            }
        }
    }
    close(listeningSocket);
    return 0;
}