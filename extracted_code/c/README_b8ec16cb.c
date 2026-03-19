/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\README.md
 * Line: 425
 * Language: c
 * Block ID: b8ec16cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "coap_client.h"

int main() {
    coap_client_t client;

    // 初始化客户端
    coap_client_init(&client, "coap.me", 5683);

    // 发送 GET 请求
    coap_request_t request = {
        .code = COAP_GET,
        .path = "hello"
    };

    coap_response_t response;
    if (coap_client_request(&client, &request, &response) == 0) {
        printf("Response: %.*s\n", response.payload_len, response.payload);
    }

    return 0;
}
