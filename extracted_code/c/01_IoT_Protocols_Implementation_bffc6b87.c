/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 348
 * Language: c
 * Block ID: bffc6b87
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
