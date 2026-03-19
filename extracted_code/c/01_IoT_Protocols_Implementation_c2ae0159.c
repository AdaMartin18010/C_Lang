/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 148
 * Language: c
 * Block ID: c2ae0159
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
