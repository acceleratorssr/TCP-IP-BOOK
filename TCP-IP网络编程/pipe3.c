#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

const int BUF_SIZE = 1024;

int main(int argc, char *argv[])
{
    int fds1[2], fds2[2];
    char str1[]="hi,i am chlid";
    char str2[]="hi,i am father";
    char buf1[BUF_SIZE];
    char buf2[BUF_SIZE];
    pid_t pid;

    pipe(fds1);
    pipe(fds2);
    pid=fork();
    if(pid==0)
    {
        write(fds1[1], str1, sizeof(str1));//1-in,0-out
        read(fds2[0], buf1, BUF_SIZE);
        printf("child:%s \n", buf1);
    }else
    {
        read(fds1[0], buf2, BUF_SIZE);
        printf("fa:%s \n", buf2);
        write(fds2[1], str2, sizeof(str2));
        sleep(5);
    }
    return 0;
}

