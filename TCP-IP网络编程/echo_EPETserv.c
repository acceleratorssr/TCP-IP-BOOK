#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

const int BUF_SIZE = 4;
const int EPOLL_SIZE = 50;

void SetNonBlockingMode(int fd);
void error_handling(char *buf);

int main(int argc, char *argv[])
{
    char *adr="10.0.12.13";
    int ser_sock, cli_sock;
    struct sockaddr_in ser_adr, cli_adr;
    socklen_t adr_sz;
    int str_len, i;
    char *buf[BUF_SIZE];

    struct epoll_event *ep_events;
    struct epoll_event event;
    int epfd, event_cnt;

    if(argc!=2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    ser_sock=socket(AF_INET, SOCK_STREAM, 0);
    memset(&ser_adr, 0, sizeof(ser_adr));
    ser_adr.sin_family=AF_INET;
    ser_adr.sin_port=htons(atoi(argv[1]));
    ser_adr.sin_addr.s_addr=inet_addr(adr);

    if(bind(ser_sock, (struct sockaddr*)&ser_adr, sizeof(ser_adr))==-1)
        error_handling("bind() error");
    if(listen(ser_sock, 5)==-1)
        error_handling("listen() error");

    epfd=epoll_create(EPOLL_SIZE);
    ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

    SetNonBlockingMode(ser_sock);
    event.events=EPOLLIN;
    event.data.fd=ser_sock;
    epoll_ctl(epfd, EPOLL_CTL_ADD, ser_sock, &event);

    while(1)
    {
        event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
        if(event_cnt==-1)
            error_handling("epoll_wait() error");
        puts("return epoll_wait");
        for(i=0; i<event_cnt; i++)
        {
            if(ep_events[i].data.fd==ser_sock)
            {
                adr_sz=sizeof(cli_adr);
                cli_sock=accept(ser_sock, (struct sockaddr*)&cli_adr, &adr_sz);
                if(cli_sock==-1)
                    error_handling("accept() error");
                SetNonBlockingMode(cli_sock);
                event.events=EPOLLIN|EPOLLET;//EPOLLET边缘触发
                event.data.fd=cli_sock;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cli_sock, &event);
                printf("connect client: %d \n", cli_sock);
            }
            else
            {
                while(1)
                {
                    str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
                    if(str_len==0)  //此时关闭连接
                    {
                        epoll_ctl(epfd, EPOLL_CTL_ADD, ep_events[i].data.fd, NULL);
                        close(ep_events[i].data.fd);
                        printf("closed clien: %d \n", cli_sock);
                    }
                    else if(str_len<0)//此时读取了输入缓冲中全部数据，故退出循环
                    {
                        if(errno==EAGAIN)
                            break;
                    }
                    else
                    {
                        write(ep_events[i].data.fd, buf, str_len);
                    }
                }
            }
        }
    }
    close(ser_sock);
    return 0;
}

void SetNonBlockingMode(int fd)
{
    int flag=fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flag|O_NONBLOCK);
}

void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}
