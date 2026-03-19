/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 6698
 * Language: c
 * Block ID: 2790b291
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file tls_wrapper.c
 * @brief TLS/DTLS封装层实现 (基于mbedTLS API)
 */

#include "tls_wrapper.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* 模拟mbedTLS结构 (实际项目应包含mbedTLS头文件) */
typedef struct {
    void *ssl;
    void *conf;
    void *ctr_drbg;
    void *entropy;
    void *cacert;
    void *clicert;
    void *pkey;
} mbedtls_ssl_context_mock;

struct tls_context {
    bool is_dtls;
    bool connected;
    tls_config_t config;

    /* mbedTLS上下文 */
    mbedtls_ssl_context_mock mbedtls;

    /* IO回调 */
    void *io_ctx;
    tls_send_t send_fn;
    tls_recv_t recv_fn;

    /* 连接信息 */
    tls_connection_info_t conn_info;

    /* 证书固定 */
    uint8_t pinning_hash[32];
    size_t pinning_hash_len;
    bool pinning_enabled;
};

/* 创建TLS上下文 */
tls_context_t* tls_create_context(const tls_config_t *config, bool is_dtls) {
    tls_context_t *ctx = (tls_context_t *)malloc(sizeof(tls_context_t));
    if (ctx == NULL) return NULL;

    memset(ctx, 0, sizeof(tls_context_t));
    ctx->is_dtls = is_dtls;
    ctx->connected = false;

    if (config != NULL) {
        memcpy(&ctx->config, config, sizeof(tls_config_t));
    } else {
        /* 默认配置 */
        ctx->config.min_version = is_dtls ? DTLS_VERSION_1_2 : TLS_VERSION_1_2;
        ctx->config.max_version = is_dtls ? DTLS_VERSION_1_2 : TLS_VERSION_1_3;
        ctx->config.verify_server_cert = true;
    }

    /* 初始化mbedTLS (简化) */
    printf("TLS context created (%s, version: %s - %s)\n",
           is_dtls ? "DTLS" : "TLS",
           tls_get_version_name(ctx->config.min_version),
           tls_get_version_name(ctx->config.max_version));

    return ctx;
}

/* 销毁上下文 */
void tls_destroy_context(tls_context_t *ctx) {
    if (ctx == NULL) return;

    tls_close(ctx);
    free(ctx);

    printf("TLS context destroyed\n");
}

/* 连接 */
int tls_connect(tls_context_t *ctx, void *io_ctx, tls_send_t send_fn, tls_recv_t recv_fn) {
    if (ctx == NULL || send_fn == NULL || recv_fn == NULL) {
        return -1;
    }

    ctx->io_ctx = io_ctx;
    ctx->send_fn = send_fn;
    ctx->recv_fn = recv_fn;

    /* 执行握手 (简化) */
    printf("Starting TLS handshake...\n");

    /* 模拟握手 */
    uint8_t client_hello[256] = {0x16, 0x03, 0x03}; /* TLS 1.2 ClientHello */
    send_fn(io_ctx, client_hello, sizeof(client_hello));

    uint8_t server_response[1024];
    int resp_len = recv_fn(io_ctx, server_response, sizeof(server_response), 10000);
    if (resp_len <= 0) {
        printf("Handshake failed\n");
        return -1;
    }

    /* 验证服务器证书 */
    if (ctx->config.verify_server_cert) {
        printf("Verifying server certificate...\n");
        /* 实际应解析并验证证书链 */
    }

    ctx->connected = true;
    ctx->conn_info.negotiated_version = TLS_VERSION_1_2;
    ctx->conn_info.cipher_suite = TLS_CIPHER_AES_128_GCM_SHA256;
    ctx->conn_info.handshake_time_ms = 150;

    printf("TLS handshake completed (%s, cipher: %s)\n",
           tls_get_version_name(ctx->conn_info.negotiated_version),
           tls_get_cipher_name(ctx->conn_info.cipher_suite));

    return 0;
}

/* 关闭连接 */
int tls_close(tls_context_t *ctx) {
    if (ctx == NULL) return -1;

    if (ctx->connected) {
        /* 发送close_notify */
        uint8_t alert[7] = {0x15, 0x03, 0x03, 0x00, 0x02, 0x01, 0x00};
        ctx->send_fn(ctx->io_ctx, alert, sizeof(alert));

        ctx->connected = false;
        printf("TLS connection closed\n");
    }

    return 0;
}

