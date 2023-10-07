#include <stdio.h>                                                          //读写锁，并行读，串行写
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


const int MAX=50;

int x;
pthread_rwlock_t rwlock;// -std=gnu89 ...

void* thread_main_w(void *arg);
void* thread_main_r(void *arg);

int main(int argc, char *argv[])
{
    pthread_t p1, p2;

    //pthread_rwlock_t s;

    //初始化读写锁
    //int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock, const pthread_rwlockattr_t *restict attr)

    //释放锁
    //int pthread_rwlock_destory(pthread_rwlock_t *rwlock);

    //锁定读操作
    //int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);

    //尝试加锁，加锁失败直接返回错误号，非阻塞
    //如果读写锁是打开状态或者已经锁定了读操作，依然可以加锁成功;已锁定写操作时加锁失败
    //int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

    //锁定写操作，如果已经锁定了读或者写操作，则线程会被阻塞
    //int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

    //尝试加锁，加锁失败直接返回错误号，非阻塞
    //如果读写锁是打开状态,可以加锁成功;已锁定写操作h或者读操作时加锁失败
    //int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

    //解锁
    //int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

    pthread_create(&p1, NULL, thread_main_w, NULL);
    pthread_create(&p2, NULL, thread_main_r, NULL);

    pthread_join(p1, NULL);
    pthread_join(p2, NULL);

    pthread_rwlock_destroy(&rwlock);
    printf("%d \n", x);

    return 0;

}

void* thread_main_r(void *arg)
{
    int i;
    pthread_rwlock_rdlock(&rwlock);
    for(i=0; i<1000; i++)
    {
        x++;
        usleep(10);
    }
    pthread_rwlock_unlock(&rwlock);
    return NULL;
}

void* thread_main_w(void *arg)
{
    int i;
    pthread_rwlock_wrlock(&rwlock);
    for(i=0; i<1000; i++)
    {
        x++;
        usleep(10);
    }
    pthread_rwlock_unlock(&rwlock); 
    return NULL;
}


