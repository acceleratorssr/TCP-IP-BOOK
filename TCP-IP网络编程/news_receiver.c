

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h> //多播

// struct ip_mreq
// {
//     struct in_addr imr_multiaddr;//主机即将加入监听的组播地址
//     struct in_addr imr_interface;//指定客户端主机加入组播使用哪张网卡的IP地址
// };

const int BUF_SIZE = 1024;

void error_handling(char *message);

int main(int argc, char *argv[])
{
    int recv_sock;
    int str_len = 0;
    char buf[BUF_SIZE];
    struct sockaddr_in adr;
    struct ip_mreq join_adr;
    if (argc != 3)
    {
        printf("Usage : %s <GrooupIP> <PORT>\n", argv[0]);
        exit(1);
    }
    recv_sock = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&adr, 0, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_port = htons(atoi(argv[2]));
    adr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    if (bind(recv_sock, (struct sockaddr *)&adr, sizeof(adr)) == -1)
        error_handling("bind() error");

    join_adr.imr_multiaddr.s_addr = inet_addr(argv[1]);
    join_adr.imr_interface.s_addr = htonl(INADDR_ANY);

    setsockopt(recv_sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&join_adr, sizeof(join_adr)); //........

    while (1)
    {
        printf("%d \n", str_len);
        str_len = recvfrom(recv_sock, buf, BUF_SIZE - 1, 0, NULL, 0);
        printf("%d \n", str_len);
        if (str_len < 0)
            break;
        buf[str_len] = 0;
        fputs(buf, stdout);
    }
    close(recv_sock);
    return 0;
}

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
