/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 6017
 * Language: c
 * Block ID: 159614b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file protocol_gateway.c
 * @brief 多协议IoT网关实现
 */

#include "protocol_gateway.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* 版本字符串 */
static const char version_string[] = "2.0.0";

/* 日志级别 */
#define LOG_LEVEL_DEBUG     0
#define LOG_LEVEL_INFO      1
#define LOG_LEVEL_WARN      2
#define LOG_LEVEL_ERROR     3

/* 内部日志函数 */
static void gateway_log(gateway_context_t *ctx, int level, const char *fmt, ...) {
    if (ctx == NULL || level < ctx->config.logging.level) return;

    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    printf("[%s] ", level_str[level]);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\n");
}

/* 协议类型转字符串 */
const char* protocol_type_str(protocol_type_t type) {
    switch (type) {
        case PROTO_NONE: return "NONE";
        case PROTO_MQTT: return "MQTT";
        case PROTO_COAP: return "CoAP";
        case PROTO_MODBUS: return "Modbus";
        case PROTO_CAN: return "CAN";
        case PROTO_LORAWAN: return "LoRaWAN";
        case PROTO_HTTP: return "HTTP";
        case PROTO_WEBSOCKET: return "WebSocket";
        case PROTO_CUSTOM: return "Custom";
        default: return "Unknown";
    }
}

/* 数据类型转字符串 */
const char* data_type_str(sensor_data_type_t type) {
    switch (type) {
        case DATA_TYPE_INT8: return "int8";
        case DATA_TYPE_UINT8: return "uint8";
        case DATA_TYPE_INT16: return "int16";
        case DATA_TYPE_UINT16: return "uint16";
        case DATA_TYPE_INT32: return "int32";
        case DATA_TYPE_UINT32: return "uint32";
        case DATA_TYPE_FLOAT: return "float";
        case DATA_TYPE_DOUBLE: return "double";
        case DATA_TYPE_BOOL: return "bool";
        case DATA_TYPE_STRING: return "string";
        case DATA_TYPE_BINARY: return "binary";
        default: return "unknown";
    }
}

/* 初始化网关 */
int gateway_init(gateway_context_t *ctx, const gateway_config_t *config) {
    if (ctx == NULL || config == NULL) {
        return -1;
    }

    memset(ctx, 0, sizeof(gateway_context_t));
    memcpy(&ctx->config, config, sizeof(gateway_config_t));

    ctx->running = false;
    ctx->start_time = 0;
    ctx->adapter_count = 0;
    ctx->device_count = 0;
    ctx->queue_head = 0;
    ctx->queue_tail = 0;
    ctx->rule_count = 0;

    gateway_log(ctx, LOG_LEVEL_INFO, "Gateway %s initializing...", version_string);
    gateway_log(ctx, LOG_LEVEL_INFO, "Gateway ID: %s", config->gateway_id);

    return 0;
}

/* 启动网关 */
int gateway_start(gateway_context_t *ctx) {
    if (ctx == NULL || ctx->running) {
        return -1;
    }

    ctx->running = true;
    ctx->start_time = (uint32_t)time(NULL);

    /* 启动所有注册的协议适配器 */
    for (int i = 0; i < ctx->adapter_count; i++) {
        if (ctx->adapters[i]->start) {
            ctx->adapters[i]->start();
            gateway_log(ctx, LOG_LEVEL_INFO, "Protocol %s started",
                       ctx->adapters[i]->name);
        }
    }

    gateway_log(ctx, LOG_LEVEL_INFO, "Gateway started successfully");
    return 0;
}

/* 停止网关 */
int gateway_stop(gateway_context_t *ctx) {
    if (ctx == NULL || !ctx->running) {
        return -1;
    }

    ctx->running = false;

    /* 停止所有协议适配器 */
    for (int i = 0; i < ctx->adapter_count; i++) {
        if (ctx->adapters[i]->stop) {
            ctx->adapters[i]->stop();
        }
    }

    gateway_log(ctx, LOG_LEVEL_INFO, "Gateway stopped");
    return 0;
}

