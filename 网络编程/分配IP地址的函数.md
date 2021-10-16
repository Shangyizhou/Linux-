## 三种方式分配IP地址

```C
// argv[1]:IP  argv[0]:port
memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // 直接利用常数分配服务器端IP地址，自动获取（除非分配多个IP地址）
serv_addr.sin_port = htons(atoi(argv[2]));
```

```C
// argv[1]:IP  argv[0]:port
memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
serv_addr.sin_addr.s_addr = inet_addr(argv[1]); // 获得大端序列并转换为整型
serv_addr.sin_port = htons(atoi(argv[2]));
```

```C
// argv[1]:IP  argv[0]:port
memset(&serv_addr, 0, sizeof(serv_addr));
serv_addr.sin_family = AF_INET;
inet_aton(argv[1], &serv_addr.sin_addr); // 直接作用于结构体，而不需要先转换再赋值
serv_addr.sin_port = htons(atoi(argv[2]));
```

## inet_addr()

```C
in_addr_t inet_addr(const char *string);
// 成功则返回32位大端序整数型值，失败时则返回INADDR_NONE
```

## inet_aton()

```C
int inet_aton(const char *string, struct in_addr *addr);
// 成功返回1，失败返回0，与inet_addr()功能一致，只不过可以直接作用于结构体
```

## INADDR_ANY

利用常数INADDR_ANY分配服务器端的IP地址，采用此种方式可自动获取运行服务器的计算机IP地址，不必亲自输入。若同一计算机中已分配多个IP地址（多宿主计算机），则只要端口号一致，就可以从不同IP地址接收数据。因此，服务器端优先考虑此种方式
