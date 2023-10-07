#define _POSIX_C_SOURCE 200809

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

const int BUF_SIZE = 1024;

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int sockfd;
    struct sockaddr_in cli_adr, ser_adr;
    char message[BUF_SIZE];
    int str_len;
    FILE *readfp;
    FILE *writefp;

    if (argc != 3)
    {
        printf("Usage : %s <IP> <port> \n", argv[0]);
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
        error_handling("socket() error");

    memset(&ser_adr, 0, sizeof(ser_adr));
    ser_adr.sin_family = AF_INET;
    ser_adr.sin_port = htons(atoi(argv[2]));
    ser_adr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr *)&ser_adr, sizeof(ser_adr)) == -1)
    {
        printf("%s\n",strerror(errno));
        error_handling("connect() error");
    }
        
    else
        puts("connect ...");

    readfp = fdopen(sockfd, "r");
    writefp = fdopen(sockfd, "w");
    while (1)
    {
        fputs("input message(q to quit): ", stdout);
        fgets(message, BUF_SIZE, stdin);
        if (!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
            break;

        fputs(message, writefp);
        fflush(writefp);
        puts("ok");
        fgets(message, BUF_SIZE, readfp);
        printf("message from server : %s", message);
    }
    fclose(writefp);
    fclose(readfp);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
