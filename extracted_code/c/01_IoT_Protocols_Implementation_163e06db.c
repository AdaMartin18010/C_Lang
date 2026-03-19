/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 2252
 * Language: c
 * Block ID: 163e06db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
