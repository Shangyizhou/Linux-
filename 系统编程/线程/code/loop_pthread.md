```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

void *thrd_func(void *arg)
{
    int i = *((int*)arg); // 将(void *)强制转换为int
    // sleep(i);
    printf("%dth thread id = %lu pid = %u\n", i + 1, pthread_self(), getpid());
    return NULL;
}

int main()
{
    int i = 0;
    int n = 5;
    pthread_t tid[n];
    for (i = 0; i < n; i++) {
        // 这里的i使用值传递 (void *)&i为地址传递
        int ret = pthread_create(&tid[i], NULL, thrd_func, (void *)&i);
        if (ret != 0) {
            fprintf(stderr, "pthread_create error:%s\n", strerror(errno));
        }
    }

    // 主线程打印完信息沉睡5s，这段时间子线程分别调用函数打印自己的信息
    printf("In main: thread id = %lu pid = %u\n", pthread_self(), getpid());
    sleep(i);
    

    return 0;
}
```

```C
In main: thread id = 140053092931392 pid = 20768
1th thread id = 140053084399360 pid = 20768
2th thread id = 140053076006656 pid = 20768
3th thread id = 140053067613952 pid = 20768
4th thread id = 140053059221248 pid = 20768
5th thread id = 140053050828544 pid = 2076
```

