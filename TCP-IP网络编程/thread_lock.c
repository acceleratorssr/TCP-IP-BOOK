#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>

const int MAX=50;

int x;
pthread_mutex_t mutex;

void* thread_main_a(void *arg);
void* thread_main_b(void *arg);

int main(int argc, char *argv[])
{
    pthread_t p1, p2;

    //初始化互斥锁
    //int pthread_mutex_init(pthread_mutex_t *restrict mutex, const pthread_mutexattr_t *restrict attc);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&p1, NULL, thread_main_a, NULL);
    pthread_create(&p2, NULL, thread_main_b, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    //释放互斥锁资源
    //int pthread_mutex_destory(pthread_mutex_t *mutex);
    pthread_mutex_destroy(&mutex);
    printf("%d \n", x);

    return 0;


    //尝试加锁，加锁失败直接返回错误号，非阻塞
    //int pthread_mutex_trylock(pthread_mutex_t *mutex);


    
}

void* thread_main_a(void *arg)
{
    int i;
    //设定为锁定状态，将状态写入参数mutex中，线程会阻塞等待获取锁
    //int pthread_mutex_lock(ptherad_mutex_t *mutex);
    pthread_mutex_lock(&mutex);
    for(i=0; i<1000; i++)
    {
        x++;
        usleep(10);
    }
    //解锁互斥锁
    //int ptherad_mutex_unlock(pthread_mutex_t *mutex);
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* thread_main_b(void *arg)
{
    int i;
    pthread_mutex_lock(&mutex);
    for(i=0; i<1000; i++)
    {
        x++;
        usleep(10);
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

