/*
 * HTTP Server - 集成 http_parser 的微型 Web 服务器
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -o http_server http_parser.c http_server.c
 * 运行: ./http_server 8080
 * 测试: curl http://localhost:8080/ ; curl -X POST -d "hello" http://localhost:8080/echo
 */
#include "http_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define BUF_SIZE 8192
#define MAX_CLIENTS 32

static void set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int make_server(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY,
    };
    bind(fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(fd, 10);
    return fd;
}

/* 简单路由处理 */
static void handle_request(const HttpRequest *req, HttpResponse *res) {
    if (strcmp(req->uri, "/") == 0) {
        http_response_init(res, 200);
        http_response_header(res, "Content-Type", "text/html");
        const char *body = "<html><body><h1>C HTTP Server</h1><p>Running!</p></body></html>";
        http_response_body(res, body, strlen(body));
    } else if (strcmp(req->uri, "/api/status") == 0) {
        http_response_init(res, 200);
        http_response_header(res, "Content-Type", "application/json");
        const char *body = "{\"status\":\"ok\",\"version\":\"1.0\"}";
        http_response_body(res, body, strlen(body));
    } else if (strcmp(req->uri, "/echo") == 0 && req->method == HTTP_POST) {
        http_response_init(res, 200);
        http_response_header(res, "Content-Type", "text/plain");
        http_response_body(res, req->body ? req->body : "", req->body_len);
    } else {
        http_response_init(res, 404);
        http_response_header(res, "Content-Type", "text/plain");
        const char *body = "404 Not Found";
        http_response_body(res, body, strlen(body));
    }
}

static void serve_client(int client_fd) {
    char buf[BUF_SIZE];
    ssize_t n = recv(client_fd, buf, sizeof(buf) - 1, 0);
    if (n <= 0) return;
    buf[n] = '\0';

    HttpRequest req;
    if (http_parse_request(&req, buf, (size_t)n) == 0) {
        printf("%s %s (body=%zu)\n", http_method_str(req.method), req.uri, req.body_len);

        HttpResponse res;
        handle_request(&req, &res);

        char response_buf[BUF_SIZE];
        int res_len = http_response_format(&res, response_buf, sizeof(response_buf));
        if (res_len > 0) {
            send(client_fd, response_buf, (size_t)res_len, 0);
        }

        if (req.body) free(req.body);
    } else {
        const char *bad = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        send(client_fd, bad, strlen(bad), 0);
    }
}

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : 8080;
    int server_fd = make_server(port);
    set_nonblock(server_fd);
    printf("HTTP server listening on port %d\n", port);

    fd_set readfds;
    int max_fd = server_fd;
    int clients[MAX_CLIENTS] = {0};

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i] > 0) {
                FD_SET(clients[i], &readfds);
                if (clients[i] > max_fd) max_fd = clients[i];
            }
        }

        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) continue;

        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int cfd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
            if (cfd >= 0) {
                set_nonblock(cfd);
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i] == 0) { clients[i] = cfd; break; }
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i];
            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                serve_client(fd);
                close(fd);
                clients[i] = 0;
            }
        }
    }
    close(server_fd);
    return 0;
}
