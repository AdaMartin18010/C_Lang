/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 4190
 * Language: c
 * Block ID: 9748b6e2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * TCP Load Balancer / Proxy
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "proxy.h"

static void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int create_listen_socket(const char *addr, uint16_t port, int backlog) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    inet_aton(addr, &sin.sin_addr);

    if (bind(fd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        close(fd);
        return -1;
    }

    if (listen(fd, backlog) < 0) {
        close(fd);
        return -1;
    }

    set_nonblocking(fd);
    return fd;
}

static int connect_to_backend(backend_t *backend) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    set_nonblocking(fd);

    int ret = connect(fd, (struct sockaddr *)&backend->addr,
                      sizeof(backend->addr));
    if (ret < 0 && errno != EINPROGRESS) {
        close(fd);
        return -1;
    }

    return fd;
}

loadbalancer_t* lb_create(const lb_config_t *config) {
    loadbalancer_t *lb = calloc(1, sizeof(loadbalancer_t));
    if (!lb) return NULL;

    memcpy(&lb->config, config, sizeof(lb_config_t));

    lb->connections = calloc(config->max_connections, sizeof(proxy_conn_t));
    if (!lb->connections) {
        free(lb);
        return NULL;
    }

    lb->sessions = calloc(10000, sizeof(session_t));
    if (!lb->sessions) {
        free(lb->connections);
        free(lb);
        return NULL;
    }

    lb->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (lb->epoll_fd < 0) {
        free(lb->sessions);
        free(lb->connections);
        free(lb);
        return NULL;
    }

    lb->listen_fd = create_listen_socket(config->bind_addr, config->listen_port,
                                         MAX_CONNECTIONS);
    if (lb->listen_fd < 0) {
        close(lb->epoll_fd);
        free(lb->sessions);
        free(lb->connections);
        free(lb);
        return NULL;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = NULL;  /* NULL indicates listen socket */
    epoll_ctl(lb->epoll_fd, EPOLL_CTL_ADD, lb->listen_fd, &ev);

    lb->running = true;
    return lb;
}

void lb_destroy(loadbalancer_t *lb) {
    if (!lb) return;

    lb_stop(lb);

    if (lb->listen_fd >= 0) close(lb->listen_fd);
    if (lb->epoll_fd >= 0) close(lb->epoll_fd);

    free(lb->connections);
    free(lb->sessions);
    free(lb);
}

int lb_add_backend(loadbalancer_t *lb, const backend_t *backend) {
    if (lb->backend_count >= MAX_BACKENDS) return -1;

    memcpy(&lb->backends[lb->backend_count], backend, sizeof(backend_t));
    lb->backends[lb->backend_count].status = BACKEND_UP;
    lb->backend_count++;

    return 0;
}

/* Hash function for IP hash algorithm */
static uint32_t hash_ip(uint32_t ip) {
    return ip * 2654435761U;
}

backend_t* lb_select_backend(loadbalancer_t *lb, uint32_t client_ip) {
    backend_t *selected = NULL;
    int min_conn = INT_MAX;

    switch (lb->config.algorithm) {
        case LB_ROUND_ROBIN:
            for (int i = 0; i < lb->backend_count; i++) {
                int idx = (lb->rr_current + i) % lb->backend_count;
                if (lb->backends[idx].status == BACKEND_UP) {
                    selected = &lb->backends[idx];
                    lb->rr_current = idx + 1;
                    break;
                }
            }
            break;

        case LB_LEAST_CONN:
            for (int i = 0; i < lb->backend_count; i++) {
                if (lb->backends[i].status == BACKEND_UP &&
                    lb->backends[i].active_connections < min_conn) {
                    min_conn = lb->backends[i].active_connections;
                    selected = &lb->backends[i];
                }
            }
            break;

        case LB_IP_HASH:
            if (lb->backend_count > 0) {
                uint32_t hash = hash_ip(client_ip);
                int start_idx = hash % lb->backend_count;

                for (int i = 0; i < lb->backend_count; i++) {
                    int idx = (start_idx + i) % lb->backend_count;
                    if (lb->backends[idx].status == BACKEND_UP) {
                        selected = &lb->backends[idx];
                        break;
                    }
                }
            }
            break;

        case LB_WEIGHTED_RR:
            /* Simplified weighted round robin */
            for (int i = 0; i < lb->backend_count; i++) {
                int idx = (lb->rr_current + i) % lb->backend_count;
                if (lb->backends[idx].status == BACKEND_UP) {
                    selected = &lb->backends[idx];
                    lb->rr_current = idx + 1;
                    break;
                }
            }
            break;

        case LB_RANDOM:
            for (int attempts = 0; attempts < lb->backend_count; attempts++) {
                int idx = rand() % lb->backend_count;
                if (lb->backends[idx].status == BACKEND_UP) {
                    selected = &lb->backends[idx];
                    break;
                }
            }
            break;
    }

    if (selected) {
        selected->active_connections++;
        selected->total_connections++;
    }

    return selected;
}

static void close_connection(loadbalancer_t *lb, proxy_conn_t *conn) {
    if (conn->client_fd >= 0) {
        epoll_ctl(lb->epoll_fd, EPOLL_CTL_DEL, conn->client_fd, NULL);
        close(conn->client_fd);
    }
    if (conn->backend_fd >= 0) {
        epoll_ctl(lb->epoll_fd, EPOLL_CTL_DEL, conn->backend_fd, NULL);
        close(conn->backend_fd);
    }

    if (conn->backend) {
        conn->backend->active_connections--;
        conn->backend->bytes_sent += conn->bytes_from_client;
        conn->backend->bytes_received += conn->bytes_from_backend;
    }

    memset(conn, 0, sizeof(proxy_conn_t));
    conn->client_fd = -1;
    conn->backend_fd = -1;
    lb->connection_count--;
}

static int handle_new_connection(loadbalancer_t *lb) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int client_fd = accept(lb->listen_fd, (struct sockaddr *)&client_addr, &addr_len);
    if (client_fd < 0) {
        if (errno != EAGAIN && errno != EWOULDBLOCK) {
            perror("accept");
        }
        return -1;
    }

    set_nonblocking(client_fd);

    if (lb->config.tcp_nodelay) {
        int nodelay = 1;
        setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY, &nodelay, sizeof(nodelay));
    }

    /* Select backend */
    uint32_t client_ip = ntohl(client_addr.sin_addr.s_addr);
    backend_t *backend = lb_select_backend(lb, client_ip);

    if (!backend) {
        close(client_fd);
        return -1;
    }

    /* Connect to backend */
    int backend_fd = connect_to_backend(backend);
    if (backend_fd < 0) {
        backend->status = BACKEND_DOWN;
        close(client_fd);
        return -1;
    }

    /* Find free connection slot */
    proxy_conn_t *conn = NULL;
    for (int i = 0; i < lb->config.max_connections; i++) {
        if (lb->connections[i].client_fd == 0) {
            conn = &lb->connections[i];
            break;
        }
    }

    if (!conn) {
        close(client_fd);
        close(backend_fd);
        return -1;
    }

    memset(conn, 0, sizeof(proxy_conn_t));
    conn->client_fd = client_fd;
    conn->backend_fd = backend_fd;
    conn->backend = backend;
    conn->created = time(NULL);
    conn->last_activity = conn->created;

    /* Add to epoll */
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLET;
    ev.data.ptr = conn;
    epoll_ctl(lb->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

    ev.events = EPOLLOUT | EPOLLET;
    ev.data.ptr = conn;
    epoll_ctl(lb->epoll_fd, EPOLL_CTL_ADD, backend_fd, &ev);

    lb->connection_count++;

    return 0;
}

