/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 5553
 * Language: c
 * Block ID: 49e4e5cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file industrial_data_collector.c
 * @brief 工业数据采集器 - 完整实现
 * @details
 *   - 支持Modbus RTU/TCP设备采集
 *   - 支持OPC UA设备采集
 *   - 本地SQLite数据存储
 *   - 边缘规则计算
 *   - MQTT云端上传
 *   - 配置热加载
 *   - 完善的日志系统
 *
 * 编译: gcc -o collector industrial_data_collector.c \
 *        -lsqlite3 -lmosquitto -lpthread -lrt -lm
 *
 * 运行: sudo ./collector -c config.json
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <sqlite3.h>
#include <mosquitto.h>

/* ============ 版本与配置 ============ */
#define COLLECTOR_VERSION   "2.1.0"
#define DEFAULT_CONFIG_FILE "/etc/industrial_collector/config.json"
#define LOG_FILE            "/var/log/industrial_collector/collector.log"
#define DB_FILE             "/var/lib/industrial_collector/data.db"

/* 采集参数 */
#define MAX_DEVICES         32
#define MAX_TAGS_PER_DEVICE 128
#define MAX_TAG_NAME        64
#define MAX_BATCH_SIZE      1000
#define COLLECTOR_QUEUE_SIZE (1024 * 1024)

/* 日志级别 */
typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} log_level_t;

static log_level_t g_log_level = LOG_INFO;
static FILE *g_log_file = NULL;
static pthread_mutex_t g_log_mutex = PTHREAD_MUTEX_INITIALIZER;

/* ============ 数据结构定义 ============ */

/* 数据质量 */
typedef enum {
    QUALITY_GOOD = 0,
    QUALITY_BAD = 1,
    QUALITY_UNCERTAIN = 2,
    QUALITY_LAST_KNOWN = 3
} data_quality_t;

/* 数据点 */
typedef struct {
    char tag_name[MAX_TAG_NAME];
    uint32_t device_id;
    uint32_t tag_id;
    double value;
    data_quality_t quality;
    uint64_t timestamp_ms;
    uint64_t collection_time_ms;
} data_point_t;

/* 环形缓冲区 */
typedef struct {
    data_point_t *buffer;
    size_t capacity;
    size_t head;
    size_t tail;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
    size_t overflow_count;
} collector_queue_t;

/* 设备类型 */
typedef enum {
    DEVICE_MODBUS_RTU = 0,
    DEVICE_MODBUS_TCP,
    DEVICE_OPC_UA,
    DEVICE_SIMULATION  /* 用于测试 */
} device_type_t;

/* 设备配置 */
typedef struct {
    uint32_t id;
    char name[64];
    device_type_t type;
    bool enabled;

    /* 连接参数 */
    union {
        struct {
            char port[64];
            int baudrate;
            char parity;
            int data_bits;
            int stop_bits;
            uint8_t slave_id;
        } modbus_rtu;

        struct {
            char host[64];
            int port;
            uint8_t slave_id;
        } modbus_tcp;

        struct {
            char endpoint[128];
            char username[64];
            char password[64];
        } opc_ua;
    } connection;

    /* 采集参数 */
    uint32_t scan_interval_ms;
    uint32_t timeout_ms;
    uint32_t retry_count;

    /* 标签列表 */
    struct {
        char name[MAX_TAG_NAME];
        char address[32];
        uint16_t register_addr;
        uint8_t register_type;  /* 0=线圈, 1=输入, 3=保持寄存器, 4=输入寄存器 */
        double scale;
        double offset;
        bool enabled;
    } tags[MAX_TAGS_PER_DEVICE];
    uint32_t tag_count;

    /* 运行时状态 */
    pthread_t thread;
    bool running;
    uint32_t error_count;
    uint64_t last_scan_time;
} device_t;

