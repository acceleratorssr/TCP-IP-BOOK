#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

const int BUF_SIZE = 1024;
void error_handling(const char *message);

// 定义操作符栈结构
typedef struct
{
    char data[100];
    int top;
} Stack;

// 函数用于将中缀表达式转换为后缀表达式
void infixToPostfix(char *infix, char *postfix)
{
    int i, j = 0;
    Stack stack;
    stack.top = -1;

    for (i = 0; infix[i]; i++)
    {
        char c = infix[i];
        if (c >= '0' && c <= '9')
        {
            postfix[j++] = c; // 如果是数字，直接添加到后缀表达式中
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            while (stack.top >= 0 && stack.data[stack.top] != '(' &&
                   (c == '+' || c == '-' || (stack.data[stack.top] == '*' || stack.data[stack.top] == '/')))
            {
                // 弹出操作符并添加到后缀表达式中，直到满足优先级条件
                postfix[j++] = stack.data[stack.top--];
            }
            stack.data[++stack.top] = c; // 将操作符入栈
        }
        else if (c == '(')
        {
            stack.data[++stack.top] = c; // 如果是左括号，入栈
        }
        else if (c == ')')
        {
            while (stack.top >= 0 && stack.data[stack.top] != '(')
            {
                // 弹出操作符并添加到后缀表达式中，直到遇到匹配的左括号
                postfix[j++] = stack.data[stack.top--];
            }
            if (stack.data[stack.top] == '(')
                stack.top--; // 弹出左括号
        }
    }

    while (stack.top >= 0)
    {
        // 将栈中的剩余操作符添加到后缀表达式中
        postfix[j++] = stack.data[stack.top--];
    }

    postfix[j] = '\0'; // 为后缀表达式添加终止符
}

// 函数用于计算后缀表达式的结果
int evaluatePostfix(char *postfix)
{
    Stack stack;
    stack.top = -1;

    for (int i = 0; postfix[i]; i++)
    {
        char c = postfix[i];
        if (c >= '0' && c <= '9')
        {
            stack.data[++stack.top] = c - '0'; // 如果是数字，将其转换为整数并入栈
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            if (stack.top < 1)
            {
                printf("Error: Invalid expression.\n");
                exit(1); // 无效表达式
            }
            int operand2 = stack.data[stack.top--];
            int operand1 = stack.data[stack.top--];
            int result = 0;
            if (c == '+')
                result = operand1 + operand2;
            if (c == '-')
                result = operand1 - operand2;
            if (c == '*')
                result = operand1 * operand2;
            if (c == '/')
            {
                if (operand2 == 0)
                {
                    printf("Error: Division by zero.\n");
                    exit(1); // 除零错误
                }
                result = operand1 / operand2;
            }
            stack.data[++stack.top] = result; // 执行操作并将结果入栈
        }
    }

    if (stack.top == 0)
    {
        return stack.data[stack.top]; // 返回最终结果
    }
    else
    {
        printf("Error: Invalid expression.\n");
        exit(1); // 无效表达式
    }
}

int main(int argc, char *argv[])
{
    // char *addr="127.0.0.1";
    char postfix[100];         // 后缀表达式
    char *addr = "10.0.12.13"; // 公网无法绑定，内网才行（不知道为什么）
    int serv_sockfd, cli_sockfd;
    int str_len;
    char message[BUF_SIZE];
    struct sockaddr_in serv_adr, cli_adr;
    socklen_t cli_adr_sz;

    if (argc != 2)
    {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    serv_sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    if (serv_sockfd == -1)
    {
        error_handling("socket() error");
    }

    memset(&serv_adr, 0, sizeof(serv_adr)); // 将serv_addr结构体或变量的所有字节都设置为0
    serv_adr.sin_family = AF_INET;          // 设置IPv4

    serv_adr.sin_addr.s_addr = inet_addr(addr); // 设置服务器地址
    // serv_adr.sin_addr.s_addr=htonl(INADDR_ANY);
    char server_ip[INET_ADDRSTRLEN];                                      // 定义一个字符数组来存储点分十进制表示的IP地址
    inet_ntop(AF_INET, &(serv_adr.sin_addr), server_ip, INET_ADDRSTRLEN); // 将二进制IP地址转换为点分十进制表示
    printf("Server IP Address: %s\n", server_ip);                         // 输出点分十进制表示的IP地址

    serv_adr.sin_port = htons(atoi(argv[1])); // 传入端口号

    if (bind(serv_sockfd, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) // sockaddr_in方便写入数据，sockaddr统一接口
    {
        perror("bind() error");
        error_handling("bind() error");
    }

    if (listen(serv_sockfd, 5) == -1) // 最多等五个
    {
        error_handling("listen() error");
    }

    cli_adr_sz = sizeof(cli_adr);

    for (int i = 0; i < 2; i++)
    {
        cli_sockfd = accept(serv_sockfd, (struct sockaddr *)&cli_adr, &cli_adr_sz); // ACCEPT得到客户端sockfd
        if (cli_sockfd == -1)
        {
            error_handling("accept() error");
        }
        else
        {
            printf("Connect client %d\n", i + 1);
        }
        while ((str_len = read(cli_sockfd, message, BUF_SIZE)) != 0)
        {
            infixToPostfix(message, postfix);
            int result = evaluatePostfix(postfix);
            printf("Result: %d\n", result);
            write(cli_sockfd, &result, sizeof(result));
        }
        close(cli_sockfd);
    }
    close(serv_sockfd);
    return 0;
}

void error_handling(const char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
