/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\01_Network_Server.md
 * Line: 289
 * Language: c
 * Block ID: 1c8687af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* high_perf_server.c - 高性能TCP服务器实现 */
#ifdef PLATFORM_WINDOWS
/* Windows平台使用IOCP实现 */
#else

#include "high_perf_server.h"
#include <pthread.h>

/* 获取当前时间(ms) */
static uint64_t get_current_time(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* 设置非阻塞模式 */
static bool set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return false;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK) >= 0;
}

/* 设置TCP_NODELAY */
static bool set_tcp_nodelay(int fd) {
    int nodelay = 1;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY,
                      &nodelay, sizeof(nodelay)) == 0;
}

/* 设置SO_REUSEADDR */
static bool set_reuse_addr(int fd) {
    int reuse = 1;
    return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
                      &reuse, sizeof(reuse)) == 0;
}

/*==================== 定时器管理 ====================*/

static void timer_init(timer_manager_t *mgr) {
    mgr->head = NULL;
    mgr->current_time = get_current_time();
}

static timer_node_t* timer_add(timer_manager_t *mgr,
                                int conn_fd,
                                uint64_t timeout_ms) {
    timer_node_t *node = malloc(sizeof(timer_node_t));
    if (!node) return NULL;

    node->expire_time = mgr->current_time + timeout_ms;
    node->conn_fd = conn_fd;
    node->prev = NULL;

    /* 按过期时间排序插入 */
    timer_node_t *curr = mgr->head;
    while (curr && curr->expire_time < node->expire_time) {
        curr = curr->next;
    }

    if (curr) {
        node->next = curr;
        node->prev = curr->prev;
        if (curr->prev) {
            curr->prev->next = node;
        } else {
            mgr->head = node;
        }
        curr->prev = node;
    } else {
        node->next = NULL;
        if (mgr->head) {
            timer_node_t *tail = mgr->head;
            while (tail->next) tail = tail->next;
            tail->next = node;
            node->prev = tail;
        } else {
            mgr->head = node;
        }
    }

    return node;
}

static void timer_remove(timer_manager_t *mgr, timer_node_t *node) {
    if (!node) return;

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        mgr->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    }

    free(node);
}

static void timer_update(timer_manager_t *mgr,
                         timer_node_t *node,
                         uint64_t timeout_ms) {
    timer_remove(mgr, node);
    timer_add(mgr, node->conn_fd, timeout_ms);
}

/* 处理过期定时器，返回处理的过期数量 */
static int timer_process_expired(timer_manager_t *mgr,
                                  int *expired_fds,
                                  int max_expired) {
    mgr->current_time = get_current_time();
    int count = 0;

    while (mgr->head &&
           mgr->head->expire_time <= mgr->current_time &&
           count < max_expired) {
        expired_fds[count++] = mgr->head->conn_fd;
        timer_node_t *next = mgr->head->next;
        free(mgr->head);
        mgr->head = next;
        if (mgr->head) {
            mgr->head->prev = NULL;
        }
    }

    return count;
}

/*==================== 连接池管理 ====================*/

static bool conn_pool_init(conn_pool_t *pool, size_t capacity) {
    pool->pool = calloc(capacity, sizeof(connection_t));
    if (!pool->pool) return false;

    pool->capacity = capacity;
    pool->used = 0;
    pool->free_list = NULL;

    /* 初始化空闲链表 */
    for (size_t i = 0; i < capacity; i++) {
        pool->pool[i].fd = -1;
        pool->pool[i].next = pool->free_list;
        pool->free_list = &pool->pool[i];
    }

    pthread_mutex_init(&pool->lock, NULL);
    return true;
}

