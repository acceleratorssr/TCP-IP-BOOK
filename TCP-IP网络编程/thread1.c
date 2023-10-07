#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void* thread_main(void *arg);
void error_handling(char *buf);

int main(int argc, char *argv[])
{
    pthread_t t_id;
    int thread_param=5;

    if(pthread_create(&t_id, NULL, thread_main, (void*)&thread_param)!=0)
        error_handling("pthread_create() error");
    sleep(10);
    puts("end of main");
    return 0;
}

void* thread_main(void *arg)
{
    int i;
    for(i=0; i<5; i++)
    {
        printf("%d", i);
        fflush(stdout);
        sleep(1);
        puts("well");
    }
    return NULL;
}

void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}
