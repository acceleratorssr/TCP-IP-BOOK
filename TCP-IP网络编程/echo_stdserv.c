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
    char *adr = "10.0.12.13";
    int serv_sock, cli_sock;
    char message[BUF_SIZE];
    int str_len, i;

    struct sockaddr_in serv_adr, cli_adr;
    socklen_t cli_adr_sz;
    FILE *readfp;
    FILE *writefp;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock == -1)
        error_handling("socket() error");

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_port = htons(atoi(argv[1]));
    serv_adr.sin_addr.s_addr = inet_addr(adr);

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    
    if (listen(serv_sock, 5) == -1)
    {
        printf("%s\n",strerror(errno));
        error_handling("listen() error");
    }
    
    cli_adr_sz = sizeof(cli_adr);

    for (i = 0; i < 5; i++)
    {
        cli_sock = accept(serv_sock, (struct sockaddr *)&cli_adr, &cli_adr_sz);
        if (cli_sock == -1)
            error_handling("accept() error");
        else
            printf("connect client %d \n", i + 1);

        readfp = fdopen(cli_sock, "r");
        writefp = fdopen(cli_sock, "w");
        while (!feof(readfp))
        {
            fgets(message, BUF_SIZE, readfp);
            fputs(message, writefp);
            fflush(writefp); // 强刷
        }
        fclose(readfp);
        fclose(writefp);
    }
    close(serv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
