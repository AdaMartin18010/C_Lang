/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 2328
 * Language: c
 * Block ID: 53dabdb0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA服务器示例
 *
 * 编译: gcc -o rdma_server rdma_server.c -libverbs
 */

#include "rdma_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* 包含前面定义的函数实现... */
/* 为简洁省略，实际使用时需要包含 */

static int tcp_listen(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 1) < 0) {
        perror("listen");
        close(sock);
        return -1;
    }

    printf("TCP监听端口 %d...\n", port);
    return sock;
}

static int accept_client(int listen_sock) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    int sock = accept(listen_sock, (struct sockaddr*)&client_addr, &addr_len);
    if (sock < 0) {
        perror("accept");
        return -1;
    }

    printf("客户端连接: %s:%d\n",
           inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    return sock;
}

int main(int argc, char *argv[]) {
    rdma_session_t *sess;
    int listen_sock, client_sock;
    int ret;

    printf("=== RDMA服务器 ===\n\n");

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

    /* 3. 监听TCP连接 */
    listen_sock = tcp_listen(PORT);
    if (listen_sock < 0) {
        goto cleanup;
    }

    client_sock = accept_client(listen_sock);
    if (client_sock < 0) {
        close(listen_sock);
        goto cleanup;
    }

    /* 4. 交换连接信息 */
    ret = exchange_info(sess, client_sock);
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

    printf("\nRDMA连接已建立，准备接收数据...\n\n");

    /* 6. 通信循环 */
    for (int i = 0; i < 10; i++) {
        /* Post Recv准备接收 */
        struct ibv_sge sge = {
            .addr = (uint64_t)sess->buf,
            .length = BUFFER_SIZE,
            .lkey = sess->mr->lkey
        };
        struct ibv_recv_wr wr = {
            .wr_id = i,
            .sg_list = &sge,
            .num_sge = 1
        };
        struct ibv_recv_wr *bad_wr;

        ibv_post_recv(sess->qp, &wr, &bad_wr);

        /* 等待接收完成 */
        struct ibv_wc wc;
        int num;
        do {
            num = ibv_poll_cq(sess->cq, 1, &wc);
        } while (num == 0);

        if (wc.status == IBV_WC_SUCCESS) {
            printf("收到消息 [%d]: %s\n", i, (char*)sess->buf);

            /* 回显 */
            sprintf(sess->buf, "ACK from server: message %d received", i);

            /* Send回复 */
            struct ibv_sge send_sge = {
                .addr = (uint64_t)sess->buf,
                .length = strlen(sess->buf) + 1,
                .lkey = sess->mr->lkey
            };
            struct ibv_send_wr send_wr = {
                .wr_id = i + 100,
                .opcode = IBV_WR_SEND,
                .sg_list = &send_sge,
                .num_sge = 1,
                .send_flags = IBV_SEND_SIGNALED
            };
            struct ibv_send_wr *bad_send_wr;

            ibv_post_send(sess->qp, &send_wr, &bad_send_wr);

            /* 等待发送完成 */
            do {
                num = ibv_poll_cq(sess->cq, 1, &wc);
            } while (num == 0);
        }
    }

    printf("\n通信完成\n");

cleanup_sock:
    close(client_sock);
    close(listen_sock);
cleanup:
    destroy_session(sess);
    return ret;
}
