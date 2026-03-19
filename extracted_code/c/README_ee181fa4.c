/*
 * Auto-generated from: 03_System_Technology_Domains\09_High_Performance_Log\README.md
 * Line: 621
 * Language: c
 * Block ID: ee181fa4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * JSON 结构化日志实现
 *
 * 每条日志都是一个 JSON 对象，便于机器解析和分析
 */

#include <stdio.h>
#include <time.h>
#include <string.h>

typedef struct {
    const char *key;
    const char *value;
} LogField;

typedef struct {
    time_t timestamp;
    LogLevel level;
    const char *message;
    const char *service;
    const char *trace_id;
    LogField *fields;
    int field_count;
} StructuredLogEntry;

// 安全的 JSON 字符串转义
void json_escape(char *dest, size_t dest_size, const char *src)
{
    size_t j = 0;
    for (size_t i = 0; src[i] && j < dest_size - 1; i++) {
        switch (src[i]) {
            case '"':  if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = '"'; } break;
            case '\\': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = '\\'; } break;
            case '\n': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = 'n'; } break;
            case '\r': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = 'r'; } break;
            case '\t': if (j + 2 < dest_size) { dest[j++] = '\\'; dest[j++] = 't'; } break;
            default:   dest[j++] = src[i]; break;
        }
    }
    dest[j] = '\0';
}

// 格式化结构化日志
void format_structured_log(char *output, size_t size,
                            const StructuredLogEntry *entry)
{
    char timestamp[64];
    struct tm tm;
    localtime_r(&entry->timestamp, &tm);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%dT%H:%M:%S%z", &tm);

    char escaped_message[2048];
    json_escape(escaped_message, sizeof(escaped_message), entry->message);

    int pos = snprintf(output, size,
        "{"
        "\"timestamp\":\"%s\","
        "\"level\":\"%s\","
        "\"message\":\"%s\","
        "\"service\":\"%s\","
        "\"trace_id\":\"%s\"",
        timestamp,
        level_strings[entry->level],
        escaped_message,
        entry->service ? entry->service : "unknown",
        entry->trace_id ? entry->trace_id : ""
    );

    // 添加额外字段
    for (int i = 0; i < entry->field_count && pos < (int)size - 1; i++) {
        char escaped_value[1024];
        json_escape(escaped_value, sizeof(escaped_value),
                    entry->fields[i].value);
        pos += snprintf(output + pos, size - pos,
            ",\"%s\":\"%s\"",
            entry->fields[i].key, escaped_value);
    }

    if (pos < (int)size - 1) {
        strcat(output + pos, "}\n");
    }
}

// 使用示例
void log_user_login(const char *user_id, const char *ip_address,
                     bool success)
{
    StructuredLogEntry entry = {
        .timestamp = time(NULL),
        .level = LOG_LEVEL_INFO,
        .message = "User login attempt",
        .service = "auth-service",
        .trace_id = "abc-123-def",
    };

    LogField fields[] = {
        {"user_id", user_id},
        {"ip_address", ip_address},
        {"success", success ? "true" : "false"},
        {"attempt_count", "3"},
    };
    entry.fields = fields;
    entry.field_count = 4;

    char json_output[4096];
    format_structured_log(json_output, sizeof(json_output), &entry);

    // 写入日志
    async_log(logger, "%s", json_output);
}
