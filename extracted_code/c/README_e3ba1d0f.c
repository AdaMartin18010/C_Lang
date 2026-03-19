/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 307
 * Language: c
 * Block ID: e3ba1d0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 有问题的网络连接处理代码
void handle_connections(int server_fd) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        // 接受新连接
        int client_fd = accept(server_fd,
                               (struct sockaddr*)&client_addr,
                               &addr_len);

        // 为客户端数据分配内存
        client_data_t* data = malloc(sizeof(client_data_t));
        data->fd = client_fd;

        // 处理请求
        process_request(data);

        // 错误：忘记释放 data！
        // 也忘记关闭 client_fd！
    }
}
