#include <stdio.h>                                          //线程分离
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void* pthread_main(void *arg);
void error_handling(char *buf);

int main(int argc, char *argv[])
{
    pthread_t t_id;
    if(pthread_create(&t_id, NULL, pthread_main, NULL)!=0)
        error_handling("pthread_create() error");

    //int pthread_cancel(pthread_t thread);成功返回0
    //在线程A中调用cancel函数后，指定删除的线程不会立刻被删除，
    //而是等到被指定删除的线程进行了一次系统调用（用户区切换到内核区（如printf）后被杀死
    sleep(3);
    pthread_cancel(t_id);
    //int pthread_equal(pthread_t t1, pthread_t2);线程id的比较，不相同返回0
    pthread_exit(NULL);

    return 0;//此处不会被执行
}

void* pthread_main(void *arg)
{
    int i;
    printf("i am thread, my id is: %ld \n", pthread_self());
    sleep(3);
    for(i=0; i<1000; i++)
    {
        printf("%d ", i);
    }

    printf("%d ", i);
    return NULL;
}



void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

