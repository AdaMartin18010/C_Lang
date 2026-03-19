/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 770
 * Language: c
 * Block ID: c227941b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全MQTT over TLS 1.3
 * 设备到云端的安全消息通道
 */

#include <mosquitto.h>
#include <openssl/ssl.h>

/* 零信任MQTT连接 */
struct zerotrust_mqtt_client {
    struct mosquitto *mosq;
    SSL_CTX *ssl_ctx;
    TPM2_HANDLE device_key;
    char device_id[64];
    uint32_t session_nonce;
};

/* 初始化安全MQTT客户端 */
struct zerotrust_mqtt_client* zt_mqtt_init(const char *device_id,
                                          TPM2_HANDLE key_handle) {
    struct zerotrust_mqtt_client *client = calloc(1, sizeof(*client));

    strncpy(client->device_id, device_id, sizeof(client->device_id));
    client->device_key = key_handle;

    /* 创建TLS上下文（双向认证） */
    client->ssl_ctx = create_device_ssl_context(key_handle);

    /* 配置MQTT客户端 */
    client->mosq = mosquitto_new(device_id, true, client);

    /* 设置TLS */
    mosquitto_tls_set(client->mosq,
                      "/etc/ssl/certs/ca.crt",     // CA证书
                      NULL,                         // CAPATH
                      "/etc/device.crt",            // 设备证书
                      NULL,                         // 设备密钥（TPM管理）
                      NULL);                        // 密码回调

    /* 设置回调 */
    mosquitto_message_callback_set(client->mosq, on_secure_message);
    mosquitto_connect_callback_set(client->mosq, on_connect);

    return client;
}

/* 安全消息发布（加密+签名） */
int zt_mqtt_publish(struct zerotrust_mqtt_client *client,
                    const char *topic,
                    const void *payload,
                    size_t payload_len) {
    /* 1. 构造消息信封 */
    struct message_envelope {
        char device_id[64];
        uint32_t timestamp;
        uint32_t nonce;
        uint8_t payload_hash[32];
    } envelope;

    strncpy(envelope.device_id, client->device_id, 64);
    envelope.timestamp = time(NULL);
    envelope.nonce = ++client->session_nonce;
    sha256(payload, payload_len, envelope.payload_hash);

    /* 2. 使用设备密钥签名 */
    uint8_t signature[64];
    tpm_sign(client->device_key,
             &envelope, sizeof(envelope),
             signature, sizeof(signature));

    /* 3. 构造完整消息 */
    uint8_t message[2048];
    size_t offset = 0;
    memcpy(message + offset, &envelope, sizeof(envelope));
    offset += sizeof(envelope);
    memcpy(message + offset, signature, sizeof(signature));
    offset += sizeof(signature);
    memcpy(message + offset, payload, payload_len);
    offset += payload_len;

    /* 4. 发布到受限主题 */
    char secure_topic[128];
    snprintf(secure_topic, sizeof(secure_topic),
             "devices/%s/telemetry", client->device_id);

    return mosquitto_publish(client->mosq, NULL, secure_topic,
                             offset, message, 1, false);
}
