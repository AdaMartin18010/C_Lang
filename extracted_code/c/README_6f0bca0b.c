/*
 * Auto-generated from: 03_System_Technology_Domains\13_RDMA_Network\README.md
 * Line: 72
 * Language: c
 * Block ID: 6f0bca0b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RoCE 配置示例
#include <rdma/rdma_cma.h>

struct rdma_event_channel *ec;
struct rdma_cm_id *conn_id;
struct ibv_qp_init_attr qp_attr = {};

// 创建事件通道
ec = rdma_create_event_channel();
if (!ec) {
    perror("rdma_create_event_channel");
    return -1;
}

// 创建 RDMA 连接标识符
ret = rdma_create_id(ec, &conn_id, NULL, RDMA_PS_TCP);
if (ret) {
    perror("rdma_create_id");
    return -1;
}

// 解析地址
struct sockaddr_in addr = {};
addr.sin_family = AF_INET;
addr.sin_port = htons(port);
inet_pton(AF_INET, server_ip, &addr.sin_addr);

ret = rdma_resolve_addr(conn_id, NULL,
                        (struct sockaddr *)&addr, 2000);
if (ret) {
    perror("rdma_resolve_addr");
    return -1;
}
