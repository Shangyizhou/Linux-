#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

void *func(void *arg)
{
    printf("I am child tid, my tid is %lu\n", pthread_self());
}

int main()
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, func, NULL);    
    sleep(1); // 睡眠一秒，一般都是主线程抢到CPU，return 0的话直接结束整个进程，不会看到子线程的打印
    printf("The main tid is %lu\n", pthread_self());

    return 0;
}
