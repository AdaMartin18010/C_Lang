/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 336
 * Language: c
 * Block ID: ee926f30
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef CONNECTION_H
#define CONNECTION_H

#include "http_parser.h"
#include <time.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CONN_STATE_READING_REQUEST,
    CONN_STATE_PROCESSING,
    CONN_STATE_WRITING_RESPONSE,
    CONN_STATE_KEEP_ALIVE,
    CONN_STATE_CLOSING
} conn_state_t;

typedef struct connection {
    int fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len;

    /* Buffers */
    char read_buf[BUFFER_SIZE];
    size_t read_len;
    char write_buf[BUFFER_SIZE * 2];
    size_t write_len;
    size_t write_pos;

    /* HTTP state */
    http_request_t request;
    http_response_t response;
    conn_state_t state;

    /* Timing */
    time_t last_activity;
    int request_count;

    /* Statistics */
    uint64_t bytes_received;
    uint64_t bytes_sent;
} connection_t;

typedef struct connection_pool {
    connection_t *connections;
    int max_connections;
    int active_count;
    int *free_list;
    int free_count;
} connection_pool_t;

/* Connection management */
connection_pool_t* conn_pool_create(int max_connections);
void conn_pool_destroy(connection_pool_t *pool);
connection_t* conn_acquire(connection_pool_t *pool, int fd,
                           struct sockaddr_in *addr, socklen_t addr_len);
void conn_release(connection_pool_t *pool, connection_t *conn);
void conn_reset(connection_t *conn);

/* Connection operations */
int conn_read(connection_t *conn);
int conn_write(connection_t *conn);
bool conn_should_close(const connection_t *conn);
void conn_update_activity(connection_t *conn);

#endif /* CONNECTION_H */
