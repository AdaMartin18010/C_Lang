/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\04_CSP_Model_C.md
 * Line: 856
 * Language: c
 * Block ID: 878593b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 使用CSP模型实现的并发HTTP服务器 */

typedef struct {
    int fd;
    char client_ip[16];
} connection_t;

void http_server(int port) {
    int listen_fd = socket_listen(port);
    chan_t* conn_ch = make_chan(sizeof(connection_t), 100);

    /* 启动工作池 */
    for (int i = 0; i < 10; i++) {
        go((void(*)(void*))worker, conn_ch);
    }

    /* 接受连接 */
    while (1) {
        connection_t conn;
        conn.fd = accept(listen_fd, conn.client_ip);

        select_case_t cases[] = {
            {conn_ch, &conn, SELECT_SEND},
            {NULL, NULL, SELECT_DEFAULT}
        };

        if (chan_select(cases, 2, 0) == 1) {
            /* 队列满，拒绝连接 */
            close(conn.fd);
        }
    }
}

void worker(chan_t* conn_ch) {
    connection_t conn;
    while (chan_recv(conn_ch, &conn)) {
        handle_request(conn.fd);
        close(conn.fd);
    }
}
