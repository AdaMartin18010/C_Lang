/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 744
 * Language: c
 * Block ID: be02517e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * HTTP Web Server - Core Server Implementation
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>

#include "server.h"
#include "connection.h"
#include "http_parser.h"
#include "static_handler.h"
#include "cgi_handler.h"
#include "logger.h"

extern logger_t *g_logger;

static void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static int create_listen_socket(uint16_t port, int backlog) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        LOG_ERROR(g_logger, "socket() failed: %s", strerror(errno));
        return -1;
    }

    /* Socket options */
    int reuse = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        LOG_WARN(g_logger, "setsockopt(SO_REUSEADDR) failed: %s", strerror(errno));
    }

    /* TCP keepalive */
    int keepalive = 1;
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));

    /* Bind */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOG_ERROR(g_logger, "bind() failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    /* Listen */
    if (listen(fd, backlog) < 0) {
        LOG_ERROR(g_logger, "listen() failed: %s", strerror(errno));
        close(fd);
        return -1;
    }

    set_nonblocking(fd);
    LOG_INFO(g_logger, "Listening socket created on port %d", port);

    return fd;
}

static int handle_request(connection_t *conn, const server_config_t *config) {
    http_request_t *req = &conn->request;
    http_response_t *resp = &conn->response;

    http_response_init(resp);
    resp->keep_alive = req->keep_alive;

    /* Validate request */
    if (req->method == HTTP_UNKNOWN) {
        http_set_status(resp, HTTP_NOT_IMPLEMENTED);
        http_set_body(resp, "<h1>501 Not Implemented</h1>", 28);
        return -1;
    }

    /* Check request size */
    if (req->body_len > MAX_REQUEST_SIZE) {
        http_set_status(resp, HTTP_PAYLOAD_TOO_LARGE);
        http_set_body(resp, "<h1>413 Payload Too Large</h1>", 30);
        return -1;
    }

    /* Static file handler */
    static_config_t static_config;
    memset(&static_config, 0, sizeof(static_config));
    strncpy(static_config.document_root, config->document_root,
            sizeof(static_config.document_root) - 1);
    static_config.directory_listing = false;
    static_config.cache_max_age = 3600;

    /* Check for CGI first */
    cgi_config_t cgi_config;
    memset(&cgi_config, 0, sizeof(cgi_config));
    strncpy(cgi_config.cgi_path, config->cgi_path, sizeof(cgi_config.cgi_path) - 1);
    strcpy(cgi_config.script_alias, "/cgi-bin/");
    cgi_config.timeout = CGI_TIMEOUT;

    if (cgi_is_script_request(req->uri, &cgi_config)) {
        int ret = cgi_handle_request(req, resp, &cgi_config);
        if (ret == 0) {
            return 0;
        }
        /* Fall back to 404 if CGI fails */
    }

    /* Static file handling */
    int ret = static_handle_request(req, resp, &static_config);
    if (ret == 0) {
        return 0;
    }

    /* 404 Not Found */
    http_set_status(resp, HTTP_NOT_FOUND);
    const char *not_found =
        "<!DOCTYPE html>\n"
        "<html><head><title>404 Not Found</title></head>\n"
        "<body><h1>404 Not Found</h1>\n"
        "<p>The requested resource was not found on this server.</p>\n"
        "</body></html>\n";
    http_set_body(resp, not_found, strlen(not_found));

    return 0;
}

