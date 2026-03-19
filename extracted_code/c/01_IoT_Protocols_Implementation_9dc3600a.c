/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 5718
 * Language: c
 * Block ID: 9dc3600a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file protocol_gateway.h
 * @brief 多协议IoT网关
 */

#ifndef PROTOCOL_GATEWAY_H
#define PROTOCOL_GATEWAY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 网关版本 */
#define GATEWAY_VERSION_MAJOR   2
#define GATEWAY_VERSION_MINOR   0
#define GATEWAY_VERSION_PATCH   0

/* 最大配置 */
#define GATEWAY_MAX_PROTOCOLS   8
#define GATEWAY_MAX_DEVICES     256
#define GATEWAY_MAX_SENSORS     1024
#define GATEWAY_MAX_DATA_QUEUE  1000
#define GATEWAY_MAX_RULES       64
#define GATEWAY_DEVICE_ID_LEN   32
#define GATEWAY_TOPIC_MAX_LEN   128

/* 协议类型 */
typedef enum {
    PROTO_NONE = 0,
    PROTO_MQTT = 1,
    PROTO_COAP = 2,
    PROTO_MODBUS = 3,
    PROTO_CAN = 4,
    PROTO_LORAWAN = 5,
    PROTO_HTTP = 6,
    PROTO_WEBSOCKET = 7,
    PROTO_CUSTOM = 8
} protocol_type_t;

/* 数据类型 */
typedef enum {
    DATA_TYPE_INT8 = 0,
    DATA_TYPE_UINT8 = 1,
    DATA_TYPE_INT16 = 2,
    DATA_TYPE_UINT16 = 3,
    DATA_TYPE_INT32 = 4,
    DATA_TYPE_UINT32 = 5,
    DATA_TYPE_FLOAT = 6,
    DATA_TYPE_DOUBLE = 7,
    DATA_TYPE_BOOL = 8,
    DATA_TYPE_STRING = 9,
    DATA_TYPE_BINARY = 10
} sensor_data_type_t;

/* 传感器数据 */
typedef struct {
    char sensor_id[32];
    char device_id[GATEWAY_DEVICE_ID_LEN];
    sensor_data_type_t type;
    union {
        int8_t i8;
        uint8_t u8;
        int16_t i16;
        uint16_t u16;
        int32_t i32;
        uint32_t u32;
        float f32;
        double f64;
        bool b;
        char str[256];
        uint8_t binary[256];
    } value;
    size_t data_len;
    uint32_t timestamp;
    uint32_t seq_num;
    uint8_t quality;  /* 0-100 */
} sensor_data_t;

/* 设备信息 */
typedef struct {
    char device_id[GATEWAY_DEVICE_ID_LEN];
    protocol_type_t protocol;
    char protocol_addr[64];
    char name[64];
    char type[32];
    uint32_t last_seen;
    bool online;
    uint8_t rssi;
    uint32_t rx_count;
    uint32_t tx_count;
    void *protocol_ctx;
} device_info_t;

/* 协议适配器接口 */
typedef struct {
    protocol_type_t type;
    const char *name;

    /* 生命周期 */
    int (*init)(void *config);
    int (*deinit)(void);
    int (*start)(void);
    int (*stop)(void);

    /* 设备管理 */
    int (*add_device)(const char *device_id, const char *address, void **ctx);
    int (*remove_device)(const char *device_id);
    int (*send_command)(const char *device_id, const uint8_t *cmd, size_t len);

    /* 数据处理回调设置 */
    void (*set_data_callback)(void (*callback)(const char *device_id,
                                                const sensor_data_t *data));
} protocol_adapter_t;

/* 数据处理器 */
typedef struct {
    const char *name;

    /* 处理函数 */
    int (*process)(sensor_data_t *data, void *user_data);

    /* 条件判断 */
    bool (*match)(const sensor_data_t *data, void *user_data);
} data_processor_t;

/* 转发规则 */
typedef struct {
    char name[64];
    bool enabled;

    /* 匹配条件 */
    char device_pattern[64];      /* 设备ID匹配模式 */
    char sensor_pattern[64];      /* 传感器ID匹配模式 */
    protocol_type_t source_proto; /* 源协议 */

    /* 目标配置 */
    protocol_type_t target_proto;
    char target_topic[GATEWAY_TOPIC_MAX_LEN];
    char target_address[128];

    /* 转换配置 */
    bool format_json;             /* 转换为JSON */
    bool compress;                /* 启用压缩 */
    uint32_t throttle_ms;         /* 限流间隔 */

    /* 统计 */
    uint32_t match_count;
    uint32_t forward_count;
    uint32_t drop_count;
    uint32_t last_forward_time;
} forward_rule_t;

