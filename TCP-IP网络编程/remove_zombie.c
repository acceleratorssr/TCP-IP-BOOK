#define _XOPEN_SOURCE 700//不加的话会报错：不允许使用不完整的类类型 "struct sigaction"C/C++(3365)

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sched.h>

void read_childproc(int sig)
{
    int status;
    pid_t id=waitpid(-1, &status, WNOHANG);//-1为等待任意子进程终止，WNOHANG用于防止阻塞
    if(WIFEXITED(status))
    {
        printf("removed proc id: %d \n", id);
        printf("child send: %d \n", WEXITSTATUS(status));
    }
}

int main(int argc, char *argv[])
{
    pid_t pid;
    pid_t pid2;
    struct sigaction act;
    act.sa_handler=read_childproc;
    sigaction(SIGCHLD, &act, 0);
    // signal(SIGCHLD, read_childproc);//先2后1，不知道为什么没有返回24

    pid=fork();
    if(pid==0)
    {
        puts("hi, i am child process1");
        sleep(10);
        return 12;
    }
    else
    {
        printf("child proc id1: %d \n", pid);
        pid2=fork();
        if(pid2==0)
        {
            puts("hi, i am child process2");
            sleep(10);
            exit(24);
        }
        else
        {
            int i;
            printf("child proc id2: %d \n", pid2);
            for(i=0; i<5; i++)
            {
                puts("wait ...");
                sleep(5);
            }
        }
    }
    return 0;
}
