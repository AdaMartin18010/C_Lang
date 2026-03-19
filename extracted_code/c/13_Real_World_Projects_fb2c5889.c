/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 4964
 * Language: c
 * Block ID: fb2c5889
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Log Collection Daemon - High Performance Log Aggregation
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <stdatomic.h>

#include "logd.h"

/* Global stats */
static atomic_ulong g_received = 0;
static atomic_ulong g_written = 0;
static atomic_ulong g_dropped = 0;

ring_buffer_t* ring_buffer_create(size_t capacity) {
    ring_buffer_t *rb = calloc(1, sizeof(ring_buffer_t));
    if (!rb) return NULL;

    rb->entries = calloc(capacity, sizeof(log_entry_t));
    if (!rb->entries) {
        free(rb);
        return NULL;
    }

    rb->capacity = capacity;
    rb->head = 0;
    rb->tail = 0;
    pthread_mutex_init(&rb->lock, NULL);
    pthread_cond_init(&rb->not_empty, NULL);
    pthread_cond_init(&rb->not_full, NULL);

    return rb;
}

void ring_buffer_destroy(ring_buffer_t *rb) {
    if (!rb) return;

    for (size_t i = 0; i < rb->capacity; i++) {
        if (rb->entries[i].raw_data) {
            free(rb->entries[i].raw_data);
        }
    }

    free(rb->entries);
    pthread_mutex_destroy(&rb->lock);
    pthread_cond_destroy(&rb->not_empty);
    pthread_cond_destroy(&rb->not_full);
    free(rb);
}

int ring_buffer_push(ring_buffer_t *rb, const log_entry_t *entry) {
    pthread_mutex_lock(&rb->lock);

    while ((rb->head + 1) % rb->capacity == rb->tail) {
        pthread_cond_wait(&rb->not_full, &rb->lock);
    }

    memcpy(&rb->entries[rb->head], entry, sizeof(log_entry_t));
    if (entry->raw_data && entry->raw_len > 0) {
        rb->entries[rb->head].raw_data = malloc(entry->raw_len);
        memcpy(rb->entries[rb->head].raw_data, entry->raw_data, entry->raw_len);
    }

    rb->head = (rb->head + 1) % rb->capacity;

    pthread_cond_signal(&rb->not_empty);
    pthread_mutex_unlock(&rb->lock);

    return 0;
}

int ring_buffer_pop(ring_buffer_t *rb, log_entry_t *entry) {
    pthread_mutex_lock(&rb->lock);

    while (rb->head == rb->tail) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;

        if (pthread_cond_timedwait(&rb->not_empty, &rb->lock, &ts) == ETIMEDOUT) {
            pthread_mutex_unlock(&rb->lock);
            return -1;
        }
    }

    memcpy(entry, &rb->entries[rb->tail], sizeof(log_entry_t));
    memset(&rb->entries[rb->tail], 0, sizeof(log_entry_t));

    rb->tail = (rb->tail + 1) % rb->capacity;

    pthread_cond_signal(&rb->not_full);
    pthread_mutex_unlock(&rb->lock);

    return 0;
}

bool ring_buffer_is_empty(ring_buffer_t *rb) {
    pthread_mutex_lock(&rb->lock);
    bool empty = (rb->head == rb->tail);
    pthread_mutex_unlock(&rb->lock);
    return empty;
}

bool ring_buffer_is_full(ring_buffer_t *rb) {
    pthread_mutex_lock(&rb->lock);
    bool full = ((rb->head + 1) % rb->capacity == rb->tail);
    pthread_mutex_unlock(&rb->lock);
    return full;
}

/* Syslog parsing (RFC 5424 / RFC 3164) */
int parse_syslog(const char *data, size_t len, log_entry_t *entry) {
    memset(entry, 0, sizeof(log_entry_t));
    entry->timestamp = time(NULL);

    /* Simple RFC 3164 parsing */
    const char *p = data;
    const char *end = data + len;

    /* Skip PRI if present */
    if (*p == '<') {
        p++;
        int pri = 0;
        while (p < end && isdigit((unsigned char)*p)) {
            pri = pri * 10 + (*p - '0');
            p++;
        }
        if (p < end && *p == '>') p++;

        entry->level = (log_level_t)(pri & 0x07);
    }

    /* Parse timestamp (simplified) */
    if (p + 15 < end) {
        /* Mmm dd hh:mm:ss format */
        p += 15;
    }

    /* Parse hostname */
    while (p < end && isspace((unsigned char)*p)) p++;
    const char *host_start = p;
    while (p < end && !isspace((unsigned char)*p)) p++;
    size_t host_len = p - host_start;
    if (host_len < sizeof(entry->host)) {
        memcpy(entry->host, host_start, host_len);
        entry->host[host_len] = '\0';
    }

    /* Rest is message */
    while (p < end && isspace((unsigned char)*p)) p++;
    size_t msg_len = end - p;
    if (msg_len > sizeof(entry->message) - 1) {
        msg_len = sizeof(entry->message) - 1;
    }
    memcpy(entry->message, p, msg_len);
    entry->message[msg_len] = '\0';
    entry->message_len = msg_len;

    return 0;
}

