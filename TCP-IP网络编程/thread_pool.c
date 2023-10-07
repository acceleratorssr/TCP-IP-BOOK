#include "thread_pool.h"

// typedef struct Task
// {
//     void (*function)(void *arg);
//     void *arg;
// } Task;

// typedef struct ThreadPool
// {
//     Task *taskQ;

//     int queueCapacity; // 容量
//     int queueSize;     // 当前任务个数
//     int queueFront;    // 队头 --> 取数据
//     int queueRear;     // 队尾 --> 放数据，此时代表的位置是任务队列中最后一个任务的下一个位置，因为一开始队列没有任务，所以值为0

//     pthread_t managerID;  // 管理者线程id
//     pthread_t *threadIDs; // 工作者线程id

//     int minNum;  // 最小线程数量
//     int maxNum;  // 最大线程数量
//     int busyNum; // 正在工作的线程数量
//     int liveNum; // 存活的线程数量
//     int exitNum; // 要销毁的线程数量

//     pthread_mutex_t mutexPool; // 整个线程池的锁
//     pthread_mutex_t mutexBusy; // busyNum的锁

//     pthread_cond_t notFull;  // 任务队列是否满了
//     pthread_cond_t notEmpty; // 任务队列是否空了

//     int shutdown; // 是否销毁线程池，销毁为1，不消耗为0

// } ThreadPool;

ThreadPool *threadPoolCreate(int min, int max, int queueSize)
{
    int i;
    ThreadPool *pool = (ThreadPool *)malloc(sizeof(ThreadPool));
    do // 为了在遇到错误时，可以使用break退出循环，然后执行free
    {
        if (pool == NULL)
        {
            printf("malloc threadpool fail ... \n");
            break;
        }

        pool->threadIDs = (pthread_t *)malloc(sizeof(pthread_t) * max);
        if (pool->threadIDs == NULL)
        {
            printf("malloc threadIDs fail ... \n");
            break;
        }
        memset(pool->threadIDs, 0, sizeof(pthread_t) * max);
        pool->minNum = min;
        pool->maxNum = max;
        pool->liveNum = min;
        pool->busyNum = 0;
        pool->exitNum = 0;

        if (pthread_mutex_init(&pool->mutexPool, NULL) != 0 ||
            pthread_mutex_init(&pool->mutexBusy, NULL) != 0 ||
            pthread_cond_init(&pool->notEmpty, NULL) != 0 ||
            pthread_cond_init(&pool->notFull, NULL) != 0)
        {
            printf("mutex or condition init fail ... \n");
            break;
        }

        // 任务队列
        pool->taskQ = (Task *)malloc(sizeof(Task) * queueSize);
        pool->queueCapacity = queueSize;
        pool->queueFront = 0;
        pool->queueRear = 0;
        pool->queueSize = 0;

        pool->shutdown = 0;

        // 创建线程
        pthread_create(&pool->managerID, NULL, manager, pool);
        for (i = 0; i < min; i++)
        {
            pthread_create(&pool->threadIDs[i], NULL, worker, pool);
        }
        return pool;
    } while (0);

    if (pool && pool->threadIDs)
        free(pool->threadIDs);
    if (pool && pool->taskQ)
        free(pool->taskQ);
    if (pool)
        free(pool);

    return NULL;
}

int threadPoolDestory(ThreadPool *pool)
{
    int i;
    if (pool == NULL)
    {
        return -1;
    }

    // 关闭线程池
    pool->shutdown = 1;

    // 阻塞回收 管理者线程
    pthread_join(pool->managerID, NULL);

    // 唤醒阻塞的消费者线程
    for (i = 0; i < pool->liveNum; ++i)
    {
        pthread_cond_signal(&pool->notEmpty);
    }

    // 释放堆内存
    if (pool->taskQ)
    {
        free(pool->taskQ);
    }
    if (pool->threadIDs)
    {
        free(pool->threadIDs);
    }

    pthread_mutex_destroy(&pool->mutexPool);
    pthread_mutex_destroy(&pool->mutexBusy);
    pthread_cond_destroy(&pool->notEmpty);
    pthread_cond_destroy(&pool->notFull);

    free(pool);
    pool = NULL;

    return 0;
}

void threadPoolAdd(ThreadPool *pool, void (*func)(void *), void *arg)
{
    pthread_mutex_lock(&pool->mutexPool);
    while (pool->queueSize == pool->queueCapacity && !pool->shutdown)
        pthread_cond_wait(&pool->notFull, &pool->mutexPool); // 阻塞生产者线程
    if (pool->shutdown)
    {
        pthread_mutex_unlock(&pool->mutexPool);
        return;
    }

    // 添加任务
    pool->taskQ[pool->queueRear].function = func;
    pool->taskQ[pool->queueRear].arg = arg;
    pool->queueRear = (pool->queueRear + 1) % (pool->queueCapacity);
    pool->queueSize++;

    pthread_cond_signal(&pool->notEmpty);
    pthread_mutex_unlock(&pool->mutexPool);
}

