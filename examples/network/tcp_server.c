/*
 * TCP Echo 服务器（单线程 + select）示例
 * 编译: gcc -Wall -Wextra -o tcp_server tcp_server.c
 * 运行: ./tcp_server 然后另终端 nc localhost 8080
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>

#define PORT 8080
#define MAX_CLIENTS 32
#define BUF_SIZE 1024

static void set_nonblock(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main(void) {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return 1;
    }
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(PORT),
        .sin_addr.s_addr = INADDR_ANY,
    };
    
    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        return 1;
    }
    
    printf("Server listening on port %d...\n", PORT);
    
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
        
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) < 0) {
            perror("select");
            continue;
        }
        
        if (FD_ISSET(server_fd, &readfds)) {
            struct sockaddr_in client_addr;
            socklen_t len = sizeof(client_addr);
            int new_fd = accept(server_fd, (struct sockaddr *)&client_addr, &len);
            if (new_fd >= 0) {
                printf("New connection from %s:%d\n",
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                for (int i = 0; i < MAX_CLIENTS; i++) {
                    if (clients[i] == 0) {
                        clients[i] = new_fd;
                        break;
                    }
                }
            }
        }
        
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = clients[i];
            if (fd > 0 && FD_ISSET(fd, &readfds)) {
                char buf[BUF_SIZE];
                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                if (n <= 0) {
                    printf("Client disconnected\n");
                    close(fd);
                    clients[i] = 0;
                } else {
                    buf[n] = '\0';
                    printf("Recv: %s", buf);
                    write(fd, buf, n);  /* echo */
                }
            }
        }
    }
    
    close(server_fd);
    return 0;
}
