/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 303
 * Language: c
 * Block ID: 00f618e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 嵌入式设备mTLS客户端实现
 * 使用设备证书进行双向认证
 */

#include <openssl/ssl.h>
#include <openssl/err.h>

/* 加载TPM中的设备证书和私钥 */
SSL_CTX* create_device_ssl_context(TPM2_HANDLE key_handle) {
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    /* 1. 设置证书验证模式（必须验证服务器） */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       verify_server_certificate);

    /* 2. 加载CA证书链 */
    SSL_CTX_load_verify_locations(ctx, "/etc/ssl/certs/ca-chain.pem", NULL);

    /* 3. 加载设备证书（从TPM NV读取） */
    X509 *device_cert = load_cert_from_tpm_nv(TPM2_NV_DEVICE_CERT_INDEX);
    SSL_CTX_use_certificate(ctx, device_cert);

    /* 4. 设置TPM私钥（不导出私钥，使用TPM运算） */
    EVP_PKEY *tpm_key = create_tpm_pkey_wrapper(key_handle);
    SSL_CTX_use_PrivateKey(ctx, tpm_key);

    /* 5. 强制TLS 1.3 + 强密码套件 */
    SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION);
    SSL_CTX_set_cipher_list(ctx, "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256");

    return ctx;
}

/* 连接到云服务 */
int secure_connect(const char *hostname, int port, TPM2_HANDLE key_handle) {
    SSL_CTX *ctx = create_device_ssl_context(key_handle);

    int sock = socket(AF_INET6, SOCK_STREAM, 0);
    // ... 连接建立代码 ...

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, sock);

    /* 执行握手（包含双向认证） */
    if (SSL_connect(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }

    /* 验证对端身份 */
    X509 *server_cert = SSL_get_peer_certificate(ssl);
    if (!verify_cloud_certificate(server_cert, hostname)) {
        fprintf(stderr, "服务器证书验证失败\n");
        return -1;
    }

    printf("mTLS连接建立成功，双方身份已验证\n");
    return ssl;
}
