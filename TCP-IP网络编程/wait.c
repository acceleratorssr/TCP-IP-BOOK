#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t pid1=fork();
    pid_t pid2;

    if(pid1==0)
    {
        return 3;
    }
    else
    {
        printf("child pid1: %d \n", pid1);
        pid2=fork();
        if(pid2==0)
        {
            exit(7);
        }
        else
        {
            printf("child pid2: %d \n", pid2);
            wait(&status);//如果此时没有子进程已终止，那么主进程将阻塞直到有子进程终止
            if(WIFEXITED(status))
                printf("child send one: %d \n", WEXITSTATUS(status));

            wait(&status);
            if(WIFEXITED(status))
                printf("child send two: %d \n", WEXITSTATUS(status));
            sleep(30);
        }
    }
    return 0;
}
