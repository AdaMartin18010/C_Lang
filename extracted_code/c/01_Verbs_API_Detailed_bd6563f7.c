/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 2509
 * Language: c
 * Block ID: bd6563f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA客户端示例
 *
 * 编译: gcc -o rdma_client rdma_client.c -libverbs
 */

#include "rdma_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int tcp_connect(const char *server_ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return -1;
    }

    printf("已连接到服务器 %s:%d\n", server_ip, port);
    return sock;
}

int main(int argc, char *argv[]) {
    rdma_session_t *sess;
    int sock;
    int ret;
    const char *server_ip = (argc > 1) ? argv[1] : "127.0.0.1";

    printf("=== RDMA客户端 ===\n");
    printf("服务器地址: %s\n\n", server_ip);

    /* 1. 创建RDMA会话 */
    sess = create_session(NULL, 1);
    if (!sess) {
        fprintf(stderr, "创建RDMA会话失败\n");
        return 1;
    }

    /* 2. 创建QP */
    ret = setup_qp(sess, IBV_QPT_RC);
    if (ret) {
        fprintf(stderr, "创建QP失败\n");
        goto cleanup;
    }

    /* 3. 连接TCP套接字 */
    sock = tcp_connect(server_ip, PORT);
    if (sock < 0) {
        goto cleanup;
    }

    /* 4. 交换连接信息 */
    ret = exchange_info(sess, sock);
    if (ret) {
        fprintf(stderr, "交换连接信息失败\n");
        goto cleanup_sock;
    }

    /* 5. 建立RDMA连接 */
    ret = connect_qp(sess);
    if (ret) {
        fprintf(stderr, "建立RDMA连接失败\n");
        goto cleanup_sock;
    }

    printf("\nRDMA连接已建立，开始发送数据...\n\n");

    /* 6. 通信循环 */
    for (int i = 0; i < 10; i++) {
        /* 准备消息 */
        sprintf(sess->buf, "Hello from client, message %d", i);

        /* Post Send */
        struct ibv_sge sge = {
            .addr = (uint64_t)sess->buf,
            .length = strlen(sess->buf) + 1,
            .lkey = sess->mr->lkey
        };
        struct ibv_send_wr wr = {
            .wr_id = i,
            .opcode = IBV_WR_SEND,
            .sg_list = &sge,
            .num_sge = 1,
            .send_flags = IBV_SEND_SIGNALED
        };
        struct ibv_send_wr *bad_wr;

        ret = ibv_post_send(sess->qp, &wr, &bad_wr);
        if (ret) {
            fprintf(stderr, "Post Send失败: %d\n", ret);
            break;
        }

        /* 等待发送完成 */
        struct ibv_wc wc;
        int num;
        do {
            num = ibv_poll_cq(sess->cq, 1, &wc);
        } while (num == 0);

        if (wc.status == IBV_WC_SUCCESS) {
            printf("消息 %d 发送成功\n", i);
        }

        /* Post Recv准备接收回复 */
        struct ibv_sge recv_sge = {
            .addr = (uint64_t)sess->buf,
            .length = BUFFER_SIZE,
            .lkey = sess->mr->lkey
        };
        struct ibv_recv_wr recv_wr = {
            .wr_id = i + 100,
            .sg_list = &recv_sge,
            .num_sge = 1
        };
        struct ibv_recv_wr *bad_recv_wr;

        ibv_post_recv(sess->qp, &recv_wr, &bad_recv_wr);

        /* 等待接收完成 */
        do {
            num = ibv_poll_cq(sess->cq, 1, &wc);
        } while (num == 0);

        if (wc.status == IBV_WC_SUCCESS) {
            printf("收到回复: %s\n\n", (char*)sess->buf);
        }

        sleep(1);
    }

    printf("\n通信完成\n");

cleanup_sock:
    close(sock);
cleanup:
    destroy_session(sess);
    return ret;
}
