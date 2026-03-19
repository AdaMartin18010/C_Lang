/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\02_Structured_Binary_Log.md
 * Line: 161
 * Language: c
 * Block ID: 82c88be8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 日志条目头 - 固定12字节 */
typedef struct __attribute__((packed)) {
    uint32_t magic;          /* 魔数 'BLOG' = 0x424C4F47 */
    uint16_t version;        /* 格式版本 */
    uint16_t flags;          /* 压缩标志等 */
    uint32_t payload_len;    /* 负载长度 */
} LogEntryHeader;

/* 日志负载 - 结构化数据 */
typedef struct __attribute__((packed)) {
    /* 时间戳 - 微秒级UTC */
    uint64_t timestamp_us;

    /* 日志级别 - 1字节 */
    uint8_t level;

    /* 线程ID - varint编码 */
    uint64_t thread_id;

    /* 源文件信息 */
    uint32_t file_id;        /* 文件名字典索引 */
    uint32_t line;           /* 行号 */

    /* 消息内容 - 长度前缀字节数组 */
    uint32_t message_len;
    char     message[0];     /* 变长 */
} LogPayload;

/* 完整日志条目 */
typedef struct {
    LogEntryHeader header;
    LogPayload payload;
} LogEntry;

/* 魔数定义 */
#define LOG_MAGIC      0x424C4F47  /* "BLOG" */
#define LOG_VERSION    1

/* 标志位 */
#define LOG_FLAG_NONE       0x0000
#define LOG_FLAG_COMPRESSED 0x0001
#define LOG_FLAG_ENCRYPTED  0x0002
