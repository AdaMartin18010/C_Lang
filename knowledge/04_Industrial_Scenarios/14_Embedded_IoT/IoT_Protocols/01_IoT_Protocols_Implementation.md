# IoT 协议栈实现指南

> 轻量级 MQTT、CoAP、LwM2M、Modbus 协议嵌入式实现

---

## 1. 协议选型对比

| 协议 | 传输层 | 头部大小 | 适用场景 | 复杂度 |
|:-----|:-------|:---------|:---------|:-------|
| MQTT | TCP | 2字节 |  telemetry, 命令控制 | 低 |
| CoAP | UDP | 4字节 | RESTful, 受限设备 | 中 |
| LwM2M | CoAP | ~10字节 | 设备管理, OTA | 高 |
| Modbus RTU | 串口 | 2字节 | 工业现场总线 | 低 |
| Modbus TCP | TCP | 7字节 | 工业以太网 | 低 |

---

## 2. MQTT 客户端实现

### 2.1 最小 MQTT 客户端（< 5KB RAM）

```c
/* mini_mqtt.h */
#ifndef MINI_MQTT_H
#define MINI_MQTT_H

#include <stdint.h>
#include <stdbool.h>

#define MQTT_MAX_PKT_SIZE   256
#define MQTT_KEEPALIVE      60

typedef enum {
    MQTT_CONNECT = 1,
    MQTT_CONNACK = 2,
    MQTT_PUBLISH = 3,
    MQTT_PUBACK = 4,
    MQTT_SUBSCRIBE = 8,
    MQTT_SUBACK = 9,
    MQTT_PINGREQ = 12,
    MQTT_PINGRESP = 13,
    MQTT_DISCONNECT = 14
} mqtt_packet_type_t;

typedef struct {
    uint8_t buffer[MQTT_MAX_PKT_SIZE];
    uint16_t pos;
    int (*send)(const uint8_t *data, uint16_t len);
    int (*recv)(uint8_t *data, uint16_t max_len, uint32_t timeout_ms);
    void (*msg_callback)(const char *topic, const uint8_t *payload, uint16_t len);
} mqtt_client_t;

int mqtt_init(mqtt_client_t *client,
              const char *client_id,
              int (*send_fn)(const uint8_t*, uint16_t),
              int (*recv_fn)(uint8_t*, uint16_t, uint32_t));
int mqtt_connect(mqtt_client_t *client);
int mqtt_publish(mqtt_client_t *client, const char *topic,
                 const uint8_t *payload, uint16_t len, uint8_t qos);
int mqtt_subscribe(mqtt_client_t *client, const char *topic, uint8_t qos);
int mqtt_loop(mqtt_client_t *client);

#endif
```

