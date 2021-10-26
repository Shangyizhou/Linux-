## fork

```C
#include <sys/types.h>
#include <unistd.h>
pid_t fork(void);
/*
该函数每次调用返回两次
	父进程中返回子进程的PID
	子进程返回0
*/
```

```C
pid_t类型表示进程ID，但为了表示-1，它是有符号整型。(0不是有效进程ID，init最小，为1)
```

fork函数复制当前进程，在内核进程表中创建一个新的进程表项。

## 循环创建子进程

![image-20211021141248480](https://syz-picture.oss-cn-shenzhen.aliyuncs.com/image-20211021141248480.png)



```C
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int n = 5, i;				

	if (argc == 2) {	
		n = atoi(argv[1]);
	}

	for (i = 0; i < n; i++)	// 循环创建子进程
		if (fork() == 0)    // 如果是子进程则退出循环（只有父进程调用fork）
			break;			

	if (n == i) {   // i == 5 时候，则fork完毕，为父进程
		sleep(n);
		printf("I am parent, pid = %d\n", getpid());
	} else {        // 此时为子进程，相应的沉睡i秒（间隔一秒打出各个子进程）
		// sleep(i);
		printf("I'm %dth child, pid = %d\n", 
				i+1, getpid());
	}

	return 0;
}
```

```c
I'm 1th child, pid = 8107
I'm 2th child, pid = 8108
I'm 3th child, pid = 8109
I'm 4th child, pid = 8110
I'm 5th child, pid = 8111
I am parent, pid = 8106
```

## 相关函数

### **getpid函数**

```C
// 获取当前进程ID
pid_t getpid(void);	
```

### getppid函数

```C
// 获取当前进程的父进程ID
		pid_t getppid(void);
/*
区分一个函数是“系统函数”还是“库函数”依据：
② 是否访问内核数据结构
② 是否访问外部硬件资源		二者有任一 → 系统函数；二者均无 → 库函数
*/
```

### getuid函数

```C
// 获取当前进程实际用户ID
uid_t getuid(void);

// 获取当前进程有效用户ID
uid_t geteuid(void);
```

### getgid函数

```C
// 获取当前进程使用用户组ID
gid_t getgid(void);
// 获取当前进程有效用户组ID
gid_t getegid(void);
```

