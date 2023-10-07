#include <stdio.h>
#include <unistd.h>
#include <sched.h>

int main(int argc, char *argv[])
{
    pid_t pid=fork();

    if(pid==0)
    {
        puts("child");
    }
    else{
        printf("child pid: %d \n", pid);
        sleep(15);
    }

    if(pid==0)
        puts("end c_pid");
    else
        puts("end p_pid");
    return 0;

} 

