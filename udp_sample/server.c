/*************************************************************************
    > File Name: echoser_udp.c
    > Author: Simba
    > Mail: dameng34@163.com
    > Created Time: Sun 03 Mar 2013 06:13:55 PM CST
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

void echo_ser(int sock)
{
    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;

    while (1){
        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf)); // buffer 歸0
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,(struct sockaddr *)&peeraddr, &peerlen);
        if (n == -1){
            if (errno == EINTR){
                continue;
            }
            ERR_EXIT("recvfrom error");
        }
        else if(n > 0){
            fputs(recvbuf, stdout);
            sendto(sock, recvbuf, n, 0,(struct sockaddr *)&peeraddr, peerlen);
        }
    }
    close(sock);
}

int main(void){
    // 建立socket
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        ERR_EXIT("socket error");
    }
    // server 資訊    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int err_flag;

    while (1){
        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf)); // buffer 歸0
        err_flag = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,(struct sockaddr *)&peeraddr, &peerlen);
        if (err_flag == -1){
            if (errno == EINTR){
                continue;
            }
            ERR_EXIT("recvfrom error");
        }
        else if(err_flag > 0){
            fputs(recvbuf, stdout);
            sendto(sock, recvbuf,err_flag, 0,(struct sockaddr *)&peeraddr, peerlen);
        }
    }
    close(sock);

    return 0;
}
