# Lab 7: Proxy Lab - Web代理服务器实验

## 1. 实验目标

### 1.1 总体目标

Proxy Lab通过实现一个支持并发和缓存的Web代理服务器来学习网络编程和并发控制。

| 目标 | 描述 |
|------|------|
| **HTTP协议** | 理解HTTP请求和响应格式 |
| **Socket编程** | 掌握TCP socket操作 |
| **并发处理** | 多线程或多进程并发 |
| **缓存机制** | 实现Web缓存策略 |

### 1.2 功能需求

```
┌─────────────────────────────────────────┐
│           代理服务器功能                 │
├─────────────────────────────────────────┤
│ 基础功能:                               │
│   - 接收客户端HTTP请求                  │
│   - 转发请求到目标服务器                │
│   - 返回响应给客户端                    │
├─────────────────────────────────────────┤
│ 高级功能:                               │
│   - 多线程并发处理                      │
│   - LRU缓存机制                         │
│   - 健壮的错误处理                      │
│   - 支持GET方法                         │
└─────────────────────────────────────────┘

架构:
┌─────────┐     ┌─────────────┐     ┌─────────────┐
│ Client  │◄───►│   Proxy     │◄───►│   Server    │
│ (浏览器)│     │  (你的实现)  │     │  (目标网站) │
└─────────┘     └─────────────┘     └─────────────┘
```

### 1.3 评分标准

| 部分 | 描述 | 分值 |
|------|------|------|
| Part I | 顺序代理 | 30分 |
| Part II | 并发代理 | 25分 |
| Part III | 缓存代理 | 25分 |
| 代码风格 | 健壮性和注释 | 20分 |

---

## 2. 背景知识

### 2.1 HTTP协议基础

```
HTTP请求格式:
┌─────────────────────────────────────────┐
│ GET /index.html HTTP/1.1\r\n           │ 请求行
│ Host: www.example.com\r\n              │ 头部
│ User-Agent: Mozilla/5.0\r\n            │ 头部
│ Accept: text/html\r\n                  │ 头部
│ \r\n                                   │ 空行
└─────────────────────────────────────────┘

HTTP响应格式:
┌─────────────────────────────────────────┐
│ HTTP/1.1 200 OK\r\n                    │ 状态行
│ Content-Type: text/html\r\n            │ 头部
│ Content-Length: 1234\r\n               │ 头部
│ \r\n                                   │ 空行
│ <html>...</html>                       │ 主体
└─────────────────────────────────────────┘
```

### 2.2 Socket编程接口

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// 创建socket
int socket(int domain, int type, int protocol);
// domain: AF_INET (IPv4), AF_INET6 (IPv6)
// type: SOCK_STREAM (TCP), SOCK_DGRAM (UDP)

// 绑定地址
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// 监听连接
int listen(int sockfd, int backlog);

// 接受连接
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

// 连接到服务器
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

// 发送/接收数据
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags);

// 关闭socket
int close(int fd);
```

### 2.3 POSIX线程

```c
#include <pthread.h>

// 创建线程
int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
                   void *(*start_routine)(void *), void *arg);

// 等待线程结束
int pthread_join(pthread_t thread, void **retval);

// 线程分离
int pthread_detach(pthread_t thread);

// 互斥锁
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

// 信号量
#include <semaphore.h>
sem_t sem;
int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_wait(sem_t *sem);
int sem_post(sem_t *sem);
```

---

## 3. 顺序代理实现 (Part I)

### 3.1 主函数和初始化

```c
// proxy.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MAXLINE 8192
#define MAXBUF  102400
#define LISTENQ 1024