```c
/* mini_mqtt.c */
#include "mini_mqtt.h"
#include <string.h>

/* 编码变长整数 */
static int encode_var_int(uint8_t *buf, uint32_t value)
{
    int len = 0;
    do {
        uint8_t byte = value % 128;
        value /= 128;
        if(value > 0) byte |= 0x80;
        buf[len++] = byte;
    } while(value > 0);
    return len;
}

/* 编码字符串 */
static int encode_string(uint8_t *buf, const char *str)
{
    uint16_t len = strlen(str);
    buf[0] = len >> 8;
    buf[1] = len & 0xFF;
    memcpy(&buf[2], str, len);
    return 2 + len;
}

int mqtt_init(mqtt_client_t *client, const char *client_id,
              int (*send_fn)(const uint8_t*, uint16_t),
              int (*recv_fn)(uint8_t*, uint16_t, uint32_t))
{
    memset(client, 0, sizeof(*client));
    client->send = send_fn;
    client->recv = recv_fn;
    return 0;
}

int mqtt_connect(mqtt_client_t *client)
{
    uint8_t *pkt = client->buffer;
    int pos = 0;

    /* 固定头部 */
    pkt[pos++] = MQTT_CONNECT << 4;

    /* 可变头部 + 负载 */
    int var_start = pos + 1;  /* 预留长度位置 */
    int var_pos = var_start;

    /* 协议名 */
    var_pos += encode_string(&pkt[var_pos], "MQTT");
    /* 协议级别 */
    pkt[var_pos++] = 4;
    /* 连接标志 */
    pkt[var_pos++] = 0x02;  /* Clean Session */
    /* Keep Alive */
    pkt[var_pos++] = MQTT_KEEPALIVE >> 8;
    pkt[var_pos++] = MQTT_KEEPALIVE & 0xFF;
    /* Client ID */
    var_pos += encode_string(&pkt[var_pos], "ESP32_001");

    /* 计算并填入剩余长度 */
    int payload_len = var_pos - var_start;
    int len_bytes = encode_var_int(&pkt[pos], payload_len);

    /* 移动负载 */
    if(len_bytes > 1) {
        memmove(&pkt[pos + len_bytes], &pkt[var_start], payload_len);
    }

    int total_len = pos + len_bytes + payload_len;

    /* 发送 */
    return client->send(pkt, total_len);
}

int mqtt_publish(mqtt_client_t *client, const char *topic,
                 const uint8_t *payload, uint16_t len, uint8_t qos)
{
    uint8_t *pkt = client->buffer;
    int pos = 0;

    /* 固定头部 */
    uint8_t header = (MQTT_PUBLISH << 4) | (qos << 1);
    pkt[pos++] = header;

    /* 计算剩余长度 */
    int rem_len = 2 + strlen(topic) + len;
    if(qos > 0) rem_len += 2;  /* Packet ID */

    pos += encode_var_int(&pkt[pos], rem_len);

    /* Topic */
    pos += encode_string(&pkt[pos], topic);

    /* Packet ID (如果 QoS > 0) */
    if(qos > 0) {
        static uint16_t packet_id = 1;
        pkt[pos++] = packet_id >> 8;
        pkt[pos++] = packet_id & 0xFF;
        packet_id++;
    }

    /* Payload */
    memcpy(&pkt[pos], payload, len);
    pos += len;

    return client->send(pkt, pos);
}

int mqtt_subscribe(mqtt_client_t *client, const char *topic, uint8_t qos)
{
    uint8_t *pkt = client->buffer;
    int pos = 0;

    pkt[pos++] = (MQTT_SUBSCRIBE << 4) | 0x02;  /* QoS 1 for SUBSCRIBE */

    int rem_len = 2 + 1;  /* Packet ID + 至少1字节 */
    rem_len += 2 + strlen(topic) + 1;  /* Topic + QoS */

    pos += encode_var_int(&pkt[pos], rem_len);

    /* Packet ID */
    static uint16_t packet_id = 1;
    pkt[pos++] = packet_id >> 8;
    pkt[pos++] = packet_id & 0xFF;
    packet_id++;

    /* Topic + QoS */
    pos += encode_string(&pkt[pos], topic);
    pkt[pos++] = qos;

    return client->send(pkt, pos);
}

int mqtt_ping(mqtt_client_t *client)
{
    uint8_t ping[2] = {MQTT_PINGREQ << 4, 0};
    return client->send(ping, 2);
}

int mqtt_loop(mqtt_client_t *client)
{
    uint8_t rx_buf[MQTT_MAX_PKT_SIZE];
    int len = client->recv(rx_buf, sizeof(rx_buf), 100);

    if(len < 2) return 0;

    uint8_t packet_type = (rx_buf[0] >> 4) & 0x0F;

    switch(packet_type) {
    case MQTT_PUBLISH: {
        /* 解析 PUBLISH */
        int pos = 1;
        uint32_t rem_len = 0;
        uint32_t multiplier = 1;

        do {
            rem_len += (rx_buf[pos] & 0x7F) * multiplier;
            multiplier *= 128;
        } while(rx_buf[pos++] & 0x80);

        /* Topic */
        uint16_t topic_len = (rx_buf[pos] << 8) | rx_buf[pos+1];
        pos += 2;
        char topic[64];
        memcpy(topic, &rx_buf[pos], topic_len);
        topic[topic_len] = '\0';
        pos += topic_len;

        /* Payload */
        if(client->msg_callback) {
            uint8_t qos = (rx_buf[0] >> 1) & 0x03;
            if(qos > 0) pos += 2;  /* Skip packet ID */

            uint16_t payload_len = rem_len - (pos - 1) + 1;
            client->msg_callback(topic, &rx_buf[pos], payload_len);
        }
        break;
    }

    case MQTT_PINGRESP:
        /* Pong */
        break;

    case MQTT_CONNACK:
        if(rx_buf[3] != 0) {
            return -1;  /* Connection refused */
        }
        break;
    }

    return 0;
}
```

### 2.2 使用示例

