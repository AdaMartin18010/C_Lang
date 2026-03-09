# Socket网络编程

> **层级定位**: 03 System Technology Domains / 15 Network_Programming
> **对应标准**: POSIX.1-2008, BSD Socket API
> **难度级别**: L3-L5
> **预估学习时间**: 8-10小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | Socket API、TCP/UDP、I/O多路复用、并发服务器 |
| **前置知识** | C基础、进程/线程 |
| **后续延伸** | HTTP协议、网络安全、高性能网络 |
| **权威来源** | POSIX, Stevens《Unix Network Programming》 |

---

## 📖 1. Socket基础

### 1.1 TCP客户端

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int tcp_client(const char *ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, ip, &server_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&server_addr,
                sizeof(server_addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    // 发送数据
    const char *msg = "Hello, Server!";
    send(sock, msg, strlen(msg), 0);

    // 接收响应
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        printf("Received: %s\n", buffer);
    }

    close(sock);
    return 0;
}
```

### 1.2 TCP服务器

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int tcp_server(int port) {
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock < 0) {
        perror("socket");
        return -1;
    }

    // 地址复用
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
               &opt, sizeof(opt));

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(listen_sock, (struct sockaddr*)&server_addr,
             sizeof(server_addr)) < 0) {
        perror("bind");
        close(listen_sock);
        return -1;
    }

    if (listen(listen_sock, 5) < 0) {
        perror("listen");
        close(listen_sock);
        return -1;
    }

    printf("Server listening on port %d...\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_sock = accept(listen_sock,
                                 (struct sockaddr*)&client_addr,
                                 &addr_len);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr,
                  client_ip, sizeof(client_ip));
        printf("Connection from %s:%d\n", client_ip,
               ntohs(client_addr.sin_port));

        // 处理客户端请求
        char buffer[1024];
        int n = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);
            send(client_sock, "ACK", 3, 0);
        }

        close(client_sock);
    }

    close(listen_sock);
    return 0;
}
```

---

## 📖 2. UDP编程

```c
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int udp_server(int port) {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    char buffer[1024];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    while (1) {
        int n = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                         (struct sockaddr*)&client_addr, &addr_len);
        if (n > 0) {
            buffer[n] = '\0';
            printf("Received: %s\n", buffer);

            sendto(sock, "ACK", 3, 0,
                   (struct sockaddr*)&client_addr, addr_len);
        }
    }

    close(sock);
    return 0;
}
```

---

## 📖 3. I/O多路复用

### 3.1 select

```c
#include <sys/select.h>
#include <sys/time.h>

void handle_multiple_clients(int listen_sock) {
    fd_set read_fds;
    int max_fd = listen_sock;

    FD_ZERO(&read_fds);
    FD_SET(listen_sock, &read_fds);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

    if (activity < 0) {
        perror("select");
        return;
    }

    if (FD_ISSET(listen_sock, &read_fds)) {
        // 有新连接
        int client = accept(listen_sock, NULL, NULL);
        // 处理...
    }
}
```

### 3.2 epoll (Linux)

```c
#include <sys/epoll.h>

void epoll_server(int listen_sock) {
    int epoll_fd = epoll_create1(0);

    struct epoll_event event, events[10];
    event.events = EPOLLIN;
    event.data.fd = listen_sock;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_sock, &event);

    while (1) {
        int n = epoll_wait(epoll_fd, events, 10, -1);

        for (int i = 0; i < n; i++) {
            if (events[i].data.fd == listen_sock) {
                // 新连接
                int client = accept(listen_sock, NULL, NULL);
                event.events = EPOLLIN | EPOLLET;  // 边缘触发
                event.data.fd = client;
                epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &event);
            } else {
                // 可读
                char buffer[1024];
                int n = read(events[i].data.fd, buffer, sizeof(buffer));
                // 处理...
            }
        }
    }
}
```

---

## ⚠️ 常见陷阱

### 陷阱 SOCK01: 忘记字节序转换

```c
// 错误
server_addr.sin_port = 8080;  // 端口可能不正确！

// 正确
server_addr.sin_port = htons(8080);  // 主机字节序转网络字节序
```

### 陷阱 SOCK02: 忽略信号中断

```c
// accept可能被信号中断
while ((client = accept(listen_sock, NULL, NULL)) < 0) {
    if (errno == EINTR) continue;  // 被信号中断，重试
    perror("accept");
    break;
}
```

---

## ✅ 质量验收清单

- [x] TCP客户端/服务器
- [x] UDP编程
- [x] select多路复用
- [x] epoll高性能I/O
- [x] 字节序处理
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 创建，补充核心缺失主题
