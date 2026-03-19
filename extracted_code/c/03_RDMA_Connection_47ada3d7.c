/*
 * Auto-generated from: 03_System_Technology_Domains\03_RDMA_Connection.md
 * Line: 460
 * Language: c
 * Block ID: 47ada3d7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RDMA CM提供更简单的连接管理
#include <rdma/rdma_cma.h>

// 服务端
void server_listen() {
    struct rdma_cm_id *listen_id;
    struct rdma_event_channel *ec = rdma_create_event_channel();

    rdma_create_id(ec, &listen_id, NULL, RDMA_PS_TCP);
    rdma_bind_addr(listen_id, (struct sockaddr *)&server_addr);
    rdma_listen(listen_id, 10);

    // 等待连接请求
    struct rdma_cm_event *event;
    rdma_get_cm_event(ec, &event);
    if (event->event == RDMA_CM_EVENT_CONNECT_REQUEST) {
        struct rdma_cm_id *conn_id = event->id;
        // 接受连接...
        rdma_accept(conn_id, &conn_param);
    }
}

// 客户端
void client_connect() {
    struct rdma_cm_id *conn_id;
    struct rdma_event_channel *ec = rdma_create_event_channel();

    rdma_create_id(ec, &conn_id, NULL, RDMA_PS_TCP);
    rdma_resolve_addr(conn_id, NULL, (struct sockaddr *)&server_addr, 2000);
    rdma_resolve_route(conn_id, 2000);
    rdma_connect(conn_id, &conn_param);
}
