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
    sleep(1); // ˯��һ�룬һ�㶼�����߳�����CPU��return 0�Ļ�ֱ�ӽ����������̣����ῴ�����̵߳Ĵ�ӡ
    printf("The main tid is %lu\n", pthread_self());

    return 0;
}
