/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 1040
 * Language: c
 * Block ID: c415f3f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
