/*
 * TCP 客户端示例
 * 编译: gcc -Wall -Wextra -o tcp_client tcp_client.c
 * 运行: ./tcp_client localhost 8080
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <host> <port>\n", argv[0]);
        return 1;
    }
    
    const char *host = argv[1];
    int port = atoi(argv[2]);
    
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }
    
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
    };
    
    struct hostent *he = gethostbyname(host);
    if (!he) {
        fprintf(stderr, "Failed to resolve %s\n", host);
        return 1;
    }
    memcpy(&addr.sin_addr, he->h_addr_list[0], he->h_length);
    
    if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("connect");
        return 1;
    }
    
    printf("Connected to %s:%d\n", host, port);
    
    char buf[BUF_SIZE];
    while (fgets(buf, sizeof(buf), stdin)) {
        send(fd, buf, strlen(buf), 0);
        ssize_t n = recv(fd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        printf("Server: %s", buf);
    }
    
    close(fd);
    return 0;
}