/* 销毁网关 */
void gateway_destroy(gateway_context_t *ctx) {
    if (ctx == NULL) return;

    gateway_stop(ctx);

    /* 清理资源 */
    for (int i = 0; i < ctx->adapter_count; i++) {
        if (ctx->adapters[i]->deinit) {
            ctx->adapters[i]->deinit();
        }
    }

    gateway_log(ctx, LOG_LEVEL_INFO, "Gateway destroyed");
}

/* 注册协议适配器 */
int gateway_register_protocol(gateway_context_t *ctx, protocol_adapter_t *adapter) {
    if (ctx == NULL || adapter == NULL || ctx->adapter_count >= GATEWAY_MAX_PROTOCOLS) {
        return -1;
    }

    ctx->adapters[ctx->adapter_count++] = adapter;

    if (adapter->init) {
        adapter->init(NULL);
    }

    gateway_log(ctx, LOG_LEVEL_INFO, "Protocol %s registered", adapter->name);
    return 0;
}

/* 添加设备 */
int gateway_add_device(gateway_context_t *ctx, const char *device_id,
                        protocol_type_t proto, const char *address) {
    if (ctx == NULL || device_id == NULL || address == NULL) {
        return -1;
    }

    if (ctx->device_count >= GATEWAY_MAX_DEVICES) {
        return -1;
    }

    /* 检查是否已存在 */
    for (int i = 0; i < ctx->device_count; i++) {
        if (strcmp(ctx->devices[i].device_id, device_id) == 0) {
            return -1; /* 已存在 */
        }
    }

    device_info_t *dev = &ctx->devices[ctx->device_count++];
    strncpy(dev->device_id, device_id, sizeof(dev->device_id) - 1);
    dev->protocol = proto;
    strncpy(dev->protocol_addr, address, sizeof(dev->protocol_addr) - 1);
    dev->online = true;
    dev->last_seen = (uint32_t)time(NULL);

    /* 通知协议适配器 */
    protocol_adapter_t *adapter = gateway_get_protocol(ctx, proto);
    if (adapter && adapter->add_device) {
        adapter->add_device(device_id, address, &dev->protocol_ctx);
    }

    ctx->stats.connected_devices++;

    gateway_log(ctx, LOG_LEVEL_INFO, "Device %s added (protocol: %s)",
               device_id, protocol_type_str(proto));

    if (ctx->on_device_connect) {
        ctx->on_device_connect(device_id);
    }

    return 0;
}

/* 查找设备 */
device_info_t* gateway_find_device(gateway_context_t *ctx, const char *device_id) {
    if (ctx == NULL || device_id == NULL) return NULL;

    for (int i = 0; i < ctx->device_count; i++) {
        if (strcmp(ctx->devices[i].device_id, device_id) == 0) {
            return &ctx->devices[i];
        }
    }
    return NULL;
}

/* 获取协议适配器 */
protocol_adapter_t* gateway_get_protocol(gateway_context_t *ctx, protocol_type_t type) {
    if (ctx == NULL) return NULL;

    for (int i = 0; i < ctx->adapter_count; i++) {
        if (ctx->adapters[i]->type == type) {
            return ctx->adapters[i];
        }
    }
    return NULL;
}

/* 推送数据到队列 */
int gateway_push_data(gateway_context_t *ctx, const sensor_data_t *data) {
    if (ctx == NULL || data == NULL) return -1;

    uint32_t next_tail = (ctx->queue_tail + 1) % GATEWAY_MAX_DATA_QUEUE;
    if (next_tail == ctx->queue_head) {
        /* 队列满 */
        ctx->stats.dropped_messages++;
        return -1;
    }

    ctx->data_queue[ctx->queue_tail] = *data;
    ctx->queue_tail = next_tail;

    ctx->stats.total_messages++;

    return 0;
}

