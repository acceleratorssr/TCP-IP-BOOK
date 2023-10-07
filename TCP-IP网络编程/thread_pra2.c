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
    //int pthread_create(pthread_t thread);
    //子线程就可以和主线程分离(不会产生僵尸线程)，当子线程退出时，其占用的内核资源
    //其退出状态不由其他线程获取，而直接自己自动释放（自己清理掉PCB的残留资源)
    //PS.注意进程没有这一机制
    pthread_detach(t_id);
    pthread_exit(NULL);

    return 0;//此处不会被执行
}

void* pthread_main(void *arg)
{
    printf("i am thread, my id is: %ld \n", pthread_self());
    sleep(3);
    printf("i am thread, my id is: %ld \n", pthread_self());
    return NULL;
}



void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

