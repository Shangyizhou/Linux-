## POSIX信号量

POSIX信号量函数的名字都以`sem_	`开头，并不像大多数线程函数 那样以`pthread_`开头。常用的POSIX信号量函数是下面5个：

```C
#include＜semaphore.h＞
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_wait(sem_t *sem);
int sem_trywait(sem_t *sem);
int sem_post(sem_t *sem);
```

```C
int sem_init(sem_t *sem, int pshared, unsigned int value);

/*
sem_init函数用于初始化一个未命名的信号量
参数：
	sem_t	指向被操作的信号量
	pshared	指定信号量的类型。如果其值为0，就表示这个信号量是当前进程的局部信号量，否则该信号量就可以在多个进程之间共享
	value	指定信号量的初始值
	初始化一个已经被初始化的信号量将导致不可预期的结果
*/
```

```C
int sem_destroy(sem_t *sem);
// 销毁信号量，以释放其占用的内核资源。如果销毁一个正被其他线程等待的信号量，则将导致不可预期的结果
```

```C
int sem_wait(sem_t *sem);
// sem_wait函数以原子操作的方式将信号量的值减1。如果信号量的值为0，则sem_wait将被阻塞，直到这个信号量具有非0值。
```

```C
int sem_trywait(sem_t *sem);
// sem_trywait与sem_wait函数相似，不过它始终立即返回，而不论被操作的信号量是否具有非0值
```

```C
int sem_post(sem_t *sem);
// sem_post函数以原子操作的方式将信号量的值加1。当信号量的值大于0时，其他正在调用sem_wait等待信号量的线程将被唤醒。
```

