/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\02_Structured_Binary_Log.md
 * Line: 311
 * Language: c
 * Block ID: e0524b76
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 日志读取器 - 支持mmap */
typedef struct {
    int fd;
    uint8_t *mmap_base;
    size_t mmap_size;
    size_t file_size;

    /* 当前读取位置 */
    size_t read_pos;

    /* 预读缓冲 */
    uint8_t *read_buf;
    size_t buf_size;
    size_t buf_pos;
    size_t buf_valid;
} LogReader;

/* 打开日志文件 */
LogReader* log_reader_open(const char *path) {
    LogReader *r = calloc(1, sizeof(LogReader));

    r->fd = open(path, O_RDONLY);
    if (r->fd < 0) goto error;

    /* 获取文件大小 */
    struct stat st;
    if (fstat(r->fd, &st) < 0) goto error;
    r->file_size = st.st_size;

    /* 尝试mmap */
    r->mmap_base = mmap(NULL, r->file_size, PROT_READ, MAP_PRIVATE, r->fd, 0);
    if (r->mmap_base != MAP_FAILED) {
        /* 使用mmap零拷贝读取 */
        r->read_buf = r->mmap_base;
        r->buf_size = r->file_size;
        r->buf_valid = r->file_size;
    } else {
        /* 回退到普通读取 */
        r->buf_size = 256 * 1024;  /* 256KB缓冲 */
        r->read_buf = malloc(r->buf_size);
        r->mmap_base = NULL;
    }

    return r;

error:
    free(r);
    return NULL;
}

/* 读取下一条日志 */
bool log_reader_next(LogReader *r, LogEntry *entry) {
    /* 确保有足够数据读取头部 */
    if (r->buf_pos + sizeof(LogEntryHeader) > r->buf_valid) {
        if (!reader_refill(r)) return false;
    }

    const uint8_t *p = r->read_buf + r->buf_pos;

    /* 验证魔数 */
    uint32_t magic = decode_u32_le(p);
    if (magic != LOG_MAGIC) {
        /* 尝试同步到下一个有效条目 */
        return reader_resync(r, entry);
    }

    /* 解析头部 */
    entry->header.magic = magic;
    entry->header.version = p[4] | (p[5] << 8);
    entry->header.flags = p[6] | (p[7] << 8);
    entry->header.payload_len = decode_u32_le(p + 8);

    size_t total_len = sizeof(LogEntryHeader) + entry->header.payload_len;

    /* 确保负载数据可用 */
    if (r->buf_pos + total_len > r->buf_valid) {
        if (!reader_refill(r)) return false;
    }

    /* 零拷贝：直接指向mmap内存 */
    p = r->read_buf + r->buf_pos + sizeof(LogEntryHeader);

    /* 解析payload */
    entry->payload.timestamp_us = *(uint64_t *)p;
    p += 8;

    entry->payload.level = *p++;

    size_t tmp_pos = 0;  /* 用于varint解码的临时位置 */
    decode_varint(p, &tmp_pos, entry->header.payload_len - 9,
                  &entry->payload.thread_id);
    p += tmp_pos;

    entry->payload.file_id = decode_u32_le(p);
    p += 4;

    entry->payload.line = decode_u32_le(p);
    p += 4;

    uint64_t msg_len;
    tmp_pos = 0;
    decode_varint(p, &tmp_pos, entry->header.payload_len - (p - (r->read_buf + r->buf_pos + sizeof(LogEntryHeader))), &msg_len);
    p += tmp_pos;

    entry->payload.message_len = msg_len;
    entry->payload.message = (char *)p;

    /* 推进读取位置 */
    r->buf_pos += total_len;
    r->read_pos += total_len;

    return true;
}
