#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUF_SIZE    100
#define NAME_SIZE   20

char name[NAME_SIZE]="[DEFAULT]";
char msg[BUF_SIZE];

void* send_msg(void *arg);
void* recv_msg(void *arg);
void error_handling(char *msg);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in ser_adr;
    pthread_t s_thread, r_thread;
    void *thread_return;

    if(argc!=4)
    {
        printf("Usage : %s <IP> <port> <name>\n", argv[0]);
        exit(1);
    }

    // sprintf(name, "[%s]", argv[3]);
    strcpy(name, argv[3]);

    sockfd=socket(AF_INET, SOCK_STREAM, 0);

    memset(&ser_adr, 0, sizeof(ser_adr));
    ser_adr.sin_family=AF_INET;
    ser_adr.sin_port=htons(atoi(argv[2]));
    ser_adr.sin_addr.s_addr=inet_addr(argv[1]);

    if(connect(sockfd, (struct sockaddr*)&ser_adr, sizeof(ser_adr))==-1)
        error_handling("connect() error");

    pthread_create(&s_thread, NULL, send_msg, (void *)&sockfd);
    pthread_create(&r_thread, NULL, recv_msg, (void *)&sockfd);

    pthread_join(s_thread, &thread_return);
    pthread_join(r_thread, &thread_return);
    close(sockfd);
    return 0;
}

void* send_msg(void *arg)
{
    int sock=*((int *)arg);
    char name_msg[NAME_SIZE+BUF_SIZE];
    while(1)
    {
        fgets(msg, BUF_SIZE, stdin);
        if(!strcmp(msg, "q\n")||!strcmp(msg, "Q\n"))
        {
            close(sock);
            exit(0);
        }
        sprintf(name_msg, "%s %s", name, msg);
        write(sock, name_msg, strlen(name_msg));
    }
    puts("send over");
    return NULL;
}

void* recv_msg(void *arg)
{
    int sock=*((int *)arg);
    char name_msg[NAME_SIZE+BUF_SIZE];
    int str_len;
    while(1)
    {
        str_len=read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
        if(str_len==-1)
            return (void *)-1;
        name_msg[str_len]=0;
        fputs(name_msg, stdout);
    }
    puts("recv over");
    return NULL;
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

