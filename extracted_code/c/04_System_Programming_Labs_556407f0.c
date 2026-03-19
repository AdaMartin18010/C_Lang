/*
 * Auto-generated from: 12_Practice_Exercises\04_System_Programming_Labs.md
 * Line: 348
 * Language: c
 * Block ID: 556407f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用select同时处理键盘输入和网络数据
fd_set fds;
FD_ZERO(&fds);
FD_SET(STDIN_FILENO, &fds);
FD_SET(sock_fd, &fds);

select(max_fd + 1, &fds, NULL, NULL, NULL);

if (FD_ISSET(STDIN_FILENO, &fds)) {
    // 读取用户输入并发送
}
if (FD_ISSET(sock_fd, &fds)) {
    // 接收服务器消息并显示
}
