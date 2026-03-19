/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 722
 * Language: c
 * Block ID: 25d5bfeb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简化的安全客户端
typedef struct {
    SSL_CTX* ctx;
    SSL* ssl;
    int socket;
} SecureConnection;

SecureConnection* secure_connect(const char* host, int port) {
    SecureConnection* conn = calloc(1, sizeof(SecureConnection));

    // 初始化SSL
    SSL_library_init();
    conn->ctx = SSL_CTX_new(TLS_client_method());

    // 强制TLS 1.3
    SSL_CTX_set_min_proto_version(conn->ctx, TLS1_3_VERSION);

    // 设置证书验证
    SSL_CTX_set_default_verify_paths(conn->ctx);
    SSL_CTX_set_verify(conn->ctx, SSL_VERIFY_PEER, NULL);

    // 创建socket
    conn->socket = socket(AF_INET, SOCK_STREAM, 0);
    // ... 连接代码

    // 创建SSL连接
    conn->ssl = SSL_new(conn->ctx);
    SSL_set_fd(conn->ssl, conn->socket);
    SSL_set_tlsext_host_name(conn->ssl, host);  // SNI

    if (SSL_connect(conn->ssl) <= 0) {
        // 连接失败
        secure_close(conn);
        return NULL;
    }

    // 验证证书
    X509* cert = SSL_get_peer_certificate(conn->ssl);
    if (cert) {
        long verify = SSL_get_verify_result(conn->ssl);
        if (verify != X509_V_OK) {
            fprintf(stderr, "证书验证失败\n");
        }
        X509_free(cert);
    }

    return conn;
}

int secure_send(SecureConnection* conn, const void* data, size_t len) {
    return SSL_write(conn->ssl, data, len);
}

int secure_recv(SecureConnection* conn, void* buf, size_t len) {
    return SSL_read(conn->ssl, buf, len);
}
