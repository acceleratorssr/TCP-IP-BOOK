#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

pthread_mutex_t mutex;
sem_t psem, csem;

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
    sem_init(&psem, 0, 5);//生产者线程一共有5个信号灯
    sem_init(&csem, 0, 0);//消费者线程一共有0个信号灯

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

    sem_destroy(&psem);
    sem_destroy(&csem);
    pthread_mutex_destroy(&mutex);

    return 0;


}

void* producer(void *arg)
{
    while(1)
    {
        sem_wait(&psem);            //可生产的数量-1

        pthread_mutex_lock(&mutex);
        struct Node* pnew = (struct Node*)malloc(sizeof(struct Node));
        pnew->number=rand()%1000;
        pnew->next=head;
        head=pnew;
        printf("+++producer, number = %d, t_id = %ld\n", pnew->number, pthread_self());
        pthread_mutex_unlock(&mutex);
        
        sem_post(&csem);

        sleep(rand()%3);
    }
    return NULL;
}

void* consumer(void *arg)
{
    while(1)
    {
        sem_wait(&csem);//可消费的商品数量-1

        pthread_mutex_lock(&mutex);
        struct Node* pnode = head;
        printf("--consumer: number: %d, t_id = %ld\n", pnode->number, pthread_self());
        head = pnode->next;
        free(pnode);
        pthread_mutex_unlock(&mutex);

        sem_post(&psem);

        sleep(rand() % 3);
    }
    return NULL;
}
