## 高级IO

Linux提供了很多高级的I/O函数。它们并不像Linux基础I/O函数 （比如open和read）那么常用（编写内核模块时一般要实现这些I/O函 数），但在特定的条件下却表现出优秀的性能。

这些函数大致分为三类：

- 用于创建文件描述符的函数，包括pipe、dup/dup2函数。 
- 用于读写数据的函数，包括readv/writev、sendfile、 mmap/munmap、splice和tee函数。 
- 用于控制I/O行为和属性的函数，包括fcntl函数。

### pipe

```C
#include <unistd.h>
int pipe(int fd[2]);
/*
pipe函数可用于创建一个管道，以实现进程间通信
参数：
	一个包含两个int型整数的数组指针
返回：
	成功时返回0，并将一对打开的文件描述符值填入其参数指向的数组
	失败，则返回-1并设置errno
*/
```

> 管道简介

通过pipe函数创建的这两个文件描述符fd[0]和fd[1]分别构成管道的两端，往fd[1]写入的数据可以从fd[0]读出。fd[0]只能用于从管道读出数据，fd[1]则只能用于往管道写入数据，而不能反过来使用。

**fd[0]只能用于从管道读出数据，fd[1]则只能用于往管道写入数据，而不能反过来使用。默认情况下，这一对文件描述符都是阻塞的**

> 管道内部传输的数据是字节流，这和TCP字节流的概念相同。但 二者又有细微的区别

- 应用层程序能往一个TCP连接中写入多少字节 的数据，取决于对方的接收通告窗口的大小和本端的拥塞窗口的大小。
- 而管道本身拥有一个容量限制，它规定如果应用程序不将数据从 管道读走的话，该管道最多能被写入多少字节的数据。

### socketpair

```C
#include＜sys/types.h＞
#include＜sys/socket.h＞
int socketpair(int domain,int type,int protocol,int fd[2]);
/*
作用：
	能够方便地创建双向管道
参数：
	与socket参数类似（注意domain只能使用AF_UNIX，因为我们仅能在本地使用这个双向管道）
	最后一个参数与pipe基本一致，只是socketpair创建的这对文件描述符都是既可读又可写的
返回：
	成功时返回0
	失败时返回-1并设置errno
*/
```

### dup函数和dup2函数

```C
#include＜unistd.h＞
int dup(int file_descriptor);
int dup2(int file_descriptor_one,int file_descriptor_two);

/*
dup(int file_descriptor)
描述：
	- 创建一个新的文件描述符，该新文件描述符和原有文件描述符file_descriptor指向相同的文件、管道或者网络连接
	- dup返回的文件描述符总是取系统当前可用的最小整数值

dup2(int file_descriptor_one,int file_descriptor_two)
描述：
	dup2和dup类似，不过它将返回第一个不小于file_descriptor_two的整数值。
返回：
	dup和dup2系统调用失败时返回-1并设置errno。
*/
```

有时我们希望把标准输入重定向到一个文件，或者把标准输出重定向到一个网络连接（比如CGI编程）。这可以通过下面的用于复制文件描述符的dup或dup2函数来实现

> **Demo**

[dup.c](dup.c)

我们先关闭标准输出文件描述符 STDOUT_FILENO（其值是1），然后复制socket文件描述符connfd。 因为dup总是返回系统中最小的可用文件描述符，所以它的返回值实际上是1，即之前关闭的标准输出文件描述符的值。这样一来，服务器输 出到标准输出的内容（这里是“abcd”）就会直接发送到与客户连接对应的socket上，因此printf调用的输出将被客户端获得（而不是显示在服务器程序的终端上）。这就是CGI服务器的基本工作原理

## readv()和writev()

```C
#include＜sys/uio.h＞
ssize_t readv(int fd,const struct iovec*vector,int count)；
ssize_t writev(int fd,const struct iovec*vector,int count);
/*
描述：
	readv函数将数据从文件描述符读到分散的内存块中，即分散读；
	writev函数则将多块分散的内存数据一并写入文件描述符中，即集中写
参数：
	fd 		文件描述符
	vector 	iovec数组（该结构体描述一块内存区）
	count 	vector数组长度（有多少块内存数据需要从fd读出或写到fd）
返回：
	成功时返回读出/写入fd的字节数
	失败则返回-1并设置errno
*/
```

## 案例

当Web服务器解析完一个HTTP请求之后，如果目标文档存在且客户具有读取该文档的权限，那么就需要发送一个HTTP应答来传输该文档。

这个HTTP应答包含1个状态行、多个头部字段、1个空行和文档的内容。

其中，前3部分的内容可能被Web服务器放置在一块内存中，而文档的内容则通常被读入到另外一块单独的内存中（通过read函数或mmap函数）。

我们并不需要把这两部分内容拼接到一起再发送，而是可以使用writev函数将它们同时写出

[writev.cpp](writev.md)

## sendfile

sendfile函数在两个文件描述符之间直接传递数据（完全在内核中 操作），从而避免了内核缓冲区和用户缓冲区之间的数据拷贝，效率很高，这被称为零拷贝

```C
#include＜sys/sendfile.h＞
ssize_t sendfile(int out_fd,int in_fd,off_t*offset,size_t
count);
/*
参数：
	in_fd 	待读出的文件描述符
	out_fd 	待写入的文件描述符
	offset	指定从读入文件流的哪个位置开始读（如果为空，则使用读入文件流默认的起始位置）
	count	指定在文件描述符in_fd和out_fd之间传输的字节数
返回：
	成功时返回传输的字节数
	失败则返回-1并设置errno
*/
```

