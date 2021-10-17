### epoll函数

#### **epoll_create**

select直接声明了fd_set遍历保存监视文件描述符，而epoll的则由操作系统负责，需要向操作系统请求创建保存文件描述符的空间。

```C
#include <sys/epoll.h>
int epoll_create(int size);
/*
成功时返回 epoll 的文件描述符，失败时返回 -1
size：epoll 实例的大小
*/
```

我们传递的size关系epoll例程的大小，但只是**建议**操作系统，并不决定

该函数返回文件描述符，我们正常像socket一样操作

#### epoll_event结构体

epoll 方式通过如下结构体 epoll_event **将发生变化的文件描述符**单独集中在一起。（不像select，全部文件描述符都循环监视一遍）

epoll_event 结构体用于保存事件和文件描述符结合。但也可以在 epoll 例程中注册文件描述符时，用于注册关注的事件。

```C
struct epoll_event
{
    __uint32_t events; // 关注事件
    epoll_data_t data; 
};
typedef union epoll_data {
    void *ptr;
    int fd;	// 文件描述符
    __uint32_t u32;
    __uint64_t u64;
} epoll_data_t;
```

```C
event.events = EPOLLIN; //需要读取数据的情况
event.data.fd = serv_sock;
```

#### **epoll_ctl**

```C
#include <sys/epoll.h>
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
/*
成功时返回 0 ，失败时返回 -1
epfd：用于注册监视对象的 epoll 例程的文件描述符
op：用于制定监视对象的添加、删除或更改等操作
fd：需要注册的监视对象文件描述符
event：监视对象的事件类型
*/
```

```C
epoll_ctl(A,EPOLL_CTL_ADD,B,C); // epoll 例程 A 中注册文件描述符 B ，主要目的是为了监视参数 C 中的事件
```

```C
epoll_ctl(A,EPOLL_CTL_DEL,B,NULL); // 从 epoll 例程 A 中删除文件描述符 B
```

**参数op**

- EPOLL_CTL_ADD：将文件描述符注册到 epoll 例程
- EPOLL_CTL_DEL：从 epoll 例程中删除文件描述符
- EPOLL_CTL_MOD：更改注册的文件描述符的关注事件发生情况

**参数event**

- EPOLLOUT：输出缓冲为空，可以立即发送数据的情况
- EPOLLPRI：收到 OOB 数据的情况
- EPOLLRDHUP：断开连接或半关闭的情况，这在边缘触发方式下非常有用
- EPOLLERR：发生错误的情况
- EPOLLET：以边缘触发的方式得到事件通知
- EPOLLONESHOT：发生一次事件后，相应文件描述符不再收到事件通知。因此需要向 epoll_ctl 函数的第二个参数传递 EPOLL_CTL_MOD ，再次设置事件。

#### epoll_wait

```C
#include <sys/epoll.h>
int epoll_wait(int epfd, struct epoll_event *events, int maxevents, int timeout);
/*
成功时返回发生事件的文件描述符，失败时返回 -1
epfd : 表示事件发生监视范围的 epoll 例程的文件描述符
events : 保存发生事件的文件描述符集合的结构体地址值
maxevents : 第二个参数中可以保存的最大事件数
timeout : 以 1/1000 秒为单位的等待时间，传递 -1 时，一直等待直到发生事件
*/
```

需要注意的是，第二个参数所指缓冲需要动态分配。

```C
int event_cnt;
struct epoll_event *ep_events;
...
ep_events=malloc(sizeof(struct epoll_event)*EPOLL_SIZE);//EPOLL_SIZE是宏常量
...
event_cnt=epoll_wait(epfd,ep_events,EPOLL_SIZE,-1);
...
```

**events** :

保存发生事件的文件描述符集合的结构体地址值，我们只给他分配空间，在等待时传入，如果某些文件描述符变化，则他们会被保存在events中，我们再循环访问events，注意，里面只有变化的文件描述符

###  基于select的I/O复用技术速度慢的原因

之前实现了基于 select 的 I/O 复用技术服务端，其中有不合理的设计如下：

- 调用 select 函数后常见的针对所有文件描述符的循环语句
- 每次调用 select 函数时都需要向该函数传递监视对象信息

**调用 select 函数后，并不是把发生变化的文件描述符单独集中在一起，而是通过作为监视对象的 fd_set 变量的变化，找出发生变化的文件描述符，因此无法避免针对所有监视对象的循环语句**。

而且，作为监视对象的 fd_set 会发生变化，所以调用 select 函数前应该复制并保存原有信息，并在每次调用 select 函数时传递新的监视对象信息

我们希望

> 仅向操作系统传递一次监视对象，监视范围或内容发生变化时只通知发生变化的事项

### 使用epoll优点

- 无需编写以监视状态变化为目的的针对所有文件描述符的循环语句
- 调用对应于 select 函数的 epoll_wait 函数时无需每次传递监视对象信息。

### select和epoll的区别

- select 函数中为了保存监视对象的文件描述符，直接声明了 fd_set 变量，但 epoll 方式下的**操作系统负责保存监视对象文件描述符**，因此需要向操作系统请求创建保存文件描述符的空间，此时用的函数就是 epoll_create 。
- select 方式中需要 FD_SET、FD_CLR 函数。但在 epoll 方式中，通过 epoll_ctl 函数请求操作系统完成。
- select 方式下调用 select 函数等待文件描述符的变化，而 epoll_wait 调用 epoll_wait 函数。

- select每次传递监视对象信息，套接字是操作系统管理的，所以 select 函数要借助操作系统才能完成功能。**而epoll仅向操作系统传递一次监视对象，监视范围或内容发生变化时只通知发生变化的事项**


[echo_epollserv.c](https://github.com/Shangyizhou/Linux-CPP-/blob/main/%E7%BD%91%E7%BB%9C%E7%BC%96%E7%A8%8B/IO%E5%A4%8D%E7%94%A8/epoll/echo_epollserv.c)
