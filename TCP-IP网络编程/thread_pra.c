#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void* pthread_main(void *arg);
void error_handling(char *buf);

int i;

int main(int argc, char *argv[])
{
    pthread_t t_id;//无符号长整形
    void *thread_result;
    int thread_param = 5;
    //int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_rooutine) (void *), void *arg);
    //attr:线程的数学，一般用默认属性-NULL
    //arg:作为实参传递到start_routine指针指向的函数内部,此处传入若为
    if(pthread_create(&t_id, NULL, pthread_main, (void *)&thread_param)!=0)
        error_handling("pthread_create() error");

    //int pthread_join(pthread_t thread, void **retval);阻塞等待子线程退出
    pthread_join(t_id, &thread_result);
    printf("第一种返回方式: return global num: %d \n", *((int *)thread_result));
    printf("i am master, my id is: %ld \n", pthread_self());
    sleep(5);
    return 0;
}

void* pthread_main(void *arg)
{
    i=*((int *)arg);
    printf("%d \n", i);
    i++;
    arg=&i;
    //pthread_t pthread_self(void);返回当前线程的id
    printf("i am thread, my id is: %ld \n", pthread_self());
    printf("第二种返回方式: puts 主线程栈区的数据: %ld \n", *((int *)arg));
    pthread_exit(&i);
    return NULL;
}

void error_handling(char *buf)
{
    fputs(buf, stderr);
    fputc('\n', stderr);
    exit(1);
}

