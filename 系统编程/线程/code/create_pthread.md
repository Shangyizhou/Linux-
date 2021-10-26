```C
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
```

```C
I am child tid, my tid is 140045665883904
The main tid is 140045674415936
```

如果主线程不睡眠一秒，会怎么样

```C
The main tid is 140549295167296
```

只打印了主进程
