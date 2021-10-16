#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>

#define BUF_SIZE 100

void error_handling(char *message)
{
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

int main(int argc, char *argv[])
{
    int serv_sock, clnt_sock;   // 服务器端需要两个socket
    struct sockaddr_in serv_adr, clnt_adr;
    struct timeval timeout;     // select需要的定时器
    fd_set reads, cpy_reads;    // fd_set结构体，储存文件描述符

    socklen_t adr_sz;
    int fd_max, str_len, fd_num, i;
    char buf[BUF_SIZE];
    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(argv[1]));

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1) {
        error_handling("bind() error");
    }
    if (listen(serv_sock, 5) == -1) {
        error_handling("listen() error");
    }

    // select的设置
    FD_ZERO(&reads);
    FD_SET(serv_sock, &reads); // 注册服务器套接字，将serv_sock描述符设置为1
    fd_max = serv_sock;        // 最大描述符数为serv_sock

    while (1)
    {
        cpy_reads = reads; // 会更新fd_set数组，所以需要保存原来的
        // 设置定时器
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fd_num = select(fd_max + 1, &cpy_reads, 0, 0, &timeout)) == -1) {
            break;
        }
        if (fd_num == 0)
            continue;
        
        for (i = 0; i < fd_max + 1; i++) { 
            if (FD_ISSET(i, &cpy_reads)) {  // 循环监视,查找发生变化的套接字文件描述符，查看cpy_reads[i]是否为1
                if (i == serv_sock) {       // 如果是服务端套接字时,受理连接请求
                    adr_sz = sizeof(clnt_adr);
                    clnt_sock = accept(serv_sock, (struct sockaddr *)&serv_adr, &adr_sz); // 从4开始 0 1 2分别被标准输入、输出、错误占据，serv_sock占据了3

                    FD_SET(clnt_sock, &reads); // 同样的，我们也要监视新创建的与客户端交流的套接字
                    if (fd_max < clnt_sock)
                        fd_max = clnt_sock; // 更新文件描述符最大数目
                    printf("Connected client: %d \n", clnt_sock);
                }
                else { // 不是服务端套接字(即是客户端套接字，这个时候需要读取信息)
                    str_len = read(i, buf, BUF_SIZE);
                    if (str_len == 0) {    // 读完毕的情况
                        FD_CLR(i, &reads); // 将该位置套接字为0，关闭该套接字
                        close(i);
                        printf("closed client: %d \n", i);
                    }
                    else { // 否则则正常接收到信息，我们再将信息写回
                        write(1, buf, str_len);
                    }
                }   

            }
        }
    } 

    close(serv_sock);
    return 0;
}
