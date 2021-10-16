## socket编程

网络编程中和接受连接请求的套接字创建过程可整理如下：

1. 第一步：调用 socket 函数创建套接字。
2. 第二步：调用 bind 函数分配IP地址和端口号。
3. 第三步：调用 listen 函数转换为可接受请求状态。
4. 第四步：调用 accept 函数受理套接字请求。

### **socket**

```C
#include <sys/socket.h>
int socket(int domain, int type, int protocol);
//成功时返回文件描述符，失败时返回-1
```

### **bind**

```C
#include <sys/socket.h>
int bind(int sockfd, struct sockaddr *myaddr, socklen_t addrlen);
//成功时返回0，失败时返回-1
```

### **listen**

```C
#include <sys/socket.h>
int listen(int sockfd, int backlog);
//成功时返回0，失败时返回-1
```

### **accept**

```C
#include <sys/socket.h>
int accept(int sockfd,struct sockaddr *addr,socklen_t *addrlen);
//成功时返回文件描述符，失败时返回-1
```

## 编写hello world套接字程序

**服务器端**

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

// 处理错误函数
void error_handling(char *message) 
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int serv_sock; // 客户端套接字 
    int clnt_sock; // 服务端套接字

    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;

    char message[30] = "Hello World!";

    if (argc != 2) {
        printf("Usage:%s <port>\n", argv[0]);
        exit(1);
    }
    
    serv_sock = socket(PF_INET, SOCK_STREAM, 0); // socket
    if (serv_sock == -1) {

        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) {
        error_handling("bind() error");
    }

    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    int clnt_addr_size = sizeof(clnt_addr);
    clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_addr_size);
    if (clnt_sock == -1) {
        error_handling("accept() error");
    }

    write(clnt_sock, message, sizeof(message) - 1); // 将字符串写到套接字中
    close(clnt_sock);
    close(serv_sock);

    return 0;
}
```

服务器端（server）是能够受理连接请求的程序。下面构建服务端以验证之前提到的函数调用过程，该服务器端收到连接请求后向请求者返回`Hello World!`答复。

**客户端**

```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void error_handling(char *message) 
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in serv_addr;
    char message[30];
    int str_len;

    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        error_handling("socket() error");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));

   if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1){
		error_handling("connect() error\r\n");
	}

    str_len = read(sock, message, sizeof(message) - 1);
    if (str_len == -1) {
        error_handling("read() error!");
    }

    printf("Message from server :%s\n", message);
    close(sock);

    return 0;
}
```

客户端程序只有`调用 socket 函数创建套接字` 和 `调用 connect 函数向服务端发送连接请求`这两个步骤，下面给出客户端，需要查看以下两方面的内容：

1. 调用 socket 函数 和 connect 函数
2. 与服务端共同运行以收发字符串数据
