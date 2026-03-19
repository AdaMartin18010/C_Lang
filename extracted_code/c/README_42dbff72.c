/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 334
 * Language: c
 * Block ID: 42dbff72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 修复后的代码
void handle_connections(int server_fd) {
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);

        int client_fd = accept(server_fd,
                               (struct sockaddr*)&client_addr,
                               &addr_len);
        if (client_fd < 0) {
            log_error("Accept failed: %s", strerror(errno));
            continue;
        }

        // 使用 RAII 风格的资源管理
        client_data_t* data = malloc(sizeof(client_data_t));
        if (!data) {
            close(client_fd);
            log_error("Memory allocation failed");
            continue;
        }

        data->fd = client_fd;

        // 处理请求
        int result = process_request(data);

        // 确保资源被正确释放
        cleanup:
            if (data) {
                if (data->buffer) free(data->buffer);
                free(data);
            }
            if (client_fd >= 0) {
                close(client_fd);
            }
    }
}
