/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3214
 * Language: c
 * Block ID: 2c0096e8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 错误示例1: 忽略EINTR
 */
void bad_ignore_eintr() {
    int n = recv(sock, buffer, size, 0);
    if (n < 0) {
        perror("recv");  // 如果是EINTR,这是正常的
        close(sock);     // 不应该关闭!
    }
}

// ✅ 正确处理
void good_handle_eintr() {
    ssize_t n;
    do {
        n = recv(sock, buffer, size, 0);
    } while (n < 0 && errno == EINTR);

    if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // 非阻塞,稍后再试
        } else {
            perror("recv");
            close(sock);
        }
    } else if (n == 0) {
        // 连接关闭
        close(sock);
    }
}

/**
 * ❌ 错误示例2: 部分发送不处理
 */
void bad_partial_send() {
    const char *msg = "Hello World";
    send(sock, msg, strlen(msg), 0);  // 可能只发送了部分数据!
}

// ✅ 正确处理部分发送
ssize_t good_full_send(int sock, const void *buf, size_t len) {
    const char *p = buf;
    size_t remaining = len;

    while (remaining > 0) {
        ssize_t n = send(sock, p, remaining, MSG_NOSIGNAL);
        if (n < 0) {
            if (errno == EINTR) continue;
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // 等待可写事件(如果使用非阻塞)
                return len - remaining;  // 返回已发送字节数
            }
            return -1;  // 错误
        }
        p += n;
        remaining -= n;
    }
    return len;
}

/**
 * ❌ 错误示例3: 错误的错误码检查顺序
 */
void bad_error_order() {
    close(sock);
    if (errno == ECONNRESET) {  // 错误! close可能重置errno
        // ...
    }
}

// ✅ 正确做法
void good_error_order() {
    int saved_errno = errno;
    close(sock);
    if (saved_errno == ECONNRESET) {
        // ...
    }
}
