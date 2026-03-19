/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\01_PMDK_Basics.md
 * Line: 307
 * Language: c
 * Block ID: da243536
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 基于PM的环形日志 */
#define LOG_MAGIC 0x504D4C4F  /* "PMLO" */
#define LOG_ENTRY_SIZE 256

typedef struct {
    uint32_t magic;
    uint32_t version;
    uint64_t write_offset;  /* 写入位置 */
    uint64_t read_offset;   /* 读取位置 */
    uint64_t capacity;
    uint32_t entry_size;
    uint32_t flags;
} PMemLogHeader;

typedef struct {
    PMemPool *pool;
    PMemLogHeader *header;
    void *data_region;
    size_t data_size;
} PMemLog;

/* 初始化日志 */
PMemLog* pmem_log_open(PMemPool *pool) {
    PMemLog *log = calloc(1, sizeof(PMemLog));
    log->pool = pool;
    log->header = (PMemLogHeader *)pool->addr;

    if (log->header->magic != LOG_MAGIC) {
        /* 初始化新日志 */
        log->header->magic = LOG_MAGIC;
        log->header->version = 1;
        log->header->write_offset = 0;
        log->header->read_offset = 0;
        log->header->capacity = pool->mapped_len - sizeof(PMemLogHeader);
        log->header->entry_size = LOG_ENTRY_SIZE;
        log->header->flags = 0;

        /* 持久化头部 */
        pmem_persist(log->header, sizeof(PMemLogHeader));
    }

    log->data_region = (char *)pool->addr + sizeof(PMemLogHeader);
    log->data_size = log->header->capacity;

    return log;
}

/* 追加日志条目 */
bool pmem_log_append(PMemLog *log, const void *data, size_t len) {
    if (len > log->header->entry_size - sizeof(uint32_t)) {
        return false;  /* 数据太大 */
    }

    uint64_t offset = log->header->write_offset;
    void *entry = (char *)log->data_region + offset;

    /* 写入长度前缀 */
    *(uint32_t *)entry = (uint32_t)len;

    /* 复制数据（不持久化） */
    pmem_memcpy_nodrain((char *)entry + sizeof(uint32_t), data, len);

    /* 计算下一个位置 */
    uint64_t entry_total = sizeof(uint32_t) + len;
    entry_total = (entry_total + 63) & ~63;  /* 64字节对齐 */
    uint64_t next_offset = offset + entry_total;

    if (next_offset >= log->data_size) {
        next_offset = 0;  /* 回绕 */
    }

    /* 持久化写入的数据 */
    pmem_persist(entry, entry_total);

    /* 更新并持久化write_offset */
    log->header->write_offset = next_offset;
    pmem_persist(&log->header->write_offset, sizeof(uint64_t));

    return true;
}

/* 读取日志条目 */
bool pmem_log_read(PMemLog *log, void *buffer, size_t *len) {
    uint64_t read_off = log->header->read_offset;
    uint64_t write_off = log->header->write_offset;

    if (read_off == write_off) {
        return false;  /* 空 */
    }

    void *entry = (char *)log->data_region + read_off;
    uint32_t data_len = *(uint32_t *)entry;

    memcpy(buffer, (char *)entry + sizeof(uint32_t), data_len);
    *len = data_len;

    /* 更新read_offset */
    uint64_t entry_total = sizeof(uint32_t) + data_len;
    entry_total = (entry_total + 63) & ~63;
    log->header->read_offset = (read_off + entry_total) % log->data_size;

    return true;
}
