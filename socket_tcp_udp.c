#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <arpa/inet.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

// error exit
#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

// print error message
void error(const char *msg){
    perror(msg);
    exit(0);
}
// calculate file size
int getFileSize(char *type,long file){
    if(!strcmp(type,"Bytes")){
        return file;
    }
    if(!strcmp(type,"KB")){
        return file / 1024;
    }
    if(!strcmp(type,"MB")){
        return file / (1024*1024);
    }
    if(!strcmp(type,"GB")){
        return file / (1024*1024*1024);
    }
    return file;
}
// choose file size type
char *getFileType(long size){
    if(size / 1024 == 0){
        return "Bytes";
    }
    if(size / (1024*1024) == 0){
        return "KB";
    }
    if(size / (1024*1024*1024) == 0){
        return "MB";
    }
    if(size > (1024*1024*1024)){
        return "GB";
    }
    return "Bytes";
}

void client_tcp(char *hostIP , int port, char*filename){
    struct sockaddr_in serv_addr;
    int err_flag;

    // 檢查hostIP
    struct hostent *server;
    server = gethostbyname(hostIP);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    // 建立socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }
    // server端的資訊
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length); // 將server中的h_addr複製到serv_addr中，長度為h_length(address的長度)
    serv_addr.sin_port = htons(port);
    // connect
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }
    
    //Sending file size
    struct stat st;
    stat(filename,&st);
    long filesize = st.st_size;
    write(sockfd,&filesize,sizeof(filesize));

    //Sending file
    FILE *fp = fopen(filename,"rb");
    int process = 0;
    long numbytes, sendbytes = 0;
    char buffer[1000];
    time_t cur_time, start_time,end_time;

    time(&cur_time);
    start_time = clock();
    printf("%d%% %s",process,ctime(&cur_time));
    // 讀檔後傳送
	while(!feof(fp)){
		numbytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
        numbytes = write(sockfd, buffer, numbytes);
        
        sendbytes += numbytes;
        if(((double)sendbytes / (double)filesize) >= 0.25 && process == 0){
            process = 25;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)sendbytes / (double)filesize) >= 0.5 && process == 25){
            process = 50;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)sendbytes / (double)filesize) >= 0.75 && process == 50){
            process = 75;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)sendbytes / (double)filesize) == 1 && process == 75){
            process = 100;
            time(&cur_time);
            end_time = clock();
            printf("%d%% %s",process,ctime(&cur_time));
        }
	}
    fclose(fp);
    
    printf("total trans time : %f ms\n",difftime(end_time,start_time)/1000000);
    printf("file size : %d %s\n", getFileSize(getFileType(filesize),filesize), getFileType(filesize));
    
    close(sockfd);
    return;
}

void server_tcp(int port){
    struct sockaddr_in serv_addr, cli_addr;
    int err_flag;

    // 建立socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); // 初始化，將struct涵蓋的bits設為0
    serv_addr.sin_family = AF_INET; // IPV4
    serv_addr.sin_addr.s_addr = INADDR_ANY; // 表示不在乎loacl IP是什麼
    serv_addr.sin_port = htons(port);
    // bind
    if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }
    // listen
    listen(sockfd,5);
    // accept
    socklen_t clilen = sizeof(cli_addr);
    int newsockfd = accept(sockfd,(struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0){
        error("ERROR on accept");
    }

    // Receive file size
    long filesize;
    read(newsockfd,&filesize,sizeof(filesize));

    // Receive file
    FILE *fp = fopen("receive_tcp.txt","wb");
    long numbytes, receivebytes = 0;
    int process = 0;
    char buffer[1000];
    time_t cur_time, start_time,end_time;

    time(&cur_time);
    start_time = clock();
    printf("%d%% %s",process,ctime(&cur_time));
    // 接收檔案，直到收到的檔案為0bytes
	while(1){
		numbytes = read(newsockfd, buffer, sizeof(buffer));
		if(numbytes == 0){
			break;
		}
		numbytes = fwrite(buffer, sizeof(char), numbytes, fp);
        receivebytes += numbytes;
        if(((double)receivebytes / (double)filesize) >= 0.25 && process == 0){
            process = 25;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)receivebytes / (double)filesize) >= 0.5 && process == 25){
            process = 50;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)receivebytes / (double)filesize) >= 0.75 && process == 50){
            process = 75;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)receivebytes / (double)filesize) == 1 && process == 75){
            process = 100;
            time(&cur_time);
            end_time = clock();
            printf("%d%% %s",process,ctime(&cur_time));
        }
	}
    fclose(fp);

    printf("total trans time : %f ms\n",difftime(end_time,start_time)/1000000);
    printf("file size : %d %s\n", getFileSize(getFileType(filesize),filesize), getFileType(filesize));

    close(newsockfd);
    close(sockfd);

    return;
}