static void relay_data(proxy_conn_t *conn, int from_fd, int to_fd,
                       uint8_t *buf, size_t *buf_len, size_t *buf_sent,
                       uint64_t *byte_counter, struct epoll_event *ev) {
    if (*buf_sent < *buf_len) {
        /* Continue sending buffered data */
        ssize_t sent = send(to_fd, buf + *buf_sent, *buf_len - *buf_sent,
                           MSG_NOSIGNAL);
        if (sent > 0) {
            *buf_sent += (size_t)sent;
            *byte_counter += (size_t)sent;
        }

        if (*buf_sent >= *buf_len) {
            *buf_len = 0;
            *buf_sent = 0;
        }
    } else {
        /* Read new data */
        ssize_t received = recv(from_fd, buf, BUFFER_SIZE, 0);
        if (received > 0) {
            *buf_len = (size_t)received;
            *buf_sent = 0;

            /* Try to send immediately */
            ssize_t sent = send(to_fd, buf, *buf_len, MSG_NOSIGNAL);
            if (sent > 0) {
                *buf_sent = (size_t)sent;
                *byte_counter += (size_t)sent;
            } else if (sent < 0 && errno != EAGAIN) {
                /* Error */
            }
        } else if (received == 0 || (received < 0 && errno != EAGAIN)) {
            /* Connection closed or error */
            if (from_fd == conn->client_fd) {
                conn->client_closed = true;
            } else {
                conn->backend_closed = true;
            }
        }
    }
}

