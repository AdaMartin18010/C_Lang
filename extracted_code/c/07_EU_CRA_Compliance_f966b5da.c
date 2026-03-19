/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\07_EU_CRA_Compliance.md
 * Line: 2339
 * Language: c
 * Block ID: f966b5da
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * EU CRA合规安全日志实现
 */

#include <time.h>
#include <stdarg.h>

/* 日志级别 */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_CRITICAL,
    LOG_SECURITY,
} log_level_t;

/* 安全事件类型 */
typedef enum {
    SEC_EVENT_LOGIN_SUCCESS = 1000,
    SEC_EVENT_LOGIN_FAILURE,
    SEC_EVENT_LOGOUT,
    SEC_EVENT_PASSWORD_CHANGE,
    SEC_EVENT_AUTH_FAILURE,
    SEC_EVENT_AUTH_SUCCESS,
    SEC_EVENT_ACCESS_DENIED,
    SEC_EVENT_PRIVILEGE_ESCALATION,
    SEC_EVENT_CONFIG_CHANGE,
    SEC_EVENT_FIRMWARE_UPDATE,
    SEC_EVENT_VULNERABILITY_DETECTED,
    SEC_EVENT_SECURITY_VIOLATION,
} security_event_t;

/* 日志条目结构 */
typedef struct __attribute__((packed)) {
    uint64_t timestamp;         /* Unix时间戳 */
    uint32_t sequence;          /* 序列号 */
    log_level_t level;
    uint32_t event_id;
    char source[32];            /* 日志来源模块 */
    char message[256];
    uint8_t mac[32];            /* HMAC-SHA256 */
} secure_log_entry_t;

/* 安全日志上下文 */
typedef struct {
    FILE *log_file;
    uint8_t hmac_key[32];
    uint32_t sequence;
    secure_mutex_t lock;
} secure_logger_t;

static secure_logger_t g_logger;

/* 初始化安全日志 */
int secure_log_init(const char *log_path, const uint8_t *hmac_key) {
    g_logger.log_file = fopen(log_path, "a+");
    if (!g_logger.log_file) return -1;

    memcpy(g_logger.hmac_key, hmac_key, 32);
    g_logger.sequence = get_last_sequence();

    mutex_init(&g_logger.lock);

    /* 记录启动事件 */
    secure_log_write(LOG_INFO, 0, "SYSTEM", "Security logger initialized");

    return 0;
}

/* 写入安全日志 */
int secure_log_write(log_level_t level, uint32_t event_id,
                     const char *source, const char *format, ...) {
    secure_log_entry_t entry = {0};

    /* 填充条目 */
    entry.timestamp = time(NULL);
    entry.sequence = ++g_logger.sequence;
    entry.level = level;
    entry.event_id = event_id;
    strncpy(entry.source, source, sizeof(entry.source) - 1);

    /* 格式化消息 */
    va_list args;
    va_start(args, format);
    vsnprintf(entry.message, sizeof(entry.message), format, args);
    va_end(args);

    /* 计算MAC（序列号之前的所有字段）*/
    size_t mac_data_len = offsetof(secure_log_entry_t, mac);
    hmac_sha256(g_logger.hmac_key, 32,
                (uint8_t *)&entry, mac_data_len,
                entry.mac);

    /* 写入日志 */
    secure_lock(&g_logger.lock);

    /* 文本格式 */
    char timestamp_str[64];
    format_timestamp(entry.timestamp, timestamp_str, sizeof(timestamp_str));

    fprintf(g_logger.log_file,
        "%s | SEQ:%08X | LVL:%d | EVT:%04X | SRC:%s | MSG:%s | MAC:%s\n",
        timestamp_str,
        entry.sequence,
        entry.level,
        entry.event_id,
        entry.source,
        entry.message,
        hex_encode(entry.mac, 32));

    fflush(g_logger.log_file);

    secure_unlock(&g_logger.lock);

    return 0;
}

/* 日志完整性验证 */
int verify_log_integrity(const char *log_path, const uint8_t *hmac_key) {
    FILE *fp = fopen(log_path, "r");
    if (!fp) return -1;

    char line[1024];
    uint32_t last_seq = 0;
    int errors = 0;

    while (fgets(line, sizeof(line), fp)) {
        /* 解析日志条目 */
        secure_log_entry_t entry;
        if (parse_log_line(line, &entry) != 0) {
            errors++;
            continue;
        }

        /* 验证序列号 */
        if (entry.sequence != last_seq + 1) {
            printf("Sequence gap detected: %u -> %u\n", last_seq, entry.sequence);
            errors++;
        }
        last_seq = entry.sequence;

        /* 验证MAC */
        uint8_t computed_mac[32];
        size_t mac_data_len = offsetof(secure_log_entry_t, mac);
        hmac_sha256(hmac_key, 32,
                    (uint8_t *)&entry, mac_data_len,
                    computed_mac);

        if (memcmp(computed_mac, entry.mac, 32) != 0) {
            printf("MAC verification failed for entry %u\n", entry.sequence);
            errors++;
        }
    }

    fclose(fp);
    return errors;
}

/* 安全事件记录宏 */
#define LOG_SECURITY_EVENT(event, source, ...) \
    secure_log_write(LOG_SECURITY, event, source, __VA_ARGS__)

#define LOG_AUTH_SUCCESS(user) \
    LOG_SECURITY_EVENT(SEC_EVENT_LOGIN_SUCCESS, "AUTH", \
                       "User %s logged in successfully", user)

#define LOG_AUTH_FAILURE(user, reason) \
    LOG_SECURITY_EVENT(SEC_EVENT_LOGIN_FAILURE, "AUTH", \
                       "User %s login failed: %s", user, reason)

#define LOG_ACCESS_DENIED(user, resource) \
    LOG_SECURITY_EVENT(SEC_EVENT_ACCESS_DENIED, "AUTH", \
                       "User %s denied access to %s", user, resource)