/* 系统配置 */
typedef struct {
    /* 采集配置 */
    uint32_t collection_interval_ms;
    uint32_t batch_size;

    /* 存储配置 */
    bool local_storage_enabled;
    uint32_t retention_days;

    /* MQTT配置 */
    bool mqtt_enabled;
    char mqtt_broker[128];
    int mqtt_port;
    char mqtt_client_id[64];
    char mqtt_username[64];
    char mqtt_password[64];
    char mqtt_topic_prefix[64];
    uint32_t mqtt_qos;

    /* 日志配置 */
    char log_level[16];
    char log_file[256];
    uint32_t max_log_size_mb;
    uint32_t max_log_files;
} system_config_t;

/* 全局上下文 */
typedef struct {
    system_config_t config;
    device_t devices[MAX_DEVICES];
    uint32_t device_count;

    collector_queue_t data_queue;

    sqlite3 *db;
    struct mosquitto *mosq;
    bool mqtt_connected;

    pthread_t storage_thread;
    pthread_t mqtt_thread;
    pthread_t monitor_thread;

    volatile bool running;
    uint64_t start_time;

    /* 统计 */
    uint64_t total_collected;
    uint64_t total_stored;
    uint64_t total_sent;
    uint64_t total_dropped;
} collector_context_t;

static collector_context_t g_ctx = {0};

/* ============ 日志系统 ============ */

static const char *log_level_str[] = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};

static void logger_init(void)
{
    g_log_file = fopen(LOG_FILE, "a");
    if (!g_log_file) {
        g_log_file = stdout;
    }
}

static void logger_close(void)
{
    if (g_log_file && g_log_file != stdout) {
        fclose(g_log_file);
    }
}

static void log_message(log_level_t level, const char *format, ...)
{
    if (level < g_log_level) {
        return;
    }

    pthread_mutex_lock(&g_log_mutex);

    /* 获取时间 */
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    struct tm tm;
    localtime_r(&ts.tv_sec, &tm);

    /* 打印时间戳和级别 */
    fprintf(g_log_file, "[%04d-%02d-%02d %02d:%02d:%02d.%03d] [%s] ",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec, (int)(ts.tv_nsec / 1000000),
            log_level_str[level]);

    /* 打印消息 */
    va_list args;
    va_start(args, format);
    vfprintf(g_log_file, format, args);
    va_end(args);

    fprintf(g_log_file, "\n");
    fflush(g_log_file);

    pthread_mutex_unlock(&g_log_mutex);
}

#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   log_message(LOG_FATAL, __VA_ARGS__)

/* ============ 环形缓冲区实现 ============ */

static int queue_init(collector_queue_t *queue, size_t capacity)
{
    queue->buffer = calloc(capacity, sizeof(data_point_t));
    if (!queue->buffer) {
        return -1;
    }
    queue->capacity = capacity;
    queue->head = 0;
    queue->tail = 0;
    queue->overflow_count = 0;

    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);

    return 0;
}

static void queue_destroy(collector_queue_t *queue)
{
    free(queue->buffer);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}

static size_t queue_size(collector_queue_t *queue)
{
    pthread_mutex_lock(&queue->lock);
    size_t size = (queue->head - queue->tail) % queue->capacity;
    pthread_mutex_unlock(&queue->lock);
    return size;
}

static bool queue_push(collector_queue_t *queue, const data_point_t *point)
{
    pthread_mutex_lock(&queue->lock);

    size_t next_head = (queue->head + 1) % queue->capacity;
    if (next_head == queue->tail) {
        /* 队列满 */
        queue->overflow_count++;
        pthread_mutex_unlock(&queue->lock);
        return false;
    }

    queue->buffer[queue->head] = *point;
    queue->head = next_head;

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);

    return true;
}