/* 发送加密数据 */
int tls_send(tls_context_t *ctx, const uint8_t *data, size_t len) {
    if (ctx == NULL || !ctx->connected || data == NULL) {
        return -1;
    }

    /* 构建TLS记录 */
    uint8_t record[4096];
    size_t record_len = 0;

    /* TLS记录头 */
    record[record_len++] = 0x17; /* Application Data */
    record[record_len++] = 0x03;
    record[record_len++] = 0x03;

    /* 加密并添加MAC (简化) */
    size_t enc_len = len + 16 + 16; /* 数据 + IV + TAG */
    record[record_len++] = (uint8_t)(enc_len >> 8);
    record[record_len++] = (uint8_t)(enc_len & 0xFF);

    /* 填充加密数据 (简化 - 实际应使用AES-GCM) */
    memcpy(record + record_len, data, len);
    record_len += len;

    int sent = ctx->send_fn(ctx->io_ctx, record, record_len);
    if (sent > 0) {
        ctx->conn_info.bytes_encrypted += len;
    }

    return (sent > 0) ? (int)len : -1;
}

/* 接收解密数据 */
int tls_recv(tls_context_t *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms) {
    if (ctx == NULL || !ctx->connected || data == NULL) {
        return -1;
    }

    /* 接收TLS记录 */
    uint8_t record[4096];
    int recvd = ctx->recv_fn(ctx->io_ctx, record, sizeof(record), timeout_ms);
    if (recvd < 5) return 0;

    /* 解析记录头 */
    uint8_t content_type = record[0];
    /* uint16_t version = (record[1] << 8) | record[2]; */
    uint16_t length = (record[3] << 8) | record[4];

    if (content_type == 0x15) { /* Alert */
        printf("TLS Alert received\n");
        return -1;
    }

    if (content_type != 0x17) { /* Not Application Data */
        return 0;
    }

    /* 解密 (简化) */
    size_t plaintext_len = length < max_len ? length : max_len;
    memcpy(data, record + 5, plaintext_len);

    ctx->conn_info.bytes_decrypted += plaintext_len;

    return (int)plaintext_len;
}

/* 检查连接状态 */
bool tls_is_connected(tls_context_t *ctx) {
    return (ctx != NULL) && ctx->connected;
}

/* 获取密码套件名称 */
const char* tls_get_cipher_name(tls_cipher_suite_t cipher) {
    switch (cipher) {
        case TLS_CIPHER_AES_128_GCM_SHA256:
            return "TLS_AES_128_GCM_SHA256";
        case TLS_CIPHER_AES_256_GCM_SHA384:
            return "TLS_AES_256_GCM_SHA384";
        case TLS_CIPHER_CHACHA20_POLY1305_SHA256:
            return "TLS_CHACHA20_POLY1305_SHA256";
        case TLS_CIPHER_AES_128_CCM_SHA256:
            return "TLS_AES_128_CCM_SHA256";
        case TLS_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256:
            return "TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256";
        case TLS_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256:
            return "TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256";
        default:
            return "Unknown";
    }
}

/* 获取版本名称 */
const char* tls_get_version_name(tls_version_t version) {
    switch (version) {
        case TLS_VERSION_1_0: return "TLS 1.0";
        case TLS_VERSION_1_1: return "TLS 1.1";
        case TLS_VERSION_1_2: return "TLS 1.2";
        case TLS_VERSION_1_3: return "TLS 1.3";
        case DTLS_VERSION_1_0: return "DTLS 1.0";
        case DTLS_VERSION_1_2: return "DTLS 1.2";
        case DTLS_VERSION_1_3: return "DTLS 1.3";
        default: return "Unknown";
    }
}

/* 获取连接信息 */
int tls_get_connection_info(tls_context_t *ctx, tls_connection_info_t *info) {
    if (ctx == NULL || info == NULL) return -1;
    memcpy(info, &ctx->conn_info, sizeof(tls_connection_info_t));
    return 0;
}

/* 设置证书固定 */
int tls_set_pinning_hash(tls_context_t *ctx, const uint8_t *hash, size_t hash_len) {
    if (ctx == NULL || hash == NULL || hash_len > 32) return -1;
    memcpy(ctx->pinning_hash, hash, hash_len);
    ctx->pinning_hash_len = hash_len;
    ctx->pinning_enabled = true;
    return 0;
}

/* 检查证书固定 */
int tls_check_pinning(tls_context_t *ctx, bool *pinned) {
    if (ctx == NULL || pinned == NULL) return -1;

    if (!ctx->pinning_enabled) {
        *pinned = false;
        return 0;
    }

    /* 实际应比较证书公钥的哈希 */
    *pinned = true; /* 简化 */
    return 0;
}