/* 网关配置 */
typedef struct {
    char gateway_id[32];
    char gateway_name[64];
    char location[64];

    /* 云端配置 */
    struct {
        bool enabled;
        protocol_type_t protocol;
        char server[128];
        uint16_t port;
        char username[64];
        char password[64];
        char client_id[64];
        uint32_t keepalive_interval;
        bool use_tls;
    } cloud;

    /* 本地配置 */
    struct {
        uint16_t http_port;
        uint16_t mqtt_port;
        bool local_storage;
        uint32_t storage_max_size;
    } local;

    /* 数据处理 */
    struct {
        bool edge_computing;
        uint32_t batch_size;
        uint32_t flush_interval_ms;
        float compression_ratio;
    } processing;

    /* 日志 */
    struct {
        uint8_t level;
        bool file_output;
        char log_path[256];
        uint32_t max_file_size;
    } logging;
} gateway_config_t;

/* 网关上下文 */
typedef struct {
    gateway_config_t config;
    bool running;
    uint32_t start_time;

    /* 协议适配器 */
    protocol_adapter_t *adapters[GATEWAY_MAX_PROTOCOLS];
    uint8_t adapter_count;

    /* 设备管理 */
    device_info_t devices[GATEWAY_MAX_DEVICES];
    uint16_t device_count;

    /* 数据队列 */
    sensor_data_t data_queue[GATEWAY_MAX_DATA_QUEUE];
    volatile uint32_t queue_head;
    volatile uint32_t queue_tail;

    /* 转发规则 */
    forward_rule_t rules[GATEWAY_MAX_RULES];
    uint8_t rule_count;

    /* 统计 */
    struct {
        uint64_t total_rx_bytes;
        uint64_t total_tx_bytes;
        uint64_t total_messages;
        uint64_t dropped_messages;
        uint64_t forwarded_messages;
        uint32_t connected_devices;
        uint32_t protocol_errors;
    } stats;

    /* 线程同步 */
    void *mutex;
    void *cond;

    /* 回调 */
    void (*on_device_connect)(const char *device_id);
    void (*on_device_disconnect)(const char *device_id);
    void (*on_data_received)(const sensor_data_t *data);
} gateway_context_t;

/* API函数 */

/* 初始化和生命周期 */
int gateway_init(gateway_context_t *ctx, const gateway_config_t *config);
int gateway_start(gateway_context_t *ctx);
int gateway_stop(gateway_context_t *ctx);
void gateway_destroy(gateway_context_t *ctx);

/* 协议管理 */
int gateway_register_protocol(gateway_context_t *ctx, protocol_adapter_t *adapter);
int gateway_unregister_protocol(gateway_context_t *ctx, protocol_type_t type);
protocol_adapter_t* gateway_get_protocol(gateway_context_t *ctx, protocol_type_t type);

/* 设备管理 */
int gateway_add_device(gateway_context_t *ctx, const char *device_id,
                        protocol_type_t proto, const char *address);
int gateway_remove_device(gateway_context_t *ctx, const char *device_id);
device_info_t* gateway_find_device(gateway_context_t *ctx, const char *device_id);
int gateway_send_command(gateway_context_t *ctx, const char *device_id,
                          const uint8_t *cmd, size_t len);

/* 数据处理 */
int gateway_push_data(gateway_context_t *ctx, const sensor_data_t *data);
int gateway_process_data(gateway_context_t *ctx, sensor_data_t *data);
int gateway_register_processor(gateway_context_t *ctx, data_processor_t *processor);

/* 转发规则 */
int gateway_add_rule(gateway_context_t *ctx, const forward_rule_t *rule);
int gateway_remove_rule(gateway_context_t *ctx, const char *name);
int gateway_enable_rule(gateway_context_t *ctx, const char *name, bool enable);

/* 数据格式转换 */
int gateway_data_to_json(const sensor_data_t *data, char *json, size_t max_len);
int gateway_json_to_data(const char *json, sensor_data_t *data);
int gateway_data_to_binary(const sensor_data_t *data, uint8_t *buf, size_t max_len);

/* 统计和状态 */
void gateway_get_stats(gateway_context_t *ctx, void *stats);
void gateway_print_status(gateway_context_t *ctx);
const char* gateway_version_string(void);

/* 工具函数 */
const char* protocol_type_str(protocol_type_t type);
const char* data_type_str(sensor_data_type_t type);
uint32_t gateway_get_uptime(gateway_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* PROTOCOL_GATEWAY_H */
