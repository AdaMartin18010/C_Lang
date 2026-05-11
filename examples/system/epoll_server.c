/*
 * epoll TCP 服务器示例（Linux）
 * 编译: gcc -Wall -Wextra -o epoll_server epoll_server.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 8080
#define MAX_EVENTS 64
#define BUF_SIZE 1024

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
    listen(fd, 128);
    return fd;
}

int main(void) {
    int server_fd = make_server(PORT);
    set_nonblock(server_fd);
    
    int epfd = epoll_create1(0);
    struct epoll_event ev, events[MAX_EVENTS];
    
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, server_fd, &ev);
    
    printf("epoll server on port %d\n", PORT);
    
    while (1) {
        int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                struct sockaddr_in client;
                socklen_t len = sizeof(client);
                int cfd = accept(server_fd, (struct sockaddr *)&client, &len);
                set_nonblock(cfd);
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &ev);
                printf("New connection from %s:%d\n",
                       inet_ntoa(client.sin_addr), ntohs(client.sin_port));
            } else {
                char buf[BUF_SIZE];
                int fd = events[i].data.fd;
                ssize_t n = read(fd, buf, sizeof(buf) - 1);
                if (n <= 0) {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL);
                    close(fd);
                    printf("Client disconnected\n");
                } else {
                    buf[n] = '\0';
                    printf("Recv: %s", buf);
                    write(fd, buf, n);
                }
            }
        }
    }
    
    close(epfd);
    close(server_fd);
    return 0;
}