```c
/* 配合 ESP32 WiFi */
#include "mini_mqtt.h"
#include "esp_wifi.h"

static mqtt_client_t mqtt_client;

int tcp_send(const uint8_t *data, uint16_t len)
{
    return send(mqtt_socket, data, len, 0);
}

int tcp_recv(uint8_t *data, uint16_t max_len, uint32_t timeout_ms)
{
    struct timeval tv = {.tv_sec = timeout_ms / 1000,
                         .tv_usec = (timeout_ms % 1000) * 1000};
    setsockopt(mqtt_socket, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    return recv(mqtt_socket, data, max_len, 0);
}

void on_message(const char *topic, const uint8_t *payload, uint16_t len)
{
    printf("Topic: %s, Payload: ", topic);
    for(int i = 0; i < len; i++) printf("%c", payload[i]);
    printf("\n");
}

void mqtt_task(void *pv)
{
    /* 连接 WiFi */
    wifi_connect("SSID", "PASS");

    /* 连接 MQTT Broker */
    mqtt_socket = socket(AF_INET, SOCK_STREAM, 0);
    connect(mqtt_socket, (struct sockaddr*)&broker_addr, sizeof(broker_addr));

    mqtt_init(&mqtt_client, "ESP32_001", tcp_send, tcp_recv);
    mqtt_client.msg_callback = on_message;

    mqtt_connect(&mqtt_client);
    mqtt_subscribe(&mqtt_client, "home/sensors/+", 0);

    TickType_t last_ping = xTaskGetTickCount();

    while(1) {
        mqtt_loop(&mqtt_client);

        /* 定时发布 */
        static TickType_t last_pub = 0;
        if(xTaskGetTickCount() - last_pub > pdMS_TO_TICKS(5000)) {
            char payload[32];
            snprintf(payload, sizeof(payload), "{\"temp\":%.1f}", read_temp());
            mqtt_publish(&mqtt_client, "home/sensors/temp",
                        (uint8_t*)payload, strlen(payload), 0);
            last_pub = xTaskGetTickCount();
        }

        /* 保活 */
        if(xTaskGetTickCount() - last_ping > pdMS_TO_TICKS(30000)) {
            mqtt_ping(&mqtt_client);
            last_ping = xTaskGetTickCount();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

---

## 3. CoAP 服务器/客户端

### 3.1 CoAP 简化实现

```c
/* mini_coap.h */
#ifndef MINI_COAP_H
#define MINI_COAP_H

#include <stdint.h>

#define COAP_PORT       5683
#define COAP_MAX_PKT    256

/* 类型 */
#define COAP_TYPE_CON   0   /* Confirmable */
#define COAP_TYPE_NON   1   /* Non-confirmable */
#define COAP_TYPE_ACK   2   /* Acknowledgment */
#define COAP_TYPE_RST   3   /* Reset */

/* 方法码 */
#define COAP_GET        1
#define COAP_POST       2
#define COAP_PUT        3
#define COAP_DELETE     4

/* 响应码 */
#define COAP_CREATED    0x41
#define COAP_DELETED    0x42
#define COAP_VALID      0x43
#define COAP_CHANGED    0x44
#define COAP_CONTENT    0x45
#define COAP_NOT_FOUND  0x84

typedef struct {
    uint8_t ver_t_tkl;  /* Ver(2) | T(2) | TKL(4) */
    uint8_t code;       /* Class(3) | Detail(5) */
    uint16_t msg_id;
    uint8_t token[8];
    uint8_t payload[COAP_MAX_PKT];
} coap_packet_t;

typedef int (*coap_handler_t)(const char *path,
                              uint8_t method,
                              const uint8_t *payload, uint16_t len,
                              uint8_t *response, uint16_t *resp_len);

int coap_parse(const uint8_t *data, uint16_t len, coap_packet_t *pkt);
int coap_build_response(uint8_t *buf, uint16_t buf_len,
                        const coap_packet_t *req,
                        uint8_t code,
                        const uint8_t *payload, uint16_t payload_len);

#endif
```

```c
/* mini_coap.c */
#include "mini_coap.h"
#include <string.h>

int coap_parse(const uint8_t *data, uint16_t len, coap_packet_t *pkt)
{
    if(len < 4) return -1;

    pkt->ver_t_tkl = data[0];
    pkt->code = data[1];
    pkt->msg_id = (data[2] << 8) | data[3];

    uint8_t tkl = pkt->ver_t_tkl & 0x0F;
    if(tkl > 8) return -1;

    int pos = 4;

    /* Token */
    memcpy(pkt->token, &data[pos], tkl);
    pos += tkl;

    /* Options (简化处理) */
    while(pos < len && data[pos] != 0xFF) {
        uint8_t delta = (data[pos] >> 4) & 0x0F;
        uint8_t opt_len = data[pos] & 0x0F;

        /* 跳过选项 */
        pos++;
        if(delta == 13) pos++;
        else if(delta == 14) pos += 2;

        if(opt_len == 13) opt_len = data[pos++] + 13;
        else if(opt_len == 14) {
            opt_len = ((data[pos] << 8) | data[pos+1]) + 269;
            pos += 2;
        }

        pos += opt_len;
    }

    /* Payload */
    if(pos < len && data[pos] == 0xFF) {
        pos++;  /* Skip 0xFF */
        memcpy(pkt->payload, &data[pos], len - pos);
    }

    return 0;
}

