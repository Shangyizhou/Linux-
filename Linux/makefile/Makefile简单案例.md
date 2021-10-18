## 案例一

```C
#include <stdio.h>

int main()
{
    printf("hello world\n");

    return 0;
}
```

```C
gcc test.c -o test
```

**makefile**

```C
test:test.c
	gcc test.c -o test
```

## 案例二

合并三个文件，**add.c，sub.c，main.c**

**makefile**

```C
main:main.c add.o
	gcc main.c add.o -o main

add.o:add.c
	gcc -c add.c
    
sub.o:sub.c	
    gcc -c sub.c
    
clean: 
	rm *.o main
```

**执行情况**

```C
gcc -c add.c
gcc -c sub.c
gcc main.c add.o sub.o -o main
```

**更改add.c**

```C
gcc -c add.c
gcc main.c add.o sub.o -o main
```

我们发现只有add.c文件被重新编译了

如果文件越来越多，第一种编译方法会效率低下，因为所有文件都需要重新编译，但如果按照路径寻找，编译器只会重新编译更改过后的文件，这样子效率提升

**clean**

```c
clean: 
	rm *.o main
```

删除多余的中间文件和最终生成文件

**注意事项**

我们只增加了.c文件而没有.h文件，因为在预编译阶段已经宏展开了

## 案例三

每次更改会很麻烦，我们可以引入变量

**makefile**

```C
CC = gcc
CFLAGS = -lm -Wall -g

main:main.c add.o sub.o
	$(CC) $(CFLAGS) main.c add.o sub.o -o main

add.o:add.c
	$(CC) $(CFLAGS) -c add.c

sub.o:sub.c
	$(CC) $(CFLAGS) -c sub.c

clean: 
	rm *.o main
```

```C
gcc -lm -Wall -g -c add.c
gcc -lm -Wall -g -c sub.c
gcc -lm -Wall -g main.c add.o sub.o -o main
```

我们发现，CC和CFLAGS里的内容被替换了

## 案例四

现在我们需要一次生成两个文件，分别是`main_add`，`main_sub`

**这是我们的错误写法**

```C
CC = gcc
CFLAGS = -lm -Wall -g

main_add:main_add.o add.o 
	$(CC) $(CFLAGS) main_add.c add.o -o main_add

main_sub:main_sub.c sub.o
	$(CC) $(CFLAGS) main_sub.c sub.o -o main_sub

add.o:add.c
	$(CC) $(CFLAGS) -c add.c

sub.o:sub.c
	$(CC) $(CFLAGS) -c sub.c

clean: 
	rm *.o main
```

```C
gcc -lm -Wall -g   -c -o main_add.o main_add.c
gcc -lm -Wall -g -c add.c
gcc -lm -Wall -g main_add.c add.o -o main_add
```

> 只生成了main_add文件

如果要生成多个文件，应这样写makefile

**makefile**

```C
CC = gcc
CFLAGS = -lm -Wall -g

all:main_add main_sub

main_add:main_add.o add.o 
	$(CC) $(CFLAGS) main_add.c add.o -o main_add

main_sub:main_sub.c sub.o
	$(CC) $(CFLAGS) main_sub.c sub.o -o main_sub

add.o:add.c
	$(CC) $(CFLAGS) -c add.c

sub.o:sub.c
	$(CC) $(CFLAGS) -c sub.c

clean: 
	rm *.o main_add main_sub
```

```C
gcc -lm -Wall -g   -c -o main_add.o main_add.c
gcc -lm -Wall -g -c add.c
gcc -lm -Wall -g main_add.c add.o -o main_add
gcc -lm -Wall -g -c sub.c
gcc -lm -Wall -g main_sub.c sub.o -o main_sub
```

```C
rm *.o main_add main_sub
```

