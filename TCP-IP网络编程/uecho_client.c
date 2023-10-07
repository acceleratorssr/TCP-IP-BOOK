
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
    socklen_t adr_sz;
    char message[BUF_SIZE];
    struct sockaddr_in ser_adr, from_adr;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    if ((ser_sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket() error");
        exit(1);
    }

    memset(&ser_adr, 0, sizeof(ser_adr));
    ser_adr.sin_addr.s_addr = inet_addr(argv[1]);
    ser_adr.sin_family = AF_INET;
    ser_adr.sin_port = htons(atoi(argv[2]));

    if (bind(ser_sockfd, (struct sockaddr *)&ser_adr, sizeof(ser_adr)) == -1)
    {
        perror("bind() error");
        exit(1);
    }

    while (1)
    {
        fputs("Insert message(q to quit): ", stdout);
        fgets(message, sizeof(message), stdin);
        if(!strcmp(message, "q\n")||!strcmp(message, "Q\n"))
            break;
        sendto(ser_sockfd, message, BUF_SIZE, 0, (struct sockaddr*)&ser_adr, sizeof(ser_adr));
        adr_sz=sizeof(from_adr);
        str_len=recvfrom(ser_sockfd, message, BUF_SIZE, 0, (struct sockaddr*)&from_adr, &adr_sz);
        message[str_len]=0;
        printf("Message from server: %s", message);
    }

    close(ser_sockfd);
    return 0;
}