static connection_t* conn_pool_alloc(conn_pool_t *pool) {
    pthread_mutex_lock(&pool->lock);

    if (!pool->free_list) {
        pthread_mutex_unlock(&pool->lock);
        return NULL;
    }

    connection_t *conn = pool->free_list;
    pool->free_list = conn->next;
    pool->used++;

    /* 初始化连接 */
    memset(conn, 0, sizeof(connection_t));
    conn->fd = -1;
    conn->state = CONN_STATE_IDLE;
    conn->last_active = get_current_time();

    pthread_mutex_unlock(&pool->lock);
    return conn;
}

static void conn_pool_free(conn_pool_t *pool, connection_t *conn) {
    if (!conn) return;

    pthread_mutex_lock(&pool->lock);

    if (conn->fd >= 0) {
        close(conn->fd);
        conn->fd = -1;
    }

    conn->state = CONN_STATE_IDLE;
    conn->next = pool->free_list;
    pool->free_list = conn;
    pool->used--;

    pthread_mutex_unlock(&pool->lock);
}

static void conn_pool_cleanup(conn_pool_t *pool) {
    pthread_mutex_lock(&pool->lock);

    for (size_t i = 0; i < pool->capacity; i++) {
        if (pool->pool[i].fd >= 0) {
            close(pool->pool[i].fd);
        }
    }

    free(pool->pool);
    pool->pool = NULL;
    pool->free_list = NULL;

    pthread_mutex_unlock(&pool->lock);
    pthread_mutex_destroy(&pool->lock);
}

/*==================== 网络操作 ====================*/

static bool server_socket_init(server_context_t *ctx, int port) {
    ctx->listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (ctx->listen_fd < 0) {
        perror("socket");
        return false;
    }

    if (!set_nonblocking(ctx->listen_fd) ||
        !set_reuse_addr(ctx->listen_fd) ||
        !set_tcp_nodelay(ctx->listen_fd)) {
        close(ctx->listen_fd);
        return false;
    }

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(port),
        .sin_addr.s_addr = INADDR_ANY
    };

    if (bind(ctx->listen_fd, (struct sockaddr *)&addr,
             sizeof(addr)) < 0) {
        perror("bind");
        close(ctx->listen_fd);
        return false;
    }

    if (listen(ctx->listen_fd, BACKLOG) < 0) {
        perror("listen");
        close(ctx->listen_fd);
        return false;
    }

    printf("Server listening on port %d\n", port);
    return true;
}

static bool epoll_init(server_context_t *ctx) {
    ctx->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (ctx->epoll_fd < 0) {
        perror("epoll_create1");
        return false;
    }

    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.ptr = NULL  /* NULL表示监听套接字 */
    };

    if (epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD,
                  ctx->listen_fd, &ev) < 0) {
        perror("epoll_ctl");
        close(ctx->epoll_fd);
        return false;
    }

    return true;
}

/*==================== 请求处理 ====================*/

/* 简单的HTTP响应生成 */
static void generate_http_response(connection_t *conn,
                                    const char *body) {
    const char *template =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: %zu\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "%s";

    snprintf(conn->send_buf, BUFFER_SIZE,
             template, strlen(body), body);
    conn->send_len = strlen(conn->send_buf);
    conn->send_offset = 0;
}

/* 处理接收到的数据 */
static void handle_request(connection_t *conn,
                           server_context_t *ctx) {
    /* 简单处理：检查是否收到完整HTTP请求 */
    char *end = strstr(conn->recv_buf, "\r\n\r\n");
    if (!end) return;  /* 请求不完整，继续接收 */

    ctx->total_requests++;
    ctx->total_bytes += conn->recv_len;

    /* 生成响应 */
    char body[256];
    snprintf(body, sizeof(body),
             "Hello from High-Perf Server!\n"
             "Request #%llu, Total bytes: %llu\n",
             (unsigned long long)ctx->total_requests,
             (unsigned long long)ctx->total_bytes);

    generate_http_response(conn, body);
    conn->state = CONN_STATE_WRITING;
    conn->recv_len = 0;  /* 清空接收缓冲区 */

    /* 更新关注事件为写 */
    struct epoll_event ev = {
        .events = EPOLLOUT | EPOLLET,
        .data.ptr = conn
    };
    epoll_ctl(ctx->epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);
}

