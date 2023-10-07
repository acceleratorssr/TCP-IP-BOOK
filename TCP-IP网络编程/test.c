#include <stdio.h>
#include <pthread.h>

static pthread_t thread1;
static pthread_t thread2;

//静态初始化
static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond=PTHREAD_COND_INITIALIZER;

void *function1()

{
	while(1)
	{
		pthread_mutex_lock(&lock);
		printf("=====  线程1进入睡眠        ====\n");
		pthread_cond_wait(&cond,&lock);//没抢到锁就阻塞等待，最好加while
		printf("====  线程1    唤醒    ====\n");
		pthread_mutex_unlock(&lock);
	}
}

void *function2()

{
	while(1)
	{
		pthread_mutex_lock(&lock);
		printf("=====  线程2进入睡眠        ====\n");
		pthread_cond_wait(&cond,&lock);
		printf("====  线程2    唤醒    ====\n");
		pthread_mutex_unlock(&lock);
	}
}


int main()
{
	int i=0;
    int n=2;
	if(-1==pthread_create(&thread1,NULL,function1,NULL))
	{
		printf("thread_create1 fail!\n");
		pthread_detach(thread1);
	}

	if(-1==pthread_create(&thread2,NULL,function2,NULL))
	{
		printf("thread_create fail!\n");
		pthread_detach(thread1);
	}

	while(n--)
	{
		sleep(2);
		i++;
		printf("\n 第%d次唤醒\n",i);
		pthread_mutex_lock(&lock);
		if(-1==pthread_cond_broadcast(&cond))//pthread_cond_broadcast, pthread_cond_signal
		{
			printf("pthread_cond_broadcast error!\n");
		}
		pthread_mutex_unlock(&lock);
        sleep(100);
    }
	return 0;
}