/* GELF parsing (simplified JSON) */
int parse_gelf(const char *json, size_t len, log_entry_t *entry) {
    memset(entry, 0, sizeof(log_entry_t));
    entry->timestamp = time(NULL);

    /* Very simple JSON extraction */
    const char *version = strstr(json, "\"version\"");
    const char *host = strstr(json, "\"host\"");
    const char *short_msg = strstr(json, "\"short_message\"");
    const char *timestamp = strstr(json, "\"timestamp\"");
    const char *level = strstr(json, "\"level\"");

    if (short_msg) {
        const char *colon = strchr(short_msg, ':');
        if (colon) {
            const char *quote = strchr(colon + 1, '"');
            if (quote) {
                const char *end = strchr(quote + 1, '"');
                if (end) {
                    size_t msg_len = end - quote - 1;
                    if (msg_len > sizeof(entry->message) - 1) {
                        msg_len = sizeof(entry->message) - 1;
                    }
                    memcpy(entry->message, quote + 1, msg_len);
                    entry->message[msg_len] = '\0';
                }
            }
        }
    }

    if (host) {
        const char *colon = strchr(host, ':');
        if (colon) {
            const char *quote = strchr(colon + 1, '"');
            if (quote) {
                const char *end = strchr(quote + 1, '"');
                if (end && (end - quote - 1) < sizeof(entry->host)) {
                    memcpy(entry->host, quote + 1, end - quote - 1);
                }
            }
        }
    }

    if (level) {
        const char *colon = strchr(level, ':');
        if (colon) {
            entry->level = (log_level_t)atoi(colon + 1);
        }
    }

    return 0;
}

/* Syslog UDP handler */
void* syslog_udp_handler(void *arg) {
    logd_t *logd = arg;

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return NULL;
    }

    int reuse = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(514);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(fd);
        return NULL;
    }

    printf("Syslog UDP handler listening on port 514\n");

    char buffer[65536];
    while (logd->running) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);

        ssize_t n = recvfrom(fd, buffer, sizeof(buffer) - 1, 0,
                            (struct sockaddr *)&client, &client_len);
        if (n > 0) {
            buffer[n] = '\0';

            log_entry_t entry;
            if (parse_syslog(buffer, (size_t)n, &entry) == 0) {
                entry.id = atomic_fetch_add(&g_received, 1);

                if (ring_buffer_push(logd->queue, &entry) != 0) {
                    atomic_fetch_add(&g_dropped, 1);
                }
            }
        }
    }

    close(fd);
    return NULL;
}

/* Storage worker */
void* storage_worker(void *arg) {
    logd_t *logd = arg;
    log_entry_t entry;

    int batch_count = 0;
    log_entry_t batch[BATCH_SIZE];

    while (logd->running) {
        if (ring_buffer_pop(logd->queue, &entry) == 0) {
            memcpy(&batch[batch_count], &entry, sizeof(log_entry_t));
            batch_count++;

            if (batch_count >= BATCH_SIZE) {
                /* Write batch to storage */
                for (int i = 0; i < batch_count; i++) {
                    storage_write(logd->storage, &batch[i]);
                    if (batch[i].raw_data) {
                        free(batch[i].raw_data);
                    }
                }
                atomic_fetch_add(&g_written, batch_count);
                batch_count = 0;
            }
        } else {
            /* Timeout, flush partial batch */
            if (batch_count > 0) {
                for (int i = 0; i < batch_count; i++) {
                    storage_write(logd->storage, &batch[i]);
                    if (batch[i].raw_data) {
                        free(batch[i].raw_data);
                    }
                }
                atomic_fetch_add(&g_written, batch_count);
                batch_count = 0;
            }
        }
    }

    return NULL;
}

logd_t* logd_create(const char *data_dir, uint16_t port) {
    logd_t *logd = calloc(1, sizeof(logd_t));
    if (!logd) return NULL;

    logd->listen_port = port;
    logd->start_time = time(NULL);

    /* Create queue */
    logd->queue = ring_buffer_create(QUEUE_SIZE);
    if (!logd->queue) {
        free(logd);
        return NULL;
    }

    /* Create storage */
    logd->storage = storage_create(data_dir, COMPRESS_LZ4);
    if (!logd->storage) {
        ring_buffer_destroy(logd->queue);
        free(logd);
        return NULL;
    }

    /* Create worker threads */
    logd->worker_count = 4;
    logd->workers = calloc(logd->worker_count, sizeof(pthread_t));

    return logd;
}

void logd_destroy(logd_t *logd) {
    if (!logd) return;

    logd_stop(logd);

    ring_buffer_destroy(logd->queue);
    storage_destroy(logd->storage);
    free(logd->workers);
    free(logd);
}

int logd_start(logd_t *logd) {
    logd->running = true;

    /* Start input handlers */
    pthread_t syslog_thread;
    pthread_create(&syslog_thread, NULL, syslog_udp_handler, logd);

    /* Start storage workers */
    for (int i = 0; i < logd->worker_count; i++) {
        pthread_create(&logd->workers[i], NULL, storage_worker, logd);
    }

    printf("Logd started. Data directory: %s\n", logd->storage->data_dir);

    /* Wait for threads */
    pthread_join(syslog_thread, NULL);
    for (int i = 0; i < logd->worker_count; i++) {
        pthread_join(logd->workers[i], NULL);
    }

    return 0;
}

void logd_stop(logd_t *logd) {
    logd->running = false;
}

void logd_get_stats(logd_t *logd, logd_stats_t *stats) {
    stats->total_received = atomic_load(&g_received);
    stats->total_written = atomic_load(&g_written);
    stats->total_dropped = atomic_load(&g_dropped);
    stats->uptime = time(NULL) - logd->start_time;
    stats->queue_size = (logd->queue->head - logd->queue->tail + logd->queue->capacity)
                        % logd->queue->capacity;
    stats->queue_capacity = logd->queue->capacity;
}