/* 接受新连接 */
static bool accept_connection(server_context_t *ctx) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(ctx->listen_fd,
                           (struct sockaddr *)&client_addr,
                           &addr_len);

    if (client_fd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return true;  /* 没有更多连接 */
        }
        perror("accept");
        return false;
    }

    if (!set_nonblocking(client_fd) || !set_tcp_nodelay(client_fd)) {
        close(client_fd);
        return false;
    }

    connection_t *conn = conn_pool_alloc(&ctx->conn_pool);
    if (!conn) {
        close(client_fd);
        fprintf(stderr, "Connection pool exhausted\n");
        return false;
    }

    conn->fd = client_fd;
    conn->state = CONN_STATE_READING;
    conn->last_active = get_current_time();

    /* 添加定时器 */
    conn->timer = timer_add(&ctx->timer_mgr, client_fd, TIMEOUT_MS);

    /* 添加到epoll */
    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLET,  /* 边沿触发 */
        .data.ptr = conn
    };

    if (epoll_ctl(ctx->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
        conn_pool_free(&ctx->conn_pool, conn);
        return false;
    }

    printf("New connection: fd=%d, pool used: %zu/%zu\n",
           client_fd, ctx->conn_pool.used, ctx->conn_pool.capacity);

    return true;
}

/* 读取数据 */
static bool handle_read(connection_t *conn, server_context_t *ctx) {
    while (conn->recv_len < BUFFER_SIZE - 1) {
        ssize_t n = recv(conn->fd,
                         conn->recv_buf + conn->recv_len,
                         BUFFER_SIZE - conn->recv_len - 1,
                         0);

        if (n > 0) {
            conn->recv_len += n;
        } else if (n == 0) {
            return false;  /* 连接关闭 */
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;  /* 数据读取完毕 */
            }
            return false;  /* 读取错误 */
        }
    }

    conn->recv_buf[conn->recv_len] = '\0';
    conn->last_active = get_current_time();

    /* 更新定时器 */
    if (conn->timer) {
        timer_remove(&ctx->timer_mgr, conn->timer);
        conn->timer = timer_add(&ctx->timer_mgr, conn->fd, TIMEOUT_MS);
    }

    handle_request(conn, ctx);
    return true;
}

/* 发送数据 */
static bool handle_write(connection_t *conn, server_context_t *ctx) {
    while (conn->send_offset < conn->send_len) {
        ssize_t n = send(conn->fd,
                         conn->send_buf + conn->send_offset,
                         conn->send_len - conn->send_offset,
                         MSG_NOSIGNAL);

        if (n > 0) {
            conn->send_offset += n;
        } else if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                return true;  /* 发送缓冲区满，下次继续 */
            }
            return false;  /* 发送错误 */
        }
    }

    /* 发送完成，切换回读模式 */
    conn->send_len = 0;
    conn->send_offset = 0;
    conn->state = CONN_STATE_READING;

    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLET,
        .data.ptr = conn
    };
    epoll_ctl(ctx->epoll_fd, EPOLL_CTL_MOD, conn->fd, &ev);

    conn->last_active = get_current_time();
    return true;
}

