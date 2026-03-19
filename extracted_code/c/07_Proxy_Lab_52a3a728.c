/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\07_Proxy_Lab.md
 * Line: 145
 * Language: c
 * Block ID: 52a3a728
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