// 错误处理函数
void error_exit(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

void error_msg(const char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    char client_hostname[MAXLINE], client_port[MAXLINE];

    // 忽略SIGPIPE信号
    signal(SIGPIPE, SIG_IGN);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 打开监听socket
    listenfd = open_listenfd(argv[1]);
    if (listenfd < 0) {
        error_exit("open_listenfd failed");
    }

    printf("Proxy listening on port %s\n", argv[1]);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (connfd < 0) {
            error_msg("accept failed");
            continue;
        }

        // 获取客户端信息
        getnameinfo((struct sockaddr *)&clientaddr, clientlen,
                    client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to %s:%s\n", client_hostname, client_port);

        // 处理请求（顺序版本）
        doit(connfd);

        close(connfd);
    }

    return 0;
}
```

### 3.2 Socket辅助函数

```c
// 打开监听socket
int open_listenfd(const char *port) {
    struct addrinfo hints, *listp, *p;
    int listenfd, optval = 1;
    int rc;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG;
    hints.ai_flags |= AI_NUMERICSERV;

    if ((rc = getaddrinfo(NULL, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(rc));
        return -1;
    }

    for (p = listp; p; p = p->ai_next) {
        if ((listenfd = socket(p->ai_family, p->ai_socktype,
                               p->ai_protocol)) < 0)
            continue;

        setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR,
                   (const void *)&optval, sizeof(int));

        if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0)
            break;

        close(listenfd);
    }

    freeaddrinfo(listp);
    if (!p) return -1;

    if (listen(listenfd, LISTENQ) < 0) {
        close(listenfd);
        return -1;
    }

    return listenfd;
}

// 连接到目标服务器
int open_clientfd(const char *hostname, const char *port) {
    struct addrinfo hints, *listp, *p;
    int clientfd;
    int rc;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV;
    hints.ai_flags |= AI_ADDRCONFIG;

    if ((rc = getaddrinfo(hostname, port, &hints, &listp)) != 0) {
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(rc));
        return -2;
    }

    for (p = listp; p; p = p->ai_next) {
        if ((clientfd = socket(p->ai_family, p->ai_socktype,
                               p->ai_protocol)) < 0)
            continue;

        if (connect(clientfd, p->ai_addr, p->ai_addrlen) != -1)
            break;

        close(clientfd);
    }

    freeaddrinfo(listp);
    if (!p) return -1;

    return clientfd;
}
```

### 3.3 请求处理

```c
// 处理单个HTTP事务
void doit(int fd) {
    struct stat sbuf;
    char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
    char hostname[MAXLINE], path[MAXLINE], port[MAXLINE];
    rio_t rio;

    // 读取请求行
    Rio_readinitb(&rio, fd);
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;

    printf("Request: %s", buf);
    sscanf(buf, "%s %s %s", method, uri, version);

    // 只支持GET方法
    if (strcasecmp(method, "GET")) {
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }

    // 解析URI
    parse_uri(uri, hostname, path, port);
    printf("Parsed: host=%s, path=%s, port=%s\n", hostname, path, port);

    // 转发请求到目标服务器
    fetch_and_forward(fd, hostname, path, port, &rio);
}

// 解析URI
void parse_uri(char *uri, char *hostname, char *path, char *port) {
    char *ptr;

    // 跳过 http:// 或 https://
    if ((ptr = strstr(uri, "://")) != NULL) {
        uri = ptr + 3;
    }

    // 提取主机名
    ptr = strchr(uri, '/');
    if (ptr) {
        strcpy(path, ptr);
        *ptr = '\0';
        strcpy(hostname, uri);
    } else {
        strcpy(hostname, uri);
        strcpy(path, "/");
    }

    // 检查端口号
    ptr = strchr(hostname, ':');
    if (ptr) {
        *ptr = '\0';
        strcpy(port, ptr + 1);
    } else {
        strcpy(port, "80");
    }
}
```

### 3.4 请求转发

```c
// 从服务器获取数据并转发给客户端
void fetch_and_forward(int clientfd, char *hostname, char *path,
                       char *port, rio_t *client_rio) {
    int serverfd;
    char buf[MAXLINE], new_request[MAXLINE];
    rio_t server_rio;

    // 连接目标服务器
    serverfd = open_clientfd(hostname, port);
    if (serverfd < 0) {
        clienterror(clientfd, hostname, "404", "Not found",
                    "Cannot connect to server");
        return;
    }

    // 构造新的HTTP请求
    sprintf(new_request, "GET %s HTTP/1.0\r\n", path);
    sprintf(new_request + strlen(new_request), "Host: %s\r\n", hostname);
    sprintf(new_request + strlen(new_request), "User-Agent: %s", user_agent_hdr);
    sprintf(new_request + strlen(new_request), "Connection: close\r\n");
    sprintf(new_request + strlen(new_request), "Proxy-Connection: close\r\n");

    // 转发其他请求头
    char line[MAXLINE];
    while (Rio_readlineb(client_rio, line, MAXLINE) > 0) {
        if (!strcmp(line, "\r\n")) break;
        if (strncasecmp(line, "Host:", 5) == 0) continue;
        if (strncasecmp(line, "User-Agent:", 11) == 0) continue;
        if (strncasecmp(line, "Connection:", 11) == 0) continue;
        if (strncasecmp(line, "Proxy-Connection:", 17) == 0) continue;
        strcat(new_request, line);
    }
    strcat(new_request, "\r\n");

    // 发送请求到服务器
    Rio_writen(serverfd, new_request, strlen(new_request));

    // 接收响应并转发给客户端
    Rio_readinitb(&server_rio, serverfd);
    int n;
    while ((n = Rio_readlineb(&server_rio, buf, MAXLINE)) != 0) {
        Rio_writen(clientfd, buf, n);
    }

    close(serverfd);
}
```

### 3.5 Rio包辅助函数

```c
// 健壮I/O包
typedef struct {
    int rio_fd;
    int rio_cnt;
    char *rio_bufptr;
    char rio_buf[MAXBUF];
} rio_t;

