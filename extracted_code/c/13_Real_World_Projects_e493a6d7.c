/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 1399
 * Language: c
 * Block ID: e493a6d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Connection Pool and Connection Management
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <sys/socket.h>
#include "connection.h"
#include "logger.h"

extern logger_t *g_logger;

connection_pool_t* conn_pool_create(int max_connections) {
    connection_pool_t *pool = calloc(1, sizeof(connection_pool_t));
    if (!pool) return NULL;

    pool->connections = calloc(max_connections, sizeof(connection_t));
    pool->free_list = calloc(max_connections, sizeof(int));

    if (!pool->connections || !pool->free_list) {
        free(pool->connections);
        free(pool->free_list);
        free(pool);
        return NULL;
    }

    pool->max_connections = max_connections;
    pool->active_count = 0;
    pool->free_count = max_connections;

    /* Initialize free list (LIFO) */
    for (int i = 0; i < max_connections; i++) {
        pool->free_list[i] = max_connections - 1 - i;
    }

    return pool;
}

void conn_pool_destroy(connection_pool_t *pool) {
    if (!pool) return;

    /* Close any remaining connections */
    for (int i = 0; i < pool->max_connections; i++) {
        if (pool->connections[i].fd >= 0) {
            close(pool->connections[i].fd);
            http_request_free(&pool->connections[i].request);
            http_response_free(&pool->connections[i].response);
        }
    }

    free(pool->connections);
    free(pool->free_list);
    free(pool);
}

connection_t* conn_acquire(connection_pool_t *pool, int fd,
                           struct sockaddr_in *addr, socklen_t addr_len) {
    if (pool->free_count == 0) {
        return NULL;  /* Pool exhausted */
    }

    int idx = pool->free_list[--pool->free_count];
    connection_t *conn = &pool->connections[idx];

    memset(conn, 0, sizeof(connection_t));
    conn->fd = fd;
    memcpy(&conn->client_addr, addr, addr_len);
    conn->addr_len = addr_len;
    conn->state = CONN_STATE_READING_REQUEST;
    conn->last_activity = time(NULL);

    http_request_init(&conn->request);
    http_response_init(&conn->response);

    pool->active_count++;

    return conn;
}

void conn_release(connection_pool_t *pool, connection_t *conn) {
    if (!pool || !conn) return;

    /* Cleanup */
    http_request_free(&conn->request);
    http_response_free(&conn->response);

    /* Return to pool */
    int idx = (int)(conn - pool->connections);
    pool->free_list[pool->free_count++] = idx;
    pool->active_count--;
}

void conn_reset(connection_t *conn) {
    if (!conn) return;

    /* Clear buffers */
    conn->read_len = 0;
    conn->write_len = 0;
    conn->write_pos = 0;

    /* Reset HTTP state */
    http_request_free(&conn->request);
    http_response_free(&conn->response);
    http_request_init(&conn->request);
    http_response_init(&conn->response);

    conn->state = CONN_STATE_READING_REQUEST;
    conn->last_activity = time(NULL);
}

int conn_read(connection_t *conn) {
    if (conn->read_len >= sizeof(conn->read_buf)) {
        return -1;  /* Buffer full */
    }

    ssize_t n = recv(conn->fd,
                     conn->read_buf + conn->read_len,
                     sizeof(conn->read_buf) - conn->read_len - 1,
                     0);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;  /* No data available */
        }
        return -1;  /* Error */
    }

    if (n == 0) {
        return -1;  /* Connection closed */
    }

    conn->read_len += (size_t)n;
    conn->read_buf[conn->read_len] = '\0';
    conn->bytes_received += (uint64_t)n;
    conn->last_activity = time(NULL);

    return (int)n;
}

int conn_write(connection_t *conn) {
    if (conn->write_pos >= conn->write_len) {
        return 0;
    }

    ssize_t n = send(conn->fd,
                     conn->write_buf + conn->write_pos,
                     conn->write_len - conn->write_pos,
                     MSG_NOSIGNAL);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }

    conn->write_pos += (size_t)n;
    conn->bytes_sent += (uint64_t)n;
    conn->last_activity = time(NULL);

    return (int)n;
}

bool conn_should_close(const connection_t *conn) {
    time_t now = time(NULL);

    /* Check timeout */
    if (now - conn->last_activity > REQUEST_TIMEOUT) {
        return true;
    }

    /* Check max keep-alive requests */
    if (conn->request_count >= MAX_KEEP_ALIVE_REQUESTS) {
        return true;
    }

    return false;
}

void conn_update_activity(connection_t *conn) {
    conn->last_activity = time(NULL);
}
