#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(){
    // file descriptors for the client and server sockets
    int S_sockfd;
    int C_sockfd;

    // used to store legnths of the server and client socket address
    int S_len;
    int C_len;
    
    // variables to store the client and server addresses
    struct sockaddr_in S_addr;
    struct sockaddr_in C_addr;

    // variable to store incomming message
    char fromClient;

    // server configuration
    S_addr.sin_family = AF_INET;
    S_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    S_addr.sin_port = htons(9734);
    S_len = sizeof(S_addr);

    //creation of a socket
    S_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    // bind the socket to the address/ports
    bind(S_sockfd, (struct sockaddr *) &S_addr, S_len);

    // make socket listen for incomming connections
    listen(S_sockfd, 5);


    while(1){
        // print that server is waiting for an incomming connection
        printf("Server %d is waiting\n", getpid());

        // configure client fd on a connection accept
        C_len = sizeof(C_addr);
        C_sockfd = accept(S_sockfd, (struct sockaddr *) &C_addr, &C_len);

        // open the file to which we are supposed to write to 
        int fd = open("copyFile.txt", O_RDWR | O_CREAT, 0777);
        // check for errors in file opening
        if(fd == -1){
            printf("Error Creating/Opening File!!\n");
            exit(1);
        }

        int bytesWritten = 0;

        // read the messages from the client
        while(read(C_sockfd, &fromClient, 1) == 1){
            // send the messaged to the copied file
            write(fd, &fromClient, 1);
            bytesWritten++;
        }

        printf("The Server Wrote %d bytes!\n", bytesWritten);

        // close the connection
        close(C_sockfd);
    }

}