void Rio_readinitb(rio_t *rp, int fd) {
    rp->rio_fd = fd;
    rp->rio_cnt = 0;
    rp->rio_bufptr = rp->rio_buf;
}

static ssize_t rio_read(rio_t *rp, char *usrbuf, size_t n) {
    int cnt;

    while (rp->rio_cnt <= 0) {
        rp->rio_cnt = read(rp->rio_fd, rp->rio_buf, sizeof(rp->rio_buf));
        if (rp->rio_cnt < 0) {
            if (errno != EINTR) return -1;
        } else if (rp->rio_cnt == 0) {
            return 0;
        } else {
            rp->rio_bufptr = rp->rio_buf;
        }
    }

    cnt = n;
    if (rp->rio_cnt < n)
        cnt = rp->rio_cnt;
    memcpy(usrbuf, rp->rio_bufptr, cnt);
    rp->rio_bufptr += cnt;
    rp->rio_cnt -= cnt;
    return cnt;
}

ssize_t Rio_readlineb(rio_t *rp, void *usrbuf, size_t maxlen) {
    int n, rc;
    char c, *bufp = usrbuf;

    for (n = 1; n < maxlen; n++) {
        if ((rc = rio_read(rp, &c, 1)) == 1) {
            *bufp++ = c;
            if (c == '\n')
                break;
        } else if (rc == 0) {
            if (n == 1) return 0;
            else break;
        } else {
            return -1;
        }
    }
    *bufp = '\0';
    return n;
}

