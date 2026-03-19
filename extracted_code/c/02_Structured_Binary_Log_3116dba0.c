/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\02_Structured_Binary_Log.md
 * Line: 212
 * Language: c
 * Block ID: 3116dba0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 序列化上下文 */
typedef struct {
    BufferWriter writer;

    /* 字符串去重字典 */
    struct {
        char **strings;
        uint32_t count;
        uint32_t capacity;
    } string_dict;
} LogSerializer;

/* 初始化序列化器 */
LogSerializer* log_serializer_create(size_t initial_capacity) {
    LogSerializer *s = calloc(1, sizeof(LogSerializer));
    s->writer.capacity = initial_capacity;
    s->writer.buf = malloc(initial_capacity);
    s->writer.pos = 0;

    s->string_dict.capacity = 256;
    s->string_dict.strings = malloc(256 * sizeof(char *));

    return s;
}

/* 序列化日志条目 */
int log_serialize(LogSerializer *s, const LogEntry *entry) {
    BufferWriter *w = &s->writer;

    /* 检查空间 */
    size_t required = sizeof(LogEntryHeader) +
                      sizeof(LogPayload) + entry->payload.message_len + 16;
    if (w->pos + required > w->capacity) {
        return -1;  /* 需要扩容或刷新 */
    }

    size_t start_pos = w->pos;

    /* 1. 写入头部（跳过payload_len，稍后回填） */
    encode_u32_le(w->buf + w->pos, LOG_MAGIC);
    w->pos += 4;
    w->buf[w->pos++] = LOG_VERSION & 0xFF;
    w->buf[w->pos++] = (LOG_VERSION >> 8) & 0xFF;
    w->buf[w->pos++] = entry->header.flags & 0xFF;
    w->buf[w->pos++] = (entry->header.flags >> 8) & 0xFF;
    w->pos += 4;  /* 预留payload_len */

    /* 2. 写入payload */
    size_t payload_start = w->pos;

    /* 时间戳 */
    memcpy(w->buf + w->pos, &entry->payload.timestamp_us, 8);
    w->pos += 8;

    /* 日志级别 */
    w->buf[w->pos++] = entry->payload.level;

    /* 线程ID (varint) */
    encode_varint(w, entry->payload.thread_id);

    /* 文件ID */
    encode_u32_le(w->buf + w->pos, entry->payload.file_id);
    w->pos += 4;

    /* 行号 */
    encode_u32_le(w->buf + w->pos, entry->payload.line);
    w->pos += 4;

    /* 消息长度和内容 */
    encode_varint(w, entry->payload.message_len);
    memcpy(w->buf + w->pos, entry->payload.message, entry->payload.message_len);
    w->pos += entry->payload.message_len;

    /* 3. 回填payload_len */
    size_t payload_len = w->pos - payload_start;
    encode_u32_le(w->buf + start_pos + 8, payload_len);

    return 0;
}

/* 批量序列化优化 */
int log_serialize_batch(LogSerializer *s, const LogEntry **entries,
                        uint32_t count) {
    for (uint32_t i = 0; i < count; i++) {
        if (log_serialize(s, entries[i]) != 0) {
            /* 刷新并重试 */
            log_serializer_flush(s);
            if (log_serialize(s, entries[i]) != 0) {
                return -1;  /* 单条太大 */
            }
        }
    }
    return 0;
}