**注意：**

- `in_fd`必须是一个支持类似mmap函数的文件描述符，即它必须指向真实的文件，不能是socket和管道
- 而`out_fd`则必须是一个socket。
- **以上特性显示，sendfile几乎是专门为在网络上传输文件而设计的。**

## 案例

我们将目标文件作为第3个参数传递给服务器程 序，客户telnet到该服务器上即可获得该文件。**相比以前，本程序没有为目标文件分配任何用户空间的缓存，也没有执行读取文件的操作，但同样实现了文件的发送，其效率显然要高得多**

[sendfile.cpp](sendfile.md)

## mmap和munmap

mmap函数用于申请一段内存空间。我们可以将这段内存作为进程 间通信的共享内存，也可以将文件直接映射到其中。munmap函数则释 放由mmap创建的这段内存空间

```C
#include＜sys/mman.h＞
void *mmap(void*start,size_t length,int prot,int flags,int
fd,off_t offset);
int munmap(void*start,size_t length);

/*
参数：
	start	允许用户使用某个特定的地址作为这段内存的起始地址（NULL，则系统自动分配）
	length	指定内存段的长度
	prot	用来设置内存段的访问权限
			- PROT_READ， 内存段可读。
			- PROT_WRITE，内存段可写。
			- PROT_EXEC， 内存段可执行。
			- PROT_NONE， 内存段不能被访问
	flags	控制内存段内容被修改后程序的行为
			- MAP_SHARED
			- MAP_PRIVATE
	fd		被映射文件对应的文件描述符（可以由open获得）
	offset	设置从文件的何处开始映射
返回：
	mmap：
		成功时返回指向目标内存区域的指针
		失败则返回MAP_FAILED（(void*)-1）并设置errno。
	munmap：
		函数成功时返回0，失败则返回-1并设置errno。
*/
```

## splice

splice函数用于在两个文件描述符之间移动数据，也是零拷贝操作

```C
#include＜fcntl.h＞
ssize_t splice(int fd_in, loff_t *off_in, int fd_out, loff_t *off_out, size_t len, unsigned int flags);

/*
参数：
	fd_in	待输入数据的文件描述符
	off_in	表示从输入数据流的何处开始读取数据，NULL表示从当前位置读（如果fd_in是管道文件描述符，那么必须设置为NULL）
	len		指定移动数据的长度
	flags	则控制数据如何移动
返回：
	该管道读数据成功返回移动字节的数量，返回0表示无数据移动
	该管道没有被写入任何数据时，失败返回-1并设置errno
*/
```

![image-20211022123825693](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211022123825693.png)

使用splice函数时，fd_in和fd_out必须至少有一个是管道文件描述符。

## 案例

[splice.cpp](splice.md)

> 往fd[1]写入的数据可以从fd[0]读出。fd[0]只能用于从管道读出数据，fd[1]则只能用于往管道写入数据，而不能反过来使用。

我们通过splice函数将客户端的内容读入到pipefd[1]中，然后再使用splice函数从pipefd[0]中读出该内容到客户端，从而实现了简单高效的回射服务。整个过程未执行recv/send操作，因此也未涉及用户空间和 内核空间之间的数据拷贝。

## tee

tee函数在两个管道文件描述符之间复制数据，也是零拷贝操作。 它不消耗数据，因此源文件描述符上的数据仍然可以用于后续的读操 作。

```C
#include＜fcntl.h＞
ssize_t tee(int fd_in,int fd_out,size_t len,unsigned int flags);

/*
参数：
	与splice类似（但fd_in和fd_out必须都是管道文件描述符）
返回：
	tee函数成功时返回在两个文件描述符之间复制的数据数量（字节数）。
	返回0表示没有复制任何数据。tee失败时返回-1并设置errno。
*/
```

## 案例

[tee.cpp](tee.md)

## fcntl（file control）

提供了对文件描述符的各种控制操作。另外一个常见的控制文件描述符属性和行为的系统调用是ioctl，而且ioctl比fcntl能够执行更多的控制。

```C
#include＜fcntl.h＞
int fcntl(int fd,int cmd,…);
```

![image-20211022175639653](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211022175639653.png)

**在网络编程中，fcntl函数通常用来将一个文件描述符设置为非阻塞 的**

```C
int setnonblocking(int fd)
{
    int old_option = fcntl(fd,F_GETFL);/*获取文件描述符旧的状态标志*/
    int new_option = old_option | O_NONBLOCK;/*设置非阻塞标志*/
    fcntl(fd,F_SETFL,new_option);
    return old_option;/*返回文件描述符旧的状态标志，以便*/
    /*日后恢复该状态标志*/
}
```

**扩展**

它们必须与某个文件描述符相关联方可使用：当被关联的文件描述符可读或可写时，系统将触发SIGIO信号；当被关联的文件描述符（而且必须是 一个socket）上有带外数据可读时，系统将触发SIGURG信号。**将信号和文件描述符关联的方法，就是使用fcntl函数为目标文件描述符指定宿主进程或进程组，那么被指定的宿主进程或进程组将捕获这两个信号**。使用SIGIO时，还需要利用fcntl设置其O_ASYNC标志（异步I/O标 志，不过SIGIO信号模型并非真正意义上的异步I/O模型