ssize_t Rio_writen(int fd, void *usrbuf, size_t n) {
    size_t nleft = n;
    ssize_t nwritten;
    char *bufp = usrbuf;

    while (nleft > 0) {
        if ((nwritten = write(fd, bufp, nleft)) <= 0) {
            if (errno == EINTR) nwritten = 0;
            else return -1;
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return n;
}
```

### 3.6 错误处理

```c
// 返回错误页面给客户端
void clienterror(int fd, char *cause, char *errnum,
                 char *shortmsg, char *longmsg) {
    char buf[MAXLINE], body[MAXBUF];

    // 构建HTTP响应体
    sprintf(body, "<html><title>Proxy Error</title>");
    sprintf(body + strlen(body), "<body bgcolor="
ffffff\">\n");
    sprintf(body + strlen(body), "%s: %s\n", errnum, shortmsg);
    sprintf(body + strlen(body), "<p>%s: %s\n", longmsg, cause);
    sprintf(body + strlen(body), "<hr><em>The Proxy</em>\n");

    // 发送HTTP响应
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}

// 用户代理头
const char *user_agent_hdr = "Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
```

---

## 4. 并发代理实现 (Part II)

### 4.1 多线程版本

```c
// 线程函数声明
void *thread(void *vargp);

// 修改后的主函数
int main(int argc, char **argv) {
    int listenfd, *connfdp;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    signal(SIGPIPE, SIG_IGN);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    if (listenfd < 0) {
        error_exit("open_listenfd failed");
    }

    while (1) {
        clientlen = sizeof(clientaddr);
        connfdp = malloc(sizeof(int));
        *connfdp = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);

        if (*connfdp < 0) {
            free(connfdp);
            continue;
        }

        // 创建新线程处理请求
        pthread_create(&tid, NULL, thread, connfdp);
    }

    return 0;
}

// 线程处理函数
void *thread(void *vargp) {
    int connfd = *((int *)vargp);
    pthread_detach(pthread_self());
    free(vargp);

    doit(connfd);
    close(connfd);

    return NULL;
}
```

### 4.2 预线程版本（线程池）

```c
#define NTHREADS  16
#define SBUFSIZE  1024

// 同步缓冲区
typedef struct {
    int *buf;
    int n;
    int front;
    int rear;
    sem_t mutex;
    sem_t slots;
    sem_t items;
} sbuf_t;

sbuf_t sbuf;

void sbuf_init(sbuf_t *sp, int n) {
    sp->buf = calloc(n, sizeof(int));
    sp->n = n;
    sp->front = sp->rear = 0;
    sem_init(&sp->mutex, 0, 1);
    sem_init(&sp->slots, 0, n);
    sem_init(&sp->items, 0, 0);
}

void sbuf_deinit(sbuf_t *sp) {
    free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item) {
    sem_wait(&sp->slots);
    sem_wait(&sp->mutex);
    sp->buf[(++sp->rear) % (sp->n)] = item;
    sem_post(&sp->mutex);
    sem_post(&sp->items);
}

int sbuf_remove(sbuf_t *sp) {
    int item;
    sem_wait(&sp->items);
    sem_wait(&sp->mutex);
    item = sp->buf[(++sp->front) % (sp->n)];
    sem_post(&sp->mutex);
    sem_post(&sp->slots);
    return item;
}

// 工作者线程
void *worker_thread(void *vargp) {
    pthread_detach(pthread_self());

    while (1) {
        int connfd = sbuf_remove(&sbuf);
        doit(connfd);
        close(connfd);
    }
    return NULL;
}

// 预线程主函数
int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    pthread_t tid;

    signal(SIGPIPE, SIG_IGN);

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(1);
    }

    listenfd = open_listenfd(argv[1]);
    sbuf_init(&sbuf, SBUFSIZE);

    // 创建工作者线程池
    for (int i = 0; i < NTHREADS; i++)
        pthread_create(&tid, NULL, worker_thread, NULL);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        if (connfd >= 0) {
            sbuf_insert(&sbuf, connfd);
        }
    }

    sbuf_deinit(&sbuf);
    return 0;
}
```

---

## 5. 缓存实现 (Part III)

### 5.1 缓存数据结构

```c
#define MAX_CACHE_SIZE 1049000    // 1MB
#define MAX_OBJECT_SIZE 102400    // 100KB
#define MAX_CACHE_BLOCKS 10

// 缓存块
typedef struct cache_block {
    char url[MAXLINE];
    char data[MAX_OBJECT_SIZE];
    int size;
    int valid;
    int timestamp;
    struct cache_block *prev;
    struct cache_block *next;
} cache_block_t;

typedef struct {
    cache_block_t blocks[MAX_CACHE_BLOCKS];
    int total_size;
    int readcnt;
    sem_t mutex;
    sem_t w;
    int timestamp_counter;
} cache_t;

cache_t cache;

