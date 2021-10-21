有时需要在子进程中执行其他程序，替换当前进程映像。

fork创建子进程后执行的是和父进程相同的程序（但有可能执行不同的代码分支），子进程往往要调用一种exec函数以执行另一个程序。当进程调用一种exec函数时，该进程的用户空间代码和数据完全被新程序替换，从新程序的启动例程开始执行。

**调用exec并不创建新进程，所以调用exec前后该进程的id并未改变。**

## exec函数族

```C
#include <unistd.h>
extern char **environ;

int execl(const char *path, const char *arg, ...);
int execlp(const char *file, const char *arg, ...);
int execle(const char *path, const char *arg, ..., char *const envp[]);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);
int execve(const char *path, char *const argv[], char *const envp[]);
```

```C
/*
path参数指定可执行文件的完整路径
file参数可以接受文件名，该文件的具体位置则在环境变量PATH中搜寻。
arg接受可变参数
argv则接受参数数组，它们都会被传递给新程序（path或file指定的程序）的main函数。
envp参数用于设置新程序的环境变量。如果未设置它，则新程序将使用由全局变量environ指定的环境变量。
*/
```

> 一般情况下，exec函数是不返回的，除非出错。

- 它出错时返回-1，并设置errno。

- 如果没出错，则原程序中exec调用之后的代码都 不会执行，因为此时原程序已经被exec的参数指定的程序完全替换 （包括代码和数据）。

**exec函数不会关闭原程序打开的文件描述符，除非该文件描述符被设置了类似SOCK_CLOEXEC的属性**