static void event_loop(server_t *server) {
    struct epoll_event events[MAX_EVENTS];
    connection_pool_t *pool = conn_pool_create(server->config.max_connections);

    if (!pool) {
        LOG_ERROR(g_logger, "Failed to create connection pool");
        return;
    }

    /* Add listen socket to epoll */
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.ptr = NULL;  /* NULL ptr indicates listen socket */
    epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, server->listen_fd, &ev);

    LOG_INFO(g_logger, "Event loop started");

    while (server->running) {
        int nfds = epoll_wait(server->epoll_fd, events, MAX_EVENTS, 100);

        for (int i = 0; i < nfds; i++) {
            struct epoll_event *event = &events[i];

            if (event->data.ptr == NULL) {
                /* New connection */
                struct sockaddr_in client_addr;
                socklen_t addr_len = sizeof(client_addr);

                int client_fd = accept(server->listen_fd,
                                       (struct sockaddr *)&client_addr,
                                       &addr_len);
                if (client_fd < 0) {
                    if (errno != EAGAIN && errno != EWOULDBLOCK) {
                        LOG_ERROR(g_logger, "accept() failed: %s", strerror(errno));
                    }
                    continue;
                }

                set_nonblocking(client_fd);

                /* TCP_NODELAY for low latency */
                int nodelay = 1;
                setsockopt(client_fd, IPPROTO_TCP, TCP_NODELAY,
                          &nodelay, sizeof(nodelay));

                connection_t *conn = conn_acquire(pool, client_fd,
                                                  &client_addr, addr_len);
                if (!conn) {
                    LOG_WARN(g_logger, "Connection pool full, rejecting connection");
                    close(client_fd);
                    continue;
                }

                /* Add to epoll */
                ev.events = EPOLLIN | EPOLLET;
                ev.data.ptr = conn;
                epoll_ctl(server->epoll_fd, EPOLL_CTL_ADD, client_fd, &ev);

                server->active_connections++;
                LOG_DEBUG(g_logger, "New connection from %s:%d (fd=%d)",
                         inet_ntoa(client_addr.sin_addr),
                         ntohs(client_addr.sin_port), client_fd);
            } else {
                /* Existing connection */
                connection_t *conn = (connection_t *)event->data.ptr;

                if (event->events & EPOLLIN) {
                    /* Read data */
                    int bytes = conn_read(conn);
                    if (bytes < 0) {
                        /* Connection error or closed */
                        epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
                        close(conn->fd);
                        conn_release(pool, conn);
                        server->active_connections--;
                        continue;
                    }

                    /* Try to parse request */
                    if (conn->state == CONN_STATE_READING_REQUEST) {
                        int ret = http_parse_request(&conn->request,
                                                     conn->read_buf,
                                                     conn->read_len);
                        if (ret == 0) {
                            /* Complete request received */
                            conn->state = CONN_STATE_PROCESSING;

                            LOG_DEBUG(g_logger, "Request: %s %s",
                                     http_method_text(conn->request.method),
                                     conn->request.uri);

                            /* Process request */
                            handle_request(conn, &server->config);
                            conn->state = CONN_STATE_WRITING_RESPONSE;

                            /* Switch to write mode */
                            ev.events = EPOLLOUT | EPOLLET;
                            ev.data.ptr = conn;
                            epoll_ctl(server->epoll_fd, EPOLL_CTL_MOD,
                                     conn->fd, &ev);
                        }
                    }
                }

                if (event->events & EPOLLOUT) {
                    /* Write response */
                    if (conn->state == CONN_STATE_WRITING_RESPONSE) {
                        /* Serialize if not done */
                        if (conn->write_len == 0) {
                            http_response_serialize(&conn->response,
                                                    conn->write_buf,
                                                    sizeof(conn->write_buf));
                            conn->write_len = strlen(conn->write_buf);

                            /* Add body if present */
                            if (conn->response.body && conn->response.body_len > 0) {
                                size_t remaining = sizeof(conn->write_buf) - conn->write_len;
                                if (conn->response.body_len < remaining) {
                                    memcpy(conn->write_buf + conn->write_len,
                                           conn->response.body,
                                           conn->response.body_len);
                                    conn->write_len += conn->response.body_len;
                                }
                            }
                        }

                        int written = conn_write(conn);
                        if (written < 0 || conn->write_pos >= conn->write_len) {
                            /* Response complete or error */
                            if (conn->request.keep_alive && conn->request_count < MAX_KEEP_ALIVE_REQUESTS) {
                                /* Keep alive */
                                conn_reset(conn);
                                conn->request_count++;
                                conn->state = CONN_STATE_READING_REQUEST;

                                /* Switch back to read mode */
                                ev.events = EPOLLIN | EPOLLET;
                                ev.data.ptr = conn;
                                epoll_ctl(server->epoll_fd, EPOLL_CTL_MOD,
                                         conn->fd, &ev);
                            } else {
                                /* Close connection */
                                epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL,
                                         conn->fd, NULL);
                                close(conn->fd);
                                conn_release(pool, conn);
                                server->active_connections--;
                            }
                        }
                    }
                }

                /* Check for errors */
                if (event->events & (EPOLLERR | EPOLLHUP)) {
                    epoll_ctl(server->epoll_fd, EPOLL_CTL_DEL, conn->fd, NULL);
                    close(conn->fd);
                    conn_release(pool, conn);
                    server->active_connections--;
                }
            }
        }

        /* TODO: Periodic timeout checks */
    }

    conn_pool_destroy(pool);
}

server_t* server_create(const server_config_t *config) {
    server_t *server = calloc(1, sizeof(server_t));
    if (!server) {
        return NULL;
    }

    memcpy(&server->config, config, sizeof(server_config_t));
    server->running = false;

    /* Create listen socket */
    server->listen_fd = create_listen_socket(config->port, config->backlog);
    if (server->listen_fd < 0) {
        free(server);
        return NULL;
    }

    /* Create epoll instance */
    server->epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    if (server->epoll_fd < 0) {
        LOG_ERROR(g_logger, "epoll_create1() failed: %s", strerror(errno));
        close(server->listen_fd);
        free(server);
        return NULL;
    }

    return server;
}

int server_start(server_t *server) {
    server->running = true;
    event_loop(server);
    return 0;
}

void server_stop(server_t *server) {
    server->running = false;
}

void server_destroy(server_t *server) {
    if (!server) return;

    if (server->epoll_fd >= 0) {
        close(server->epoll_fd);
    }
    if (server->listen_fd >= 0) {
        close(server->listen_fd);
    }

    free(server);
}

void server_default_config(server_config_t *config) {
    memset(config, 0, sizeof(server_config_t));
    config->port = DEFAULT_PORT;
    config->backlog = DEFAULT_BACKLOG;
    config->worker_threads = DEFAULT_WORKERS;
    strcpy(config->document_root, "./www");
    strcpy(config->cgi_path, "./www/cgi-bin");
    config->max_connections = 10000;
    config->daemon_mode = false;
}

int server_load_config(server_config_t *config, const char *path) {
    /* TODO: Implement config file parsing (JSON/INI) */
    (void)config;
    (void)path;
    return 0;
}