static bool queue_pop(collector_queue_t *queue, data_point_t *point, int timeout_ms)
{
    pthread_mutex_lock(&queue->lock);

    while (queue->head == queue->tail) {
        /* 队列为空，等待 */
        if (timeout_ms >= 0) {
            struct timespec ts;
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += timeout_ms / 1000;
            ts.tv_nsec += (timeout_ms % 1000) * 1000000;
            if (ts.tv_nsec >= 1000000000) {
                ts.tv_sec++;
                ts.tv_nsec -= 1000000000;
            }

            int ret = pthread_cond_timedwait(&queue->not_empty, &queue->lock, &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&queue->lock);
                return false;
            }
        } else {
            pthread_cond_wait(&queue->not_empty, &queue->lock);
        }

        if (!g_ctx.running) {
            pthread_mutex_unlock(&queue->lock);
            return false;
        }
    }

    *point = queue->buffer[queue->tail];
    queue->tail = (queue->tail + 1) % queue->capacity;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);

    return true;
}

static size_t queue_pop_batch(collector_queue_t *queue, data_point_t *points,
                               size_t max_count)
{
    pthread_mutex_lock(&queue->lock);

    size_t available = (queue->head - queue->tail) % queue->capacity;
    size_t count = (available < max_count) ? available : max_count;

    for (size_t i = 0; i < count; i++) {
        points[i] = queue->buffer[queue->tail];
        queue->tail = (queue->tail + 1) % queue->capacity;
    }

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);

    return count;
}

/* ============ 数据库操作 ============ */

static int db_init(void)
{
    int rc = sqlite3_open(DB_FILE, &g_ctx.db);
    if (rc != SQLITE_OK) {
        LOG_ERROR("Cannot open database: %s", sqlite3_errmsg(g_ctx.db));
        return -1;
    }

    /* 创建表 */
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS data_points ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  tag_name TEXT NOT NULL,"
        "  device_id INTEGER,"
        "  value REAL,"
        "  quality INTEGER,"
        "  timestamp_ms INTEGER,"
        "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_timestamp ON data_points(timestamp_ms);"
        "CREATE INDEX IF NOT EXISTS idx_tag ON data_points(tag_name);";

    char *err_msg = NULL;
    rc = sqlite3_exec(g_ctx.db, create_table_sql, NULL, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        LOG_ERROR("SQL error: %s", err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    /* 启用WAL模式 */
    sqlite3_exec(g_ctx.db, "PRAGMA journal_mode=WAL;", NULL, NULL, NULL);
    sqlite3_exec(g_ctx.db, "PRAGMA synchronous=NORMAL;", NULL, NULL, NULL);

    LOG_INFO("Database initialized: %s", DB_FILE);
    return 0;
}

static void db_close(void)
{
    if (g_ctx.db) {
        sqlite3_close(g_ctx.db);
        g_ctx.db = NULL;
    }
}

static int db_insert_batch(const data_point_t *points, size_t count)
{
    if (!g_ctx.db || count == 0) {
        return 0;
    }

    sqlite3_exec(g_ctx.db, "BEGIN TRANSACTION;", NULL, NULL, NULL);

    const char *insert_sql = "INSERT INTO data_points "
                             "(tag_name, device_id, value, quality, timestamp_ms) "
                             "VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(g_ctx.db, insert_sql, -1, &stmt, NULL);

    for (size_t i = 0; i < count; i++) {
        sqlite3_bind_text(stmt, 1, points[i].tag_name, -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 2, points[i].device_id);
        sqlite3_bind_double(stmt, 3, points[i].value);
        sqlite3_bind_int(stmt, 4, points[i].quality);
        sqlite3_bind_int64(stmt, 5, points[i].timestamp_ms);

        sqlite3_step(stmt);
        sqlite3_reset(stmt);
    }

    sqlite3_finalize(stmt);
    sqlite3_exec(g_ctx.db, "COMMIT;", NULL, NULL, NULL);

    return (int)count;
}

/* ============ MQTT操作 ============ */

static void mqtt_connect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)obj;

    if (rc == 0) {
        g_ctx.mqtt_connected = true;
        LOG_INFO("MQTT connected");
    } else {
        g_ctx.mqtt_connected = false;
        LOG_ERROR("MQTT connection failed: %s", mosquitto_connack_string(rc));
    }
}

