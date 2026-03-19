/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\README.md
 * Line: 367
 * Language: c
 * Block ID: 0b87678f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "mqtt_client.h"
#include <string.h>

// 网络接口实现
int network_read(void *ctx, uint8_t *buf, size_t len, uint32_t timeout) {
    // 实现网络读取
    return recv(*(int*)ctx, buf, len, 0);
}

int network_write(void *ctx, const uint8_t *buf, size_t len, uint32_t timeout) {
    // 实现网络写入
    return send(*(int*)ctx, buf, len, 0);
}

int main() {
    mqtt_client_t client;
    mqtt_network_t network = {
        .context = &sock_fd,
        .read = network_read,
        .write = network_write,
        .close = NULL
    };

    // 初始化客户端
    mqtt_client_init(&client, &network);

    // 配置连接选项
    mqtt_connect_options_t options = {
        .client_id = "pico_client",
        .keep_alive_sec = 60,
        .clean_session = true
    };

    // 连接服务器
    if (mqtt_client_connect(&client, &options) == 0) {
        printf("MQTT Connected!\n");

        // 订阅主题
        mqtt_client_subscribe(&client, "sensors/+/temp", MQTT_QOS_1);

        // 发布消息
        uint8_t payload[] = "25.5";
        mqtt_client_publish(&client, "sensors/room1/temp",
                           payload, sizeof(payload)-1, MQTT_QOS_1, false);

        // 处理消息
        while (1) {
            mqtt_client_process(&client, 1000);
        }
    }

    return 0;
}