void lb_run(loadbalancer_t *lb) {
    struct epoll_event events[1024];

    /* Start health check thread */
    pthread_create(&lb->health_thread, NULL, lb_health_check_thread, lb);

    while (lb->running) {
        int nfds = epoll_wait(lb->epoll_fd, events, 1024, 100);

        for (int i = 0; i < nfds; i++) {
            struct epoll_event *ev = &events[i];

            if (ev->data.ptr == NULL) {
                /* New connection */
                handle_new_connection(lb);
            } else {
                proxy_conn_t *conn = (proxy_conn_t *)ev->data.ptr;

                if (conn->client_closed && conn->backend_closed) {
                    close_connection(lb, conn);
                    continue;
                }

                if (ev->events & EPOLLIN) {
                    if (ev->data.fd == conn->client_fd) {
                        relay_data(conn, conn->client_fd, conn->backend_fd,
                                  conn->client_buf, &conn->client_buf_len,
                                  &conn->client_buf_sent,
                                  &conn->bytes_from_client, ev);
                    } else {
                        relay_data(conn, conn->backend_fd, conn->client_fd,
                                  conn->backend_buf, &conn->backend_buf_len,
                                  &conn->backend_buf_sent,
                                  &conn->bytes_from_backend, ev);
                    }
                }

                if (ev->events & (EPOLLERR | EPOLLHUP)) {
                    close_connection(lb, conn);
                }
            }
        }

        /* Cleanup idle connections */
        time_t now = time(NULL);
        for (int i = 0; i < lb->config.max_connections; i++) {
            proxy_conn_t *conn = &lb->connections[i];
            if (conn->client_fd > 0 &&
                now - conn->last_activity > lb->config.idle_timeout) {
                close_connection(lb, conn);
            }
        }
    }

    pthread_join(lb->health_thread, NULL);
}

int lb_start(loadbalancer_t *lb) {
    printf("Load balancer started on %s:%d\n",
           lb->config.bind_addr, lb->config.listen_port);
    printf("Algorithm: %d\n", lb->config.algorithm);
    printf("Backends: %d\n", lb->backend_count);

    lb_run(lb);
    return 0;
}

void lb_stop(loadbalancer_t *lb) {
    lb->running = false;
}

/* Health check implementation */
void* lb_health_check_thread(void *arg) {
    loadbalancer_t *lb = arg;

    while (lb->running) {
        sleep(lb->config.check_interval);

        for (int i = 0; i < lb->backend_count; i++) {
            backend_t *be = &lb->backends[i];

            if (be->check_type == CHECK_NONE) continue;

            int ret;
            if (be->check_type == CHECK_TCP) {
                ret = lb_check_backend_tcp(be);
            } else {
                ret = lb_check_backend_http(be, lb->config.check_http_expect);
            }

            if (ret == 0) {
                if (be->status == BACKEND_DOWN) {
                    printf("Backend %s is UP\n", be->name);
                    be->status = BACKEND_UP;
                }
                be->check_fail_count = 0;
            } else {
                be->check_fail_count++;
                if (be->check_fail_count >= be->max_fails &&
                    be->status == BACKEND_UP) {
                    printf("Backend %s is DOWN\n", be->name);
                    be->status = BACKEND_DOWN;
                    be->last_fail = time(NULL);
                }
            }

            be->last_check = time(NULL);
        }
    }

    return NULL;
}

int lb_check_backend_tcp(backend_t *backend) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct timeval tv;
    tv.tv_sec = CHECK_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    int ret = connect(fd, (struct sockaddr *)&backend->addr,
                      sizeof(backend->addr));
    close(fd);

    return ret;
}

int lb_check_backend_http(backend_t *backend, const char *expect) {
    /* Simplified HTTP health check */
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return -1;

    struct timeval tv;
    tv.tv_sec = CHECK_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    if (connect(fd, (struct sockaddr *)&backend->addr,
                sizeof(backend->addr)) < 0) {
        close(fd);
        return -1;
    }

    char request[512];
    snprintf(request, sizeof(request),
             "GET %s HTTP/1.0\r\nHost: health\r\n\r\n",
             backend->check_path);

    send(fd, request, strlen(request), 0);

    char response[1024];
    int n = recv(fd, response, sizeof(response) - 1, 0);
    close(fd);

    if (n <= 0) return -1;
    response[n] = '\0';

    if (expect && strstr(response, expect) == NULL) {
        return -1;
    }

    return 0;
}
