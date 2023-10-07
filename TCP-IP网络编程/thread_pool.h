#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct Task
{
    void (*function)(void *arg);
    void *arg;
} Task;

typedef struct ThreadPool
{
    Task *taskQ;

    int queueCapacity; // 容量
    int queueSize;     // 当前任务个数
    int queueFront;    // 队头 --> 取数据
    int queueRear;     // 队尾 --> 放数据，此时代表的位置是任务队列中最后一个任务的下一个位置，因为一开始队列没有任务，所以值为0

    pthread_t managerID;  // 管理者线程id
    pthread_t *threadIDs; // 工作者线程id

    int minNum;  // 最小线程数量
    int maxNum;  // 最大线程数量
    int busyNum; // 正在工作的线程数量
    int liveNum; // 存活的线程数量
    int exitNum; // 要销毁的线程数量

    pthread_mutex_t mutexPool; // 整个线程池的锁
    pthread_mutex_t mutexBusy; // busyNum的锁

    pthread_cond_t notFull;  // 任务队列是否满了
    pthread_cond_t notEmpty; // 任务队列是否空了

    int shutdown; // 是否销毁线程池，销毁为1，不消耗为0

} ThreadPool;

//创建线程池并初始化
ThreadPool *threadPoolCreate(int min, int max, int queueSize);

//销毁线程池
int threadPoolDestory(ThreadPool *pool);

//给线程池添加任务
void threadPoolAdd(ThreadPool *pool, void(*func)(void*), void *arg);

//获取线程池中工作的线程的个数
int threadPoolBusyNum(ThreadPool *pool);

//获取线程池中存在的线程的个数
int threadPoolAliveNum(ThreadPool *pool);


//工作的线程（消费者线程）任务函数
void* worker(void *arg);

//管理者线程任务函数
void* manager(void *arg);

//单个线程退出
void threadExit(ThreadPool *pool);

#endif
