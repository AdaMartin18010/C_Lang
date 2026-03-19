/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 377
 * Language: c
 * Block ID: 148def39
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 更好的方案：使用包装函数确保资源释放
typedef struct {
    int fd;
    void* buffer;
} managed_client_t;

managed_client_t* client_new(int fd) {
    managed_client_t* c = calloc(1, sizeof(managed_client_t));
    if (c) c->fd = fd;
    return c;
}

void client_free(managed_client_t* c) {
    if (c) {
        if (c->fd >= 0) close(c->fd);
        free(c->buffer);
        free(c);
    }
}

// 使用示例
void handle_connections_improved(int server_fd) {
    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        managed_client_t* client = client_new(client_fd);

        if (!client) {
            close(client_fd);
            continue;
        }

        process_request(client);
        client_free(client);  // 确保释放
    }
}
