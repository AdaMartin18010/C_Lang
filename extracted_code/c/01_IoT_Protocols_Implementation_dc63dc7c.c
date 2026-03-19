/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 6529
 * Language: c
 * Block ID: dc63dc7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file tls_wrapper.h
 * @brief TLS/DTLS封装层
 */

#ifndef TLS_WRAPPER_H
#define TLS_WRAPPER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* TLS版本 */
typedef enum {
    TLS_VERSION_1_0 = 0x0301,
    TLS_VERSION_1_1 = 0x0302,
    TLS_VERSION_1_2 = 0x0303,
    TLS_VERSION_1_3 = 0x0304,
    DTLS_VERSION_1_0 = 0xFEFF,
    DTLS_VERSION_1_2 = 0xFEFD,
    DTLS_VERSION_1_3 = 0xFEFC
} tls_version_t;

/* 密码套件 (简化枚举) */
typedef enum {
    TLS_CIPHER_AES_128_GCM_SHA256,
    TLS_CIPHER_AES_256_GCM_SHA384,
    TLS_CIPHER_CHACHA20_POLY1305_SHA256,
    TLS_CIPHER_AES_128_CCM_SHA256,
    TLS_CIPHER_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
    TLS_CIPHER_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256
} tls_cipher_suite_t;

/* 证书类型 */
typedef enum {
    CERT_TYPE_NONE = 0,
    CERT_TYPE_PEM = 1,
    CERT_TYPE_DER = 2
} cert_type_t;

/* 最大配置 */
#define TLS_MAX_CA_CERT_LEN     4096
#define TLS_MAX_CERT_LEN        4096
#define TLS_MAX_KEY_LEN         4096
#define TLS_MAX_HOSTNAME_LEN    256
#define TLS_SESSION_ID_LEN      32
#define TLS_MASTER_SECRET_LEN   48

/* TLS上下文 */
typedef struct tls_context tls_context_t;

/* 底层IO回调 */
typedef int (*tls_send_t)(void *ctx, const uint8_t *data, size_t len);
typedef int (*tls_recv_t)(void *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms);

/* 证书信息 */
typedef struct {
    char subject[256];
    char issuer[256];
    uint32_t not_before;
    uint32_t not_after;
    uint8_t fingerprint[32];
    bool self_signed;
} tls_cert_info_t;

/* TLS配置 */
typedef struct {
    tls_version_t min_version;
    tls_version_t max_version;
    bool verify_server_cert;
    bool verify_client_cert;
    bool use_session_tickets;
    bool use_sni;

    /* 证书 */
    uint8_t ca_cert[TLS_MAX_CA_CERT_LEN];
    size_t ca_cert_len;
    cert_type_t ca_cert_type;

    uint8_t client_cert[TLS_MAX_CERT_LEN];
    size_t client_cert_len;
    cert_type_t client_cert_type;

    uint8_t client_key[TLS_MAX_KEY_LEN];
    size_t client_key_len;
    cert_type_t client_key_type;

    char server_hostname[TLS_MAX_HOSTNAME_LEN];

    /* 密码套件 */
    tls_cipher_suite_t *cipher_suites;
    uint8_t cipher_suite_count;

    /* PSK配置 */
    uint8_t psk[32];
    size_t psk_len;
    char psk_identity[64];

} tls_config_t;

/* 连接信息 */
typedef struct {
    tls_version_t negotiated_version;
    tls_cipher_suite_t cipher_suite;
    tls_cert_info_t server_cert;
    tls_cert_info_t client_cert;
    uint32_t handshake_time_ms;
    uint64_t bytes_encrypted;
    uint64_t bytes_decrypted;
} tls_connection_info_t;

/* API函数 */

/* 初始化和清理 */
tls_context_t* tls_create_context(const tls_config_t *config, bool is_dtls);
void tls_destroy_context(tls_context_t *ctx);

/* 连接管理 */
int tls_connect(tls_context_t *ctx, void *io_ctx, tls_send_t send_fn, tls_recv_t recv_fn);
int tls_accept(tls_context_t *ctx, void *io_ctx, tls_send_t send_fn, tls_recv_t recv_fn);
int tls_renegotiate(tls_context_t *ctx);
int tls_close(tls_context_t *ctx);
bool tls_is_connected(tls_context_t *ctx);

/* 数据传输 */
int tls_send(tls_context_t *ctx, const uint8_t *data, size_t len);
int tls_recv(tls_context_t *ctx, uint8_t *data, size_t max_len, uint32_t timeout_ms);
int tls_peek(tls_context_t *ctx, uint8_t *data, size_t max_len);

/* 会话管理 */
int tls_export_session(tls_context_t *ctx, uint8_t *session_data, size_t *len);
int tls_import_session(tls_context_t *ctx, const uint8_t *session_data, size_t len);
int tls_save_session_ticket(tls_context_t *ctx, const char *filename);

/* 证书验证 */
int tls_verify_certificate(tls_context_t *ctx, bool *valid, char *error_msg, size_t max_len);
int tls_get_certificate_chain(tls_context_t *ctx, tls_cert_info_t *certs, size_t max_certs);

/* 信息查询 */
int tls_get_connection_info(tls_context_t *ctx, tls_connection_info_t *info);
const char* tls_get_cipher_name(tls_cipher_suite_t cipher);
const char* tls_get_version_name(tls_version_t version);

/* 工具函数 */
int tls_load_certificate(const char *filename, uint8_t *cert, size_t *len, cert_type_t *type);
int tls_load_private_key(const char *filename, uint8_t *key, size_t *len, cert_type_t *type);
int tls_generate_self_signed_cert(const char *hostname, uint8_t *cert, size_t *cert_len,
                                   uint8_t *key, size_t *key_len);

/* 证书固定 (Certificate Pinning) */
int tls_set_pinning_hash(tls_context_t *ctx, const uint8_t *hash, size_t hash_len);
int tls_check_pinning(tls_context_t *ctx, bool *pinned);

/* DTLS特定 */
int dtls_set_mtu(tls_context_t *ctx, uint16_t mtu);
int dtls_get_peer_mtu(tls_context_t *ctx, uint16_t *mtu);

#ifdef __cplusplus
}
#endif

#endif /* TLS_WRAPPER_H */
