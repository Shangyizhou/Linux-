```C
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

union semun
{
    int val;                 /*用于SETVAL命令*/
    struct semid_ds *buf;    /*用于IPC_STAT和IPC_SET命令*/
    unsigned short *array;   /*用于GETALL和SETALL命令*/
    struct seminfo *__buf;   /*用于IPC_INFO命令*/
};

/*op为-1时执行P操作，op为1时执行V操作*/
void pv( int sem_id, int op )
{
    struct sembuf sem_b;
    sem_b.sem_num = 0;          // 信号量集中信号量编号，0表示信号量集中的第一个信号量
    sem_b.sem_op = op;          // 指定操作类型，其可选值为正整数、0和负整数,每种类型的操作的行为又受到sem_flg成员的影响
    sem_b.sem_flg = SEM_UNDO;   // 可选值是IPC_NOWAIT和SEM_UNDO(SEM_UNDO的含义是，当进程退出时取消正在进行的semop操作。)
    
    // semop系统调用改变信号量的值，即执行P、V操作
    // int semop(int sem_id, struct sembuf*sem_ops, size_t num_sem_ops);
    semop( sem_id, &sem_b, 1 );
}

int main( int argc, char* argv[] )
{
    // 创建一个信号量集 
    // int semget(key_t key,int num_sems,int sem_flags);
    int sem_id = semget( IPC_PRIVATE, 1, 0666 );

    // 自定义的命令
    union semun sem_un;
    sem_un.val = 1;
    // int semctl(int sem_id,int sem_num,int command,...);
    // SETVAL 将信号值的semval值设置为semun.val，同时内核数据中的semid_ds.sem_ctime被更新
    semctl( sem_id, 0, SETVAL, sem_un );

    pid_t id = fork();
    if( id < 0 )
    {
        return 1;
    }

    // 子进程
    else if( id == 0 )
    {
        printf( "child try to get binary sem\n" );
        /*在父、子进程间共享IPC_PRIVATE信号量的关键就在于二者都可以操作该信号量的标识符sem_id*/
        pv( sem_id, -1 );
        printf( "child get the sem and would release it after 5 seconds\n" );
        sleep( 5 );
        pv( sem_id, 1 );
        exit( 0 );
    }
    // 父进程
    else
    {
        printf( "parent try to get binary sem\n" );
        pv( sem_id, -1 );   // p操作
        printf( "parent get the sem and would release it after 5 seconds\n" );
        sleep( 5 );
        pv( sem_id, 1 );    // v操作
    }

    waitpid( id, NULL, 0 );
    semctl( sem_id, 0, IPC_RMID, sem_un );
    return 0;
}

```

```C
parent try to get binary sem
parent get the sem and would release it after 5 seconds
child try to get binary sem
```

可以看到，父进程睡眠时，子进程并没有执行完代码，而是停滞等待，5s后才全部执行完

```C
parent try to get binary sem
parent get the sem and would release it after 5 seconds
child try to get binary sem
child get the sem and would release it after 5 seconds
```

