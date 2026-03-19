/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 4758
 * Language: c
 * Block ID: b2256c71
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef LOGD_H
#define LOGD_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define LOGD_VERSION "1.0.0"
#define MAX_LOG_SIZE (1024 * 1024)  /* 1MB per log entry */
#define BATCH_SIZE 1000
#define QUEUE_SIZE 100000
#define MAX_TAGS 32
#define MAX_FIELDS 64
#define DEFAULT_PORT 5140

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO = 1,
    LOG_WARN = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
} log_level_t;

typedef enum {
    INPUT_SYSLOG_UDP = 0,
    INPUT_SYSLOG_TCP = 1,
    INPUT_GELF_HTTP = 2,
    INPUT_GELF_TCP = 3,
    INPUT_FLUENTD = 4,
    INPUT_FILE = 5
} input_type_t;

typedef enum {
    COMPRESS_NONE = 0,
    COMPRESS_LZ4 = 1,
    COMPRESS_ZSTD = 2,
    COMPRESS_SNAPPY = 3
} compress_type_t;

/* Log entry structure */
typedef struct log_entry {
    uint64_t id;
    time_t timestamp;
    log_level_t level;
    char host[256];
    char app_name[128];
    char message[MAX_LOG_SIZE];
    size_t message_len;

    /* Structured fields */
    struct {
        char key[64];
        char value[512];
    } fields[MAX_FIELDS];
    int field_count;

    /* Tags */
    char tags[MAX_TAGS][64];
    int tag_count;

    /* Raw data for re-serialization */
    uint8_t *raw_data;
    size_t raw_len;
} log_entry_t;

/* Ring buffer for lock-free queue */
typedef struct ring_buffer {
    log_entry_t *entries;
    size_t capacity;
    size_t head;  /* Write position */
    size_t tail;  /* Read position */
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} ring_buffer_t;

/* Storage partition */
typedef struct partition {
    uint64_t id;
    time_t start_time;
    time_t end_time;
    char path[512];
    uint64_t entry_count;
    uint64_t compressed_size;
    uint64_t uncompressed_size;
    bool is_active;
    compress_type_t compression;
} partition_t;

/* Storage engine */
typedef struct storage {
    char data_dir[512];
    partition_t *partitions;
    int partition_count;
    int active_partition;
    compress_type_t default_compression;
    uint64_t max_partition_size;
    int retention_days;
    pthread_mutex_t lock;
} storage_t;

/* Query structure */
typedef struct log_query {
    time_t start_time;
    time_t end_time;
    log_level_t min_level;
    char host_pattern[256];
    char app_pattern[128];
    char message_pattern[256];
    char tags[MAX_TAGS][64];
    int tag_count;
    uint64_t limit;
    uint64_t offset;
} log_query_t;

/* Log daemon */
typedef struct logd {
    /* Inputs */
    int syslog_udp_fd;
    int syslog_tcp_fd;
    int gelf_http_fd;
    int fluentd_fd;
    uint16_t listen_port;

    /* Queue */
    ring_buffer_t *queue;

    /* Storage */
    storage_t *storage;

    /* Workers */
    pthread_t *workers;
    int worker_count;

    /* State */
    volatile bool running;
    uint64_t logs_received;
    uint64_t logs_written;
    uint64_t logs_dropped;
    time_t start_time;
} logd_t;

/* API */
logd_t* logd_create(const char *data_dir, uint16_t port);
void logd_destroy(logd_t *logd);
int logd_start(logd_t *logd);
void logd_stop(logd_t *logd);

/* Input handlers */
void* syslog_udp_handler(void *arg);
void* syslog_tcp_handler(void *arg);
void* gelf_http_handler(void *arg);
void* storage_worker(void *arg);

/* Parsing */
int parse_syslog(const char *data, size_t len, log_entry_t *entry);
int parse_gelf(const char *json, size_t len, log_entry_t *entry);
int parse_fluentd(const uint8_t *data, size_t len, log_entry_t *entry);

/* Storage */
storage_t* storage_create(const char *data_dir, compress_type_t compression);
void storage_destroy(storage_t *st);
int storage_write(storage_t *st, const log_entry_t *entry);
int storage_query(storage_t *st, const log_query_t *query,
                  log_entry_t **results, size_t *count);
int storage_rotate_partition(storage_t *st);
int storage_cleanup_old(storage_t *st);

/* Compression */
int compress_lz4(const uint8_t *src, size_t src_len,
                 uint8_t *dst, size_t dst_capacity, size_t *dst_len);
int decompress_lz4(const uint8_t *src, size_t src_len,
                   uint8_t *dst, size_t dst_capacity, size_t *dst_len);
int compress_zstd(const uint8_t *src, size_t src_len,
                  uint8_t *dst, size_t dst_capacity, size_t *dst_len);
int decompress_zstd(const uint8_t *src, size_t src_len,
                    uint8_t *dst, size_t dst_capacity, size_t *dst_len);

/* Ring buffer */
ring_buffer_t* ring_buffer_create(size_t capacity);
void ring_buffer_destroy(ring_buffer_t *rb);
int ring_buffer_push(ring_buffer_t *rb, const log_entry_t *entry);
int ring_buffer_pop(ring_buffer_t *rb, log_entry_t *entry);
bool ring_buffer_is_empty(ring_buffer_t *rb);
bool ring_buffer_is_full(ring_buffer_t *rb);

/* Stats */
typedef struct logd_stats {
    uint64_t total_received;
    uint64_t total_written;
    uint64_t total_dropped;
    uint64_t queue_size;
    uint64_t queue_capacity;
    uint64_t storage_size;
    double compression_ratio;
    time_t uptime;
} logd_stats_t;

void logd_get_stats(logd_t *logd, logd_stats_t *stats);

#endif /* LOGD_H */
