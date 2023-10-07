#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void timeout(int sig)
{
    /*SIGALRM 是一个常见的 POSIX 标准信号，用于表示闹钟超时。
    当一个程序设置了一个闹钟定时器（通过 alarm 函数或者 setitimer 函数等），
    当闹钟超时时，系统会向进程发送 SIGALRM 信号，以通知进程闹钟已经触发。*/
    if(sig==SIGALRM)
        puts("time out");
    alarm(2);
}

void keycontrol(int sig)
{
    /*按下 Ctrl+C 键组合时，会发送 SIGINT 信号给当前运行的进程*/
    if(sig==SIGINT)
        puts("ctrl+c pressed");
}

int main(int argc, char *argv[])
{
    int i;
    signal(SIGALRM, timeout);//在不同操纵系统上可能存在区别
    signal(SIGINT, keycontrol);
    alarm(2);

    for(i=0; i<3; i++)
    {
        puts("wait ...");
        sleep(100);//发生信号时将唤醒由于调用sleep函数而进入阻塞状态的进程
    }
    return 0;
}

