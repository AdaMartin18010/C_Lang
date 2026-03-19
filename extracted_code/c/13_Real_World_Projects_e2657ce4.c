/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 5366
 * Language: c
 * Block ID: e2657ce4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Storage Engine - Partitioned, Compressed Log Storage
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <time.h>
#include <zlib.h>

#include "logd.h"

#define PARTITION_FORMAT_VERSION 1
#define INDEX_ENTRY_SIZE 32

/* File format:
 * [Header][Index][Compressed Data]
 */
typedef struct partition_header {
    uint32_t magic;
    uint32_t version;
    uint32_t compression;
    uint64_t entry_count;
    uint64_t index_offset;
    uint64_t data_offset;
    time_t start_time;
    time_t end_time;
} partition_header_t;

typedef struct index_entry {
    uint64_t timestamp;
    uint64_t offset;
    uint32_t compressed_len;
    uint32_t uncompressed_len;
} index_entry_t;

storage_t* storage_create(const char *data_dir, compress_type_t compression) {
    storage_t *st = calloc(1, sizeof(storage_t));
    if (!st) return NULL;

    strncpy(st->data_dir, data_dir, sizeof(st->data_dir) - 1);
    st->default_compression = compression;
    st->max_partition_size = 100 * 1024 * 1024;  /* 100MB */
    st->retention_days = 30;
    pthread_mutex_init(&st->lock, NULL);

    /* Create data directory if needed */
    mkdir(data_dir, 0755);

    /* Create initial partition */
    storage_rotate_partition(st);

    return st;
}

void storage_destroy(storage_t *st) {
    if (!st) return;
    pthread_mutex_destroy(&st->lock);
    free(st->partitions);
    free(st);
}

static char* get_partition_path(storage_t *st, time_t t) {
    static char path[512];
    struct tm *tm = localtime(&t);
    snprintf(path, sizeof(path), "%s/%04d-%02d-%02d-%02d.logd",
             st->data_dir, tm->tm_year + 1900, tm->tm_mon + 1,
             tm->tm_mday, tm->tm_hour);
    return path;
}

int storage_rotate_partition(storage_t *st) {
    pthread_mutex_lock(&st->lock);

    time_t now = time(NULL);
    char *path = get_partition_path(st, now);

    /* Add to partition list */
    st->partitions = realloc(st->partitions,
                            (st->partition_count + 1) * sizeof(partition_t));

    partition_t *p = &st->partitions[st->partition_count];
    memset(p, 0, sizeof(partition_t));
    p->id = st->partition_count;
    p->start_time = now;
    p->compression = st->default_compression;
    strncpy(p->path, path, sizeof(p->path) - 1);
    p->is_active = true;

    st->active_partition = st->partition_count;
    st->partition_count++;

    pthread_mutex_unlock(&st->lock);

    return 0;
}

static int compress_data(const uint8_t *src, size_t src_len,
                         uint8_t **dst, size_t *dst_len,
                         compress_type_t type) {
    if (type == COMPRESS_NONE) {
        *dst = malloc(src_len);
        memcpy(*dst, src, src_len);
        *dst_len = src_len;
        return 0;
    }

    /* For simplicity, use zlib if LZ4/Zstd not available */
    uLongf compressed_size = compressBound((uLong)src_len);
    *dst = malloc(compressed_size);

    if (compress(*dst, &compressed_size, src, (uLong)src_len) != Z_OK) {
        free(*dst);
        return -1;
    }

    *dst_len = compressed_size;
    return 0;
}

int storage_write(storage_t *st, const log_entry_t *entry) {
    pthread_mutex_lock(&st->lock);

    partition_t *p = &st->partitions[st->active_partition];

    /* Serialize entry to binary format */
    uint8_t buffer[65536];
    size_t offset = 0;

    /* Header */
    memcpy(buffer + offset, &entry->timestamp, sizeof(time_t));
    offset += sizeof(time_t);
    memcpy(buffer + offset, &entry->level, sizeof(log_level_t));
    offset += sizeof(log_level_t);

    /* Host */
    uint16_t host_len = (uint16_t)strlen(entry->host);
    memcpy(buffer + offset, &host_len, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    memcpy(buffer + offset, entry->host, host_len);
    offset += host_len;

    /* Message */
    uint32_t msg_len = (uint32_t)entry->message_len;
    memcpy(buffer + offset, &msg_len, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    memcpy(buffer + offset, entry->message, msg_len);
    offset += msg_len;

    /* Compress */
    uint8_t *compressed = NULL;
    size_t compressed_len = 0;

    if (compress_data(buffer, offset, &compressed, &compressed_len,
                      p->compression) != 0) {
        pthread_mutex_unlock(&st->lock);
        return -1;
    }

    /* Write to file */
    int fd = open(p->path, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd < 0) {
        free(compressed);
        pthread_mutex_unlock(&st->lock);
        return -1;
    }

    /* Simple append format for now */
    write(fd, &entry->timestamp, sizeof(time_t));
    write(fd, &compressed_len, sizeof(uint32_t));
    write(fd, compressed, compressed_len);

    close(fd);

    p->entry_count++;
    p->compressed_size += compressed_len;
    p->uncompressed_size += offset;

    free(compressed);

    /* Rotate if too large */
    if (p->compressed_size > st->max_partition_size) {
        pthread_mutex_unlock(&st->lock);
        storage_rotate_partition(st);
    } else {
        pthread_mutex_unlock(&st->lock);
    }

    return 0;
}

int storage_cleanup_old(storage_t *st) {
    time_t cutoff = time(NULL) - (st->retention_days * 24 * 3600);

    pthread_mutex_lock(&st->lock);

    for (int i = 0; i < st->partition_count; i++) {
        partition_t *p = &st->partitions[i];
        if (!p->is_active && p->end_time < cutoff) {
            unlink(p->path);
            p->entry_count = 0;
        }
    }

    pthread_mutex_unlock(&st->lock);

    return 0;
}