/* 处理数据 */
int gateway_process_data(gateway_context_t *ctx, sensor_data_t *data) {
    if (ctx == NULL || data == NULL) return -1;

    /* 更新时间戳 */
    if (data->timestamp == 0) {
        data->timestamp = (uint32_t)time(NULL);
    }

    /* 匹配转发规则 */
    for (int i = 0; i < ctx->rule_count; i++) {
        forward_rule_t *rule = &ctx->rules[i];
        if (!rule->enabled) continue;

        /* 检查限流 */
        uint32_t now = (uint32_t)time(NULL) * 1000;
        if (rule->throttle_ms > 0 &&
            (now - rule->last_forward_time) < rule->throttle_ms) {
            rule->drop_count++;
            continue;
        }

        rule->match_count++;

        /* 格式转换 */
        if (rule->format_json) {
            char json[512];
            gateway_data_to_json(data, json, sizeof(json));
            /* 转发JSON数据 */
        }

        rule->forward_count++;
        rule->last_forward_time = now;
        ctx->stats.forwarded_messages++;
    }

    /* 调用回调 */
    if (ctx->on_data_received) {
        ctx->on_data_received(data);
    }

    return 0;
}

/* 数据转JSON */
int gateway_data_to_json(const sensor_data_t *data, char *json, size_t max_len) {
    if (data == NULL || json == NULL) return -1;

    int n = snprintf(json, max_len,
        "{"
        "\"sensor_id\":\"%s\","
        "\"device_id\":\"%s\","
        "\"type\":\"%s\","
        "\"timestamp\":%u,"
        "\"seq\":%u,"
        "\"value\":",
        data->sensor_id, data->device_id,
        data_type_str(data->type),
        data->timestamp, data->seq_num);

    switch (data->type) {
        case DATA_TYPE_INT8:
            n += snprintf(json + n, max_len - n, "%d", data->value.i8);
            break;
        case DATA_TYPE_UINT8:
            n += snprintf(json + n, max_len - n, "%u", data->value.u8);
            break;
        case DATA_TYPE_INT16:
            n += snprintf(json + n, max_len - n, "%d", data->value.i16);
            break;
        case DATA_TYPE_UINT16:
            n += snprintf(json + n, max_len - n, "%u", data->value.u16);
            break;
        case DATA_TYPE_INT32:
            n += snprintf(json + n, max_len - n, "%d", data->value.i32);
            break;
        case DATA_TYPE_UINT32:
            n += snprintf(json + n, max_len - n, "%u", data->value.u32);
            break;
        case DATA_TYPE_FLOAT:
            n += snprintf(json + n, max_len - n, "%.4f", data->value.f32);
            break;
        case DATA_TYPE_DOUBLE:
            n += snprintf(json + n, max_len - n, "%.6f", data->value.f64);
            break;
        case DATA_TYPE_BOOL:
            n += snprintf(json + n, max_len - n, "%s", data->value.b ? "true" : "false");
            break;
        case DATA_TYPE_STRING:
            n += snprintf(json + n, max_len - n, "\"%s\"", data->value.str);
            break;
        default:
            n += snprintf(json + n, max_len - n, "null");
            break;
    }

    n += snprintf(json + n, max_len - n, "}");

    return n;
}

/* 添加转发规则 */
int gateway_add_rule(gateway_context_t *ctx, const forward_rule_t *rule) {
    if (ctx == NULL || rule == NULL || ctx->rule_count >= GATEWAY_MAX_RULES) {
        return -1;
    }

    memcpy(&ctx->rules[ctx->rule_count++], rule, sizeof(forward_rule_t));

    gateway_log(ctx, LOG_LEVEL_INFO, "Forward rule '%s' added", rule->name);
    return 0;
}

