/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 4016
 * Language: c
 * Block ID: d85eb01d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef PROXY_H
#define PROXY_H

#include <stdint.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <time.h>

#define PROXY_VERSION "1.0.0"
#define MAX_BACKENDS 64
#define MAX_LISTENERS 16
#define MAX_CONNECTIONS 65536
#define BUFFER_SIZE 65536
#define CHECK_INTERVAL 5
#define CHECK_TIMEOUT 3

typedef enum {
    LB_ROUND_ROBIN = 0,
    LB_LEAST_CONN = 1,
    LB_IP_HASH = 2,
    LB_WEIGHTED_RR = 3,
    LB_RANDOM = 4
} lb_algorithm_t;

typedef enum {
    BACKEND_UP = 0,
    BACKEND_DOWN = 1,
    BACKEND_CHECKING = 2
} backend_status_t;

typedef enum {
    CHECK_TCP = 0,
    CHECK_HTTP = 1,
    CHECK_NONE = 2
} check_type_t;

/* Backend server configuration */
typedef struct backend {
    char name[64];
    struct sockaddr_in addr;
    int weight;
    int max_fails;
    int fail_timeout;
    check_type_t check_type;
    char check_path[256];  /* For HTTP checks */

    /* Runtime state */
    backend_status_t status;
    int active_connections;
    int total_connections;
    uint64_t bytes_sent;
    uint64_t bytes_received;
    time_t last_check;
    time_t last_fail;
    int fail_count;
    int check_fail_count;
} backend_t;

/* Load balancer configuration */
typedef struct lb_config {
    uint16_t listen_port;
    char bind_addr[64];
    lb_algorithm_t algorithm;
    int max_connections;
    int connection_timeout;
    int idle_timeout;
    bool tcp_nodelay;
    bool tcp_keepalive;

    /* Health check */
    int check_interval;
    int check_timeout;
    char check_http_expect[256];

    /* Stats */
    char stats_addr[64];
    uint16_t stats_port;
} lb_config_t;

/* Session tracking */
typedef struct session {
    uint32_t client_ip;
    uint32_t backend_idx;
    time_t created;
    time_t last_used;
} session_t;

/* Connection structure */
typedef struct proxy_conn {
    int client_fd;
    int backend_fd;
    backend_t *backend;

    /* Buffers */
    uint8_t client_buf[BUFFER_SIZE];
    size_t client_buf_len;
    size_t client_buf_sent;

    uint8_t backend_buf[BUFFER_SIZE];
    size_t backend_buf_len;
    size_t backend_buf_sent;

    /* State */
    time_t created;
    time_t last_activity;
    bool client_closed;
    bool backend_closed;

    /* Stats */
    uint64_t bytes_from_client;
    uint64_t bytes_from_backend;
} proxy_conn_t;

/* Load balancer handle */
typedef struct loadbalancer {
    lb_config_t config;
    backend_t backends[MAX_BACKENDS];
    int backend_count;

    int listen_fd;
    int epoll_fd;

    /* Connection tracking */
    proxy_conn_t *connections;
    int connection_count;

    /* Session table */
    session_t *sessions;
    int session_count;

    /* Load balancing state */
    int rr_current;  /* Round robin index */

    /* Health check thread */
    pthread_t health_thread;
    volatile bool running;
} loadbalancer_t;

/* API */
loadbalancer_t* lb_create(const lb_config_t *config);
void lb_destroy(loadbalancer_t *lb);
int lb_add_backend(loadbalancer_t *lb, const backend_t *backend);
int lb_remove_backend(loadbalancer_t *lb, const char *name);
int lb_start(loadbalancer_t *lb);
void lb_stop(loadbalancer_t *lb);

/* Load balancing */
backend_t* lb_select_backend(loadbalancer_t *lb, uint32_t client_ip);
int lb_update_backend_status(loadbalancer_t *lb, int idx, backend_status_t status);

/* Health checks */
void* lb_health_check_thread(void *arg);
int lb_check_backend_tcp(backend_t *backend);
int lb_check_backend_http(backend_t *backend, const char *expect);

/* Statistics */
typedef struct lb_stats {
    uint64_t total_connections;
    uint64_t active_connections;
    uint64_t total_bytes_sent;
    uint64_t total_bytes_received;
    double avg_response_time;
    backend_t backend_stats[MAX_BACKENDS];
} lb_stats_t;

void lb_get_stats(loadbalancer_t *lb, lb_stats_t *stats);
void lb_print_stats(loadbalancer_t *lb);

#endif /* PROXY_H */
