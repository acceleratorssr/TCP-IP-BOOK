#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE=1024;
void error_handling(char *message);
void read_routine(int sock, char *buf);
void write_routine(int sock, char *buf);

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in cli_adr;
    char buf[BUF_SIZE];
    pid_t pid;

    if(argc!=3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
    }

    sock=socket(AF_INET, SOCK_STREAM, 0);
    cli_adr.sin_family=AF_INET;
    cli_adr.sin_port=htons(atoi(argv[2]));
    cli_adr.sin_addr.s_addr=inet_addr(argv[1]);

    if(connect(sock, (struct sockaddr*)&cli_adr, sizeof(cli_adr))==-1)
    {
        error_handling("connect() error");
    }

    pid=fork();
    if(pid==0)
    {
        write_routine(sock, buf);
    }
    else
    {
        read_routine(sock, buf);
    }
    close(sock);
    return 0;

}

void read_routine(int sock, char *buf)
{
    while(1)
    {
        int str_len=read(sock, buf, BUF_SIZE);
        if(str_len==0)
            return;

        buf[str_len]=0;
        printf("Message from server: %s \n", buf);
    }
}

void write_routine(int sock, char *buf)
{
    while(1)
    {
        fgets(buf, BUF_SIZE, stdin);
        if(!strcmp(buf, "q\n")||!strcmp(buf, "q\n"))
        {
            shutdown(sock, SHUT_WR);
            /*关闭一个已经建立的套接字连接,SHUT_RD（或 0）：关闭套接字的读（接收）部分，使其不能再接收数据。
            SHUT_WR（或 1）：关闭套接字的写（发送）部分，使其不能再发送数据。
            SHUT_RDWR（或 2）：同时关闭套接字的读和写部分，等同于分别调用 shutdown 两次分别关闭读和写。*/
            return;
        }
        write(sock, buf, strlen(buf));
    }
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