/* 打印状态 */
void gateway_print_status(gateway_context_t *ctx) {
    if (ctx == NULL) return;

    printf("\n========== Gateway Status ==========\n");
    printf("Gateway ID: %s\n", ctx->config.gateway_id);
    printf("Version: %s\n", version_string);
    printf("Uptime: %u seconds\n", gateway_get_uptime(ctx));
    printf("Running: %s\n", ctx->running ? "Yes" : "No");
    printf("\n--- Devices ---\n");
    printf("Total: %d, Connected: %lu\n", ctx->device_count,
           (unsigned long)ctx->stats.connected_devices);

    printf("\n--- Statistics ---\n");
    printf("RX Bytes: %lu\n", (unsigned long)ctx->stats.total_rx_bytes);
    printf("TX Bytes: %lu\n", (unsigned long)ctx->stats.total_tx_bytes);
    printf("Total Messages: %lu\n", (unsigned long)ctx->stats.total_messages);
    printf("Forwarded: %lu\n", (unsigned long)ctx->stats.forwarded_messages);
    printf("Dropped: %lu\n", (unsigned long)ctx->stats.dropped_messages);
    printf("Protocol Errors: %lu\n", (unsigned long)ctx->stats.protocol_errors);

    printf("\n--- Queue Status ---\n");
    uint32_t queue_used = (ctx->queue_tail - ctx->queue_head + GATEWAY_MAX_DATA_QUEUE)
                          % GATEWAY_MAX_DATA_QUEUE;
    printf("Queue Usage: %u/%d\n", queue_used, GATEWAY_MAX_DATA_QUEUE);
    printf("====================================\n\n");
}

/* 获取版本字符串 */
const char* gateway_version_string(void) {
    return version_string;
}

/* 获取运行时间 */
uint32_t gateway_get_uptime(gateway_context_t *ctx) {
    if (ctx == NULL || ctx->start_time == 0) return 0;
    return (uint32_t)time(NULL) - ctx->start_time;
}

/* 主函数示例 */
int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    gateway_context_t gateway;
    gateway_config_t config = {
        .gateway_id = "GW-001",
        .gateway_name = "Industrial Gateway",
        .location = "Factory Floor 1",
        .cloud = {
            .enabled = true,
            .protocol = PROTO_MQTT,
            .server = "mqtt.cloud-platform.com",
            .port = 8883,
            .keepalive_interval = 60,
            .use_tls = true
        },
        .local = {
            .http_port = 8080,
            .mqtt_port = 1883,
            .local_storage = true,
            .storage_max_size = 100 * 1024 * 1024
        },
        .logging = {
            .level = LOG_LEVEL_INFO,
            .file_output = true
        }
    };

    /* 初始化网关 */
    if (gateway_init(&gateway, &config) != 0) {
        printf("Failed to initialize gateway\n");
        return 1;
    }

    /* 添加转发规则 */
    forward_rule_t rule = {
        .name = "temperature_to_cloud",
        .enabled = true,
        .device_pattern = "temp_sensor_*",
        .sensor_pattern = "*",
        .source_proto = PROTO_MQTT,
        .target_proto = PROTO_MQTT,
        .target_topic = "sensors/temperature",
        .format_json = true,
        .throttle_ms = 1000
    };
    gateway_add_rule(&gateway, &rule);

    /* 启动网关 */
    if (gateway_start(&gateway) != 0) {
        printf("Failed to start gateway\n");
        return 1;
    }

    /* 模拟运行 */
    printf("Gateway running. Press Ctrl+C to stop.\n");

    for (int i = 0; i < 10; i++) {
        /* 添加模拟设备 */
        char device_id[32];
        snprintf(device_id, sizeof(device_id), "temp_sensor_%02d", i);
        gateway_add_device(&gateway, device_id, PROTO_MQTT, "192.168.1.100");

        /* 模拟数据 */
        sensor_data_t data = {
            .sensor_id = "temperature",
            .type = DATA_TYPE_FLOAT,
            .value.f32 = 20.0f + (i * 1.5f),
            .quality = 95
        };
        strcpy(data.device_id, device_id);

        gateway_push_data(&gateway, &data);
        gateway_process_data(&gateway, &data);

        /* 打印状态 */
        gateway_print_status(&gateway);

#ifdef _WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

    /* 停止和清理 */
    gateway_destroy(&gateway);

    printf("Gateway shutdown complete\n");
    return 0;
}