// 初始化缓存
void cache_init() {
    cache.total_size = 0;
    cache.readcnt = 0;
    cache.timestamp_counter = 0;
    sem_init(&cache.mutex, 0, 1);
    sem_init(&cache.w, 0, 1);

    for (int i = 0; i < MAX_CACHE_BLOCKS; i++) {
        cache.blocks[i].valid = 0;
        cache.blocks[i].prev = (i > 0) ? &cache.blocks[i-1] : NULL;
        cache.blocks[i].next = (i < MAX_CACHE_BLOCKS-1) ? &cache.blocks[i+1] : NULL;
    }
}
```

### 5.2 缓存操作

```c
// 查找缓存
int cache_find(char *url, char *data) {
    int found = 0;

    sem_wait(&cache.mutex);
    cache.readcnt++;
    if (cache.readcnt == 1)
        sem_wait(&cache.w);
    sem_post(&cache.mutex);

    // 读者临界区
    for (int i = 0; i < MAX_CACHE_BLOCKS; i++) {
        if (cache.blocks[i].valid && !strcmp(cache.blocks[i].url, url)) {
            memcpy(data, cache.blocks[i].data, cache.blocks[i].size);
            found = cache.blocks[i].size;
            // 更新时间戳
            cache.blocks[i].timestamp = ++cache.timestamp_counter;
            break;
        }
    }

    sem_wait(&cache.mutex);
    cache.readcnt--;
    if (cache.readcnt == 0)
        sem_post(&cache.w);
    sem_post(&cache.mutex);

    return found;
}

// 添加缓存（LRU替换）
void cache_insert(char *url, char *data, int size) {
    if (size > MAX_OBJECT_SIZE) return;

    sem_wait(&cache.w);

    // 查找空块或最旧的块
    int idx = -1;
    int min_timestamp = cache.timestamp_counter + 1;

    for (int i = 0; i < MAX_CACHE_BLOCKS; i++) {
        if (!cache.blocks[i].valid) {
            idx = i;
            break;
        }
        if (cache.blocks[i].timestamp < min_timestamp) {
            min_timestamp = cache.blocks[i].timestamp;
            idx = i;
        }
    }

    // 插入新数据
    strcpy(cache.blocks[idx].url, url);
    memcpy(cache.blocks[idx].data, data, size);
    cache.blocks[idx].size = size;
    cache.blocks[idx].valid = 1;
    cache.blocks[idx].timestamp = ++cache.timestamp_counter;

    sem_post(&cache.w);
}
```

### 5.3 集成缓存的转发函数

```c
void fetch_and_forward_cached(int clientfd, char *hostname, char *path,
                               char *port, rio_t *client_rio, char *url) {
    char cache_data[MAX_OBJECT_SIZE];
    int cache_size;

    // 先检查缓存
    if ((cache_size = cache_find(url, cache_data)) > 0) {
        printf("Cache hit for %s\n", url);
        Rio_writen(clientfd, cache_data, cache_size);
        return;
    }

    // 缓存未命中，从服务器获取
    int serverfd = open_clientfd(hostname, port);
    if (serverfd < 0) {
        clienterror(clientfd, hostname, "404", "Not found",
                    "Cannot connect to server");
        return;
    }

    // 构造并发送请求
    char new_request[MAXLINE];
    sprintf(new_request, "GET %s HTTP/1.0\r\n", path);
    sprintf(new_request + strlen(new_request), "Host: %s\r\n", hostname);
    sprintf(new_request + strlen(new_request), "User-Agent: %s", user_agent_hdr);
    sprintf(new_request + strlen(new_request), "Connection: close\r\n");
    sprintf(new_request + strlen(new_request), "Proxy-Connection: close\r\n\r\n");

    Rio_writen(serverfd, new_request, strlen(new_request));

    // 接收响应并缓存
    rio_t server_rio;
    Rio_readinitb(&server_rio, serverfd);

    char response_buf[MAX_OBJECT_SIZE];
    int total_size = 0;
    int n;

    while ((n = Rio_readlineb(&server_rio, response_buf + total_size,
                               MAX_OBJECT_SIZE - total_size)) > 0) {
        Rio_writen(clientfd, response_buf + total_size, n);
        total_size += n;
    }

    close(serverfd);

    // 添加到缓存
    if (total_size <= MAX_OBJECT_SIZE) {
        cache_insert(url, response_buf, total_size);
    }
}
```

---

## 6. 测试方法

```bash
# 编译
cd proxylab-handout
make

# 启动代理
./proxy 15213

# 配置浏览器使用代理 localhost:15213
# 或使用curl测试
curl -v --proxy http://localhost:15213 http://www.example.com/

# 使用提供的测试脚本
./driver.sh

# 性能测试
./nop-server.py 15214 &  # 启动测试服务器
./proxy 15213 &          # 启动代理
./test-proxy.sh

# 并发测试
./test-concurrency.sh
```

---

**创建日期**: 2026-03-19


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
