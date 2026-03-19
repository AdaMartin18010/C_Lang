/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 736
 * Language: c
 * Block ID: cedf92cc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 二进制结构化日志（更高效的存储和解析）
 */

#include <stdint.h>
#include <arpa/inet.h>

#define LOG_MAGIC 0x4C4F4701  // "LOG" + 版本 1

// 字段类型枚举
typedef enum {
    FIELD_TYPE_INT8 = 1,
    FIELD_TYPE_INT16,
    FIELD_TYPE_INT32,
    FIELD_TYPE_INT64,
    FIELD_TYPE_FLOAT,
    FIELD_TYPE_DOUBLE,
    FIELD_TYPE_STRING,
    FIELD_TYPE_BOOL,
    FIELD_TYPE_TIMESTAMP,
} FieldType;

// 二进制日志头部
typedef struct __attribute__((packed)) {
    uint32_t magic;           // 魔数
    uint32_t entry_length;    // 条目总长度
    uint64_t timestamp_ns;    // 纳秒时间戳
    uint8_t level;            // 日志级别
    uint16_t message_len;     // 消息长度
    uint16_t field_count;     // 字段数量
    // 后面跟着：消息字符串 + 字段数组
} BinaryLogHeader;

// 字段定义
typedef struct __attribute__((packed)) {
    uint8_t type;             // FieldType
    uint8_t name_len;         // 字段名长度
    uint16_t value_len;       // 值长度
    // 后面跟着：name（字符串）+ value
} BinaryLogField;

// 序列化结构化日志
size_t serialize_binary_log(uint8_t *buffer, size_t buffer_size,
                             const StructuredLogEntry *entry)
{
    if (buffer_size < sizeof(BinaryLogHeader)) return 0;

    BinaryLogHeader *header = (BinaryLogHeader *)buffer;
    header->magic = htonl(LOG_MAGIC);
    header->timestamp_ns = htobe64(entry->timestamp * 1000000000LL);
    header->level = entry->level;
    header->field_count = htons(entry->field_count);

    size_t pos = sizeof(BinaryLogHeader);

    // 写入消息
    header->message_len = htons(strlen(entry->message));
    memcpy(buffer + pos, entry->message, ntohs(header->message_len));
    pos += ntohs(header->message_len);

    // 写入字段
    for (int i = 0; i < entry->field_count; i++) {
        BinaryLogField field;
        field.type = FIELD_TYPE_STRING;  // 简化处理
        field.name_len = strlen(entry->fields[i].key);
        field.value_len = htons(strlen(entry->fields[i].value));

        memcpy(buffer + pos, &field, sizeof(field));
        pos += sizeof(field);

        memcpy(buffer + pos, entry->fields[i].key, field.name_len);
        pos += field.name_len;

        memcpy(buffer + pos, entry->fields[i].value,
               ntohs(field.value_len));
        pos += ntohs(field.value_len);
    }

    header->entry_length = htonl(pos);
    return pos;
}
