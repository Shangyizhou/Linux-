#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#define BUF_SIZE 30

int main(int argc, char *argv[])
{
    fd_set reads, temps;
    int result, str_len;
    char buf[BUF_SIZE];
    struct timeval timeout;

    FD_ZERO(&reads);    // 初始化数组fd_set变量
    FD_SET(0, &reads);  // 设置监视对象

    while (1)
    {
        temps = reads;      // 保存，因为select后，除发生变化的文件描述符对应位外，剩下的都初始化为0，为了记住初始值所以复制
        timeout.tv_sec = 5; // 设置定时（我们需在循环内设置，调用select后每次定时器的值将被替换为剩下时间，所以我们需要重复赋予初值）
        timeout.tv_usec = 0;
        result = select(1, &temps, 0, 0, &timeout); // 本案例是读取，所以readset为&temp，其他默认为0
        if (result == -1) {
            puts("select error!");
        }
        else if (result == 0)
        {
            puts("Time-out!"); // 超时
        }
        else
        {
            if (FD_ISSET(0, &temps)) // FD_ISSET判断是否改变
            {
                // 从该描述符中读数据
                str_len = read(0, buf, BUF_SIZE); 
                buf[str_len] = 0;
                printf("message from console: %s", buf);
            }
        }
    }

    return 0;
}