int threadPoolBusyNum(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutexBusy);
    int busyNum = pool->busyNum;
    pthread_mutex_unlock(&pool->mutexBusy);
    return busyNum;
}

int threadPoolAliveNum(ThreadPool *pool)
{
    pthread_mutex_lock(&pool->mutexPool);
    int aliveNum = pool->liveNum;
    pthread_mutex_unlock(&pool->mutexPool);
    return aliveNum;
}

void *worker(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;

    while (1)
    {
        pthread_mutex_lock(&pool->mutexPool);

        // 当前队列是否为空
        while (pool->queueSize == 0 && !pool->shutdown)
        {
            // 阻塞工作线程
            pthread_cond_wait(&pool->notEmpty, &pool->mutexPool);

            // 判断是否要销毁线程（自杀）
            if (pool->exitNum > 0)
            {
                pool->exitNum--;
                if (pool->liveNum > pool->minNum)
                {
                    pool->liveNum--;
                    pthread_mutex_unlock(&pool->mutexPool);
                    threadExit(pool);
                }
            }
        }
        // 判断线程池是否被关闭了
        if (pool->shutdown)
        {
            pthread_mutex_unlock(&pool->mutexPool);
            threadExit(pool);
        }

        // 从任务队列中取出一个任务
        Task task;
        task.function = pool->taskQ[pool->queueFront].function;
        task.arg = pool->taskQ[pool->queueFront].arg;

        // 移动头节点
        pool->queueFront = (pool->queueFront + 1) % (pool->queueCapacity);
        pool->queueSize--;

        // 解锁
        pthread_cond_signal(&pool->notFull);
        pthread_mutex_unlock(&pool->mutexPool);

        printf("thread %ld start working...\n", pthread_self());
        pthread_mutex_lock(&pool->mutexBusy);
        pool->busyNum++;
        pthread_mutex_unlock(&pool->mutexBusy);
        task.function(task.arg);
        free(task.arg);
        task.arg = NULL;

        printf("thread %ld end working...\n", pthread_self());
        pool->busyNum--;
        pthread_mutex_unlock(&pool->mutexBusy);
    }
    return NULL;
}

void *manager(void *arg)
{
    ThreadPool *pool = (ThreadPool *)arg;
    while (!pool->shutdown)
    {
        // 每隔3s检测一次
        sleep(3);

        // 取出当前线程池中任务的数量和线程的数量
        pthread_mutex_lock(&pool->mutexPool);
        int queueSize = pool->queueSize;
        int liveNum = pool->liveNum;
        pthread_mutex_unlock(&pool->mutexPool);

        // 取出正在工作的线程数量
        pthread_mutex_lock(&pool->mutexBusy);
        int busyNum = pool->busyNum;
        pthread_mutex_unlock(&pool->mutexBusy);

        // 添加线程
        // 任务的个数>存活的线程个数 && 存活的线程数<最大线程数
        if (queueSize > liveNum && liveNum < pool->maxNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            int counter = 0;
            for (int i = 0; i < pool->maxNum && counter < 2 && pool->liveNum < pool->maxNum; ++i)
            {
                if (pool->threadIDs[i] = 0)
                {
                    pthread_create(&pool->threadIDs[i], NULL, worker, pool);
                    counter++;
                    pool->liveNum++;
                }
            }
            pthread_mutex_unlock(&pool->mutexPool);
        }

        // 销毁线程
        // 正在工作的线程*2<存活的线程数 && 存活的线程>最小线程数
        if (busyNum * 2 < liveNum && liveNum > pool->minNum)
        {
            pthread_mutex_lock(&pool->mutexPool);
            pool->exitNum = 2;
            pthread_mutex_unlock(&pool->mutexPool);
            // 让工作的线程自杀
            for (int i = 0; i < 2; ++i)
            {
                pthread_cond_signal(&pool->notEmpty);
            }
        }
    }
    return NULL;
}

void threadExit(ThreadPool *pool)
{
    pthread_t t_id=pthread_self();
    for(int i=0; i<pool->maxNum; ++i)
    {
        if(pool->threadIDs[i]==t_id)
        {
            pool->threadIDs[i]=0;
            printf("threadExit() called, %ld exiting...\n", t_id);
            break;
        }
    }
    pthread_exit(NULL);
}
