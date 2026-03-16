# IoT协议栈实现指南

## 目录

- [IoT协议栈实现指南](#iot协议栈实现指南)
  - [目录](#目录)
  - [1. 概述](#1-概述)
    - [1.1 IoT协议分层模型](#11-iot协议分层模型)
    - [1.2 协议对比表](#12-协议对比表)
    - [1.3 项目目录结构](#13-项目目录结构)
  - [2. MQTT客户端实现](#2-mqtt客户端实现)
    - [2.1 MQTT协议基础](#21-mqtt协议基础)
    - [2.2 MQTT数据包结构](#22-mqtt数据包结构)
    - [2.3 完整MQTT客户端实现](#23-完整mqtt客户端实现)
    - [2.4 MQTT使用示例](#24-mqtt使用示例)
  - [3. CoAP协议实现](#3-coap协议实现)
    - [3.1 CoAP协议概述](#31-coap协议概述)
    - [3.2 CoAP数据包格式](#32-coap数据包格式)
    - [3.3 完整CoAP客户端实现](#33-完整coap客户端实现)
    - [3.4 CoAP使用示例](#34-coap使用示例)

---

## 1. 概述

### 1.1 IoT协议分层模型

```
┌─────────────────────────────────────────────────────────────┐
│                    应用层 (Application)                      │
│  MQTT | CoAP | HTTP | WebSocket | Modbus | LoRaWAN         │
├─────────────────────────────────────────────────────────────┤
│                    传输层 (Transport)                        │
│  TCP | UDP | TLS/DTLS | SMS                                 │
├─────────────────────────────────────────────────────────────┤
│                    网络层 (Network)                          │
│  IPv4 | IPv6 | 6LoWPAN | RPL                                │
├─────────────────────────────────────────────────────────────┤
│                    链路层 (Link)                             │
│  Ethernet | WiFi | BLE | ZigBee | LoRa | NB-IoT | CAN       │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 协议对比表

| 协议 | 传输层 | 模式 | 功耗 | 复杂度 | 适用场景 |
|------|--------|------|------|--------|----------|
| MQTT | TCP | 发布/订阅 | 中 | 低 | 实时消息、传感器数据 |
| CoAP | UDP | 请求/响应 | 低 | 低 | 受限设备、REST API |
| HTTP | TCP | 请求/响应 | 高 | 中 | Web服务、文件传输 |
| Modbus | TCP/RTU | 主/从 | 低 | 低 | 工业控制 |
| LoRaWAN | LoRa | ALOHA | 极低 | 高 | 远距离、低功耗 |
| CAN | 物理层 | 广播 | 低 | 中 | 车载、工业现场 |

### 1.3 项目目录结构

```
IoT_Protocol_Stack/
├── include/
│   ├── mqtt_client.h      # MQTT客户端头文件
│   ├── coap_client.h      # CoAP客户端头文件
│   ├── modbus.h           # Modbus协议头文件
│   ├── can_driver.h       # CAN驱动头文件
│   ├── lorawan_stack.h    # LoRaWAN协议栈头文件
│   ├── http_client.h      # HTTP客户端头文件
│   ├── websocket.h        # WebSocket实现头文件
│   ├── tls_wrapper.h      # TLS封装头文件
│   └── protocol_gateway.h # 多协议网关头文件
├── src/
│   ├── mqtt_client.c
│   ├── coap_client.c
│   ├── modbus.c
│   ├── can_driver.c
│   ├── lorawan_stack.c
│   ├── http_client.c
│   ├── websocket.c
│   ├── tls_wrapper.c
│   └── protocol_gateway.c
├── examples/
│   ├── mqtt_example.c
│   ├── coap_example.c
│   ├── modbus_example.c
│   └── gateway_example.c
├── tests/
│   └── test_protocols.c
├── Makefile
└── README.md
```

---

## 2. MQTT客户端实现

### 2.1 MQTT协议基础

MQTT (Message Queuing Telemetry Transport) 是一种轻量级的发布/订阅消息协议，专为低带宽、高延迟或不可靠网络环境设计。

**关键特性：**

- 发布/订阅模式解耦生产者和消费者
- 三个QoS等级：0(最多一次)、1(至少一次)、2(恰好一次)
- 保留消息和遗嘱消息
- 会话保持机制

### 2.2 MQTT数据包结构

```
固定报头 (Fixed Header)
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
│  7-4    │   3     │   2     │   1     │   0     │  bit7   │  ...    │  bit0   │
├─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┤
│  报文类型   │ 标志位(DUP, QoS, RETAIN) │            剩余长度                    │
└───────────────────────────────────────────────────────────────────────────────┘

可变报头 (Variable Header) - 根据报文类型变化
有效载荷 (Payload) - 根据报文类型变化
```

### 2.3 完整MQTT客户端实现

```c
/**
 * @file mqtt_client.h
 * @brief MQTT客户端实现 - 轻量级嵌入式版本
 * @version 1.0
 */

#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* MQTT版本定义 */
#define MQTT_PROTOCOL_NAME_3_1     "MQIsdp"
#define MQTT_PROTOCOL_NAME_3_1_1   "MQTT"
#define MQTT_PROTOCOL_VERSION_3_1  3
#define MQTT_PROTOCOL_VERSION_3_1_1 4

/* MQTT控制报文类型 */
typedef enum {
    MQTT_CONNECT = 1,       /* 客户端请求连接服务器 */
    MQTT_CONNACK = 2,       /* 连接确认 */
    MQTT_PUBLISH = 3,       /* 发布消息 */
    MQTT_PUBACK = 4,        /* QoS1发布确认 */
    MQTT_PUBREC = 5,        /* QoS2发布收到 */
    MQTT_PUBREL = 6,        /* QoS2发布释放 */
    MQTT_PUBCOMP = 7,       /* QoS2发布完成 */
    MQTT_SUBSCRIBE = 8,     /* 订阅请求 */
    MQTT_SUBACK = 9,        /* 订阅确认 */
    MQTT_UNSUBSCRIBE = 10,  /* 取消订阅 */
    MQTT_UNSUBACK = 11,     /* 取消订阅确认 */
    MQTT_PINGREQ = 12,      /* Ping请求 */
    MQTT_PINGRESP = 13,     /* Ping响应 */
    MQTT_DISCONNECT = 14    /* 断开连接 */
} mqtt_packet_type_t;

/* QoS等级 */
typedef enum {
    MQTT_QOS_0 = 0,         /* 最多交付一次 */
    MQTT_QOS_1 = 1,         /* 至少交付一次 */
    MQTT_QOS_2 = 2          /* 恰好交付一次 */
} mqtt_qos_t;

/* 连接状态 */
typedef enum {
    MQTT_STATE_DISCONNECTED = 0,
    MQTT_STATE_CONNECTING,
    MQTT_STATE_CONNECTED,
    MQTT_STATE_DISCONNECTING
} mqtt_state_t;

/* 连接返回码 */
typedef enum {
    MQTT_CONN_ACCEPTED = 0,                 /* 连接已接受 */
    MQTT_CONN_REFUSED_PROTOCOL = 1,         /* 不支持的协议版本 */
    MQTT_CONN_REFUSED_IDENTIFIER = 2,       /* 不合格的客户端标识符 */
    MQTT_CONN_REFUSED_SERVER = 3,           /* 服务器不可用 */
    MQTT_CONN_REFUSED_CREDENTIALS = 4,      /* 用户名或密码错误 */
    MQTT_CONN_REFUSED_AUTH = 5              /* 未授权 */
} mqtt_connack_code_t;

/* 最大配置 */
#define MQTT_MAX_CLIENT_ID_LEN      23
#define MQTT_MAX_TOPIC_LEN          128
#define MQTT_MAX_MESSAGE_LEN        1024
#define MQTT_MAX_PACKET_LEN         4096
#define MQTT_KEEP_ALIVE_DEFAULT     60
#define MQTT_MAX_SUBSCRIPTIONS      16

/* 前向声明 */
struct mqtt_client;
typedef struct mqtt_client mqtt_client_t;

/* 网络传输回调 */
typedef int (*mqtt_network_read_t)(void *network, uint8_t *buf, size_t len, uint32_t timeout_ms);
typedef int (*mqtt_network_write_t)(void *network, const uint8_t *buf, size_t len, uint32_t timeout_ms);
typedef void (*mqtt_network_close_t)(void *network);
typedef int (*mqtt_network_connect_t)(void *network, const char *host, uint16_t port);

/* 事件回调 */
typedef void (*mqtt_on_connect_t)(mqtt_client_t *client, int return_code);
typedef void (*mqtt_on_disconnect_t)(mqtt_client_t *client);
typedef void (*mqtt_on_message_t)(mqtt_client_t *client, const char *topic,
                                   const uint8_t *payload, size_t len, mqtt_qos_t qos);
typedef void (*mqtt_on_publish_t)(mqtt_client_t *client, uint16_t msg_id);
typedef void (*mqtt_on_subscribe_t)(mqtt_client_t *client, uint16_t msg_id,
                                     const uint8_t *granted_qos, int qos_count);

/* 网络接口 */
typedef struct {
    void *context;
    mqtt_network_connect_t connect;
    mqtt_network_read_t read;
    mqtt_network_write_t write;
    mqtt_network_close_t close;
} mqtt_network_t;

/* 订阅条目 */
typedef struct {
    char topic[MQTT_MAX_TOPIC_LEN];
    mqtt_qos_t qos;
    bool active;
    mqtt_on_message_t callback;
} mqtt_subscription_t;

/* 消息结构 */
typedef struct {
    uint16_t msg_id;
    mqtt_qos_t qos;
    bool retained;
    bool dup;
    char topic[MQTT_MAX_TOPIC_LEN];
    uint8_t payload[MQTT_MAX_MESSAGE_LEN];
    size_t payload_len;
    uint32_t timestamp;
} mqtt_message_t;

/* 客户端配置 */
typedef struct {
    const char *client_id;
    const char *username;
    const char *password;
    const char *will_topic;
    const uint8_t *will_payload;
    size_t will_payload_len;
    mqtt_qos_t will_qos;
    bool will_retain;
    uint16_t keep_alive_sec;
    bool clean_session;
    uint8_t protocol_version;
} mqtt_connect_options_t;

/* MQTT客户端结构 */
struct mqtt_client {
    mqtt_network_t network;
    mqtt_state_t state;

    /* 回调函数 */
    mqtt_on_connect_t on_connect;
    mqtt_on_disconnect_t on_disconnect;
    mqtt_on_message_t on_message;
    mqtt_on_publish_t on_publish;
    mqtt_on_subscribe_t on_subscribe;

    /* 订阅管理 */
    mqtt_subscription_t subscriptions[MQTT_MAX_SUBSCRIPTIONS];
    int subscription_count;

    /* 消息ID管理 */
    uint16_t next_msg_id;

    /* 保活计时 */
    uint32_t last_activity;
    uint16_t keep_alive_ms;

    /* 接收缓冲区 */
    uint8_t rx_buffer[MQTT_MAX_PACKET_LEN];
    size_t rx_len;

    /* 发送缓冲区 */
    uint8_t tx_buffer[MQTT_MAX_PACKET_LEN];

    /* 配置 */
    mqtt_connect_options_t options;

    /* 用户数据 */
    void *user_data;
};

/* 函数声明 */
int mqtt_client_init(mqtt_client_t *client, mqtt_network_t *network);
int mqtt_client_connect(mqtt_client_t *client, const mqtt_connect_options_t *options);
int mqtt_client_disconnect(mqtt_client_t *client);
int mqtt_client_publish(mqtt_client_t *client, const char *topic,
                         const uint8_t *payload, size_t len,
                         mqtt_qos_t qos, bool retain);
int mqtt_client_subscribe(mqtt_client_t *client, const char *topic, mqtt_qos_t qos);
int mqtt_client_unsubscribe(mqtt_client_t *client, const char *topic);
int mqtt_client_ping(mqtt_client_t *client);
int mqtt_client_process(mqtt_client_t *client, uint32_t timeout_ms);
mqtt_state_t mqtt_client_state(mqtt_client_t *client);
bool mqtt_client_connected(mqtt_client_t *client);
void mqtt_client_set_callback(mqtt_client_t *client,
                               mqtt_on_connect_t on_connect,
                               mqtt_on_disconnect_t on_disconnect,
                               mqtt_on_message_t on_message);

#ifdef __cplusplus
}
#endif

#endif /* MQTT_CLIENT_H */
```

```c
/**
 * @file mqtt_client.c
 * @brief MQTT客户端实现
 */

#include "mqtt_client.h"
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/time.h>
    #include <time.h>
#endif

/* 获取当前时间戳(毫秒) */
static uint32_t mqtt_get_time_ms(void) {
#ifdef _WIN32
    return GetTickCount();
#else
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
#endif
}

/* 编码剩余长度 */
static int mqtt_encode_remaining_length(uint8_t *buf, uint32_t remaining_len) {
    int len = 0;
    do {
        uint8_t byte = remaining_len % 128;
        remaining_len /= 128;
        if (remaining_len > 0) {
            byte |= 0x80;
        }
        buf[len++] = byte;
    } while (remaining_len > 0);
    return len;
}

/* 解码剩余长度 */
static int mqtt_decode_remaining_length(const uint8_t *buf, uint32_t *remaining_len, int *bytes_read) {
    int multiplier = 1;
    *remaining_len = 0;
    *bytes_read = 0;

    do {
        if (*bytes_read >= 4) {
            return -1; /* 格式错误 */
        }
        uint8_t byte = buf[(*bytes_read)++];
        *remaining_len += (byte & 0x7F) * multiplier;
        multiplier *= 128;
        if (multiplier > 128 * 128 * 128) {
            return -1; /* 格式错误 */
        }
    } while ((buf[*bytes_read - 1] & 0x80) != 0);

    return 0;
}

/* 生成下一个消息ID */
static uint16_t mqtt_next_msg_id(mqtt_client_t *client) {
    if (++client->next_msg_id == 0) {
        client->next_msg_id = 1;
    }
    return client->next_msg_id;
}

/* 写入字符串(带长度前缀) */
static int mqtt_write_string(uint8_t *buf, const char *str) {
    uint16_t len = (uint16_t)strlen(str);
    buf[0] = (uint8_t)(len >> 8);
    buf[1] = (uint8_t)(len & 0xFF);
    memcpy(buf + 2, str, len);
    return len + 2;
}

/* 发送数据包 */
static int mqtt_send_packet(mqtt_client_t *client, uint8_t *packet, size_t len) {
    if (client->network.write == NULL) {
        return -1;
    }

    int ret = client->network.write(client->network.context, packet, len, 5000);
    if (ret > 0) {
        client->last_activity = mqtt_get_time_ms();
    }
    return ret;
}

/* 读取固定报头 */
static int mqtt_read_fixed_header(mqtt_client_t *client, uint8_t *header,
                                   uint32_t *remaining_len, uint32_t timeout_ms) {
    uint8_t buf[5];
    int pos = 0;
    uint32_t start_time = mqtt_get_time_ms();

    /* 读取第一个字节(报文类型和标志) */
    while (pos < 1) {
        int ret = client->network.read(client->network.context, &buf[pos], 1, 100);
        if (ret > 0) {
            pos++;
        } else if (mqtt_get_time_ms() - start_time > timeout_ms) {
            return -1; /* 超时 */
        }
    }
    *header = buf[0];

    /* 读取剩余长度 */
    int len_bytes;
    do {
        int ret = client->network.read(client->network.context, &buf[pos], 1, 100);
        if (ret > 0) {
            pos++;
        } else if (mqtt_get_time_ms() - start_time > timeout_ms) {
            return -1; /* 超时 */
        }
    } while ((buf[pos - 1] & 0x80) && pos < 5);

    if (mqtt_decode_remaining_length(buf + 1, remaining_len, &len_bytes) != 0) {
        return -1;
    }

    return 0;
}

/* 初始化MQTT客户端 */
int mqtt_client_init(mqtt_client_t *client, mqtt_network_t *network) {
    if (client == NULL || network == NULL) {
        return -1;
    }

    memset(client, 0, sizeof(mqtt_client_t));
    memcpy(&client->network, network, sizeof(mqtt_network_t));
    client->state = MQTT_STATE_DISCONNECTED;
    client->next_msg_id = 1;

    return 0;
}

/* 构建CONNECT报文 */
static int mqtt_build_connect(mqtt_client_t *client, const mqtt_connect_options_t *options) {
    uint8_t *buf = client->tx_buffer;
    int pos = 0;

    /* 固定报头占位 */
    buf[pos++] = MQTT_CONNECT << 4;
    int remaining_len_pos = pos;
    buf[pos++] = 0; /* 剩余长度占位 */

    /* 可变报头 */
    /* 协议名 */
    const char *protocol_name = (options->protocol_version == MQTT_PROTOCOL_VERSION_3_1)
                                 ? MQTT_PROTOCOL_NAME_3_1
                                 : MQTT_PROTOCOL_NAME_3_1_1;
    pos += mqtt_write_string(buf + pos, protocol_name);

    /* 协议级别 */
    buf[pos++] = options->protocol_version;

    /* 连接标志 */
    uint8_t connect_flags = 0;
    if (options->clean_session) {
        connect_flags |= 0x02;
    }
    if (options->will_topic != NULL) {
        connect_flags |= 0x04;
        connect_flags |= (options->will_qos & 0x03) << 3;
        if (options->will_retain) {
            connect_flags |= 0x20;
        }
    }
    if (options->username != NULL) {
        connect_flags |= 0x80;
        if (options->password != NULL) {
            connect_flags |= 0x40;
        }
    }
    buf[pos++] = connect_flags;

    /* 保活时间 */
    buf[pos++] = (uint8_t)(options->keep_alive_sec >> 8);
    buf[pos++] = (uint8_t)(options->keep_alive_sec & 0xFF);

    /* 有效载荷 */
    /* 客户端标识符 */
    const char *client_id = options->client_id ? options->client_id : "";
    pos += mqtt_write_string(buf + pos, client_id);

    /* 遗嘱主题和消息 */
    if (options->will_topic != NULL) {
        pos += mqtt_write_string(buf + pos, options->will_topic);
        if (options->will_payload != NULL && options->will_payload_len > 0) {
            buf[pos++] = (uint8_t)(options->will_payload_len >> 8);
            buf[pos++] = (uint8_t)(options->will_payload_len & 0xFF);
            memcpy(buf + pos, options->will_payload, options->will_payload_len);
            pos += options->will_payload_len;
        } else {
            buf[pos++] = 0;
            buf[pos++] = 0;
        }
    }

    /* 用户名 */
    if (options->username != NULL) {
        pos += mqtt_write_string(buf + pos, options->username);
    }

    /* 密码 */
    if (options->password != NULL) {
        pos += mqtt_write_string(buf + pos, options->password);
    }

    /* 计算并写入剩余长度 */
    uint32_t remaining_len = pos - remaining_len_pos - 1;
    uint8_t len_buf[4];
    int len_bytes = mqtt_encode_remaining_length(len_buf, remaining_len);

    /* 移动数据以腾出空间 */
    memmove(buf + 1 + len_bytes, buf + remaining_len_pos + 1, pos - remaining_len_pos - 1);
    memcpy(buf + 1, len_buf, len_bytes);

    return 1 + len_bytes + remaining_len;
}

/* 连接到MQTT代理 */
int mqtt_client_connect(mqtt_client_t *client, const mqtt_connect_options_t *options) {
    if (client == NULL || options == NULL) {
        return -1;
    }

    if (client->state != MQTT_STATE_DISCONNECTED) {
        return -1;
    }

    client->state = MQTT_STATE_CONNECTING;
    memcpy(&client->options, options, sizeof(mqtt_connect_options_t));

    /* 构建并发送CONNECT报文 */
    int packet_len = mqtt_build_connect(client, options);
    if (packet_len < 0) {
        client->state = MQTT_STATE_DISCONNECTED;
        return -1;
    }

    if (mqtt_send_packet(client, client->tx_buffer, packet_len) != packet_len) {
        client->state = MQTT_STATE_DISCONNECTED;
        return -1;
    }

    /* 等待CONNACK */
    uint8_t header;
    uint32_t remaining_len;
    if (mqtt_read_fixed_header(client, &header, &remaining_len, 30000) != 0) {
        client->state = MQTT_STATE_DISCONNECTED;
        return -1;
    }

    if ((header >> 4) != MQTT_CONNACK || remaining_len != 2) {
        client->state = MQTT_STATE_DISCONNECTED;
        return -1;
    }

    /* 读取CONNACK内容 */
    uint8_t connack_data[2];
    int ret = client->network.read(client->network.context, connack_data, 2, 5000);
    if (ret != 2) {
        client->state = MQTT_STATE_DISCONNECTED;
        return -1;
    }

    /* session present = connack_data[0] & 0x01 */
    uint8_t return_code = connack_data[1];

    if (return_code == MQTT_CONN_ACCEPTED) {
        client->state = MQTT_STATE_CONNECTED;
        client->keep_alive_ms = options->keep_alive_sec * 1000;
        client->last_activity = mqtt_get_time_ms();

        if (client->on_connect != NULL) {
            client->on_connect(client, return_code);
        }
        return 0;
    } else {
        client->state = MQTT_STATE_DISCONNECTED;
        if (client->on_connect != NULL) {
            client->on_connect(client, return_code);
        }
        return -1;
    }
}

/* 断开连接 */
int mqtt_client_disconnect(mqtt_client_t *client) {
    if (client == NULL || client->state != MQTT_STATE_CONNECTED) {
        return -1;
    }

    client->state = MQTT_STATE_DISCONNECTING;

    /* 发送DISCONNECT报文 */
    uint8_t packet[2] = { MQTT_DISCONNECT << 4, 0 };
    mqtt_send_packet(client, packet, 2);

    /* 关闭网络连接 */
    if (client->network.close != NULL) {
        client->network.close(client->network.context);
    }

    client->state = MQTT_STATE_DISCONNECTED;

    if (client->on_disconnect != NULL) {
        client->on_disconnect(client);
    }

    return 0;
}

/* 发布消息 */
int mqtt_client_publish(mqtt_client_t *client, const char *topic,
                         const uint8_t *payload, size_t len,
                         mqtt_qos_t qos, bool retain) {
    if (client == NULL || topic == NULL || client->state != MQTT_STATE_CONNECTED) {
        return -1;
    }

    if (strlen(topic) > MQTT_MAX_TOPIC_LEN || len > MQTT_MAX_MESSAGE_LEN) {
        return -1;
    }

    uint8_t *buf = client->tx_buffer;
    int pos = 0;

    /* 固定报头 */
    uint8_t header = MQTT_PUBLISH << 4;
    if (qos > MQTT_QOS_0) {
        header |= (qos & 0x03) << 1;
    }
    if (retain) {
        header |= 0x01;
    }
    buf[pos++] = header;

    /* 剩余长度占位 */
    int remaining_len_pos = pos;
    buf[pos++] = 0;

    /* 可变报头 - 主题名 */
    pos += mqtt_write_string(buf + pos, topic);

    /* QoS > 0时需要消息ID */
    uint16_t msg_id = 0;
    if (qos > MQTT_QOS_0) {
        msg_id = mqtt_next_msg_id(client);
        buf[pos++] = (uint8_t)(msg_id >> 8);
        buf[pos++] = (uint8_t)(msg_id & 0xFF);
    }

    /* 有效载荷 */
    memcpy(buf + pos, payload, len);
    pos += len;

    /* 计算并写入剩余长度 */
    uint32_t remaining_len = pos - remaining_len_pos - 1;
    uint8_t len_buf[4];
    int len_bytes = mqtt_encode_remaining_length(len_buf, remaining_len);
    memmove(buf + 1 + len_bytes, buf + remaining_len_pos + 1, pos - remaining_len_pos - 1);
    memcpy(buf + 1, len_buf, len_bytes);

    int packet_len = 1 + len_bytes + remaining_len;

    if (mqtt_send_packet(client, buf, packet_len) != packet_len) {
        return -1;
    }

    /* QoS 0不需要确认 */
    if (qos == MQTT_QOS_0 && client->on_publish != NULL) {
        client->on_publish(client, 0);
    }

    return (int)msg_id;
}

/* 订阅主题 */
int mqtt_client_subscribe(mqtt_client_t *client, const char *topic, mqtt_qos_t qos) {
    if (client == NULL || topic == NULL || client->state != MQTT_STATE_CONNECTED) {
        return -1;
    }

    if (strlen(topic) > MQTT_MAX_TOPIC_LEN) {
        return -1;
    }

    uint8_t *buf = client->tx_buffer;
    int pos = 0;

    /* 固定报头 */
    buf[pos++] = MQTT_SUBSCRIBE << 4 | 0x02; /* 固定标志位 */

    /* 剩余长度占位 */
    int remaining_len_pos = pos;
    buf[pos++] = 0;

    /* 消息ID */
    uint16_t msg_id = mqtt_next_msg_id(client);
    buf[pos++] = (uint8_t)(msg_id >> 8);
    buf[pos++] = (uint8_t)(msg_id & 0xFF);

    /* 主题过滤器 */
    pos += mqtt_write_string(buf + pos, topic);

    /* 请求的QoS */
    buf[pos++] = (uint8_t)qos;

    /* 计算并写入剩余长度 */
    uint32_t remaining_len = pos - remaining_len_pos - 1;
    uint8_t len_buf[4];
    int len_bytes = mqtt_encode_remaining_length(len_buf, remaining_len);
    memmove(buf + 1 + len_bytes, buf + remaining_len_pos + 1, pos - remaining_len_pos - 1);
    memcpy(buf + 1, len_buf, len_bytes);

    int packet_len = 1 + len_bytes + remaining_len;

    if (mqtt_send_packet(client, buf, packet_len) != packet_len) {
        return -1;
    }

    /* 添加到订阅列表 */
    if (client->subscription_count < MQTT_MAX_SUBSCRIPTIONS) {
        mqtt_subscription_t *sub = &client->subscriptions[client->subscription_count++];
        strncpy(sub->topic, topic, MQTT_MAX_TOPIC_LEN - 1);
        sub->topic[MQTT_MAX_TOPIC_LEN - 1] = '\0';
        sub->qos = qos;
        sub->active = true;
        sub->callback = NULL;
    }

    return (int)msg_id;
}

/* 取消订阅 */
int mqtt_client_unsubscribe(mqtt_client_t *client, const char *topic) {
    if (client == NULL || topic == NULL || client->state != MQTT_STATE_CONNECTED) {
        return -1;
    }

    uint8_t *buf = client->tx_buffer;
    int pos = 0;

    /* 固定报头 */
    buf[pos++] = MQTT_UNSUBSCRIBE << 4 | 0x02;

    /* 剩余长度占位 */
    int remaining_len_pos = pos;
    buf[pos++] = 0;

    /* 消息ID */
    uint16_t msg_id = mqtt_next_msg_id(client);
    buf[pos++] = (uint8_t)(msg_id >> 8);
    buf[pos++] = (uint8_t)(msg_id & 0xFF);

    /* 主题过滤器 */
    pos += mqtt_write_string(buf + pos, topic);

    /* 计算并写入剩余长度 */
    uint32_t remaining_len = pos - remaining_len_pos - 1;
    uint8_t len_buf[4];
    int len_bytes = mqtt_encode_remaining_length(len_buf, remaining_len);
    memmove(buf + 1 + len_bytes, buf + remaining_len_pos + 1, pos - remaining_len_pos - 1);
    memcpy(buf + 1, len_buf, len_bytes);

    int packet_len = 1 + len_bytes + remaining_len;

    if (mqtt_send_packet(client, buf, packet_len) != packet_len) {
        return -1;
    }

    /* 从订阅列表移除 */
    for (int i = 0; i < client->subscription_count; i++) {
        if (strcmp(client->subscriptions[i].topic, topic) == 0) {
            /* 移动后面的条目 */
            for (int j = i; j < client->subscription_count - 1; j++) {
                client->subscriptions[j] = client->subscriptions[j + 1];
            }
            client->subscription_count--;
            break;
        }
    }

    return (int)msg_id;
}

/* 发送PING */
int mqtt_client_ping(mqtt_client_t *client) {
    if (client == NULL || client->state != MQTT_STATE_CONNECTED) {
        return -1;
    }

    uint8_t packet[2] = { MQTT_PINGREQ << 4, 0 };
    return mqtt_send_packet(client, packet, 2);
}

/* 处理接收到的PUBLISH */
static void mqtt_handle_publish(mqtt_client_t *client, uint8_t header,
                                 uint32_t remaining_len) {
    uint8_t *buf = client->rx_buffer;
    int pos = 0;

    /* 读取所有剩余数据 */
    int ret = client->network.read(client->network.context, buf, remaining_len, 5000);
    if (ret != (int)remaining_len) {
        return;
    }

    /* 解析标志位 */
    mqtt_qos_t qos = (header >> 1) & 0x03;
    bool dup = (header & 0x08) != 0;
    bool retain = (header & 0x01) != 0;

    /* 解析主题 */
    uint16_t topic_len = (buf[pos] << 8) | buf[pos + 1];
    pos += 2;

    char topic[MQTT_MAX_TOPIC_LEN];
    int copy_len = topic_len < MQTT_MAX_TOPIC_LEN - 1 ? topic_len : MQTT_MAX_TOPIC_LEN - 1;
    memcpy(topic, buf + pos, copy_len);
    topic[copy_len] = '\0';
    pos += topic_len;

    /* QoS > 0时解析消息ID */
    uint16_t msg_id = 0;
    if (qos > MQTT_QOS_0) {
        msg_id = (buf[pos] << 8) | buf[pos + 1];
        pos += 2;
    }

    /* 有效载荷 */
    uint8_t *payload = buf + pos;
    size_t payload_len = remaining_len - pos;

    /* 发送确认 */
    if (qos == MQTT_QOS_1) {
        uint8_t puback[4] = { MQTT_PUBACK << 4, 2, (uint8_t)(msg_id >> 8), (uint8_t)(msg_id & 0xFF) };
        mqtt_send_packet(client, puback, 4);
    } else if (qos == MQTT_QOS_2) {
        uint8_t pubrec[4] = { MQTT_PUBREC << 4, 2, (uint8_t)(msg_id >> 8), (uint8_t)(msg_id & 0xFF) };
        mqtt_send_packet(client, pubrec, 4);
        /* 简化处理：自动发送PUBREL和PUBCOMP */
        /* 实际实现中需要状态机跟踪 */
    }

    /* 调用回调 */
    if (client->on_message != NULL) {
        client->on_message(client, topic, payload, payload_len, qos);
    }
}

/* 处理SUBACK */
static void mqtt_handle_suback(mqtt_client_t *client, uint32_t remaining_len) {
    uint8_t buf[256];
    int ret = client->network.read(client->network.context, buf, remaining_len, 5000);
    if (ret != (int)remaining_len || remaining_len < 3) {
        return;
    }

    uint16_t msg_id = (buf[0] << 8) | buf[1];

    if (client->on_subscribe != NULL) {
        client->on_subscribe(client, msg_id, buf + 2, remaining_len - 2);
    }
}

/* 处理PINGRESP */
static void mqtt_handle_pingresp(mqtt_client_t *client) {
    /* Ping响应接收成功，保活计时器在读取时已经更新 */
    (void)client;
}

/* 处理接收的数据 */
int mqtt_client_process(mqtt_client_t *client, uint32_t timeout_ms) {
    if (client == NULL) {
        return -1;
    }

    if (client->state != MQTT_STATE_CONNECTED) {
        return -1;
    }

    /* 检查保活 */
    uint32_t current_time = mqtt_get_time_ms();
    if (client->keep_alive_ms > 0 &&
        (current_time - client->last_activity) > client->keep_alive_ms) {
        /* 发送PINGREQ */
        mqtt_client_ping(client);
    }

    /* 尝试读取报文 */
    uint8_t header;
    uint32_t remaining_len;

    if (mqtt_read_fixed_header(client, &header, &remaining_len, timeout_ms) != 0) {
        return 0; /* 没有数据 */
    }

    mqtt_packet_type_t packet_type = (header >> 4) & 0x0F;

    switch (packet_type) {
        case MQTT_PUBLISH:
            mqtt_handle_publish(client, header, remaining_len);
            break;

        case MQTT_PUBACK:
        case MQTT_PUBREC:
        case MQTT_PUBREL:
        case MQTT_PUBCOMP:
            /* 处理QoS确认(简化实现) */
            if (remaining_len == 2) {
                uint8_t msg_id_buf[2];
                client->network.read(client->network.context, msg_id_buf, 2, 1000);
                uint16_t msg_id = (msg_id_buf[0] << 8) | msg_id_buf[1];
                if (packet_type == MQTT_PUBACK && client->on_publish != NULL) {
                    client->on_publish(client, msg_id);
                }
            }
            break;

        case MQTT_SUBACK:
            mqtt_handle_suback(client, remaining_len);
            break;

        case MQTT_UNSUBACK:
            /* 读取消息ID并丢弃 */
            if (remaining_len == 2) {
                uint8_t discard[2];
                client->network.read(client->network.context, discard, 2, 1000);
            }
            break;

        case MQTT_PINGRESP:
            mqtt_handle_pingresp(client);
            break;

        default:
            /* 未知或不应从服务器接收的报文类型，丢弃 */
            {
                uint8_t discard[256];
                while (remaining_len > 0) {
                    size_t to_read = remaining_len > 256 ? 256 : remaining_len;
                    client->network.read(client->network.context, discard, to_read, 1000);
                    remaining_len -= to_read;
                }
            }
            break;
    }

    return 1;
}

/* 获取连接状态 */
mqtt_state_t mqtt_client_state(mqtt_client_t *client) {
    if (client == NULL) {
        return MQTT_STATE_DISCONNECTED;
    }
    return client->state;
}

/* 检查是否已连接 */
bool mqtt_client_connected(mqtt_client_t *client) {
    return mqtt_client_state(client) == MQTT_STATE_CONNECTED;
}

/* 设置回调函数 */
void mqtt_client_set_callback(mqtt_client_t *client,
                               mqtt_on_connect_t on_connect,
                               mqtt_on_disconnect_t on_disconnect,
                               mqtt_on_message_t on_message) {
    if (client == NULL) {
        return;
    }
    client->on_connect = on_connect;
    client->on_disconnect = on_disconnect;
    client->on_message = on_message;
}
```

### 2.4 MQTT使用示例

```c
/**
 * @file mqtt_example.c
 * @brief MQTT客户端使用示例
 */

#include "mqtt_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

/* 网络上下文 */
typedef struct {
    int socket_fd;
} tcp_network_t;

/* TCP连接 */
static int tcp_connect(void *context, const char *host, uint16_t port) {
    tcp_network_t *net = (tcp_network_t *)context;

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    net->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (net->socket_fd < 0) {
        return -1;
    }

    /* 设置非阻塞模式 */
#ifdef _WIN32
    u_long mode = 1;
    ioctlsocket(net->socket_fd, FIONBIO, &mode);
#else
    int flags = fcntl(net->socket_fd, F_GETFL, 0);
    fcntl(net->socket_fd, F_SETFL, flags | O_NONBLOCK);
#endif

    struct hostent *server = gethostbyname(host);
    if (server == NULL) {
        return -1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    int ret = connect(net->socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));

#ifdef _WIN32
    if (ret < 0 && WSAGetLastError() != WSAEWOULDBLOCK) {
        closesocket(net->socket_fd);
        return -1;
    }
#else
    if (ret < 0 && errno != EINPROGRESS) {
        close(net->socket_fd);
        return -1;
    }
#endif

    /* 等待连接完成 */
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(net->socket_fd, &fdset);

    struct timeval tv;
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    ret = select(net->socket_fd + 1, NULL, &fdset, NULL, &tv);
    if (ret <= 0) {
#ifdef _WIN32
        closesocket(net->socket_fd);
#else
        close(net->socket_fd);
#endif
        return -1;
    }

    return 0;
}

/* TCP读取 */
static int tcp_read(void *context, uint8_t *buf, size_t len, uint32_t timeout_ms) {
    tcp_network_t *net = (tcp_network_t *)context;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(net->socket_fd, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(net->socket_fd + 1, &fdset, NULL, NULL, &tv);
    if (ret <= 0) {
        return 0;
    }

#ifdef _WIN32
    ret = recv(net->socket_fd, (char *)buf, (int)len, 0);
#else
    ret = (int)read(net->socket_fd, buf, len);
#endif

    return ret;
}

/* TCP写入 */
static int tcp_write(void *context, const uint8_t *buf, size_t len, uint32_t timeout_ms) {
    tcp_network_t *net = (tcp_network_t *)context;

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(net->socket_fd, &fdset);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int ret = select(net->socket_fd + 1, NULL, &fdset, NULL, &tv);
    if (ret <= 0) {
        return -1;
    }

#ifdef _WIN32
    ret = send(net->socket_fd, (const char *)buf, (int)len, 0);
#else
    ret = (int)write(net->socket_fd, buf, len);
#endif

    return ret;
}

/* TCP关闭 */
static void tcp_close(void *context) {
    tcp_network_t *net = (tcp_network_t *)context;
    if (net->socket_fd >= 0) {
#ifdef _WIN32
        closesocket(net->socket_fd);
        WSACleanup();
#else
        close(net->socket_fd);
#endif
        net->socket_fd = -1;
    }
}

/* MQTT事件回调 */
static void on_connect(mqtt_client_t *client, int return_code) {
    printf("MQTT Connected, return code: %d\n", return_code);
    if (return_code == 0) {
        /* 连接成功，订阅主题 */
        mqtt_client_subscribe(client, "sensor/+/temperature", MQTT_QOS_1);
        mqtt_client_subscribe(client, "control/commands", MQTT_QOS_1);
    }
}

static void on_disconnect(mqtt_client_t *client) {
    printf("MQTT Disconnected\n");
    (void)client;
}

static void on_message(mqtt_client_t *client, const char *topic,
                        const uint8_t *payload, size_t len, mqtt_qos_t qos) {
    printf("Received message on topic '%s' (QoS %d): ", topic, qos);
    fwrite(payload, 1, len, stdout);
    printf("\n");

    /* 处理控制命令 */
    if (strstr(topic, "control/commands") != NULL) {
        if (strncmp((const char *)payload, "REBOOT", len) == 0) {
            printf("Received reboot command\n");
            /* 执行重启逻辑 */
        }
    }
    (void)client;
}

int main(int argc, char *argv[]) {
    /* MQTT代理地址 */
    const char *broker_host = (argc > 1) ? argv[1] : "broker.hivemq.com";
    uint16_t broker_port = (argc > 2) ? (uint16_t)atoi(argv[2]) : 1883;

    printf("MQTT Client Example\n");
    printf("Connecting to %s:%d\n", broker_host, broker_port);

    /* 初始化网络 */
    tcp_network_t tcp_net = { .socket_fd = -1 };
    mqtt_network_t network = {
        .context = &tcp_net,
        .connect = tcp_connect,
        .read = tcp_read,
        .write = tcp_write,
        .close = tcp_close
    };

    /* 建立TCP连接 */
    if (tcp_connect(&tcp_net, broker_host, broker_port) != 0) {
        printf("Failed to connect to broker\n");
        return 1;
    }

    /* 初始化MQTT客户端 */
    mqtt_client_t client;
    mqtt_client_init(&client, &network);
    mqtt_client_set_callback(&client, on_connect, on_disconnect, on_message);

    /* 配置连接选项 */
    mqtt_connect_options_t options = {
        .client_id = "embedded_client_001",
        .username = NULL,
        .password = NULL,
        .will_topic = "status/embedded_client_001",
        .will_payload = (const uint8_t *)"offline",
        .will_payload_len = 7,
        .will_qos = MQTT_QOS_1,
        .will_retain = true,
        .keep_alive_sec = 60,
        .clean_session = true,
        .protocol_version = MQTT_PROTOCOL_VERSION_3_1_1
    };

    /* 连接MQTT代理 */
    if (mqtt_client_connect(&client, &options) != 0) {
        printf("Failed to connect to MQTT broker\n");
        return 1;
    }

    /* 发布在线状态 */
    mqtt_client_publish(&client, "status/embedded_client_001",
                         (const uint8_t *)"online", 6, MQTT_QOS_1, true);

    /* 模拟传感器数据发送 */
    int counter = 0;
    while (mqtt_client_connected(&client) && counter < 100) {
        /* 处理接收 */
        mqtt_client_process(&client, 100);

        /* 每10次循环发送一次数据 */
        if (counter % 10 == 0) {
            char payload[256];
            int temp = 20 + (counter % 15); /* 模拟温度 20-35°C */
            snprintf(payload, sizeof(payload),
                     "{\"device\":\"sensor_001\",\"temperature\":%d,\"humidity\":%d}",
                     temp, 50 + (counter % 30));

            mqtt_client_publish(&client, "sensor/room1/temperature",
                                 (const uint8_t *)payload, strlen(payload),
                                 MQTT_QOS_1, false);
            printf("Published: %s\n", payload);
        }

#ifdef _WIN32
        Sleep(100);
#else
        usleep(100000);
#endif
        counter++;
    }

    /* 断开连接 */
    mqtt_client_disconnect(&client);

    printf("MQTT client example completed\n");
    return 0;
}
```

---

## 3. CoAP协议实现

### 3.1 CoAP协议概述

CoAP (Constrained Application Protocol) 是专为受限设备和网络设计的Web传输协议，基于UDP实现。

**核心特性：**

- RESTful架构，类似HTTP但轻量
- 基于UDP，支持组播
- 支持观察者模式(Observing)
- 块传输(Block-wise Transfer)支持大消息
- 内置重传机制

### 3.2 CoAP数据包格式

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Ver| T |  TKL  |      Code     |          Message ID           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Token (if any, TKL bytes) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Options (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|1 1 1 1 1 1 1 1|    Payload (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

版本(Ver): 2位，固定为1
类型(T): 2位 (CON=0, NON=1, ACK=2, RST=3)
令牌长度(TKL): 4位
代码(Code): 8位 (c.dd格式，c=类别，dd=详情)
消息ID: 16位
```

### 3.3 完整CoAP客户端实现

```c
/**
 * @file coap_client.h
 * @brief CoAP客户端实现 - 轻量级受限设备版本
 */

#ifndef COAP_CLIENT_H
#define COAP_CLIENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CoAP版本 */
#define COAP_VERSION 1

/* CoAP消息类型 */
typedef enum {
    COAP_TYPE_CON = 0,      /* Confirmable - 需要确认 */
    COAP_TYPE_NON = 1,      /* Non-confirmable - 不需要确认 */
    COAP_TYPE_ACK = 2,      /* Acknowledgment - 确认 */
    COAP_TYPE_RST = 3       /* Reset - 复位 */
} coap_msg_type_t;

/* CoAP请求方法 (Code 0.01-0.31) */
typedef enum {
    COAP_METHOD_GET = 1,    /* 0.01 */
    COAP_METHOD_POST = 2,   /* 0.02 */
    COAP_METHOD_PUT = 3,    /* 0.03 */
    COAP_METHOD_DELETE = 4  /* 0.04 */
} coap_method_t;

/* CoAP响应码 (Code 2.00-5.31) */
typedef enum {
    /* 成功 2.xx */
    COAP_CODE_CREATED = 0x41,           /* 2.01 */
    COAP_CODE_DELETED = 0x42,           /* 2.02 */
    COAP_CODE_VALID = 0x43,             /* 2.03 */
    COAP_CODE_CHANGED = 0x44,           /* 2.04 */
    COAP_CODE_CONTENT = 0x45,           /* 2.05 */

    /* 客户端错误 4.xx */
    COAP_CODE_BAD_REQUEST = 0x80,       /* 4.00 */
    COAP_CODE_UNAUTHORIZED = 0x81,      /* 4.01 */
    COAP_CODE_BAD_OPTION = 0x82,        /* 4.02 */
    COAP_CODE_FORBIDDEN = 0x83,         /* 4.03 */
    COAP_CODE_NOT_FOUND = 0x84,         /* 4.04 */
    COAP_CODE_METHOD_NOT_ALLOWED = 0x85, /* 4.05 */
    COAP_CODE_NOT_ACCEPTABLE = 0x86,    /* 4.06 */
    COAP_CODE_PRECONDITION_FAILED = 0x8C, /* 4.12 */
    COAP_CODE_REQUEST_TOO_LARGE = 0x8D, /* 4.13 */
    COAP_CODE_UNSUPPORTED_FORMAT = 0x8F, /* 4.15 */

    /* 服务器错误 5.xx */
    COAP_CODE_INTERNAL_ERROR = 0xA0,    /* 5.00 */
    COAP_CODE_NOT_IMPLEMENTED = 0xA1,   /* 5.01 */
    COAP_CODE_BAD_GATEWAY = 0xA2,       /* 5.02 */
    COAP_CODE_UNAVAILABLE = 0xA3,       /* 5.03 */
    COAP_CODE_GATEWAY_TIMEOUT = 0xA4,   /* 5.04 */
    COAP_CODE_PROXYING_NOT_SUPPORTED = 0xA5 /* 5.05 */
} coap_response_code_t;

/* CoAP选项 */
typedef enum {
    COAP_OPTION_IF_MATCH = 1,
    COAP_OPTION_URI_HOST = 3,
    COAP_OPTION_ETAG = 4,
    COAP_OPTION_IF_NONE_MATCH = 5,
    COAP_OPTION_URI_PORT = 7,
    COAP_OPTION_LOCATION_PATH = 8,
    COAP_OPTION_URI_PATH = 11,
    COAP_OPTION_CONTENT_FORMAT = 12,
    COAP_OPTION_MAX_AGE = 14,
    COAP_OPTION_URI_QUERY = 15,
    COAP_OPTION_ACCEPT = 17,
    COAP_OPTION_LOCATION_QUERY = 20,
    COAP_OPTION_BLOCK2 = 23,
    COAP_OPTION_BLOCK1 = 27,
    COAP_OPTION_SIZE2 = 28,
    COAP_OPTION_PROXY_URI = 35,
    COAP_OPTION_PROXY_SCHEME = 39,
    COAP_OPTION_SIZE1 = 60
} coap_option_num_t;

/* 内容格式 */
typedef enum {
    COAP_FORMAT_TEXT_PLAIN = 0,
    COAP_FORMAT_LINK_FORMAT = 40,
    COAP_FORMAT_XML = 41,
    COAP_FORMAT_OCTET_STREAM = 42,
    COAP_FORMAT_JSON = 50,
    COAP_FORMAT_CBOR = 60
} coap_content_format_t;

/* 最大配置 */
#define COAP_MAX_PAYLOAD_LEN    1024
#define COAP_MAX_OPTIONS        16
#define COAP_MAX_TOKEN_LEN      8
#define COAP_MAX_URI_LEN        128
#define COAP_MAX_PACKET_LEN     1280

/* 默认端口 */
#define COAP_DEFAULT_PORT       5683
#define COAPS_DEFAULT_PORT      5684

/* 前向声明 */
struct coap_client;
typedef struct coap_client coap_client_t;

/* 网络回调 */
typedef int (*coap_network_send_t)(void *context, const uint8_t *data, size_t len,
                                    const void *addr, size_t addr_len);
typedef int (*coap_network_recv_t)(void *context, uint8_t *data, size_t max_len,
                                    void *addr, size_t *addr_len, uint32_t timeout_ms);

/* 请求回调 */
typedef void (*coap_response_callback_t)(coap_client_t *client, uint8_t code,
                                          const uint8_t *payload, size_t len,
                                          void *user_data);

/* CoAP选项结构 */
typedef struct {
    uint16_t number;
    uint8_t value[32];
    size_t len;
} coap_option_t;

/* CoAP消息结构 */
typedef struct {
    uint8_t version;
    coap_msg_type_t type;
    uint8_t code;
    uint16_t msg_id;
    uint8_t token[COAP_MAX_TOKEN_LEN];
    uint8_t token_len;
    coap_option_t options[COAP_MAX_OPTIONS];
    uint8_t option_count;
    uint8_t payload[COAP_MAX_PAYLOAD_LEN];
    size_t payload_len;
} coap_message_t;

/* 挂起请求 */
typedef struct {
    uint16_t msg_id;
    uint8_t token[COAP_MAX_TOKEN_LEN];
    uint8_t token_len;
    coap_msg_type_t type;
    uint32_t timestamp;
    uint8_t retry_count;
    coap_response_callback_t callback;
    void *user_data;
    bool active;
} coap_pending_request_t;

/* CoAP客户端结构 */
struct coap_client {
    void *network_context;
    coap_network_send_t network_send;
    coap_network_recv_t network_recv;

    /* 挂起请求列表 */
    coap_pending_request_t pending[8];

    /* 消息ID计数器 */
    uint16_t next_msg_id;

    /* 缓冲区 */
    uint8_t tx_buffer[COAP_MAX_PACKET_LEN];
    uint8_t rx_buffer[COAP_MAX_PACKET_LEN];

    /* 重传配置 */
    uint32_t ack_timeout_ms;
    uint8_t max_retransmit;

    /* 观察者列表(简化实现) */
    struct {
        char resource[64];
        uint32_t last_observe_seq;
        bool active;
    } observations[4];
};

/* 函数声明 */
int coap_client_init(coap_client_t *client, void *network_context,
                      coap_network_send_t send_func, coap_network_recv_t recv_func);
int coap_request(coap_client_t *client, coap_method_t method,
                  const char *uri, const uint8_t *payload, size_t payload_len,
                  coap_content_format_t content_type,
                  coap_response_callback_t callback, void *user_data);
int coap_get(coap_client_t *client, const char *uri,
              coap_response_callback_t callback, void *user_data);
int coap_post(coap_client_t *client, const char *uri,
               const uint8_t *payload, size_t len,
               coap_content_format_t format,
               coap_response_callback_t callback, void *user_data);
int coap_put(coap_client_t *client, const char *uri,
              const uint8_t *payload, size_t len,
              coap_content_format_t format,
              coap_response_callback_t callback, void *user_data);
int coap_delete(coap_client_t *client, const char *uri,
                 coap_response_callback_t callback, void *user_data);
int coap_observe(coap_client_t *client, const char *uri,
                  coap_response_callback_t callback, void *user_data);
int coap_cancel_observe(coap_client_t *client, const char *uri);
int coap_process(coap_client_t *client, uint32_t timeout_ms);

/* 辅助函数 */
int coap_build_uri_options(const char *uri, coap_option_t *options, int max_options);
const char* coap_response_code_str(uint8_t code);

#ifdef __cplusplus
}
#endif

#endif /* COAP_CLIENT_H */
```

```c
/**
 * @file coap_client.c
 * @brief CoAP客户端实现
 */

#include "coap_client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
    #include <arpa/inet.h>
#else
    #include <winsock2.h>
#endif

#ifdef _WIN32
    #include <windows.h>
    #define get_tick_ms() GetTickCount()
#else
    #include <time.h>
    static uint32_t get_tick_ms(void) {
        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
    }
#endif

/* 编码选项 */
static int coap_encode_option(uint8_t *buf, uint16_t delta, const uint8_t *value, size_t len) {
    int pos = 0;

    /* 计算扩展 */
    uint8_t delta_ext = 0;
    uint8_t len_ext = 0;
    uint16_t delta_val = delta;
    uint16_t len_val = (uint16_t)len;

    if (delta >= 269) {
        delta_val = 14;
        delta_ext = 2;
    } else if (delta >= 13) {
        delta_val = 13;
        delta_ext = 1;
    }

    if (len >= 269) {
        len_val = 14;
        len_ext = 2;
    } else if (len >= 13) {
        len_val = 13;
        len_ext = 1;
    }

    buf[pos++] = (uint8_t)((delta_val << 4) | (len_val & 0x0F));

    if (delta_ext == 1) {
        buf[pos++] = (uint8_t)(delta - 13);
    } else if (delta_ext == 2) {
        uint16_t ext = delta - 269;
        buf[pos++] = (uint8_t)(ext >> 8);
        buf[pos++] = (uint8_t)(ext & 0xFF);
    }

    if (len_ext == 1) {
        buf[pos++] = (uint8_t)(len - 13);
    } else if (len_ext == 2) {
        uint16_t ext = (uint16_t)len - 269;
        buf[pos++] = (uint8_t)(ext >> 8);
        buf[pos++] = (uint8_t)(ext & 0xFF);
    }

    memcpy(buf + pos, value, len);
    pos += (int)len;

    return pos;
}

/* 序列化消息 */
static int coap_serialize(const coap_message_t *msg, uint8_t *buf, size_t buf_len) {
    if (buf_len < 4) return -1;

    int pos = 0;

    /* 首字节: 版本 + 类型 + TKL */
    buf[pos++] = (uint8_t)((COAP_VERSION << 6) |
                           ((msg->type & 0x03) << 4) |
                           (msg->token_len & 0x0F));

    /* 代码 */
    buf[pos++] = msg->code;

    /* 消息ID */
    buf[pos++] = (uint8_t)(msg->msg_id >> 8);
    buf[pos++] = (uint8_t)(msg->msg_id & 0xFF);

    /* Token */
    if (msg->token_len > 0 && msg->token_len <= 8) {
        if (pos + msg->token_len > (int)buf_len) return -1;
        memcpy(buf + pos, msg->token, msg->token_len);
        pos += msg->token_len;
    }

    /* 选项 */
    uint16_t prev_option = 0;
    for (int i = 0; i < msg->option_count; i++) {
        uint16_t delta = msg->options[i].number - prev_option;
        int opt_len = coap_encode_option(buf + pos, delta,
                                          msg->options[i].value,
                                          msg->options[i].len);
        if (pos + opt_len > (int)buf_len) return -1;
        pos += opt_len;
        prev_option = msg->options[i].number;
    }

    /* 分隔符和Payload */
    if (msg->payload_len > 0) {
        if (pos + 1 + msg->payload_len > (int)buf_len) return -1;
        buf[pos++] = 0xFF; /* Payload分隔符 */
        memcpy(buf + pos, msg->payload, msg->payload_len);
        pos += (int)msg->payload_len;
    }

    return pos;
}

/* 反序列化消息 */
static int coap_deserialize(coap_message_t *msg, const uint8_t *buf, size_t len) {
    if (len < 4) return -1;

    int pos = 0;

    /* 首字节 */
    msg->version = (buf[pos] >> 6) & 0x03;
    msg->type = (coap_msg_type_t)((buf[pos] >> 4) & 0x03);
    msg->token_len = buf[pos++] & 0x0F;

    if (msg->version != COAP_VERSION || msg->token_len > 8) {
        return -1;
    }

    /* 代码 */
    msg->code = buf[pos++];

    /* 消息ID */
    msg->msg_id = (uint16_t)((buf[pos] << 8) | buf[pos + 1]);
    pos += 2;

    /* Token */
    if (msg->token_len > 0) {
        if (pos + msg->token_len > (int)len) return -1;
        memcpy(msg->token, buf + pos, msg->token_len);
        pos += msg->token_len;
    }

    /* 选项 */
    msg->option_count = 0;
    msg->payload_len = 0;
    uint16_t prev_option = 0;

    while (pos < (int)len && buf[pos] != 0xFF) {
        if (msg->option_count >= COAP_MAX_OPTIONS) return -1;

        uint8_t opt_header = buf[pos++];
        uint16_t delta = (opt_header >> 4) & 0x0F;
        uint16_t opt_len = opt_header & 0x0F;

        if (delta == 13) {
            if (pos >= (int)len) return -1;
            delta = 13 + buf[pos++];
        } else if (delta == 14) {
            if (pos + 1 >= (int)len) return -1;
            delta = 269 + ((buf[pos] << 8) | buf[pos + 1]);
            pos += 2;
        } else if (delta == 15) {
            return -1; /* 保留值 */
        }

        if (opt_len == 13) {
            if (pos >= (int)len) return -1;
            opt_len = 13 + buf[pos++];
        } else if (opt_len == 14) {
            if (pos + 1 >= (int)len) return -1;
            opt_len = 269 + ((buf[pos] << 8) | buf[pos + 1]);
            pos += 2;
        } else if (opt_len == 15) {
            return -1; /* 保留值 */
        }

        msg->options[msg->option_count].number = prev_option + delta;
        if (opt_len > sizeof(msg->options[0].value)) opt_len = sizeof(msg->options[0].value);
        msg->options[msg->option_count].len = (size_t)opt_len;

        if (pos + opt_len > (int)len) return -1;
        memcpy(msg->options[msg->option_count].value, buf + pos, opt_len);
        pos += (int)opt_len;

        prev_option = msg->options[msg->option_count].number;
        msg->option_count++;
    }

    /* Payload */
    if (pos < (int)len && buf[pos] == 0xFF) {
        pos++; /* 跳过分隔符 */
        msg->payload_len = len - pos;
        if (msg->payload_len > COAP_MAX_PAYLOAD_LEN) {
            msg->payload_len = COAP_MAX_PAYLOAD_LEN;
        }
        memcpy(msg->payload, buf + pos, msg->payload_len);
    }

    return 0;
}

/* 生成Token */
static void generate_token(uint8_t *token, uint8_t *len) {
    static uint32_t seed = 1;
    seed = seed * 1103515245 + 12345;
    *len = 4;
    token[0] = (uint8_t)(seed >> 24);
    token[1] = (uint8_t)(seed >> 16);
    token[2] = (uint8_t)(seed >> 8);
    token[3] = (uint8_t)(seed);
}

/* 解析URI并构建选项 */
int coap_build_uri_options(const char *uri, coap_option_t *options, int max_options) {
    char host[64] = {0};
    char port_str[8] = {0};
    char path[128] = {0};
    char query[128] = {0};

    int opt_count = 0;

    /* 简单URI解析 (格式: coap://host:port/path?query) */
    const char *p = uri;
    if (strncmp(p, "coap://", 7) == 0) {
        p += 7;
    }

    /* 提取host */
    int i = 0;
    while (*p && *p != ':' && *p != '/' && i < 63) {
        host[i++] = *p++;
    }
    host[i] = '\0';

    /* 添加URI_HOST选项 */
    if (opt_count < max_options && host[0]) {
        options[opt_count].number = COAP_OPTION_URI_HOST;
        memcpy(options[opt_count].value, host, i);
        options[opt_count].len = i;
        opt_count++;
    }

    /* 提取port */
    if (*p == ':') {
        p++;
        i = 0;
        while (*p && *p != '/' && i < 7) {
            port_str[i++] = *p++;
        }
        port_str[i] = '\0';

        if (opt_count < max_options && port_str[0]) {
            uint16_t port = (uint16_t)atoi(port_str);
            options[opt_count].number = COAP_OPTION_URI_PORT;
            options[opt_count].value[0] = (uint8_t)(port >> 8);
            options[opt_count].value[1] = (uint8_t)(port & 0xFF);
            options[opt_count].len = 2;
            opt_count++;
        }
    }

    /* 提取path */
    if (*p == '/') {
        p++;
        i = 0;
        int path_start = 0;
        while (*p && *p != '?' && i < 127) {
            if (*p == '/') {
                /* 添加路径段 */
                if (opt_count < max_options && i > path_start) {
                    options[opt_count].number = COAP_OPTION_URI_PATH;
                    memcpy(options[opt_count].value, path + path_start, i - path_start);
                    options[opt_count].len = i - path_start;
                    opt_count++;
                    path_start = i + 1;
                }
            }
            path[i++] = *p++;
        }
        path[i] = '\0';

        /* 添加最后一个路径段 */
        if (opt_count < max_options && i > path_start) {
            options[opt_count].number = COAP_OPTION_URI_PATH;
            memcpy(options[opt_count].value, path + path_start, i - path_start);
            options[opt_count].len = i - path_start;
            opt_count++;
        }
    }

    /* 提取query */
    if (*p == '?') {
        p++;
        i = 0;
        int query_start = 0;
        while (*p && i < 127) {
            if (*p == '&') {
                if (opt_count < max_options && i > query_start) {
                    options[opt_count].number = COAP_OPTION_URI_QUERY;
                    memcpy(options[opt_count].value, query + query_start, i - query_start);
                    options[opt_count].len = i - query_start;
                    opt_count++;
                    query_start = i + 1;
                }
            }
            query[i++] = *p++;
        }
        query[i] = '\0';

        if (opt_count < max_options && i > query_start) {
            options[opt_count].number = COAP_OPTION_URI_QUERY;
            memcpy(options[opt_count].value, query + query_start, i - query_start);
            options[opt_count].len = i - query_start;
            opt_count++;
        }
    }

    return opt_count;
}

/* 初始化客户端 */
int coap_client_init(coap_client_t *client, void *network_context,
                      coap_network_send_t send_func, coap_network_recv_t recv_func) {
    if (client == NULL || send_func == NULL || recv_func == NULL) {
        return -1;
    }

    memset(client, 0, sizeof(coap_client_t));
    client->network_context = network_context;
    client->network_send = send_func;
    client->network_recv = recv_func;
    client->next_msg_id = (uint16_t)(get_tick_ms() & 0xFFFF);
    client->ack_timeout_ms = 2000;
    client->max_retransmit = 4;

    return 0;
}

/* 查找挂起的请求 */
static coap_pending_request_t* find_pending_request(coap_client_t *client,
                                                     uint16_t msg_id) {
    for (int i = 0; i < 8; i++) {
        if (client->pending[i].active && client->pending[i].msg_id == msg_id) {
            return &client->pending[i];
        }
    }
    return NULL;
}

/* 添加挂起请求 */
static coap_pending_request_t* add_pending_request(coap_client_t *client,
                                                    uint16_t msg_id,
                                                    const uint8_t *token,
                                                    uint8_t token_len,
                                                    coap_msg_type_t type,
                                                    coap_response_callback_t callback,
                                                    void *user_data) {
    for (int i = 0; i < 8; i++) {
        if (!client->pending[i].active) {
            client->pending[i].active = true;
            client->pending[i].msg_id = msg_id;
            memcpy(client->pending[i].token, token, token_len);
            client->pending[i].token_len = token_len;
            client->pending[i].type = type;
            client->pending[i].timestamp = get_tick_ms();
            client->pending[i].retry_count = 0;
            client->pending[i].callback = callback;
            client->pending[i].user_data = user_data;
            return &client->pending[i];
        }
    }
    return NULL;
}

/* 发送请求 */
int coap_request(coap_client_t *client, coap_method_t method,
                  const char *uri, const uint8_t *payload, size_t payload_len,
                  coap_content_format_t content_type,
                  coap_response_callback_t callback, void *user_data) {
    if (client == NULL || uri == NULL) {
        return -1;
    }

    coap_message_t msg;
    memset(&msg, 0, sizeof(msg));

    /* 构建消息 */
    msg.version = COAP_VERSION;
    msg.type = (callback != NULL) ? COAP_TYPE_CON : COAP_TYPE_NON;
    msg.code = (uint8_t)method;
    msg.msg_id = client->next_msg_id++;

    generate_token(msg.token, &msg.token_len);

    /* 添加URI选项 */
    coap_option_t uri_options[COAP_MAX_OPTIONS];
    int uri_opt_count = coap_build_uri_options(uri, uri_options, COAP_MAX_OPTIONS);

    for (int i = 0; i < uri_opt_count && msg.option_count < COAP_MAX_OPTIONS; i++) {
        msg.options[msg.option_count++] = uri_options[i];
    }

    /* 添加Content-Format选项 */
    if (payload_len > 0 && msg.option_count < COAP_MAX_OPTIONS) {
        msg.options[msg.option_count].number = COAP_OPTION_CONTENT_FORMAT;
        if (content_type < 256) {
            msg.options[msg.option_count].value[0] = (uint8_t)content_type;
            msg.options[msg.option_count].len = 1;
        } else {
            msg.options[msg.option_count].value[0] = (uint8_t)(content_type >> 8);
            msg.options[msg.option_count].value[1] = (uint8_t)(content_type & 0xFF);
            msg.options[msg.option_count].len = 2;
        }
        msg.option_count++;
    }

    /* 添加Payload */
    if (payload != NULL && payload_len > 0) {
        if (payload_len > COAP_MAX_PAYLOAD_LEN) {
            payload_len = COAP_MAX_PAYLOAD_LEN;
        }
        memcpy(msg.payload, payload, payload_len);
        msg.payload_len = payload_len;
    }

    /* 序列化 */
    int packet_len = coap_serialize(&msg, client->tx_buffer, COAP_MAX_PACKET_LEN);
    if (packet_len < 0) {
        return -1;
    }

    /* 添加到挂起列表 */
    if (callback != NULL) {
        coap_pending_request_t *pending = add_pending_request(client, msg.msg_id,
                                                               msg.token, msg.token_len,
                                                               msg.type, callback, user_data);
        if (pending == NULL) {
            return -1;
        }
    }

    /* 发送 */
    return client->network_send(client->network_context, client->tx_buffer,
                                 packet_len, NULL, 0);
}

/* GET请求 */
int coap_get(coap_client_t *client, const char *uri,
              coap_response_callback_t callback, void *user_data) {
    return coap_request(client, COAP_METHOD_GET, uri, NULL, 0, 0, callback, user_data);
}

/* POST请求 */
int coap_post(coap_client_t *client, const char *uri,
               const uint8_t *payload, size_t len,
               coap_content_format_t format,
               coap_response_callback_t callback, void *user_data) {
    return coap_request(client, COAP_METHOD_POST, uri, payload, len, format, callback, user_data);
}

/* PUT请求 */
int coap_put(coap_client_t *client, const char *uri,
              const uint8_t *payload, size_t len,
              coap_content_format_t format,
              coap_response_callback_t callback, void *user_data) {
    return coap_request(client, COAP_METHOD_PUT, uri, payload, len, format, callback, user_data);
}

/* DELETE请求 */
int coap_delete(coap_client_t *client, const char *uri,
                 coap_response_callback_t callback, void *user_data) {
    return coap_request(client, COAP_METHOD_DELETE, uri, NULL, 0, 0, callback, user_data);
}

/* 观察资源 */
int coap_observe(coap_client_t *client, const char *uri,
                  coap_response_callback_t callback, void *user_data) {
    /* 添加Observe选项 */
    /* 简化实现：直接发送GET请求 */
    return coap_get(client, uri, callback, user_data);
}

/* 取消观察 */
int coap_cancel_observe(coap_client_t *client, const char *uri) {
    (void)client;
    (void)uri;
    /* 简化实现 */
    return 0;
}

/* 发送ACK */
static void send_ack(coap_client_t *client, uint16_t msg_id) {
    coap_message_t ack;
    memset(&ack, 0, sizeof(ack));
    ack.version = COAP_VERSION;
    ack.type = COAP_TYPE_ACK;
    ack.code = 0;
    ack.msg_id = msg_id;

    int len = coap_serialize(&ack, client->tx_buffer, COAP_MAX_PACKET_LEN);
    if (len > 0) {
        client->network_send(client->network_context, client->tx_buffer, len, NULL, 0);
    }
}

/* 处理响应 */
static void handle_response(coap_client_t *client, const coap_message_t *msg) {
    /* 查找匹配的挂起请求 */
    coap_pending_request_t *pending = find_pending_request(client, msg->msg_id);

    if (pending == NULL) {
        /* 可能是token匹配 */
        for (int i = 0; i < 8; i++) {
            if (client->pending[i].active &&
                client->pending[i].token_len == msg->token_len &&
                memcmp(client->pending[i].token, msg->token, msg->token_len) == 0) {
                pending = &client->pending[i];
                break;
            }
        }
    }

    if (pending != NULL && pending->callback != NULL) {
        pending->callback(client, msg->code, msg->payload, msg->payload_len,
                          pending->user_data);
        pending->active = false;
    }
}

/* 检查重传 */
static void check_retransmissions(coap_client_t *client) {
    uint32_t now = get_tick_ms();

    for (int i = 0; i < 8; i++) {
        if (client->pending[i].active && client->pending[i].type == COAP_TYPE_CON) {
            uint32_t elapsed = now - client->pending[i].timestamp;
            uint32_t timeout = client->ack_timeout_ms << client->pending[i].retry_count;

            if (elapsed > timeout) {
                if (client->pending[i].retry_count < client->max_retransmit) {
                    /* 重传 */
                    client->pending[i].retry_count++;
                    client->pending[i].timestamp = now;
                    /* 简化：实际应重传原始消息 */
                } else {
                    /* 超时 */
                    if (client->pending[i].callback != NULL) {
                        client->pending[i].callback(client, COAP_CODE_GATEWAY_TIMEOUT,
                                                     NULL, 0, client->pending[i].user_data);
                    }
                    client->pending[i].active = false;
                }
            }
        }
    }
}

/* 处理循环 */
int coap_process(coap_client_t *client, uint32_t timeout_ms) {
    if (client == NULL) {
        return -1;
    }

    /* 检查重传 */
    check_retransmissions(client);

    /* 接收响应 */
    size_t addr_len = 0;
    int len = client->network_recv(client->network_context, client->rx_buffer,
                                    COAP_MAX_PACKET_LEN, NULL, &addr_len, timeout_ms);

    if (len > 0) {
        coap_message_t msg;
        if (coap_deserialize(&msg, client->rx_buffer, len) == 0) {
            switch (msg.type) {
                case COAP_TYPE_CON:
                    /* 收到CON消息，需要ACK */
                    send_ack(client, msg.msg_id);
                    /* 处理消息内容 */
                    break;

                case COAP_TYPE_ACK:
                case COAP_TYPE_NON:
                    /* 响应消息 */
                    handle_response(client, &msg);
                    break;

                case COAP_TYPE_RST:
                    /* 复位消息 */
                    {
                        coap_pending_request_t *p = find_pending_request(client, msg.msg_id);
                        if (p != NULL) {
                            if (p->callback != NULL) {
                                p->callback(client, COAP_CODE_INTERNAL_ERROR, NULL, 0,
                                           p->user_data);
                            }
                            p->active = false;
                        }
                    }
                    break;
            }
            return 1;
        }
    }

    return 0;
}

/* 获取响应码描述 */
const char* coap_response_code_str(uint8_t code) {
    uint8_t class_code = (code >> 5) & 0x07;
    uint8_t detail = code & 0x1F;

    switch (class_code) {
        case 2:
            switch (detail) {
                case 1: return "2.01 Created";
                case 2: return "2.02 Deleted";
                case 3: return "2.03 Valid";
                case 4: return "2.04 Changed";
                case 5: return "2.05 Content";
                default: return "2.xx Success";
            }
        case 4:
            switch (detail) {
                case 0: return "4.00 Bad Request";
                case 1: return "4.01 Unauthorized";
                case 2: return "4.02 Bad Option";
                case 3: return "4.03 Forbidden";
                case 4: return "4.04 Not Found";
                case 5: return "4.05 Method Not Allowed";
                default: return "4.xx Client Error";
            }
        case 5:
            switch (detail) {
                case 0: return "5.00 Internal Server Error";
                case 1: return "5.01 Not Implemented";
                case 2: return "5.02 Bad Gateway";
                case 3: return "5.03 Service Unavailable";
                case 4: return "5.04 Gateway Timeout";
                default: return "5.xx Server Error";
            }
        default:
            return "Unknown";
    }
}
```

### 3.4 CoAP使用示例

```c
/**
 * @file coap_example.c
 * @brief CoAP客户端使用示例
 */

#include "coap_client.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <fcntl.h>
#endif

/* UDP网络上下文 */
typedef struct {
    int socket_fd;
    struct sockaddr_in server_addr;
} udp_network_t;

/* UDP发送 */
static int udp_send(void *context, const uint8_t *data, size_t len,
                     const void *addr, size_t addr_len) {
    udp_network_t *net = (udp_network_t *)context;
    (void)addr;
    (void)addr_len;

    ssize_t sent = sendto(net->socket_fd, (const char *)data, (int)len, 0,
                          (struct sockaddr *)&net->server_addr, sizeof(net->server_addr));
    return (sent > 0) ? (int)sent : -1;
}

/* UDP接收 */
static int udp_recv(void *context, uint8_t *data, size_t max_len,
                     void *addr, size_t *addr_len, uint32_t timeout_ms) {
    udp_network_t *net = (udp_network_t *)context;
    (void)addr;
    (void)addr_len;

    /* 设置超时 */
#ifdef _WIN32
    DWORD timeout = timeout_ms;
    setsockopt(net->socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout));
#else
    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    setsockopt(net->socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#endif

    struct sockaddr_in from_addr;
    socklen_t from_len = sizeof(from_addr);

    ssize_t recvd = recvfrom(net->socket_fd, (char *)data, (int)max_len, 0,
                              (struct sockaddr *)&from_addr, &from_len);
    return (recvd > 0) ? (int)recvd : 0;
}

/* 响应回调 */
static void response_callback(coap_client_t *client, uint8_t code,
                               const uint8_t *payload, size_t len, void *user_data) {
    printf("CoAP Response [%s]:\n", coap_response_code_str(code));
    if (len > 0) {
        printf("%.*s\n", (int)len, (const char *)payload);
    }
    (void)client;
    (void)user_data;
}

int main(int argc, char *argv[]) {
    const char *server = (argc > 1) ? argv[1] : "coap.me";
    uint16_t port = (argc > 2) ? (uint16_t)atoi(argv[2]) : 5683;

    printf("CoAP Client Example\n");
    printf("Server: %s:%d\n", server, port);

#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    /* 创建UDP socket */
    udp_network_t udp_net;
    udp_net.socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udp_net.socket_fd < 0) {
        printf("Failed to create socket\n");
        return 1;
    }

    /* 设置服务器地址 */
    memset(&udp_net.server_addr, 0, sizeof(udp_net.server_addr));
    udp_net.server_addr.sin_family = AF_INET;
    udp_net.server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server, &udp_net.server_addr.sin_addr);

    /* 初始化CoAP客户端 */
    coap_client_t client;
    coap_client_init(&client, &udp_net, udp_send, udp_recv);

    /* 测试1: GET请求 */
    printf("\n--- Test 1: GET /hello ---\n");
    char uri[256];
    snprintf(uri, sizeof(uri), "coap://%s:%d/hello", server, port);
    coap_get(&client, uri, response_callback, NULL);

    /* 等待响应 */
    for (int i = 0; i < 50; i++) {
        coap_process(&client, 100);
    }

    /* 测试2: GET /large */
    printf("\n--- Test 2: GET /large ---\n");
    snprintf(uri, sizeof(uri), "coap://%s:%d/large", server, port);
    coap_get(&client, uri, response_callback, NULL);

    for (int i = 0; i < 50; i++) {
        coap_process(&client, 100);
    }

    /* 测试3: POST请求 */
    printf("\n--- Test 3: POST /echo ---\n");
    snprintf(uri, sizeof(uri), "coap://%s:%d/echo", server, port);
    const char *post_data = "Hello CoAP!";
    coap_post(&client, uri, (const uint8_t *)post_data, strlen(post_data),
               COAP_FORMAT_TEXT_PLAIN, response_callback, NULL);

    for (int i = 0; i < 50; i++) {
        coap_process(&client, 100);
    }

    /* 清理 */
#ifdef _WIN32
    closesocket(udp_net.socket_fd);
    WSACleanup();
#else
    close(udp_net.socket_fd);
#endif

    printf("\nCoAP client example completed\n");
    return 0;
}
```

---

## 4. Modbus RTU/TCP实现

### 4.1 Modbus协议概述

Modbus是工业领域广泛使用的串行通信协议，由Modicon公司于1979年开发。支持RTU(二进制)和ASCII两种串行模式，以及TCP模式。

**关键特性：**
- 主从架构，一主多从
- 功能码定义操作类型
- CRC/LRC校验保证数据完整性
- 简单可靠，工业标准

### 4.2 Modbus数据模型

```
┌─────────────────────────────────────────────────────────────┐
│                    Modbus数据模型                           │
├──────────────┬──────────────┬──────────────┬────────────────┤
│   离散量输入 │   线圈       │   输入寄存器 │   保持寄存器   │
│  (1 bit)     │  (1 bit)     │  (16 bits)   │  (16 bits)     │
├──────────────┼──────────────┼──────────────┼────────────────┤
│   只读       │   读写       │   只读       │   读写         │
├──────────────┼──────────────┼───────────��──┼────────────────┤
│  10001-19999 │  1-9999      │ 30001-39999  │  40001-49999   │
│  (1-based)   │  (1-based)   │  (1-based)   │  (1-based)     │
└──────────────┴──────────────┴──────────────┴────────────────┘
```

### 4.3 完整Modbus实现

```c
/**
 * @file modbus.h
 * @brief Modbus RTU/TCP协议实现
 */

#ifndef MODBUS_H
#define MODBUS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Modbus模式 */
typedef enum {
    MODBUS_RTU,     /* 串行RTU模式 */
    MODBUS_ASCII,   /* 串行ASCII模式 */
    MODBUS_TCP      /* TCP模式 */
} modbus_mode_t;

/* 功能码 */
typedef enum {
    MODBUS_FC_READ_COILS = 0x01,
    MODBUS_FC_READ_DISCRETE_INPUTS = 0x02,
    MODBUS_FC_READ_HOLDING_REGISTERS = 0x03,
    MODBUS_FC_READ_INPUT_REGISTERS = 0x04,
    MODBUS_FC_WRITE_SINGLE_COIL = 0x05,
    MODBUS_FC_WRITE_SINGLE_REGISTER = 0x06,
    MODBUS_FC_READ_EXCEPTION_STATUS = 0x07,
    MODBUS_FC_DIAGNOSTICS = 0x08,
    MODBUS_FC_GET_COMM_EVENT_COUNTER = 0x0B,
    MODBUS_FC_GET_COMM_EVENT_LOG = 0x0C,
    MODBUS_FC_WRITE_MULTIPLE_COILS = 0x0F,
    MODBUS_FC_WRITE_MULTIPLE_REGISTERS = 0x10,
    MODBUS_FC_REPORT_SLAVE_ID = 0x11,
    MODBUS_FC_READ_FILE_RECORD = 0x14,
    MODBUS_FC_WRITE_FILE_RECORD = 0x15,
    MODBUS_FC_MASK_WRITE_REGISTER = 0x16,
    MODBUS_FC_READ_WRITE_MULTIPLE_REGISTERS = 0x17,
    MODBUS_FC_READ_FIFO_QUEUE = 0x18,
    MODBUS_FC_ENCAPSULATED_INTERFACE = 0x2B
} modbus_function_code_t;

/* 异常码 */
typedef enum {
    MODBUS_EX_NONE = 0x00,
    MODBUS_EX_ILLEGAL_FUNCTION = 0x01,
    MODBUS_EX_ILLEGAL_DATA_ADDRESS = 0x02,
    MODBUS_EX_ILLEGAL_DATA_VALUE = 0x03,
    MODBUS_EX_SLAVE_DEVICE_FAILURE = 0x04,
    MODBUS_EX_ACKNOWLEDGE = 0x05,
    MODBUS_EX_SLAVE_DEVICE_BUSY = 0x06,
    MODBUS_EX_NEGATIVE_ACKNOWLEDGE = 0x07,
    MODBUS_EX_MEMORY_PARITY_ERROR = 0x08,
    MODBUS_EX_GATEWAY_PATH_UNAVAILABLE = 0x0A,
    MODBUS_EX_GATEWAY_TARGET_FAILED = 0x0B
} modbus_exception_code_t;

/* 错误码 */
typedef enum {
    MODBUS_ERR_OK = 0,
    MODBUS_ERR_INVALID_PARAM = -1,
    MODBUS_ERR_TIMEOUT = -2,
    MODBUS_ERR_CRC = -3,
    MODBUS_ERR_EXCEPTION = -4,
    MODBUS_ERR_SEND = -5,
    MODBUS_ERR_RECV = -6,
    MODBUS_ERR_INVALID_SLAVE = -7,
    MODBUS_ERR_INVALID_FUNCTION = -8
} modbus_error_t;

/* 最大配置 */
#define MODBUS_MAX_ADU_SIZE         256
#define MODBUS_MAX_PDU_SIZE         253
#define MODBUS_MAX_READ_BITS        2000
#define MODBUS_MAX_WRITE_BITS       1968
#define MODBUS_MAX_READ_REGISTERS   125
#define MODBUS_MAX_WRITE_REGISTERS  123
#define MODBUS_MAX_WR_READ_REGISTERS 125
#define MODBUS_MAX_WR_WRITE_REGISTERS 121

/* TCP MBAP头长度 */
#define MODBUS_TCP_MBAP_SIZE        7

/* 默认端口 */
#define MODBUS_TCP_DEFAULT_PORT     502

/* 前向声明 */
struct modbus_context;
typedef struct modbus_context modbus_context_t;

/* 传输层回调 */
typedef int (*modbus_send_t)(void *ctx, const uint8_t *data, size_t len);
typedef int (*modbus_recv_t)(void *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms);
typedef void (*modbus_flush_t)(void *ctx);

/* Modbus上下文 */
struct modbus_context {
    modbus_mode_t mode;
    uint8_t slave_id;
    
    /* 传输层 */
    void *transport_ctx;
    modbus_send_t send;
    modbus_recv_t recv;
    modbus_flush_t flush;
    
    /* 超时配置 */
    uint32_t response_timeout_ms;
    uint32_t byte_timeout_ms;
    
    /* TCP特定 */
    uint16_t transaction_id;
    
    /* 缓冲区 */
    uint8_t tx_buffer[MODBUS_MAX_ADU_SIZE];
    uint8_t rx_buffer[MODBUS_MAX_ADU_SIZE];
    
    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
    uint32_t timeout_count;
};

/* 初始化 */
int modbus_init_rtu(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func,
                     modbus_flush_t flush_func, uint8_t slave_id);
int modbus_init_tcp(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func);

/* 主站操作 */
int modbus_read_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, 
                      uint16_t count, uint8_t *values);
int modbus_read_input_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, uint8_t *values);
int modbus_read_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                           uint16_t count, uint16_t *values);
int modbus_read_input_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                 uint16_t count, uint16_t *values);
int modbus_write_bit(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, bool value);
int modbus_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, 
                           uint16_t value);
int modbus_write_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                       uint16_t count, const uint8_t *values);
int modbus_write_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, const uint16_t *values);
int modbus_mask_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                uint16_t and_mask, uint16_t or_mask);
int modbus_write_and_read_registers(modbus_context_t *ctx, uint8_t slave_id,
                                     uint16_t write_addr, uint16_t write_count,
                                     const uint16_t *write_values,
                                     uint16_t read_addr, uint16_t read_count,
                                     uint16_t *read_values);

/* 从站操作(简化实现) */
int modbus_slave_poll(modbus_context_t *ctx, uint8_t *request, size_t *req_len,
                       uint32_t timeout_ms);
int modbus_slave_reply(modbus_context_t *ctx, const uint8_t *request,
                        const uint8_t *response, size_t resp_len);
int modbus_reply_exception(modbus_context_t *ctx, const uint8_t *request,
                            modbus_exception_code_t exception);

/* 工具函数 */
uint16_t modbus_crc16(const uint8_t *data, size_t len);
bool modbus_check_crc16(const uint8_t *data, size_t len);
const char* modbus_strerror(modbus_error_t err);
const char* modbus_exception_str(modbus_exception_code_t exc);

/* 数据转换 */
static inline uint16_t modbus_get_u16(const uint8_t *data) {
    return (uint16_t)((data[0] << 8) | data[1]);
}

static inline void modbus_set_u16(uint8_t *data, uint16_t value) {
    data[0] = (uint8_t)(value >> 8);
    data[1] = (uint8_t)(value & 0xFF);
}

static inline uint32_t modbus_get_u32(const uint8_t *data) {
    return ((uint32_t)data[0] << 24) |
           ((uint32_t)data[1] << 16) |
           ((uint32_t)data[2] << 8) |
           (uint32_t)data[3];
}

static inline void modbus_set_u32(uint8_t *data, uint32_t value) {
    data[0] = (uint8_t)(value >> 24);
    data[1] = (uint8_t)(value >> 16);
    data[2] = (uint8_t)(value >> 8);
    data[3] = (uint8_t)(value);
}

static inline float modbus_get_float(const uint8_t *data) {
    union { uint32_t u; float f; } conv;
    conv.u = modbus_get_u32(data);
    return conv.f;
}

static inline void modbus_set_float(uint8_t *data, float value) {
    union { uint32_t u; float f; } conv;
    conv.f = value;
    modbus_set_u32(data, conv.u);
}

#ifdef __cplusplus
}
#endif

#endif /* MODBUS_H */
```

```c
/**
 * @file modbus.c
 * @brief Modbus协议实现
 */

#include "modbus.h"
#include <string.h>
#include <stdio.h>

/* CRC16查找表 (Modbus多项式 0xA001) */
static const uint16_t crc16_table[256] = {
    0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
    0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
    0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
    0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
    0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
    0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
    0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
    0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
    0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
    0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
    0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
    0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
    0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
    0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
    0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
    0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
    0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
    0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
    0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
    0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
    0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
    0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
    0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
    0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
    0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
    0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
    0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
    0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
    0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
    0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
    0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
    0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

/* 计算CRC16 */
uint16_t modbus_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc16_table[(crc ^ data[i]) & 0xFF];
    }
    return crc;
}

/* 检查CRC16 */
bool modbus_check_crc16(const uint8_t *data, size_t len) {
    if (len < 2) return false;
    uint16_t calc_crc = modbus_crc16(data, len - 2);
    uint16_t recv_crc = (uint16_t)((data[len - 1] << 8) | data[len - 2]);
    return calc_crc == recv_crc;
}

/* 初始化RTU模式 */
int modbus_init_rtu(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func,
                     modbus_flush_t flush_func, uint8_t slave_id) {
    if (ctx == NULL || send_func == NULL || recv_func == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    memset(ctx, 0, sizeof(modbus_context_t));
    ctx->mode = MODBUS_RTU;
    ctx->slave_id = slave_id;
    ctx->transport_ctx = transport_ctx;
    ctx->send = send_func;
    ctx->recv = recv_func;
    ctx->flush = flush_func;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 50;
    
    return MODBUS_ERR_OK;
}

/* 初始化TCP模式 */
int modbus_init_tcp(modbus_context_t *ctx, void *transport_ctx,
                     modbus_send_t send_func, modbus_recv_t recv_func) {
    if (ctx == NULL || send_func == NULL || recv_func == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    memset(ctx, 0, sizeof(modbus_context_t));
    ctx->mode = MODBUS_TCP;
    ctx->transport_ctx = transport_ctx;
    ctx->send = send_func;
    ctx->recv = recv_func;
    ctx->response_timeout_ms = 500;
    ctx->byte_timeout_ms = 0;
    ctx->transaction_id = 1;
    
    return MODBUS_ERR_OK;
}

/* 构建RTU ADU */
static int build_rtu_adu(modbus_context_t *ctx, uint8_t slave_id,
                          uint8_t function, const uint8_t *data, size_t data_len) {
    int pos = 0;
    
    ctx->tx_buffer[pos++] = slave_id;
    ctx->tx_buffer[pos++] = function;
    
    if (data_len > 0) {
        memcpy(ctx->tx_buffer + pos, data, data_len);
        pos += (int)data_len;
    }
    
    /* 添加CRC */
    uint16_t crc = modbus_crc16(ctx->tx_buffer, pos);
    ctx->tx_buffer[pos++] = (uint8_t)(crc & 0xFF);
    ctx->tx_buffer[pos++] = (uint8_t)(crc >> 8);
    
    return pos;
}

/* 构建TCP ADU */
static int build_tcp_adu(modbus_context_t *ctx, uint8_t slave_id,
                          uint8_t function, const uint8_t *data, size_t data_len) {
    int pos = 0;
    
    /* MBAP头 */
    ctx->tx_buffer[pos++] = (uint8_t)(ctx->transaction_id >> 8);
    ctx->tx_buffer[pos++] = (uint8_t)(ctx->transaction_id & 0xFF);
    ctx->tx_buffer[pos++] = 0x00; /* 协议标识符高字节 */
    ctx->tx_buffer[pos++] = 0x00; /* 协议标识符低字节 */
    ctx->tx_buffer[pos++] = 0x00; /* 长度高字节 */
    ctx->tx_buffer[pos++] = (uint8_t)(data_len + 2); /* 长度低字节 */
    ctx->tx_buffer[pos++] = slave_id;
    ctx->tx_buffer[pos++] = function;
    
    if (data_len > 0) {
        memcpy(ctx->tx_buffer + pos, data, data_len);
        pos += (int)data_len;
    }
    
    ctx->transaction_id++;
    if (ctx->transaction_id == 0) ctx->transaction_id = 1;
    
    return pos;
}

/* 发送请求并接收响应 */
static int send_request(modbus_context_t *ctx, uint8_t slave_id,
                         uint8_t function, const uint8_t *data, size_t data_len,
                         uint8_t **response, size_t *resp_len) {
    int adu_len;
    
    if (ctx->mode == MODBUS_RTU) {
        adu_len = build_rtu_adu(ctx, slave_id, function, data, data_len);
    } else {
        adu_len = build_tcp_adu(ctx, slave_id, function, data, data_len);
    }
    
    /* 清空接收缓冲区 */
    if (ctx->flush) {
        ctx->flush(ctx->transport_ctx);
    }
    
    /* 发送 */
    if (ctx->send(ctx->transport_ctx, ctx->tx_buffer, adu_len) != adu_len) {
        return MODBUS_ERR_SEND;
    }
    ctx->tx_count++;
    
    /* 接收响应 */
    int rx_len = ctx->recv(ctx->transport_ctx, ctx->rx_buffer, MODBUS_MAX_ADU_SIZE,
                            ctx->response_timeout_ms);
    
    if (rx_len < 0) {
        ctx->error_count++;
        return MODBUS_ERR_RECV;
    }
    
    if (rx_len == 0) {
        ctx->timeout_count++;
        return MODBUS_ERR_TIMEOUT;
    }
    
    ctx->rx_count++;
    
    /* 验证响应 */
    if (ctx->mode == MODBUS_RTU) {
        if (rx_len < 5 || !modbus_check_crc16(ctx->rx_buffer, rx_len)) {
            ctx->error_count++;
            return MODBUS_ERR_CRC;
        }
        *response = ctx->rx_buffer + 2; /* 跳过slave_id和function */
        *resp_len = rx_len - 4; /* 减去slave_id, function, CRC */
    } else {
        if (rx_len < MODBUS_TCP_MBAP_SIZE + 2) {
            ctx->error_count++;
            return MODBUS_ERR_RECV;
        }
        *response = ctx->rx_buffer + MODBUS_TCP_MBAP_SIZE + 1; /* 跳过MBAP和slave_id */
        *resp_len = rx_len - MODBUS_TCP_MBAP_SIZE - 1;
    }
    
    /* 检查功能码 */
    uint8_t resp_func = (ctx->mode == MODBUS_RTU) ? ctx->rx_buffer[1] 
                                                   : ctx->rx_buffer[MODBUS_TCP_MBAP_SIZE];
    
    if (resp_func != function) {
        if (resp_func == (function | 0x80)) {
            /* 异常响应 */
            if (*resp_len >= 2) {
                return (*response)[1] | 0x100; /* 返回异常码 + 标志 */
            }
        }
        return MODBUS_ERR_INVALID_FUNCTION;
    }
    
    *resp_len -= 1; /* 减去function字节 */
    
    return MODBUS_ERR_OK;
}

/* 读取线圈 */
int modbus_read_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                      uint16_t count, uint8_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_BITS) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_COILS, req, 4, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    if (resp_len < 1 || resp[0] != (count + 7) / 8) {
        return MODBUS_ERR_RECV;
    }
    
    /* 解压位数据 */
    for (uint16_t i = 0; i < count; i++) {
        values[i] = (resp[1 + i / 8] >> (i % 8)) & 0x01;
    }
    
    return (int)count;
}

/* 读取离散输入 */
int modbus_read_input_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, uint8_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_BITS) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_DISCRETE_INPUTS, req, 4, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    if (resp_len < 1 || resp[0] != (count + 7) / 8) {
        return MODBUS_ERR_RECV;
    }
    
    for (uint16_t i = 0; i < count; i++) {
        values[i] = (resp[1 + i / 8] >> (i % 8)) & 0x01;
    }
    
    return (int)count;
}

/* 读取保持寄存器 */
int modbus_read_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                           uint16_t count, uint16_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_REGISTERS) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_HOLDING_REGISTERS, req, 4, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    if (resp_len < 1 || resp[0] != count * 2) {
        return MODBUS_ERR_RECV;
    }
    
    for (uint16_t i = 0; i < count; i++) {
        values[i] = modbus_get_u16(resp + 1 + i * 2);
    }
    
    return (int)count;
}

/* 读取输入寄存器 */
int modbus_read_input_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                 uint16_t count, uint16_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_READ_REGISTERS) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_READ_INPUT_REGISTERS, req, 4, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    if (resp_len < 1 || resp[0] != count * 2) {
        return MODBUS_ERR_RECV;
    }
    
    for (uint16_t i = 0; i < count; i++) {
        values[i] = modbus_get_u16(resp + 1 + i * 2);
    }
    
    return (int)count;
}

/* 写单个线圈 */
int modbus_write_bit(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, bool value) {
    if (ctx == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, value ? 0xFF00 : 0x0000);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_SINGLE_COIL, req, 4, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    return MODBUS_ERR_OK;
}

/* 写单个寄存器 */
int modbus_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr, uint16_t value) {
    if (ctx == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[4];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, value);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_SINGLE_REGISTER, req, 4, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    return MODBUS_ERR_OK;
}

/* 写多个线圈 */
int modbus_write_bits(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                       uint16_t count, const uint8_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_WRITE_BITS) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[256];
    int byte_count = (count + 7) / 8;
    
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    req[4] = (uint8_t)byte_count;
    
    memset(req + 5, 0, byte_count);
    for (uint16_t i = 0; i < count; i++) {
        if (values[i]) {
            req[5 + i / 8] |= (1 << (i % 8));
        }
    }
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_MULTIPLE_COILS, req, 5 + byte_count, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    return MODBUS_ERR_OK;
}

/* 写多个寄存器 */
int modbus_write_registers(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                            uint16_t count, const uint16_t *values) {
    if (ctx == NULL || values == NULL || count == 0 || count > MODBUS_MAX_WRITE_REGISTERS) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[256];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, count);
    req[4] = (uint8_t)(count * 2);
    
    for (uint16_t i = 0; i < count; i++) {
        modbus_set_u16(req + 5 + i * 2, values[i]);
    }
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_WRITE_MULTIPLE_REGISTERS, req, 5 + count * 2, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    return MODBUS_ERR_OK;
}

/* 掩码写寄存器 */
int modbus_mask_write_register(modbus_context_t *ctx, uint8_t slave_id, uint16_t addr,
                                uint16_t and_mask, uint16_t or_mask) {
    if (ctx == NULL) {
        return MODBUS_ERR_INVALID_PARAM;
    }
    
    uint8_t req[6];
    modbus_set_u16(req, addr);
    modbus_set_u16(req + 2, and_mask);
    modbus_set_u16(req + 4, or_mask);
    
    uint8_t *resp;
    size_t resp_len;
    int err = send_request(ctx, slave_id, MODBUS_FC_MASK_WRITE_REGISTER, req, 6, &resp, &resp_len);
    
    if (err < 0) return err;
    if (err >= 0x100) return MODBUS_ERR_EXCEPTION;
    
    return MODBUS_ERR_OK;
}

/* 错误码转字符串 */
const char* modbus_strerror(modbus_error_t err) {
    switch (err) {
        case MODBUS_ERR_OK: return "No error";
        case MODBUS_ERR_INVALID_PARAM: return "Invalid parameter";
        case MODBUS_ERR_TIMEOUT: return "Timeout";
        case MODBUS_ERR_CRC: return "CRC error";
        case MODBUS_ERR_EXCEPTION: return "Modbus exception";
        case MODBUS_ERR_SEND: return "Send failed";
        case MODBUS_ERR_RECV: return "Receive failed";
        case MODBUS_ERR_INVALID_SLAVE: return "Invalid slave ID";
        case MODBUS_ERR_INVALID_FUNCTION: return "Invalid function code";
        default: return "Unknown error";
    }
}

/* 异常码转字符串 */
const char* modbus_exception_str(modbus_exception_code_t exc) {
    switch (exc) {
        case MODBUS_EX_NONE: return "No exception";
        case MODBUS_EX_ILLEGAL_FUNCTION: return "Illegal function";
        case MODBUS_EX_ILLEGAL_DATA_ADDRESS: return "Illegal data address";
        case MODBUS_EX_ILLEGAL_DATA_VALUE: return "Illegal data value";
        case MODBUS_EX_SLAVE_DEVICE_FAILURE: return "Slave device failure";
        case MODBUS_EX_ACKNOWLEDGE: return "Acknowledge";
        case MODBUS_EX_SLAVE_DEVICE_BUSY: return "Slave device busy";
        case MODBUS_EX_NEGATIVE_ACKNOWLEDGE: return "Negative acknowledge";
        case MODBUS_EX_MEMORY_PARITY_ERROR: return "Memory parity error";
        case MODBUS_EX_GATEWAY_PATH_UNAVAILABLE: return "Gateway path unavailable";
        case MODBUS_EX_GATEWAY_TARGET_FAILED: return "Gateway target device failed";
        default: return "Unknown exception";
    }
}
```

---

## 5. CAN总线协议

### 5.1 CAN协议概述

CAN (Controller Area Network) 是一种串行通信协议，最初为汽车行业设计，现广泛应用于工业自动化和嵌入式系统。

**关键特性：**
- 多主架构，无主从之分
- 基于优先级的仲裁机制
- 非破坏性总线访问
- 内置错误检测和处理
- 最高1Mbps数据速率

### 5.2 CAN帧格式

```c
/**
 * @file can_driver.h
 * @brief CAN总线驱动接口
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* CAN标准帧ID长度 */
#define CAN_STD_ID_BITS     11
#define CAN_EXT_ID_BITS     29
#define CAN_MAX_STD_ID      0x7FF
#define CAN_MAX_EXT_ID      0x1FFFFFFF

/* CAN帧数据长度 */
#define CAN_MAX_DLC         8
#define CAN_FD_MAX_DLC      64

/* CAN帧类型 */
typedef enum {
    CAN_FRAME_DATA = 0,         /* 数据帧 */
    CAN_FRAME_REMOTE = 1,       /* 远程帧 */
    CAN_FRAME_ERROR = 2,        /* 错误帧 */
    CAN_FRAME_OVERLOAD = 3      /* 过载帧 */
} can_frame_type_t;

/* CAN ID类型 */
typedef enum {
    CAN_ID_STANDARD = 0,        /* 标准帧 (11位ID) */
    CAN_ID_EXTENDED = 1         /* 扩展帧 (29位ID) */
} can_id_type_t;

/* CAN帧结构 */
typedef struct {
    uint32_t id;                /* 标识符 */
    can_id_type_t id_type;      /* ID类型 */
    can_frame_type_t frame_type; /* 帧类型 */
    uint8_t dlc;                /* 数据长度代码 (0-8) */
    uint8_t data[CAN_FD_MAX_DLC]; /* 数据 */
    bool rtr;                   /* 远程传输请求 */
    uint32_t timestamp;         /* 时间戳 */
} can_frame_t;

/* CAN位时序配置 */
typedef struct {
    uint32_t bitrate;           /* 波特率 (bps) */
    uint16_t prescaler;         /* 预分频器 */
    uint8_t time_seg1;          /* 时间段1 */
    uint8_t time_seg2;          /* 时间段2 */
    uint8_t sjw;                /* 同步跳转宽度 */
    bool triple_sample;         /* 三重采样 */
} can_bit_timing_t;

/* 滤波器配置 */
typedef struct {
    uint32_t id;                /* 滤波器ID */
    uint32_t mask;              /* 掩码 */
    can_id_type_t id_type;      /* ID类型 */
    bool enabled;               /* 使能 */
} can_filter_t;

/* 错误码 */
typedef enum {
    CAN_ERR_OK = 0,
    CAN_ERR_INVALID_PARAM = -1,
    CAN_ERR_BUSY = -2,
    CAN_ERR_TIMEOUT = -3,
    CAN_ERR_BUS_OFF = -4,
    CAN_ERR_PASSIVE = -5,
    CAN_ERR_OVERRUN = -6,
    CAN_ERR_NO_MEMORY = -7
} can_error_t;

/* 总线状态 */
typedef enum {
    CAN_BUS_ERROR_ACTIVE = 0,   /* 主动错误 */
    CAN_BUS_ERROR_PASSIVE = 1,  /* 被动错误 */
    CAN_BUS_OFF = 2             /* 总线关闭 */
} can_bus_state_t;

/* 错误计数器结构 */
typedef struct {
    uint8_t tx_error_count;     /* 发送错误计数 */
    uint8_t rx_error_count;     /* 接收错误计数 */
    can_bus_state_t bus_state;  /* 总线状态 */
} can_error_counters_t;

/* 回调函数类型 */
typedef void (*can_rx_callback_t)(const can_frame_t *frame, void *user_data);
typedef void (*can_error_callback_t)(can_error_counters_t *counters, void *user_data);

/* CAN设备句柄 */
typedef struct can_device can_device_t;

/* API函数 */

/* 初始化和关闭 */
can_device_t* can_init(uint8_t controller_id, const can_bit_timing_t *timing);
void can_deinit(can_device_t *dev);

/* 配置 */
int can_set_bit_timing(can_device_t *dev, const can_bit_timing_t *timing);
int can_set_filter(can_device_t *dev, uint8_t filter_idx, const can_filter_t *filter);
int can_clear_filter(can_device_t *dev, uint8_t filter_idx);

/* 发送和接收 */
int can_send(can_device_t *dev, const can_frame_t *frame, uint32_t timeout_ms);
int can_receive(can_device_t *dev, can_frame_t *frame, uint32_t timeout_ms);
int can_try_receive(can_device_t *dev, can_frame_t *frame);

/* 中断和回调 */
int can_enable_interrupt(can_device_t *dev);
int can_disable_interrupt(can_device_t *dev);
int can_register_rx_callback(can_device_t *dev, can_rx_callback_t callback, void *user_data);
int can_register_error_callback(can_device_t *dev, can_error_callback_t callback, void *user_data);

/* 状态和控制 */
int can_start(can_device_t *dev);
int can_stop(can_device_t *dev);
int can_get_error_counters(can_device_t *dev, can_error_counters_t *counters);
int can_reset(can_device_t *dev);
bool can_is_bus_off(can_device_t *dev);

/* 实用函数 */
uint16_t can_dlc_to_len(uint8_t dlc);
uint8_t can_len_to_dlc(uint16_t len);
bool can_is_valid_id(uint32_t id, can_id_type_t type);
void can_frame_init(can_frame_t *frame, uint32_t id, can_id_type_t id_type, 
                     const uint8_t *data, uint8_t len);

/* 常用波特率配置 */
extern const can_bit_timing_t can_timing_125k;
extern const can_bit_timing_t can_timing_250k;
extern const can_bit_timing_t can_timing_500k;
extern const can_bit_timing_t can_timing_1m;

#ifdef __cplusplus
}
#endif

#endif /* CAN_DRIVER_H */
```

```c
/**
 * @file can_driver.c
 * @brief CAN驱动实现 - 基于SocketCAN (Linux) 或虚拟实现
 */

#include "can_driver.h"
#include <string.h>
#include <stdio.h>

#ifdef __linux__
    #include <sys/socket.h>
    #include <sys/ioctl.h>
    #include <net/if.h>
    #include <linux/can.h>
    #include <linux/can/raw.h>
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

/* 常用波特率配置 */
const can_bit_timing_t can_timing_125k = {
    .bitrate = 125000,
    .prescaler = 8,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

const can_bit_timing_t can_timing_250k = {
    .bitrate = 250000,
    .prescaler = 4,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

const can_bit_timing_t can_timing_500k = {
    .bitrate = 500000,
    .prescaler = 2,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

const can_bit_timing_t can_timing_1m = {
    .bitrate = 1000000,
    .prescaler = 1,
    .time_seg1 = 13,
    .time_seg2 = 2,
    .sjw = 1,
    .triple_sample = false
};

/* 设备结构 */
struct can_device {
    uint8_t controller_id;
    can_bit_timing_t timing;
    can_bus_state_t state;
    
#ifdef __linux__
    int socket_fd;
    struct sockaddr_can addr;
#endif
    
    /* 回调 */
    can_rx_callback_t rx_callback;
    void *rx_user_data;
    can_error_callback_t error_callback;
    void *error_user_data;
    
    /* 统计 */
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t error_count;
};

/* DLC到长度转换 */
uint16_t can_dlc_to_len(uint8_t dlc) {
    static const uint8_t dlc_table[] = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    if (dlc <= 8) return dlc_table[dlc];
    return 8;
}

/* 长度到DLC转换 */
uint8_t can_len_to_dlc(uint16_t len) {
    if (len > 8) len = 8;
    return (uint8_t)len;
}

/* 验证ID有效性 */
bool can_is_valid_id(uint32_t id, can_id_type_t type) {
    if (type == CAN_ID_STANDARD) {
        return id <= CAN_MAX_STD_ID;
    } else {
        return id <= CAN_MAX_EXT_ID;
    }
}

/* 初始化CAN帧 */
void can_frame_init(can_frame_t *frame, uint32_t id, can_id_type_t id_type,
                     const uint8_t *data, uint8_t len) {
    memset(frame, 0, sizeof(can_frame_t));
    frame->id = id;
    frame->id_type = id_type;
    frame->frame_type = CAN_FRAME_DATA;
    frame->rtr = false;
    
    if (len > CAN_MAX_DLC) len = CAN_MAX_DLC;
    frame->dlc = len;
    
    if (data != NULL && len > 0) {
        memcpy(frame->data, data, len);
    }
}

/* 初始化CAN设备 */
can_device_t* can_init(uint8_t controller_id, const can_bit_timing_t *timing) {
#ifdef __linux__
    can_device_t *dev = (can_device_t *)malloc(sizeof(can_device_t));
    if (dev == NULL) return NULL;
    
    memset(dev, 0, sizeof(can_device_t));
    dev->controller_id = controller_id;
    
    if (timing != NULL) {
        dev->timing = *timing;
    } else {
        dev->timing = can_timing_500k;
    }
    
    /* 创建SocketCAN socket */
    dev->socket_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (dev->socket_fd < 0) {
        free(dev);
        return NULL;
    }
    
    /* 获取接口索引 */
    struct ifreq ifr;
    snprintf(ifr.ifr_name, IFNAMSIZ, "can%d", controller_id);
    if (ioctl(dev->socket_fd, SIOCGIFINDEX, &ifr) < 0) {
        close(dev->socket_fd);
        free(dev);
        return NULL;
    }
    
    dev->addr.can_family = AF_CAN;
    dev->addr.can_ifindex = ifr.ifr_ifindex;
    
    /* 绑定 */
    if (bind(dev->socket_fd, (struct sockaddr *)&dev->addr, sizeof(dev->addr)) < 0) {
        close(dev->socket_fd);
        free(dev);
        return NULL;
    }
    
    /* 设置为非阻塞 */
    int flags = fcntl(dev->socket_fd, F_GETFL, 0);
    fcntl(dev->socket_fd, F_SETFL, flags | O_NONBLOCK);
    
    dev->state = CAN_BUS_ERROR_ACTIVE;
    return dev;
#else
    /* 非Linux平台返回虚拟设备 */
    can_device_t *dev = (can_device_t *)malloc(sizeof(can_device_t));
    if (dev == NULL) return NULL;
    memset(dev, 0, sizeof(can_device_t));
    dev->controller_id = controller_id;
    if (timing != NULL) {
        dev->timing = *timing;
    }
    dev->state = CAN_BUS_ERROR_ACTIVE;
    return dev;
#endif
}

/* 关闭CAN设备 */
void can_deinit(can_device_t *dev) {
    if (dev == NULL) return;
    
#ifdef __linux__
    if (dev->socket_fd >= 0) {
        close(dev->socket_fd);
    }
#endif
    free(dev);
}

/* 发送CAN帧 */
int can_send(can_device_t *dev, const can_frame_t *frame, uint32_t timeout_ms) {
    if (dev == NULL || frame == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    
    if (!can_is_valid_id(frame->id, frame->id_type)) {
        return CAN_ERR_INVALID_PARAM;
    }
    
#ifdef __linux__
    struct can_frame sock_frame;
    memset(&sock_frame, 0, sizeof(sock_frame));
    
    /* 设置ID */
    if (frame->id_type == CAN_ID_EXTENDED) {
        sock_frame.can_id = frame->id | CAN_EFF_FLAG;
    } else {
        sock_frame.can_id = frame->id;
    }
    
    if (frame->rtr) {
        sock_frame.can_id |= CAN_RTR_FLAG;
    }
    
    sock_frame.can_dlc = frame->dlc;
    memcpy(sock_frame.data, frame->data, frame->dlc);
    
    ssize_t sent = write(dev->socket_fd, &sock_frame, sizeof(sock_frame));
    if (sent != sizeof(sock_frame)) {
        if (errno == ENOBUFS) {
            return CAN_ERR_BUSY;
        }
        return CAN_ERR_BUS_OFF;
    }
    
    dev->tx_count++;
    return CAN_ERR_OK;
#else
    /* 虚拟实现 */
    printf("CAN TX: ID=0x%08X, DLC=%d, Data=", frame->id, frame->dlc);
    for (int i = 0; i < frame->dlc; i++) {
        printf("%02X ", frame->data[i]);
    }
    printf("\n");
    dev->tx_count++;
    return CAN_ERR_OK;
#endif
}

/* 接收CAN帧 */
int can_receive(can_device_t *dev, can_frame_t *frame, uint32_t timeout_ms) {
    if (dev == NULL || frame == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    
#ifdef __linux__
    fd_set rfds;
    struct timeval tv;
    
    FD_ZERO(&rfds);
    FD_SET(dev->socket_fd, &rfds);
    
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;
    
    int ret = select(dev->socket_fd + 1, &rfds, NULL, NULL, &tv);
    if (ret < 0) {
        return CAN_ERR_INVALID_PARAM;
    }
    if (ret == 0) {
        return CAN_ERR_TIMEOUT;
    }
    
    return can_try_receive(dev, frame);
#else
    (void)timeout_ms;
    return CAN_ERR_TIMEOUT;
#endif
}

/* 非阻塞接收 */
int can_try_receive(can_device_t *dev, can_frame_t *frame) {
    if (dev == NULL || frame == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    
#ifdef __linux__
    struct can_frame sock_frame;
    ssize_t recvd = read(dev->socket_fd, &sock_frame, sizeof(sock_frame));
    
    if (recvd < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return CAN_ERR_TIMEOUT;
        }
        return CAN_ERR_INVALID_PARAM;
    }
    
    if (recvd != sizeof(sock_frame)) {
        return CAN_ERR_INVALID_PARAM;
    }
    
    /* 转换帧 */
    memset(frame, 0, sizeof(can_frame_t));
    
    if (sock_frame.can_id & CAN_EFF_FLAG) {
        frame->id = sock_frame.can_id & CAN_EFF_MASK;
        frame->id_type = CAN_ID_EXTENDED;
    } else {
        frame->id = sock_frame.can_id & CAN_SFF_MASK;
        frame->id_type = CAN_ID_STANDARD;
    }
    
    frame->rtr = (sock_frame.can_id & CAN_RTR_FLAG) != 0;
    frame->dlc = sock_frame.can_dlc;
    memcpy(frame->data, sock_frame.data, sock_frame.can_dlc);
    frame->frame_type = CAN_FRAME_DATA;
    
    dev->rx_count++;
    
    /* 调用回调 */
    if (dev->rx_callback != NULL) {
        dev->rx_callback(frame, dev->rx_user_data);
    }
    
    return CAN_ERR_OK;
#else
    (void)frame;
    return CAN_ERR_TIMEOUT;
#endif
}

/* 设置滤波器 */
int can_set_filter(can_device_t *dev, uint8_t filter_idx, const can_filter_t *filter) {
#ifdef __linux__
    if (dev == NULL || filter == NULL || filter_idx >= 32) {
        return CAN_ERR_INVALID_PARAM;
    }
    
    struct can_filter sock_filter;
    sock_filter.can_id = filter->id;
    sock_filter.can_mask = filter->mask;
    
    if (filter->id_type == CAN_ID_EXTENDED) {
        sock_filter.can_id |= CAN_EFF_FLAG;
        sock_filter.can_mask |= CAN_EFF_FLAG;
    }
    
    if (!filter->enabled) {
        sock_filter.can_id = 0;
        sock_filter.can_mask = 0;
    }
    
    if (setsockopt(dev->socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, 
                   &sock_filter, sizeof(sock_filter)) < 0) {
        return CAN_ERR_INVALID_PARAM;
    }
    
    return CAN_ERR_OK;
#else
    (void)dev;
    (void)filter_idx;
    (void)filter;
    return CAN_ERR_OK;
#endif
}

/* 注册接收回调 */
int can_register_rx_callback(can_device_t *dev, can_rx_callback_t callback, void *user_data) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    
    dev->rx_callback = callback;
    dev->rx_user_data = user_data;
    return CAN_ERR_OK;
}

/* 启动CAN */
int can_start(can_device_t *dev) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    dev->state = CAN_BUS_ERROR_ACTIVE;
    return CAN_ERR_OK;
}

/* 停止CAN */
int can_stop(can_device_t *dev) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    return CAN_ERR_OK;
}

/* 重置CAN */
int can_reset(can_device_t *dev) {
    if (dev == NULL) {
        return CAN_ERR_INVALID_PARAM;
    }
    dev->state = CAN_BUS_ERROR_ACTIVE;
    dev->tx_count = 0;
    dev->rx_count = 0;
    dev->error_count = 0;
    return CAN_ERR_OK;
}

/* 检查总线关闭状态 */
bool can_is_bus_off(can_device_t *dev) {
    if (dev == NULL) return false;
    return dev->state == CAN_BUS_OFF;
}
```

