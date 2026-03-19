/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\04_Zero_Trust_Embedded_Architecture.md
 * Line: 863
 * Language: c
 * Block ID: 97ae3c6b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 设备间点对点安全通信
 * 无需云端中继，直接mTLS
 */

/* 设备发现 + 自动证书交换 */
int peer_to_peer_connect(const char *peer_device_id) {
    /* 1. 通过DHT或mDNS发现对等设备 */
    struct peer_info peer = discover_peer(peer_device_id);

    /* 2. 验证对等设备身份（通过云端或预置证书） */
    X509 *peer_cert = fetch_peer_certificate(peer_device_id);
    if (!verify_peer_certificate(peer_cert, peer_device_id)) {
        return -1;  // 身份验证失败
    }

    /* 3. 建立mTLS连接 */
    SSL *ssl = establish_mutual_tls(peer.address, peer.port);

    /* 4. 会话密钥派生（用于后续对称加密） */
    uint8_t session_key[32];
    derive_session_key(ssl, session_key);

    /* 5. 注册到本地对等表 */
    register_trusted_peer(peer_device_id, session_key);

    return 0;
}

/* 使用Noise协议框架（轻量级） */
#include <noise/protocol.h>

int noise_handshake(const char *peer_id, NoiseProtocolId protocol_id) {
    NoiseHandshakeState *state;
    uint8_t private_key[32];
    uint8_t public_key[32];

    /* 从TPM加载设备密钥 */
    tpm_load_key(TPM2_NV_DEVICE_KEY, private_key, 32);
    curve25519_public_from_private(public_key, private_key);

    /* 创建握手状态 */
    noise_handshakestate_new_by_id(&state, protocol_id);
    noise_handshakestate_set_role(state, NOISE_ROLE_INITIATOR);

    /* 设置本地密钥对 */
    NoiseDHState *dh;
    noise_handshakestate_get_local_keypair_dh(state, &dh);
    noise_dhstate_set_keypair(dh, private_key, 32, public_key, 32);

    /* 执行握手 */
    uint8_t message[2048];
    size_t message_len;
    noise_handshakestate_write_message(state, NULL, 0, message, &message_len);
    send_to_peer(peer_id, message, message_len);

    /* 接收响应 */
    receive_from_peer(peer_id, message, &message_len);
    noise_handshakestate_read_message(state, message, message_len, NULL, NULL);

    /* 握手完成，获取密码材料 */
    NoiseCipherState *cipher;
    noise_handshakestate_split(state, &cipher, NULL);

    return 0;
}
