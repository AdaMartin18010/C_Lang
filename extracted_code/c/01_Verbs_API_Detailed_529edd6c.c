/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\01_Verbs_API_Detailed.md
 * Line: 2279
 * Language: c
 * Block ID: 529edd6c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RDMA通信公共定义
 */

#ifndef RDMA_COMMON_H
#define RDMA_COMMON_H

#include <infiniband/verbs.h>
#include <stdint.h>
#include <stdlib.h>

#define PORT 12345
#define BUFFER_SIZE 4096
#define MAX_WR 64

typedef struct {
    uint32_t qpn;
    uint32_t psn;
    uint16_t lid;
    uint8_t gid[16];
    uint32_t rkey;
    uint64_t vaddr;
} exchange_info_t;

typedef struct {
    struct ibv_context *ctx;
    struct ibv_pd *pd;
    struct ibv_cq *cq;
    struct ibv_qp *qp;
    struct ibv_mr *mr;
    void *buf;
    exchange_info_t local;
    exchange_info_t remote;
    int port_num;
} rdma_session_t;

/* 函数声明 */
rdma_session_t* create_session(const char *dev_name, int port_num);
void destroy_session(rdma_session_t *sess);
int setup_qp(rdma_session_t *sess, enum ibv_qp_type qp_type);
int connect_qp(rdma_session_t *sess);
int exchange_info(rdma_session_t *sess, int sock_fd);

#endif /* RDMA_COMMON_H */
