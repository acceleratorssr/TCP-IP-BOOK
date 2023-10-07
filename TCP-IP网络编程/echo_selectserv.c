#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

const int BUF_SIZE = 1024;

void error_handling(char *buf);

int main(int argc, char *argv[])
{
    int ser_sock, cli_sock;
    char *adr = "10.0.12.13";
    struct sockaddr_in ser_adr, cli_adr;
    struct timeval timeout;
    fd_set reads, cpy_reads;

    socklen_t adr_sz;
    int fd_max, str_len, fd_num;
    char buf[BUF_SIZE];

    if (argc != 2)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    ser_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (ser_sock == -1)
        error_handling("socket() error");

    memset(&ser_adr, 0, sizeof(ser_adr));
    ser_adr.sin_family = AF_INET;
    ser_adr.sin_port = htons(atoi(argv[1]));
    ser_adr.sin_addr.s_addr = inet_addr(adr);

    if (bind(ser_sock, (struct sockaddr *)&ser_adr, sizeof(ser_adr)) == -1)
        error_handling("bind() error");
    if (listen(ser_sock, 5) == -1)
        error_handling("listen() error");

    FD_ZERO(&reads);
    FD_SET(ser_sock, &reads);
    fd_max = ser_sock;

    while (1)
    {
        cpy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1)
            break;
        if (fd_num == 0)
            continue;

        for (int i = 0; i < fd_max + 1; i++)
        {
            if (FD_ISSET(i, &cpy_reads))
            {
                if (i == ser_sock) // 连接请求
                {
                    adr_sz = sizeof(cli_adr);
                    cli_sock = accept(ser_sock, (struct sockaddr *)&cli_adr, &adr_sz);
                    FD_SET(cli_sock, &reads);
                    if (fd_max < cli_sock)
                        fd_max = cli_sock;
                    printf("connect client : %d \n", cli_sock);
                }
                else
                {
                    str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0)
                    {
                        FD_CLR(i, &reads);
                        close(i);
                        printf("closed client : %d \n", i);
                    }
                    else
                    {
                        write(i, buf, str_len);
                    }
                }
            }
        }
    }
    close(ser_sock);
    return 0;
}

void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}