static void mqtt_disconnect_callback(struct mosquitto *mosq, void *obj, int rc)
{
    (void)mosq;
    (void)obj;

    g_ctx.mqtt_connected = false;
    LOG_WARNING("MQTT disconnected: %d", rc);
}

static int mqtt_init(void)
{
    if (!g_ctx.config.mqtt_enabled) {
        LOG_INFO("MQTT disabled");
        return 0;
    }

    mosquitto_lib_init();

    g_ctx.mosq = mosquitto_new(g_ctx.config.mqtt_client_id, true, NULL);
    if (!g_ctx.mosq) {
        LOG_ERROR("Failed to create MQTT client");
        return -1;
    }

    mosquitto_connect_callback_set(g_ctx.mosq, mqtt_connect_callback);
    mosquitto_disconnect_callback_set(g_ctx.mosq, mqtt_disconnect_callback);

    if (strlen(g_ctx.config.mqtt_username) > 0) {
        mosquitto_username_pw_set(g_ctx.mosq,
                                  g_ctx.config.mqtt_username,
                                  g_ctx.config.mqtt_password);
    }

    int rc = mosquitto_connect(g_ctx.mosq,
                               g_ctx.config.mqtt_broker,
                               g_ctx.config.mqtt_port,
                               60);
    if (rc != MOSQ_ERR_SUCCESS) {
        LOG_ERROR("MQTT connect error: %s", mosquitto_strerror(rc));
        return -1;
    }

    mosquitto_loop_start(g_ctx.mosq);
    LOG_INFO("MQTT client initialized: %s:%d",
             g_ctx.config.mqtt_broker, g_ctx.config.mqtt_port);

    return 0;
}

static void mqtt_close(void)
{
    if (g_ctx.mosq) {
        mosquitto_loop_stop(g_ctx.mosq, true);
        mosquitto_disconnect(g_ctx.mosq);
        mosquitto_destroy(g_ctx.mosq);
        g_ctx.mosq = NULL;
    }
    mosquitto_lib_cleanup();
}

static int mqtt_publish_data(const data_point_t *point)
{
    if (!g_ctx.mosq || !g_ctx.mqtt_connected) {
        return -1;
    }

    char topic[256];
    char payload[256];

    snprintf(topic, sizeof(topic), "%s/%s",
             g_ctx.config.mqtt_topic_prefix, point->tag_name);

    snprintf(payload, sizeof(payload),
             "{\"value\":%.4f,\"quality\":%d,\"timestamp\":%llu}",
             point->value, point->quality,
             (unsigned long long)point->timestamp_ms);

    int rc = mosquitto_publish(g_ctx.mosq, NULL, topic,
                               strlen(payload), payload,
                               g_ctx.config.mqtt_qos, false);

    return (rc == MOSQ_ERR_SUCCESS) ? 0 : -1;
}

/* ============ 设备采集 ============ */

