## FIFO 

FIFO常被称为命名管道，以区分管道(pipe)。管道(pipe)只能用于“有血缘关系”的进程间。但通过FIFO，不相关的进程也能交换数据。

FIFO是Linux基础文件类型中的一种。但，FIFO文件在磁盘上没有数据块，仅仅用来标识内核中一条通道。各进程可以打开这个文件进行read/write，实际上是在读写内核通道，这样就实现了进程间通信。

创建方式：

- 命令：mkfifo 管道名

- 库函数：int mkfifo(const char *pathname,  mode_t mode); 成功：0； 失败：-1

一旦使用mkfifo创建了一个FIFO，就可以使用open打开它，常见的文件I/O函数都可用于fifo。如：close、read、write、unlink等。

> 练习，使用FIFO进行进程间通信

```C
// read.c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define oops(str, n) {perror(str); exit(n);}

int main(int ac, char *av[])
{
    int fd;
    char buf[BUFSIZ];

    if (ac < 2) {
        oops("the argms is less", 1);
    }

    fd = open(av[1], O_RDWR);
    if (fd == -1) {
        oops("open the fifo error", 1);
    }

    while (1) {
        int len = read(fd, buf, sizeof(buf));
        write(STDOUT_FILENO, buf, len);
        sleep(1);
    }
    close(fd);

    return 0;
}
```

```C
// write.c
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>

#define oops(str, n) {perror(str); exit(n);}

int main(int ac, char *av[])
{
    int fd;
    char buf[BUFSIZ];

    if (ac < 2) {
        oops("the argms is less", 1);
    }

    fd = open(av[1], O_RDWR);
    if (fd == -1) {
        oops("open the fifo error", 1);
    }

    int i = 0;
    while (1) {
        sprintf(buf, "hello itcast %d\n", i++);

        write(fd, buf, strlen(buf));
        sleep(1);
    }
    close(fd);

    return 0;
}

**过程**

```C
// 创建fifo文件
mkfifo fifo 
    
// 运行程序，需要指定文件 
./write fifo     
./read fifo 
```

**FIFO文件类型**

```C
prw-r--r-- 1 root root 0 Oct 14 00:13 fifo
```

## 共享存储映射
