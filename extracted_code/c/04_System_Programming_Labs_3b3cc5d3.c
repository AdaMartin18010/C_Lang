/*
 * Auto-generated from: 12_Practice_Exercises\04_System_Programming_Labs.md
 * Line: 302
 * Language: c
 * Block ID: 3b3cc5d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 支持多客户端（select/poll/epoll）
// 消息广播
// 用户注册/登录
// 私聊功能
// 在线用户列表

typedef struct Client {
    int fd;
    char username[32];
    bool authenticated;
    struct Client *next;
} Client;

typedef struct {
    int listen_fd;
    Client *clients;
    fd_set read_fds;
    int max_fd;
} ChatServer;

void server_init(ChatServer *server, int port);
void server_run(ChatServer *server);
void server_broadcast(ChatServer *server, const char *msg, int exclude_fd);
void server_private_msg(ChatServer *server, const char *to_user, const char *msg);
