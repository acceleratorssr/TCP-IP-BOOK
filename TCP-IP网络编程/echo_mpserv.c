#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 150;
void error_handling(char *message);
void read_childproc(int sig);

int main(int argc, char *argv[])
{
    int serv_sock, cli_sock;
    struct sockaddr_in serv_adr, cli_adr;

    pid_t pid;
    struct sigaction act;
    char buf[BUF_SIZE];
    int state, str_len;
    socklen_t adr_sz;
    char *adr = "10.0.12.13";
    char mes[BUF_SIZE * 2]; // 使用足够大的缓冲区来保存消息


    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    act.sa_handler = read_childproc;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    state = sigaction(SIGCHLD, &act, 0);

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = inet_addr(adr);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
        error_handling("bind() error");
    if (listen(serv_sock, 5) == -1) // 等待队列的最大长度为5
        error_handling("listen() error");

    while (1)
    {
        adr_sz = sizeof(cli_adr);
        cli_sock = accept(serv_sock, (struct sockaddr *)&cli_adr, &adr_sz);
        if (cli_sock == -1)
            continue;
        else
            puts("new client connect ...");
        pid = fork();
        if (pid == -1)
        {
            close(cli_sock);
            continue;
        }
        if (pid == 0)
        {
            close(serv_sock);
            puts("normal");
                
            mes[0]='\0';
            while (1) {
                str_len = read(cli_sock, buf, BUF_SIZE);
                if (str_len <= 0) {
                    break; // 客户端已关闭连接或发生错误
                }

                buf[str_len] = '\0'; // 将接收的数据以C字符串的格式结束

                strcat(mes, buf); // 将接收的数据追加到消息缓冲区

                // 查找换行符来判断是否接收到完整消息
                char *newline = strchr(mes, '\n');
                if (newline != NULL) {
                    *newline = '\0'; // 将消息截断为一行,将指针 newline 所指向的字符（也就是换行符 \n）替换为字符串的结束符 \0。
                    printf("Received message from client: %s\n%zu", mes, strlen(mes));
                    // 在这里对消息进行处理，然后发送响应
                    write(cli_sock, mes, sizeof(mes));
                    // 移除已经处理的消息
                    memmove(mes, newline + 1, strlen(newline + 1) + 1);
                }
                fputc('\n', stdout);
            }
            close(cli_sock);
            puts("client disconnected ...");
            return 0;
        }
        else
            close(cli_sock);
    }
    close(serv_sock);
    return 0;
}

void read_childproc(int sig)
{
    pid_t pid;
    int status;
    pid = waitpid(-1, &status, WNOHANG);
    printf("removed proc id: %d \n", pid);
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
