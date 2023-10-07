#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int i;
    struct hostent *host;

    if (argc != 2)
    {
        printf("Usage : %s <addr>\n", argv[0]); // perror只能接收一个字符串,且只能有前缀不能在中间
        exit(1);
    }

    host = gethostbyname(argv[1]);
    /*
    struct hostent
    {
        char *h_name;         //正式主机名
        char **h_aliases;     //主机别名
        int h_addrtype;       //主机IP地址类型：IPV4-AF_INET
        int h_length;         //主机IP地址字节长度，对于IPv4是四字节，即32位
        char **h_addr_list;   //主机的IP地址列表
    }; */
    if (!host)
        error_handling("gethostbyname() error");

    printf("Official name: %s \n", host->h_name);
    for (i = 0; host->h_aliases[i]; i++)
        printf("Aliases %d: %s \n", i + 1, host->h_aliases[i]);
    printf("Address type: %s \n", (host->h_addrtype == AF_INET) ? "AF_INET" : "AF_INET6");
    for (i = 0; host->h_addr_list[i]; i++)
        printf("IP addr %d: %s \n", i + 1, inet_ntoa(*(struct in_addr *)host->h_addr_list[i]));
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