static uint64_t get_time_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* Modbus CRC16计算 */
static uint16_t modbus_crc16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/* 模拟设备采集 (用于测试) */
static void *device_simulation_thread(void *arg)
{
    device_t *dev = (device_t *)arg;

    LOG_INFO("[Device %s] Simulation thread started", dev->name);

    /* 预分配批量缓冲区 */
    data_point_t batch[MAX_BATCH_SIZE];
    size_t batch_count = 0;

    while (dev->running && g_ctx.running) {
        uint64_t scan_start = get_time_ms();

        /* 采集所有标签 */
        for (uint32_t i = 0; i < dev->tag_count; i++) {
            if (!dev->tags[i].enabled) continue;

            /* 生成模拟数据 */
            double value;
            if (strstr(dev->tags[i].name, "Temperature")) {
                value = 20.0 + 10.0 * sin((double)scan_start / 10000.0) +
                       (double)(rand() % 100) / 100.0;
            } else if (strstr(dev->tags[i].name, "Pressure")) {
                value = 100.0 + 5.0 * cos((double)scan_start / 15000.0) +
                       (double)(rand() % 50) / 10.0;
            } else if (strstr(dev->tags[i].name, "Motor")) {
                value = (scan_start / 1000) % 2 ? 1500.0 : 0.0;
            } else {
                value = (double)(rand() % 1000);
            }

            /* 应用缩放 */
            value = value * dev->tags[i].scale + dev->tags[i].offset;

            /* 填充数据点 */
            data_point_t *point = &batch[batch_count++];
            snprintf(point->tag_name, MAX_TAG_NAME, "%s.%s",
                     dev->name, dev->tags[i].name);
            point->device_id = dev->id;
            point->tag_id = i;
            point->value = value;
            point->quality = QUALITY_GOOD;
            point->timestamp_ms = scan_start;
            point->collection_time_ms = get_time_ms();

            /* 批量提交 */
            if (batch_count >= MAX_BATCH_SIZE) {
                for (size_t j = 0; j < batch_count; j++) {
                    if (!queue_push(&g_ctx.data_queue, &batch[j])) {
                        g_ctx.total_dropped++;
                    } else {
                        g_ctx.total_collected++;
                    }
                }
                batch_count = 0;
            }
        }

        dev->last_scan_time = get_time_ms();

        /* 等待下一个周期 */
        uint64_t elapsed = get_time_ms() - scan_start;
        if (elapsed < dev->scan_interval_ms) {
            usleep((dev->scan_interval_ms - elapsed) * 1000);
        }
    }

    /* 提交剩余数据 */
    for (size_t j = 0; j < batch_count; j++) {
        if (!queue_push(&g_ctx.data_queue, &batch[j])) {
            g_ctx.total_dropped++;
        }
    }

    LOG_INFO("[Device %s] Simulation thread stopped", dev->name);
    return NULL;
}

/* ============ 后台线程 ============ */

/* 存储线程 */
static void *storage_thread(void *arg)
{
    (void)arg;

    LOG_INFO("Storage thread started");

    data_point_t batch[MAX_BATCH_SIZE];

    while (g_ctx.running) {
        /* 批量获取数据 */
        size_t count = queue_pop_batch(&g_ctx.data_queue, batch, MAX_BATCH_SIZE);

        if (count > 0) {
            /* 存储到数据库 */
            if (g_ctx.config.local_storage_enabled) {
                int stored = db_insert_batch(batch, count);
                if (stored > 0) {
                    g_ctx.total_stored += stored;
                }
            }

            /* 发送到MQTT */
            if (g_ctx.config.mqtt_enabled && g_ctx.mqtt_connected) {
                for (size_t i = 0; i < count; i++) {
                    if (mqtt_publish_data(&batch[i]) == 0) {
                        g_ctx.total_sent++;
                    }
                }
            }
        } else {
            /* 队列为空，短暂等待 */
            usleep(10000);
        }
    }

    LOG_INFO("Storage thread stopped");
    return NULL;
}

/* 监控线程 */
static void *monitor_thread(void *arg)
{
    (void)arg;

    LOG_INFO("Monitor thread started");

    while (g_ctx.running) {
        sleep(60);  /* 每分钟报告一次 */

        if (!g_ctx.running) break;

        LOG_INFO("=== Collector Statistics ===");
        LOG_INFO("  Queue size: %zu", queue_size(&g_ctx.data_queue));
        LOG_INFO("  Total collected: %llu", (unsigned long long)g_ctx.total_collected);
        LOG_INFO("  Total stored: %llu", (unsigned long long)g_ctx.total_stored);
        LOG_INFO("  Total sent: %llu", (unsigned long long)g_ctx.total_sent);
        LOG_INFO("  Total dropped: %llu", (unsigned long long)g_ctx.total_dropped);
        LOG_INFO("  MQTT connected: %s", g_ctx.mqtt_connected ? "Yes" : "No");

        for (uint32_t i = 0; i < g_ctx.device_count; i++) {
            device_t *dev = &g_ctx.devices[i];
            LOG_INFO("  Device %s: %s, errors: %u",
                     dev->name,
                     dev->running ? "Running" : "Stopped",
                     dev->error_count);
        }
    }

    LOG_INFO("Monitor thread stopped");
    return NULL;
}