int coap_build_response(uint8_t *buf, uint16_t buf_len,
                        const coap_packet_t *req,
                        uint8_t code,
                        const uint8_t *payload, uint16_t payload_len)
{
    int pos = 0;

    /* 头部 */
    buf[pos++] = 0x40 | (COAP_TYPE_ACK << 4) | (req->ver_t_tkl & 0x0F);
    buf[pos++] = code;
    buf[pos++] = req->msg_id >> 8;
    buf[pos++] = req->msg_id & 0xFF;

    /* Token */
    uint8_t tkl = req->ver_t_tkl & 0x0F;
    memcpy(&buf[pos], req->token, tkl);
    pos += tkl;

    /* Content-Format 选项 (text/plain = 0) */
    buf[pos++] = 0xC0 | 12;  /* Delta=12 (Content-Format), Len=0 */

    /* Payload marker */
    buf[pos++] = 0xFF;

    /* Payload */
    if(payload_len > buf_len - pos - 1) {
        payload_len = buf_len - pos - 1;
    }
    memcpy(&buf[pos], payload, payload_len);
    pos += payload_len;

    return pos;
}

/* CoAP 服务器示例 */
void coap_server_run(void)
{
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in addr = {
        .sin_family = AF_INET,
        .sin_port = htons(COAP_PORT),
        .sin_addr.s_addr = INADDR_ANY
    };
    bind(sock, (struct sockaddr*)&addr, sizeof(addr));

    uint8_t rx_buf[COAP_MAX_PKT];
    uint8_t tx_buf[COAP_MAX_PKT];

    while(1) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof(client);

        int len = recvfrom(sock, rx_buf, sizeof(rx_buf), 0,
                          (struct sockaddr*)&client, &client_len);

        coap_packet_t req;
        if(coap_parse(rx_buf, len, &req) < 0) continue;

        uint8_t method = req.code;
        uint8_t resp_code = COAP_CONTENT;
        uint8_t resp_payload[128];
        uint16_t resp_len = 0;

        /* 路由处理 */
        if(strncmp((char*)req.payload, "/sensor/temp", 12) == 0) {
            resp_len = snprintf((char*)resp_payload, sizeof(resp_payload),
                               "{\"temperature\":25.5}");
        }
        else if(strncmp((char*)req.payload, "/sensor/humidity", 16) == 0) {
            resp_len = snprintf((char*)resp_payload, sizeof(resp_payload),
                               "{\"humidity\":60}");
        }
        else {
            resp_code = COAP_NOT_FOUND;
            resp_len = snprintf((char*)resp_payload, sizeof(resp_payload),
                               "Not Found");
        }

        int resp_pkt_len = coap_build_response(tx_buf, sizeof(tx_buf),
                                               &req, resp_code,
                                               resp_payload, resp_len);

        sendto(sock, tx_buf, resp_pkt_len, 0,
               (struct sockaddr*)&client, client_len);
    }
}
```

---

## 4. Modbus RTU/TCP 实现

### 4.1 Modbus RTU 主站

```c
/* modbus_rtu_master.c */
#include <stdint.h>
#include <string.h>

#define MODBUS_BROADCAST_ADDR   0
#define MODBUS_MAX_PDU          253
#define MODBUS_CRC_SIZE         2

typedef struct {
    int (*write)(const uint8_t *data, uint16_t len);
    int (*read)(uint8_t *data, uint16_t max_len, uint32_t timeout);
    uint8_t slave_addr;
} modbus_rtu_t;

