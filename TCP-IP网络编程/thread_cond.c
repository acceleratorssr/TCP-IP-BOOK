#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

pthread_cond_t cond;
pthread_mutex_t mutex;

struct Node
{
    int number;
    struct Node* next;
};

struct Node* head=NULL;

void* producer(void *arg);
void* consumer(void *arg);

int main(int argc, char *argv[])
{
    int i;
    //初始化
    //int pthread_cond_init(pthread_cond_t *restrict cond, const pthread_condattr_t *restrict attr);
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    pthread_t pt_id[5], ct_id[5];
    for(i=0; i<5; i++)
    {
        pthread_create(&pt_id[i], NULL, producer, NULL);
        pthread_create(&ct_id[i], NULL, consumer, NULL);
    } 

    for(i=0; i<5; i++)
    {
        pthread_join(pt_id[i], NULL);
        pthread_join(ct_id[i], NULL);
    }

    //销毁释放资源
    //int pthread_cond_destory(pthread_cond_t *cond);
    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;

    //将线程阻塞一定的时间长度，时间到达之后，线程解除阻塞
    //int pthread_cond_timeout(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex, const struct timespec *restrict abstime);

    // time_t mytim = time(NULL);//1970.1.1 0：0：0到现在的总秒数
    // struct timespec tmsp;
    // tmsp.tv_nsec = 0;
    // tmsp.tv_sec = time(NULL)+100;//将线程阻塞100s
}

void* producer(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));
        pnew->number=rand()%1000;
        pnew->next=head;
        head=pnew;
        printf("+++producer, number = %d, t_id = %ld\n", pnew->number, pthread_self());
        pthread_mutex_unlock(&mutex);

        //唤醒阻塞在条件变量上的线程，至少有一个被解除阻塞/解除全部阻塞(pthread_cond_signal/pthread_cond_broadcast,可查看test.c)
        //int pthread_cond_signal(pthread_cond_t *cond);
        //int pthread_cond_broadcast(pthread_cond_t *cond);
        pthread_cond_broadcast(&cond);

        sleep(rand()%3);
    }
    return NULL;
}

void* consumer(void *arg)
{
    while(1)
    {
        pthread_mutex_lock(&mutex);
        while(head==NULL)
        {
            //线程阻塞函数，哪个线程调用这个函数，哪个线程就会被阻塞
            //在阻塞线程的时候，如果线程已经对互斥锁mutex上锁，那么将会把这把锁打开，是为了避免死锁
            //当线程解除阻塞的时候，函数内部会帮助这个线程再次将这个mutex互斥锁锁上，继续向下访问临界区
            //int pthread_cond_wait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex);
            pthread_cond_wait(&cond, &mutex);
            //当生产者生产了一个节点，调用pthread_cond_broadcast(&cond);唤醒了所以阻塞的线程
            //第一个消费者线程被唤醒后抢到了锁，其余没有抢到锁的线程将阻塞等待
            //直到第一个线程释放锁后，其他线程进行争抢，每一个线程在抢到锁后都分别需要判断此时有没有节点数据可供消费（条件是否符合）
            //若不符合，则重新执行wait，挂起线程，故此处需用while loop
        }
        struct Node* pnode = head;
        printf("--consumer: number: %d, t_id = %ld\n", pnode->number, pthread_self());
        head = pnode->next;
        free(pnode);
        pthread_mutex_unlock(&mutex);

        sleep(rand() % 3);
    }
    return NULL;
}