void client_udp(char *hostIP , int port, char*filename){
    // 建立 socket
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        ERR_EXIT("socket");
    }
    // server info
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(hostIP);

    //Sending file size
    struct stat st;
    stat(filename,&st);
    long filesize = st.st_size, recvsize;
    // confrim that server has got the filesize
    while(1){
        sendto(sock, &filesize, sizeof(filesize), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        int f = recvfrom(sock, &recvsize, sizeof(&recvsize), 0, NULL, NULL);
        if(filesize == recvsize){
            break;
        }
    }
    // Sending file
    FILE *fp = fopen(filename,"rb");
    int process = 0;
    long numbytes, sendbytes = 0;
    char buffer[1000];
    time_t cur_time, start_time,end_time;

    time(&cur_time);
    start_time = clock();
    printf("%d%% %s",process,ctime(&cur_time));
    // 讀檔後傳送
    while (!feof(fp)){
        numbytes = fread(buffer, sizeof(char), sizeof(buffer), fp);
        sendto(sock, buffer,numbytes,0,(struct sockaddr *)&servaddr, sizeof(servaddr));

        sendbytes += numbytes;
        if(((double)sendbytes / (double)filesize) >= 0.25 && process == 0){
            process = 25;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)sendbytes / (double)filesize) >= 0.5 && process == 25){
            process = 50;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)sendbytes / (double)filesize) >= 0.75 && process == 50){
            process = 75;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)sendbytes / (double)filesize) == 1 && process == 75){
            process = 100;
            time(&cur_time);
            end_time = clock();
            printf("%d%% %s",process,ctime(&cur_time));
        }
    }
    fclose(fp);
    // tell end
    char endbuf[3] = {'e','n','d'};
    long gotsize = -1;

    while(1){
        // 多送幾次，確保server收得到
        sendto(sock, endbuf, sizeof(endbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        sendto(sock, endbuf, sizeof(endbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        sendto(sock, endbuf, sizeof(endbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        sendto(sock, endbuf, sizeof(endbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        sendto(sock, endbuf, sizeof(endbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        int f = recvfrom(sock, &gotsize, sizeof(gotsize), 0, NULL, NULL);
        if(f>0){
            break;
        }
    }
    printf("total trans time : %f ms\n",difftime(end_time,start_time)/1000000);
    printf("file size : %d %s\n", getFileSize(getFileType(filesize),filesize), getFileType(filesize));
    if(gotsize != -1){
        printf("packet loss rate : %f %%\n",((double)(filesize - gotsize) / (double)filesize)*100);
    }
    close(sock);

    return;
}

void server_udp(int port){
    // 建立socket
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        ERR_EXIT("socket error");
    }
    // server 資訊    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    // bind
    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
        ERR_EXIT("bind error");
    }

    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int err_flag;

    // Receive file size
    long filesize;
    int recv_flag = 0;
    peerlen = sizeof(peeraddr);
    while(1){
        recv_flag = recvfrom(sock, &filesize, sizeof(filesize), 0,(struct sockaddr *)&peeraddr, &peerlen);
        if(recv_flag != 0){
            sendto(sock, &filesize ,recv_flag, 0,(struct sockaddr *)&peeraddr, peerlen);
            break;
        }
    }
    // Receive file
    FILE *fp = fopen("receive_udp.txt","wb");
    long numbytes, receivebytes = 0;
    int process = 0;
    char buffer[1000];
    time_t cur_time, start_time,end_time;

    time(&cur_time);
    start_time = clock();
    printf("%d%% %s",process,ctime(&cur_time));
    // 一直接收，直到收到end的訊息
    while (1){
        peerlen = sizeof(peeraddr);
        memset(buffer, 0, sizeof(buffer)); // buffer 歸0
        numbytes = recvfrom(sock, buffer, sizeof(buffer), 0,(struct sockaddr *)&peeraddr, &peerlen);
        // 收到sender發送的end，代表傳送結束
        if(!strcmp(buffer,"end")){
            if(process != 100){
                time(&cur_time);
                end_time = clock();
                printf("<100%% %s",ctime(&cur_time));
            }
            // 傳送多次，確保client收得到
            sendto(sock, &receivebytes ,sizeof(receivebytes), 0,(struct sockaddr *)&peeraddr, peerlen);
            sendto(sock, &receivebytes ,sizeof(receivebytes), 0,(struct sockaddr *)&peeraddr, peerlen);
            sendto(sock, &receivebytes ,sizeof(receivebytes), 0,(struct sockaddr *)&peeraddr, peerlen);
            break;    
        }
        numbytes = fwrite(buffer, sizeof(char), numbytes, fp);
        receivebytes += numbytes;
        if(((double)receivebytes / (double)filesize) >= 0.25 && process == 0){
            process = 25;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)receivebytes / (double)filesize) >= 0.5 && process == 25){
            process = 50;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)receivebytes / (double)filesize) >= 0.75 && process == 50){
            process = 75;
            time(&cur_time);
            printf("%d%% %s",process,ctime(&cur_time));
        }
        if(((double)receivebytes / (double)filesize) == 1 && process == 75){
            process = 100;
            time(&cur_time);
            end_time = clock();
            printf("%d%% %s",process,ctime(&cur_time));
        }
    }
    fclose(fp);

    printf("total trans time : %f ms\n",difftime(end_time,start_time)/1000000);
    printf("file size : %d %s\n", getFileSize(getFileType(receivebytes),receivebytes), getFileType(receivebytes));
    printf("packet loss rate : %f %%\n",((double)(filesize - receivebytes) / (double)filesize)*100);

    close(sock);
    return;
}

int main(int argc, char *argv[]){
    // argv[1]: tcp/udp , argv[2]: send/recv , argv[3]: host , argv[4]: port , argv[5]: filename
    // tcp 
    if(!strcmp(argv[1] , "tcp")){
        if(!strcmp(argv[2] , "send")){
            client_tcp(argv[3],atoi(argv[4]),argv[5]);
        }else if(!strcmp(argv[2] , "recv")){
            server_tcp(atoi(argv[4]));
        }
    }
    // udp
    else if(!strcmp(argv[1] , "udp")){
        if(!strcmp(argv[2] , "send")){
            client_udp(argv[3],atoi(argv[4]),argv[5]);
        }else if(!strcmp(argv[2] , "recv")){
            server_udp(atoi(argv[4]));
        }
    }
    return 0;
}