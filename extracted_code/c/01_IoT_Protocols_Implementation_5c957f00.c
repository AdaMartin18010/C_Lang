/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 1588
 * Language: c
 * Block ID: 5c957f00
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
