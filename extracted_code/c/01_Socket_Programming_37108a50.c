/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2074
 * Language: c
 * Block ID: 37108a50
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * simple_http_server.c - 简单HTTP/1.0服务器
 * 演示协议解析基础
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define BUFFER_SIZE 8192
#define MAX_PATH 256

typedef struct {
    char method[16];
    char path[MAX_PATH];
    char version[16];
    char headers[4096];
    int header_len;
    int content_length;
    int keep_alive;
} http_request_t;

// 解析HTTP请求行
int parse_request_line(const char *line, http_request_t *req) {
    return sscanf(line, "%15s %255s %15s", req->method, req->path, req->version);
}

// 解析HTTP头部
void parse_headers(const char *headers, http_request_t *req) {
    const char *p = headers;

    while (*p) {
        if (strncasecmp(p, "Content-Length:", 15) == 0) {
            req->content_length = atoi(p + 15);
        } else if (strncasecmp(p, "Connection:", 11) == 0) {
            req->keep_alive = (strcasestr(p, "keep-alive") != NULL);
        }

        // 跳到下一行
        while (*p && *p != '\n') p++;
        if (*p == '\n') p++;
    }
}

// 发送HTTP响应
void send_http_response(int client_fd, int status, const char *status_text,
                        const char *content_type, const char *body, size_t body_len) {
    char header[1024];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, status_text, content_type, body_len);

    send(client_fd, header, header_len, 0);
    if (body && body_len > 0) {
        send(client_fd, body, body_len, 0);
    }
}

// 处理HTTP请求
void handle_http_request(int client_fd) {
    char buffer[BUFFER_SIZE];
    http_request_t req = {0};

    // 读取请求
    int total = 0;
    while (total < BUFFER_SIZE - 1) {
        int n = recv(client_fd, buffer + total, BUFFER_SIZE - 1 - total, 0);
        if (n <= 0) return;
        total += n;
        buffer[total] = '\0';

        // 检查是否收到完整的HTTP头
        if (strstr(buffer, "\r\n\r\n") != NULL) {
            break;
        }
    }

    // 解析请求行
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end) return;

    *line_end = '\0';
    if (parse_request_line(buffer, &req) != 3) {
        send_http_response(client_fd, 400, "Bad Request",
                          "text/plain", "Bad Request", 11);
        return;
    }

    printf("%s %s %s\n", req.method, req.path, req.version);

    // 只处理GET请求
    if (strcmp(req.method, "GET") != 0) {
        send_http_response(client_fd, 405, "Method Not Allowed",
                          "text/plain", "Method Not Allowed", 18);
        return;
    }

    // 构建文件路径(安全处理)
    char file_path[MAX_PATH + 16];
    if (strcmp(req.path, "/") == 0) {
        strcpy(req.path, "/index.html");
    }

    // 防止目录遍历攻击
    if (strstr(req.path, "..") != NULL) {
        send_http_response(client_fd, 403, "Forbidden",
                          "text/plain", "Forbidden", 9);
        return;
    }

    snprintf(file_path, sizeof(file_path), "./www%s", req.path);

    // 打开并发送文件
    int fd = open(file_path, O_RDONLY);
    if (fd < 0) {
        send_http_response(client_fd, 404, "Not Found",
                          "text/plain", "Not Found", 9);
        return;
    }

    struct stat st;
    fstat(fd, &st);

    // 判断Content-Type
    const char *content_type = "text/html";
    if (strstr(file_path, ".css")) content_type = "text/css";
    else if (strstr(file_path, ".js")) content_type = "application/javascript";
    else if (strstr(file_path, ".png")) content_type = "image/png";
    else if (strstr(file_path, ".jpg")) content_type = "image/jpeg";

    // 发送头部
    char header[512];
    int header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type, st.st_size);
    send(client_fd, header, header_len, 0);

    // 发送文件内容
    while (1) {
        int n = read(fd, buffer, BUFFER_SIZE);
        if (n <= 0) break;
        send(client_fd, buffer, n, 0);
    }

    close(fd);
}
