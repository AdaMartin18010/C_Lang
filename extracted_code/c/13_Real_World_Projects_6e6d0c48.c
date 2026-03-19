/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 179
 * Language: c
 * Block ID: 6e6d0c48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>

#define SERVER_VERSION "1.0.0"
#define DEFAULT_PORT 8080
#define DEFAULT_BACKLOG 1024
#define DEFAULT_WORKERS 4
#define MAX_EVENTS 1024
#define BUFFER_SIZE 8192
#define MAX_REQUEST_SIZE 1048576  /* 1MB */
#define REQUEST_TIMEOUT 30        /* seconds */
#define KEEP_ALIVE_TIMEOUT 5      /* seconds */
#define MAX_KEEP_ALIVE_REQUESTS 100

typedef struct server_config {
    uint16_t port;
    int backlog;
    int worker_threads;
    char document_root[512];
    char cgi_path[512];
    char log_file[256];
    char access_log[256];
    bool daemon_mode;
    int max_connections;
} server_config_t;

typedef struct server {
    int listen_fd;
    int epoll_fd;
    struct sockaddr_in addr;
    server_config_t config;
    volatile bool running;
    int active_connections;
} server_t;

/* Server lifecycle */
server_t* server_create(const server_config_t *config);
int server_start(server_t *server);
void server_stop(server_t *server);
void server_destroy(server_t *server);

/* Configuration */
int server_load_config(server_config_t *config, const char *path);
void server_default_config(server_config_t *config);

#endif /* SERVER_H */
