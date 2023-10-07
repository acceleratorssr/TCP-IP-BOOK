#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

const int BUF_SIZE = 1024;

int main(int argc, char *argv[])
{
    char *addr = "10.0.12.13";
    int ser_sockfd;
    int str_len;
    socklen_t cli_adr_sz;
    char message[BUF_SIZE];
    struct sockaddr_in ser_adr, cli_adr;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    if ((ser_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket() error");
        exit(1);
    }

    memset(&ser_adr, 0, sizeof(ser_adr));
    ser_adr.sin_addr.s_addr = inet_addr(addr);
    ser_adr.sin_family = AF_INET;
    ser_adr.sin_port = htons(atoi(argv[0]));

    if (bind(ser_sockfd, (struct sockaddr *)&ser_adr, sizeof(ser_adr)) == -1)
    {
        perror("bind() error");
        exit(1);
    }

    while (1)
    {
        cli_adr_sz = sizeof(cli_adr);
        str_len = recvfrom(ser_sockfd, message, BUF_SIZE, 0,(struct sockaddr*)&cli_adr, &cli_adr_sz);
        sendto(ser_sockfd, message, BUF_SIZE, 0, (struct sockaddr*)&cli_adr, cli_adr_sz);
    }
    close(ser_sockfd);
    return 0;
}