/* 关闭连接 */
static void close_connection(connection_t *conn, server_context_t *ctx) {
    if (conn->timer) {
        timer_remove(&ctx->timer_mgr, conn->timer);
        conn->timer = NULL;
    }

    epoll_ctl(ctx->epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
    conn_pool_free(&ctx->conn_pool, conn);

    printf("Connection closed, pool used: %zu/%zu\n",
           ctx->conn_pool.used, ctx->conn_pool.capacity);
}

/*==================== 事件循环 ====================*/

void server_run(server_context_t *ctx) {
    struct epoll_event events[MAX_EVENTS];
    int expired_fds[64];

    ctx->running = true;
    ctx->start_time = get_current_time();

    printf("Server started, entering event loop...\n");

    while (ctx->running) {
        /* 等待事件，100ms超时用于定时器处理 */
        int nfds = epoll_wait(ctx->epoll_fd, events, MAX_EVENTS, 100);

        if (nfds < 0) {
            if (errno == EINTR) continue;
            perror("epoll_wait");
            break;
        }

        /* 处理事件 */
        for (int i = 0; i < nfds; i++) {
            struct epoll_event *ev = &events[i];

            if (ev->data.ptr == NULL) {
                /* 监听套接字有连接 */
                while (accept_connection(ctx)) {}
            } else {
                connection_t *conn = ev->data.ptr;

                if (ev->events & (EPOLLERR | EPOLLHUP)) {
                    close_connection(conn, ctx);
                    continue;
                }

                if (ev->events & EPOLLIN) {
                    if (!handle_read(conn, ctx)) {
                        close_connection(conn, ctx);
                        continue;
                    }
                }

                if (ev->events & EPOLLOUT) {
                    if (!handle_write(conn, ctx)) {
                        close_connection(conn, ctx);
                    }
                }
            }
        }

        /* 处理过期连接 */
        int n_expired = timer_process_expired(&ctx->timer_mgr,
                                               expired_fds, 64);
        for (int i = 0; i < n_expired; i++) {
            /* 查找连接并关闭 */
            connection_t *conn = NULL;
            for (size_t j = 0; j < ctx->conn_pool.capacity; j++) {
                if (ctx->conn_pool.pool[j].fd == expired_fds[i]) {
                    conn = &ctx->conn_pool.pool[j];
                    break;
                }
            }
            if (conn && conn->fd >= 0) {
                printf("Connection timeout: fd=%d\n", conn->fd);
                conn->timer = NULL;  /* 已被timer_process_expired释放 */
                close_connection(conn, ctx);
            }
        }
    }

    printf("Server stopped.\n");
}

/*==================== 生命周期管理 ====================*/

bool server_init(server_context_t *ctx, int port) {
    memset(ctx, 0, sizeof(*ctx));

    if (!server_socket_init(ctx, port)) return false;
    if (!epoll_init(ctx)) return false;
    if (!conn_pool_init(&ctx->conn_pool, MAX_CONNECTIONS)) {
        close(ctx->epoll_fd);
        close(ctx->listen_fd);
        return false;
    }

    timer_init(&ctx->timer_mgr);

    return true;
}

void server_stop(server_context_t *ctx) {
    ctx->running = false;
}

void server_cleanup(server_context_t *ctx) {
    conn_pool_cleanup(&ctx->conn_pool);

    if (ctx->epoll_fd >= 0) {
        close(ctx->epoll_fd);
        ctx->epoll_fd = -1;
    }

    if (ctx->listen_fd >= 0) {
        close(ctx->listen_fd);
        ctx->listen_fd = -1;
    }

    /* 清理定时器 */
    while (ctx->timer_mgr.head) {
        timer_node_t *next = ctx->timer_mgr.head->next;
        free(ctx->timer_mgr.head);
        ctx->timer_mgr.head = next;
    }
}

/*==================== 主函数 ====================*/

int main(int argc, char *argv[]) {
    int port = (argc > 1) ? atoi(argv[1]) : SERVER_PORT;
    server_context_t ctx;

    printf("High-Performance TCP Server\n");
    printf("===========================\n\n");

    if (!server_init(&ctx, port)) {
        fprintf(stderr, "Server initialization failed\n");
        return 1;
    }

    /* 处理SIGINT信号 */
    signal(SIGINT, (void (*)(int))server_stop);

    server_run(&ctx);
    server_cleanup(&ctx);

    return 0;
}

#endif /* PLATFORM_LINUX */
