/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2695
 * Language: c
 * Block ID: 9877c8b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例: 不转换字节序
void bad_byte_order() {
    struct sockaddr_in addr;
    addr.sin_port = 8080;  // 错误! 应该使用htons
    addr.sin_addr.s_addr = 0x0100007F;  // 127.0.0.1 但字节序可能错误
}

// ✅ 正确做法
void good_byte_order() {
    struct sockaddr_in addr;
    addr.sin_port = htons(8080);  // 主机字节序转网络字节序
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);  // 127.0.0.1
    // 或使用 inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
}

// ❌ 错误示例: 协议字段不转换
void bad_protocol_field() {
    typedef struct {
        uint32_t magic;
        uint32_t length;
        uint32_t checksum;
    } packet_header_t;

    packet_header_t hdr;
    hdr.magic = 0x12345678;  // 错误! 未转网络字节序
    hdr.length = data_len;
    send(sock, &hdr, sizeof(hdr), 0);
}

// ✅ 正确做法
void good_protocol_field() {
    packet_header_t hdr;
    hdr.magic = htonl(0x12345678);
    hdr.length = htonl(data_len);
    hdr.checksum = htonl(calculate_checksum(data));
    send(sock, &hdr, sizeof(hdr), 0);
}

// 接收时转换
void receive_protocol() {
    packet_header_t hdr;
    recv(sock, &hdr, sizeof(hdr), 0);
    uint32_t magic = ntohl(hdr.magic);
    uint32_t length = ntohl(hdr.length);
    // ...
}
