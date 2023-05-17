#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
    // variables for the socket 
    int sockfd;
    struct sockaddr_in address;
    int result;
    char toServ;
    int bytesWritten = 0;

    // get the name of the file
    char* fileName = argv[1];
    // open the file
    int fd = open(fileName, O_RDONLY);
    if(fd == -1){
        printf("Error Opening File!\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(9734);

    result = connect(sockfd, (struct sockaddr *) &address, sizeof(address));

    if(result == -1){
        printf("Error In client!\n");
        exit(1);
    }

    while(read(fd, &toServ, 1) == 1){
        write(sockfd, &toServ, 1);
        bytesWritten++;
    }

    printf("The Client Wrote %d bytes\n", bytesWritten);

    close(sockfd);
    exit(0);

    return 0;
}