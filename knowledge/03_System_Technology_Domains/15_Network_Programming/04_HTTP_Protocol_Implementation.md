# HTTP协议实现深度解析

> **层级定位**: 03 System Technology Domains / 15 Network Programming
> **参考标准**: RFC 9110/9112 (HTTP/1.1), RFC 7540 (HTTP/2), RFC 8441 (HTTP/3)
> **难度级别**: L4
> **预估学习时间**: 10-12 小时

---

## 📋 目录

- [HTTP协议实现深度解析](#http协议实现深度解析)
  - [📋 目录](#-目录)
  - [1. HTTP基础](#1-http基础)
    - [1.1 HTTP/1.1消息格式](#11-http11消息格式)
    - [1.2 HTTP方法](#12-http方法)
  - [2. HTTP服务器实现](#2-http服务器实现)
    - [2.1 基础HTTP服务器](#21-基础http服务器)
    - [2.2 连接管理](#22-连接管理)
  - [3. HTTP客户端实现](#3-http客户端实现)
    - [3.1 基础HTTP客户端](#31-基础http客户端)
  - [4. HTTP/2基础](#4-http2基础)
    - [4.1 HTTP/2特性](#41-http2特性)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. HTTP基础

### 1.1 HTTP/1.1消息格式

```
HTTP请求消息：

┌─────────────────────────────────────────────────────────────┐
│  请求行                                                     │
│  GET /index.html HTTP/1.1                                   │
├─────────────────────────────────────────────────────────────┤
│  请求头                                                     │
│  Host: www.example.com                                      │
│  User-Agent: Mozilla/5.0                                    │
│  Accept: text/html                                          │
│  Connection: keep-alive                                     │
├─────────────────────────────────────────────────────────────┤
│  空行                                                       │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  请求体（可选）                                             │
│  （POST请求的数据）                                         │
└─────────────────────────────────────────────────────────────┘

HTTP响应消息：

┌─────────────────────────────────────────────────────────────┐
│  状态行                                                     │
│  HTTP/1.1 200 OK                                            │
├─────────────────────────────────────────────────────────────┤
│  响应头                                                     │
│  Content-Type: text/html                                    │
│  Content-Length: 1234                                       │
│  Date: Mon, 23 May 2024 22:38:34 GMT                       │
├─────────────────────────────────────────────────────────────┤
│  空行                                                       │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│  响应体                                                     │
│  <html>...</html>                                           │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 HTTP方法

| 方法 | 描述 | 幂等性 | 安全性 |
|:-----|:-----|:-------|:-------|
| GET | 获取资源 | 是 | 是 |
| POST | 创建资源 | 否 | 否 |
| PUT | 更新/替换资源 | 是 | 否 |
| DELETE | 删除资源 | 是 | 否 |
| PATCH | 部分更新 | 否 | 否 |
| HEAD | 获取头部 | 是 | 是 |
| OPTIONS | 获取支持的方法 | 是 | 是 |

---

## 2. HTTP服务器实现

### 2.1 基础HTTP服务器

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

#define BUFFER_SIZE 4096
#define MAX_PATH 256

// HTTP请求结构
typedef struct {
    char method[16];
    char path[MAX_PATH];
    char version[16];
    char *headers;
    char *body;
    size_t content_length;
} http_request_t;

// HTTP响应结构
typedef struct {
    int status_code;
    const char *status_text;
    const char *content_type;
    char *body;
    size_t body_length;
} http_response_t;

// 状态码映射
const char *get_status_text(int code) {
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 400: return "Bad Request";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default: return "Unknown";
    }
}

// 解析HTTP请求
int parse_request(const char *buffer, http_request_t *req) {
    char *line = strdup(buffer);
    char *saveptr;

    // 解析请求行
    char *request_line = strtok_r(line, "\r\n", &saveptr);
    if (!request_line) return -1;

    sscanf(request_line, "%15s %255s %15s",
           req->method, req->path, req->version);

    // 解析头部
    req->headers = strchr(buffer, '\n') + 1;

    // 查找Content-Length
    const char *cl = strcasestr(req->headers, "Content-Length:");
    if (cl) {
        req->content_length = atoi(cl + 15);
    }

    // 查找空行后的body
    char *body_start = strstr(buffer, "\r\n\r\n");
    if (body_start) {
        req->body = body_start + 4;
    }

    free(line);
    return 0;
}

// 构建HTTP响应
void build_response(const http_response_t *resp, char *buffer, size_t bufsize) {
    int written = snprintf(buffer, bufsize,
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: keep-alive\r\n"
        "\r\n",
        resp->status_code,
        resp->status_text,
        resp->content_type,
        resp->body_length
    );

    if (resp->body && resp->body_length > 0) {
        memcpy(buffer + written, resp->body, resp->body_length);
    }
}

// 处理请求
void handle_request(int client_fd, const http_request_t *req) {
    http_response_t resp = {0};
    char response_buffer[BUFFER_SIZE];

    if (strcmp(req->method, "GET") == 0) {
        // 处理GET请求
        if (strcmp(req->path, "/") == 0) {
            resp.status_code = 200;
            resp.status_text = "OK";
            resp.content_type = "text/html";
            resp.body = "<html><body><h1>Hello, World!</h1></body></html>";
            resp.body_length = strlen(resp.body);
        } else {
            resp.status_code = 404;
            resp.status_text = "Not Found";
            resp.content_type = "text/plain";
            resp.body = "404 Not Found";
            resp.body_length = strlen(resp.body);
        }
    } else if (strcmp(req->method, "POST") == 0) {
        // 处理POST请求
        resp.status_code = 201;
        resp.status_text = "Created";
        resp.content_type = "application/json";
        resp.body = "{\"status\":\"created\"}";
        resp.body_length = strlen(resp.body);
    } else {
        resp.status_code = 405;
        resp.status_text = "Method Not Allowed";
        resp.content_type = "text/plain";
        resp.body = "Method Not Allowed";
        resp.body_length = strlen(resp.body);
    }

    build_response(&resp, response_buffer, sizeof(response_buffer));
    send(client_fd, response_buffer, strlen(response_buffer), 0);
}

// 主服务器循环
void run_server(int port) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, 10);

    printf("Server listening on port %d\n", port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                              (struct sockaddr *)&client_addr,
                              &addr_len);

        char buffer[BUFFER_SIZE];
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes > 0) {
            buffer[bytes] = '\0';

            http_request_t req = {0};
            if (parse_request(buffer, &req) == 0) {
                handle_request(client_fd, &req);
            }
        }

        close(client_fd);
    }
}
```

### 2.2 连接管理

```c
/*
 * HTTP连接管理：
 * • Connection: close - 响应后关闭连接
 * • Connection: keep-alive - 保持连接复用
 * • HTTP/1.1默认keep-alive
 */

// Keep-Alive超时处理
typedef struct {
    int fd;
    time_t last_activity;
    int keep_alive;
    int timeout;
} connection_t;

#define MAX_CONNECTIONS 100
#define KEEPALIVE_TIMEOUT 30  // 30秒

connection_t connections[MAX_CONNECTIONS];

void check_timeouts(void) {
    time_t now = time(NULL);

    for (int i = 0; i < MAX_CONNECTIONS; i++) {
        if (connections[i].fd != -1 && connections[i].keep_alive) {
            if (now - connections[i].last_activity > connections[i].timeout) {
                close(connections[i].fd);
                connections[i].fd = -1;
            }
        }
    }
}
```

---

## 3. HTTP客户端实现

### 3.1 基础HTTP客户端

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// 简单的HTTP GET请求
char *http_get(const char *host, int port, const char *path) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    // 解析主机名
    struct hostent *server = gethostbyname(host);
    if (!server) return NULL;

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port)
    };
    memcpy(&addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // 连接
    if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return NULL;
    }

    // 构建请求
    char request[1024];
    snprintf(request, sizeof(request),
        "GET %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: SimpleCClient/1.0\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, host
    );

    // 发送请求
    send(sock, request, strlen(request), 0);

    // 接收响应
    char *response = malloc(65536);
    size_t total = 0;
    ssize_t bytes;

    while ((bytes = recv(sock, response + total, 65536 - total - 1, 0)) > 0) {
        total += bytes;
    }

    response[total] = '\0';
    close(sock);

    return response;
}

// 解析响应
int parse_response(const char *response, char **body) {
    // 查找空行分隔头部和body
    char *empty_line = strstr(response, "\r\n\r\n");
    if (!empty_line) return -1;

    *body = empty_line + 4;

    // 解析状态码
    int status_code;
    sscanf(response, "HTTP/1.1 %d", &status_code);

    return status_code;
}
```

---

## 4. HTTP/2基础

### 4.1 HTTP/2特性

```
HTTP/2改进：

┌─────────────────────────────────────────────────────────────┐
│  二进制分帧层                                                │
│  • 不再是文本协议                                           │
│  • 更高效解析                                               │
├─────────────────────────────────────────────────────────────┤
│  多路复用                                                   │
│  • 单一TCP连接上的多个流                                     │
│  • 解决队头阻塞问题                                          │
├─────────────────────────────────────────────────────────────┤
│  头部压缩 (HPACK)                                           │
│  • 减少头部传输开销                                          │
│  • 静态表 + 动态表 + Huffman编码                             │
├─────────────────────────────────────────────────────────────┤
│  服务器推送                                                 │
│  • 服务器可以主动推送资源                                    │
│  • 例如：推送CSS/JS与HTML一起                                │
├─────────────────────────────────────────────────────────────┤
│  流优先级                                                   │
│  • 客户端可以指定资源优先级                                  │
│  • 优化页面加载顺序                                          │
└─────────────────────────────────────────────────────────────┘
```

---

## 关联导航

### 前置知识

- [Socket编程基础](./01_Socket_Programming.md)
- [高性能服务器](./03_High_Performance_Server.md)

### 后续延伸

- [TLS/SSL协议实现](./05_TLS_Implementation.md)
- [RESTful API设计](../18_Web_Development/01_RESTful_API_Design.md)

### 参考

- RFC 9110: HTTP Semantics
- RFC 9112: HTTP/1.1
- RFC 7540: HTTP/2
- High Performance Browser Networking, Ilya Grigorik
