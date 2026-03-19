/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\03_Infrastructure_Software.md
 * Line: 795
 * Language: c
 * Block ID: 2c5b0126
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：未关闭fd
void bad_handler(int client_fd) {
    char buffer[1024];
    read(client_fd, buffer, sizeof(buffer));
    // 忘记close(client_fd)!
}

// 正确：确保关闭
void good_handler(int client_fd) {
    char buffer[1024];
    read(client_fd, buffer, sizeof(buffer));
    close(client_fd);
}

// 更好：使用goto清理
void better_handler(int client_fd) {
    char* buffer = NULL;
    int fd2 = -1;

    buffer = malloc(1024);
    if (!buffer) goto cleanup;

    fd2 = open("log.txt", O_WRONLY);
    if (fd2 < 0) goto cleanup;

    // 处理...

cleanup:
    free(buffer);
    if (fd2 >= 0) close(fd2);
    close(client_fd);
}
