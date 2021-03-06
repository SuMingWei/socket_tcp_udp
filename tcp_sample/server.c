/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }
    // 建立socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }

    portno = atoi(argv[1]);
    bzero((char *) &serv_addr, sizeof(serv_addr)); // 初始化，將struct涵蓋的bits設為0
    serv_addr.sin_family = AF_INET; // IPV4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // 表示不在乎loacl IP是什麼
    serv_addr.sin_port = htons(portno);
    // bind
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }
    // listen
    listen(sockfd,5);
    // accept
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        error("ERROR on accept");
    }
    // read
    bzero(buffer,256);
    n = read(newsockfd,buffer,255);
    if (n < 0){
        error("ERROR reading from socket");
    } 
    printf("Here is the message: %s\n",buffer);
    // write
    n = write(newsockfd,"I got your message",18);
    if (n < 0){
        error("ERROR writing to socket");
    } 

    close(newsockfd);
    close(sockfd);
    return 0;
}