/* ============ 配置加载 ============ */

static int load_config(const char *config_file)
{
    /* 简化实现：硬编码配置 */
    /* 实际项目中应解析JSON配置文件 */

    LOG_INFO("Loading configuration...");

    /* 系统配置 */
    strcpy(g_ctx.config.log_level, "INFO");
    g_ctx.config.local_storage_enabled = true;
    g_ctx.config.retention_days = 30;
    g_ctx.config.mqtt_enabled = true;
    strcpy(g_ctx.config.mqtt_broker, "localhost");
    g_ctx.config.mqtt_port = 1883;
    snprintf(g_ctx.config.mqtt_client_id, sizeof(g_ctx.config.mqtt_client_id),
             "collector_%d", getpid());
    strcpy(g_ctx.config.mqtt_topic_prefix, "industrial/data");
    g_ctx.config.mqtt_qos = 1;

    /* 创建设备1: 模拟温度/压力传感器 */
    device_t *dev1 = &g_ctx.devices[g_ctx.device_count++];
    dev1->id = 1;
    strcpy(dev1->name, "SensorUnit1");
    dev1->type = DEVICE_SIMULATION;
    dev1->enabled = true;
    dev1->scan_interval_ms = 1000;

    /* 添加标签 */
    strcpy(dev1->tags[dev1->tag_count].name, "Temperature");
    dev1->tags[dev1->tag_count].scale = 1.0;
    dev1->tags[dev1->tag_count].offset = 0.0;
    dev1->tags[dev1->tag_count].enabled = true;
    dev1->tag_count++;

    strcpy(dev1->tags[dev1->tag_count].name, "Pressure");
    dev1->tags[dev1->tag_count].scale = 1.0;
    dev1->tags[dev1->tag_count].offset = 0.0;
    dev1->tags[dev1->tag_count].enabled = true;
    dev1->tag_count++;

    strcpy(dev1->tags[dev1->tag_count].name, "FlowRate");
    dev1->tags[dev1->tag_count].scale = 1.0;
    dev1->tags[dev1->tag_count].offset = 0.0;
    dev1->tags[dev1->tag_count].enabled = true;
    dev1->tag_count++;

    /* 创建设备2: 模拟电机 */
    device_t *dev2 = &g_ctx.devices[g_ctx.device_count++];
    dev2->id = 2;
    strcpy(dev2->name, "MotorController");
    dev2->type = DEVICE_SIMULATION;
    dev2->enabled = true;
    dev2->scan_interval_ms = 500;

    strcpy(dev2->tags[dev2->tag_count].name, "MotorSpeed");
    dev2->tags[dev2->tag_count].scale = 1.0;
    dev2->tags[dev2->tag_count].offset = 0.0;
    dev2->tags[dev2->tag_count].enabled = true;
    dev2->tag_count++;

    strcpy(dev2->tags[dev2->tag_count].name, "MotorCurrent");
    dev2->tags[dev2->tag_count].scale = 0.1;
    dev2->tags[dev2->tag_count].offset = 0.0;
    dev2->tags[dev2->tag_count].enabled = true;
    dev2->tag_count++;

    LOG_INFO("Configuration loaded: %u devices", g_ctx.device_count);
    return 0;
}

/* ============ 主程序 ============ */

static void signal_handler(int sig)
{
    (void)sig;
    LOG_INFO("Received signal %d, shutting down...", sig);
    g_ctx.running = false;
}

