## 前言

为构建并发服务器，我们创建很多个进程来回应客户端，这会占用大量的运算和内存空间，我们需要一种在不创建进程的同时向多个客户端提供服务，那就使用复用技术

## select

- 是否存在套接字接收数据
- 无需阻塞传输数据的套接字有哪些
- 哪些套接字发生了异常

select调用方法和顺序

![image-20211016223358354](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211016223358354.png)

### 设置文件描述符

利用 select 函数可以同时监视多个文件描述符。当然，监视文件描述符可以视为监视套接字。此时首先需要将要监视的文件描述符集中在一起。集中时也要按照监视项（接收、传输、异常）进行区分，即按照上述 3 种监视项分成 3 类。

利用 fd_set 数组变量执行此操作，如图所示，该数组是存有0和1的位数组。

![image-20211016230038700](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211016230038700.png)

设置为1代表该文件描述符是监视对象

我们使用宏来增设对象

- `FD_ZERO(fd_set *fdset)`：将 fd_set 变量所指的位全部初始化成0
- `FD_SET(int fd,fd_set *fdset)`：在参数 fdset 指向的变量中注册文件描述符 fd 的信息
- `FD_SLR(int fd,fd_set *fdset)`：从参数 fdset 指向的变量中清除文件描述符 fd 的信息
- `FD_ISSET(int fd,fd_set *fdset)`：若参数 fdset 指向的变量中包含文件描述符 fd 的信息，则返回「真」

![image-20211016230158462](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211016230158462.png)

### select参数

```C
#include <sys/select.h>
#include <sys/time.h>

int select(int maxfd, fd_set *readset, fd_set *writeset,
           fd_set *exceptset, const struct timeval *timeout);
/*
成功时返回大于 0 的值，失败时返回 -1
maxfd: 监视对象文件描述符数量
readset: 将所有关注「是否存在待读取数据」的文件描述符注册到 fd_set 型变量，并传递其地址值。
writeset: 将所有关注「是否可传输无阻塞数据」的文件描述符注册到 fd_set 型变量，并传递其地址值。
exceptset: 将所有关注「是否发生异常」的文件描述符注册到 fd_set 型变量，并传递其地址值。
timeout: 调用 select 函数后，为防止陷入无限阻塞的状态，传递超时(time-out)信息
返回值: 发生错误时返回 -1,超时时返回0,。因发生关注的时间返回时，返回大于0的值，该值是发生事件的文件描述符数。
*/
```

**maxfd**

传递监视对象文件描述符的数量，我们传入最后一个文件描述符大小加一即可（fd从0开始）

**readset、writeset、exceptset**

如上文所写，如果我们是要关注读取数据是否准备好，那么readset设置即可，其它可以默认为0

**timeout**

```C
struct timeval 
{
    long tv_sec;	// seconds
    long tv_usec;	// microseconds
}
```

select函数监视的文件描述符发生变化时才返回，但如果一直不变化就阻塞在这里了。如果我们不想一直阻塞，就可以设置一个定时器，超过时限退出，返回0。如果不想定时，直接传递NULL

### 调用select后查看结果

如果返回大于0的整数，说明相应数量的文件描述符发生变化

> 那么是如何发现的呢?

![image-20211016225447424](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211016225447424.png)

设置成

```C
0111
```
select调用后除了发生变化的，其余都重新设置成0
结果发现

```c
0101
```

这说明fd1，fd3发生了变化

## 案例

[select.c](https://github.com/Shangyizhou/Linux-CPP-/blob/main/%E7%BD%91%E7%BB%9C%E7%BC%96%E7%A8%8B/IO%E5%A4%8D%E7%94%A8/select/select.c)

[echo_selectserv.c](https://github.com/Shangyizhou/Linux-CPP-/blob/main/%E7%BD%91%E7%BB%9C%E7%BC%96%E7%A8%8B/IO%E5%A4%8D%E7%94%A8/select/echo_selectserv.c)
