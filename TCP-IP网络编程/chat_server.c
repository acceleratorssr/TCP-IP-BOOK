#define _REENTRANT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define MAX_CLI     256

const int BUF_SIZE = 100;

void *handle_cli(void *arg);
void send_msg(char *msg, int len);
void error_handling(char *msg);

int cli_cnt = 0;        // 临界区
int cli_socks[MAX_CLI]; // 临界区
pthread_mutex_t mutex;

int main(int argc, char *argv[])
{
    char *adr = "10.0.12.13";
    int ser_sock, cli_sock;
    struct sockaddr_in ser_adr, cli_adr;
    int cli_adr_sz;
    pthread_t t_id;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    pthread_mutex_init(&mutex, NULL);

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

    while (1)
    {
        cli_adr_sz = sizeof(cli_adr);
        cli_sock = accept(ser_sock, (struct sockaddr *)&cli_adr, &cli_adr_sz);

        pthread_mutex_lock(&mutex);
        cli_socks[cli_cnt++] = cli_sock;
        pthread_mutex_unlock(&mutex);

        pthread_create(&t_id, NULL, handle_cli, (void *)&cli_sock);
        pthread_detach(t_id);
        printf("线程id: %ld\n", t_id);
        printf("connected client ip: %s \n", inet_ntoa(cli_adr.sin_addr));
    }
    close(ser_sock);
    return 0;
}

void *handle_cli(void *arg)
{
    int cli_sock = *((int *)arg);
    int str_len = 0;
    int i;
    char msg[BUF_SIZE];

    while ((str_len = read(cli_sock, msg, sizeof(msg))) != 0)
        send_msg(msg, str_len);

    pthread_mutex_lock(&mutex);
    for (i = 0; i < cli_cnt; i++)
    {
        while (i++ < cli_cnt - 1)
            cli_socks[i] = cli_socks[i + 1];
        break;
    }
    cli_cnt--;
    pthread_mutex_unlock(&mutex);
    close(cli_sock);
    return NULL;
}

void send_msg(char *msg, int len)
{
    int i;
    pthread_mutex_lock(&mutex);
    for (i = 0; i < cli_cnt; i++)
        write(cli_socks[i], msg, len);
    pthread_mutex_unlock(&mutex);
}

void error_handling(char *msg)
{
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}