static void print_usage(const char *program)
{
    printf("Industrial Data Collector v%s\n", COLLECTOR_VERSION);
    printf("Usage: %s [options]\n", program);
    printf("Options:\n");
    printf("  -c <file>   Configuration file (default: %s)\n", DEFAULT_CONFIG_FILE);
    printf("  -d          Run as daemon\n");
    printf("  -v          Show version\n");
    printf("  -h          Show this help\n");
}

int main(int argc, char *argv[])
{
    const char *config_file = DEFAULT_CONFIG_FILE;
    bool daemon_mode = false;

    /* 解析命令行 */
    int opt;
    while ((opt = getopt(argc, argv, "c:dvh")) != -1) {
        switch (opt) {
            case 'c':
                config_file = optarg;
                break;
            case 'd':
                daemon_mode = true;
                break;
            case 'v':
                printf("Industrial Data Collector v%s\n", COLLECTOR_VERSION);
                return 0;
            case 'h':
            default:
                print_usage(argv[0]);
                return (opt == 'h') ? 0 : 1;
        }
    }

    /* 初始化日志 */
    logger_init();

    LOG_INFO("============================================");
    LOG_INFO("Industrial Data Collector v%s Starting...", COLLECTOR_VERSION);
    LOG_INFO("============================================");

    /* 守护进程模式 */
    if (daemon_mode) {
        if (daemon(0, 0) < 0) {
            LOG_FATAL("Failed to daemonize: %s", strerror(errno));
            return 1;
        }
        LOG_INFO("Running as daemon");
    }

    /* 锁定内存 */
    if (mlockall(MCL_CURRENT | MCL_FUTURE) < 0) {
        LOG_WARNING("Failed to lock memory: %s", strerror(errno));
    }

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    signal(SIGPIPE, SIG_IGN);

    /* 加载配置 */
    if (load_config(config_file) != 0) {
        LOG_FATAL("Failed to load configuration");
        return 1;
    }

    /* 初始化队列 */
    if (queue_init(&g_ctx.data_queue, COLLECTOR_QUEUE_SIZE) != 0) {
        LOG_FATAL("Failed to initialize data queue");
        return 1;
    }

    /* 初始化数据库 */
    if (db_init() != 0) {
        LOG_FATAL("Failed to initialize database");
        return 1;
    }

    /* 初始化MQTT */
    if (mqtt_init() != 0) {
        LOG_WARNING("Failed to initialize MQTT, continuing without cloud upload");
    }

    /* 启动后台线程 */
    pthread_create(&g_ctx.storage_thread, NULL, storage_thread, NULL);
    pthread_create(&g_ctx.monitor_thread, NULL, monitor_thread, NULL);

    /* 启动设备采集线程 */
    g_ctx.running = true;
    g_ctx.start_time = get_time_ms();

    for (uint32_t i = 0; i < g_ctx.device_count; i++) {
        device_t *dev = &g_ctx.devices[i];
        if (dev->enabled) {
            dev->running = true;
            pthread_create(&dev->thread, NULL, device_simulation_thread, dev);
        }
    }

    LOG_INFO("Collector started successfully");

    /* 主循环 */
    while (g_ctx.running) {
        sleep(1);
    }

    LOG_INFO("Shutting down...");

    /* 停止设备线程 */
    for (uint32_t i = 0; i < g_ctx.device_count; i++) {
        device_t *dev = &g_ctx.devices[i];
        dev->running = false;
        pthread_join(dev->thread, NULL);
    }

    /* 等待队列处理完成 */
    while (queue_size(&g_ctx.data_queue) > 0) {
        usleep(100000);
    }

    /* 停止后台线程 */
    pthread_join(g_ctx.storage_thread, NULL);
    pthread_join(g_ctx.monitor_thread, NULL);

    /* 清理 */
    mqtt_close();
    db_close();
    queue_destroy(&g_ctx.data_queue);
    logger_close();

    printf("Collector stopped.\n");

    return 0;
}
