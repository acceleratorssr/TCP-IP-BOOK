#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
void error_handling(const char *message);




int main(int argc, char *argv[])
{
    int sockfd;
    char message[BUF_SIZE];
    int str_len;
    struct sockaddr_in serv_adr;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    sockfd=socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd==-1)
    {
        error_handling("socket() error");
        exit(1);
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family=AF_INET;
    serv_adr.sin_addr.s_addr=inet_addr(argv[1]);
    serv_adr.sin_port=htons(atoi(argv[2]));

    if(connect(sockfd, (struct sockaddr *)&serv_adr, sizeof(serv_adr))==-1)
    {
        error_handling("connect() error");
    }else{
        puts("Connected ... ");
    }

    while(1)
    {
        fputs("Input jisuanshi(Q to quit): ",stdout);
        fgets(message, BUF_SIZE, stdin);
        if(!strcmp(message, "q\n")||!strcmp(message,"Q\n"))
        {
            break;
        }
        write(sockfd, message, strlen(message));
        str_len = read(sockfd, message, BUF_SIZE - 1);
        puts("ok");
        message[str_len] = 0;
        printf("Message from server: %s\n %zu\n", message, strlen(message));//ulong

        // int res;                                         //计算式
        // str_len=read(sockfd, &res, sizeof(res));
        // message[str_len]=0;
        // printf("Message from server: %d\n", res);
    }
    close(sockfd);
    return 0;
}

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}