#include <stdio.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int status;
    pid_t pid=fork();

    if(pid==0)
    {
        sleep(15);
        return 24;
    }
    else
    {
        while(!waitpid(-1, &status, WNOHANG))//第一次返回0，-1为等待任意子进程终止，WNOHANG用于防止阻塞
        {
            sleep(3);
            puts("sleep 3s.");
        }

        if(WIFEXITED(status))
            printf("child send %d \n", WEXITSTATUS(status));
    }
    return 0;
}