/* CRC16 Modbus */
static uint16_t modbus_crc16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFF;

    for(uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for(int j = 0; j < 8; j++) {
            if(crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

int modbus_read_holding_registers(modbus_rtu_t *ctx,
                                  uint16_t addr,
                                  uint16_t quantity,
                                  uint16_t *out_values)
{
    uint8_t req[8];
    req[0] = ctx->slave_addr;
    req[1] = 0x03;  /* Function: Read Holding Registers */
    req[2] = addr >> 8;
    req[3] = addr & 0xFF;
    req[4] = quantity >> 8;
    req[5] = quantity & 0xFF;

    uint16_t crc = modbus_crc16(req, 6);
    req[6] = crc & 0xFF;
    req[7] = crc >> 8;

    /* 发送 */
    ctx->write(req, 8);

    /* 接收响应 */
    uint8_t resp[256];
    int len = ctx->read(resp, sizeof(resp), 500);

    if(len < 5 + quantity * 2) return -1;
    if(resp[0] != ctx->slave_addr) return -1;
    if(resp[1] != 0x03) return -1;

    uint8_t byte_count = resp[2];
    if(byte_count != quantity * 2) return -1;

    /* 解析数据 */
    for(int i = 0; i < quantity; i++) {
        out_values[i] = (resp[3 + i*2] << 8) | resp[4 + i*2];
    }

    return 0;
}

int modbus_write_single_register(modbus_rtu_t *ctx,
                                 uint16_t addr,
                                 uint16_t value)
{
    uint8_t req[8];
    req[0] = ctx->slave_addr;
    req[1] = 0x06;  /* Function: Write Single Register */
    req[2] = addr >> 8;
    req[3] = addr & 0xFF;
    req[4] = value >> 8;
    req[5] = value & 0xFF;

    uint16_t crc = modbus_crc16(req, 6);
    req[6] = crc & 0xFF;
    req[7] = crc >> 8;

    ctx->write(req, 8);

    /* 接收确认 */
    uint8_t resp[8];
    int len = ctx->read(resp, sizeof(resp), 500);

    if(len < 8) return -1;

    /* 验证响应 */
    if(memcmp(req, resp, 6) != 0) return -1;

    return 0;
}
```

---

## 5. 完整项目：多协议网关

```c
/* protocol_gateway.c - ESP32 多协议网关 */
#include "mini_mqtt.h"
#include "mini_coap.h"
#include "modbus_rtu.h"

/* 配置 */
#define MODBUS_UART     UART_NUM_1
#define MODBUS_BAUD     9600
#define MQTT_BROKER     "mqtt.local"
#define COAP_PORT       5683

/* 从机配置 */
static const struct {
    uint8_t slave_id;
    uint16_t reg_addr;
    const char *mqtt_topic;
} slaves[] = {
    {1, 0x0000, "factory/line1/temp"},
    {1, 0x0001, "factory/line1/pressure"},
    {2, 0x0000, "factory/line2/temp"},
};

void modbus_to_mqtt_task(void *pv)
{
    modbus_rtu_t modbus = {
        .write = uart_write_bytes,
        .read = uart_read_bytes,
    };

    mqtt_client_t mqtt;
    mqtt_init(&mqtt, "Gateway_001", tcp_send, tcp_recv);
    mqtt_connect(&mqtt);

    while(1) {
        for(int i = 0; i < sizeof(slaves)/sizeof(slaves[0]); i++) {
            modbus.slave_addr = slaves[i].slave_id;

            uint16_t value;
            if(modbus_read_holding_registers(&modbus,
                                            slaves[i].reg_addr,
                                            1, &value) == 0) {
                /* 发布到 MQTT */
                char payload[32];
                snprintf(payload, sizeof(payload), "%u", value);
                mqtt_publish(&mqtt, slaves[i].mqtt_topic,
                           (uint8_t*)payload, strlen(payload), 0);

                printf("Published: %s = %u\n", slaves[i].mqtt_topic, value);
            }

            vTaskDelay(pdMS_TO_TICKS(100));
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void coap_server_task(void *pv)
{
    /* CoAP 服务器提供 REST API 访问 Modbus */
    coap_server_run();
}

void app_main(void)
{
    /* 初始化 UART for Modbus */
    uart_config_t uart_config = {
        .baud_rate = MODBUS_BAUD,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(MODBUS_UART, &uart_config);
    uart_driver_install(MODBUS_UART, 256, 256, 0, NULL, 0);

    /* 连接 WiFi */
    wifi_connect("SSID", "PASS");

    /* 启动任务 */
    xTaskCreate(modbus_to_mqtt_task, "modbus_mqtt", 4096, NULL, 5, NULL);
    xTaskCreate(coap_server_task, "coap", 4096, NULL, 5, NULL);
}
```

---

## 参考资源

- **MQTT 3.1.1 规范**: <http://docs.oasis-open.org/mqtt/>
- **CoAP RFC 7252**: <https://tools.ietf.org/html/rfc7252>
- **Modbus 协议**: <https://modbus.org/specs.php>
- **Eclipse Paho**: 嵌入式 MQTT 客户端参考实现
- **ER-OERK**: CoAP C 实现

---

> **目标平台**: ESP32/STM32/树莓派
> **内存占用**: MQTT < 5KB, CoAP < 3KB
> **代码**: 800+ 行可运行代码
