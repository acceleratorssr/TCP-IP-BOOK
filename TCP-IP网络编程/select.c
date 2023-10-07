#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>

const int BUF_SIZE=1024;

int main(int agrc, char *argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    /*struct timeval
    {
        __time_t tv_sec;		Seconds. 
        __suseconds_t tv_usec;	Microseconds.  
    };*/
    struct timeval timeout;

    FD_ZERO(&reads);
    FD_SET(0, &reads);//0是标准输入

    while(1)
    {
        temps=reads;
        timeout.tv_sec=5;
        timeout.tv_usec=0;
        result=select(1, &temps, 0, 0, &timeout);
        if(result==-1)
        {
            puts("select() error");
            break;
        }
        else if(result==0)
        {
            puts("timeout");
        }
        else
        {
            if(FD_ISSET(0, &temps))//验证发送变化的文件描述符是否为标准输入
            {
                str_len=read(0, buf, BUF_SIZE);
                buf[str_len]=0;
                printf("message from console: %s", buf);
            }
        }
    }
    return 0;
}