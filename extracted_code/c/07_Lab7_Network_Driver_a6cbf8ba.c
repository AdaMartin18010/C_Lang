/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 648
 * Language: c
 * Block ID: a6cbf8ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/ping.c
#include "kernel/types.h"
#include "user/user.h"

// ICMP Echo Request
struct icmp_packet {
    uint8 type;
    uint8 code;
    uint16 checksum;
    uint16 id;
    uint16 seq;
    char data[56];
};

uint16 checksum(void *data, int len) {
    uint32 sum = 0;
    uint16 *p = data;

    while (len > 1) {
        sum += *p++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(uint8 *)p;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ping <ip>\n");
        exit(1);
    }

    // 创建原始socket
    int sock = socket(2, 3, 1);  // AF_INET, SOCK_RAW, ICMP
    if (sock < 0) {
        printf("socket failed\n");
        exit(1);
    }

    struct icmp_packet pkt;
    pkt.type = 8;  // Echo Request
    pkt.code = 0;
    pkt.id = getpid();
    pkt.seq = 1;
    memset(pkt.data, 0xAB, sizeof(pkt.data));
    pkt.checksum = 0;
    pkt.checksum = checksum(&pkt, sizeof(pkt));

    // 发送
    send(sock, &pkt, sizeof(pkt), 0);

    // 接收响应
    char buf[128];
    int n = recv(sock, buf, sizeof(buf), 0);
    if (n > 0) {
        struct icmp_packet *resp = (void *)(buf + 20);  // 跳过IP头
        if (resp->type == 0) {  // Echo Reply
            printf("Reply from %s: bytes=%d\n", argv[1], n);
        }
    }

    close(sock);
    exit(0);
}
