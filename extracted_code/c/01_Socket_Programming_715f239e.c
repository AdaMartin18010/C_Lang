/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2600
 * Language: c
 * Block ID: 715f239e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 错误示例: 假设每次recv能收到完整的消息
 * TCP是字节流,不保留消息边界
 */
void bad_message_handling(int sock) {
    char buffer[1024];
    // 发送方发送 "Hello" 和 "World" 两次
    // 接收方可能一次收到 "HelloWorld"
    int n = recv(sock, buffer, sizeof(buffer), 0);
    // 错误假设: buffer中包含一条完整消息
    process_message(buffer, n);
}

/**
 * ✅ 正确做法1: 定长消息
 */
void good_fixed_length(int sock) {
    #define MSG_LEN 256
    char buffer[MSG_LEN];
    int received = 0;

    while (received < MSG_LEN) {
        int n = recv(sock, buffer + received, MSG_LEN - received, 0);
        if (n <= 0) return; // 错误或关闭
        received += n;
    }
    process_message(buffer, MSG_LEN);
}

/**
 * ✅ 正确做法2: 消息长度前缀
 */
typedef struct {
    uint32_t length;  // 网络字节序
    char data[];      // 变长数据
} message_t;

void good_length_prefix(int sock) {
    // 先读取4字节长度
    uint32_t net_len;
    int n = recv_all(sock, &net_len, sizeof(net_len));
    if (n != sizeof(net_len)) return;

    uint32_t len = ntohl(net_len);
    if (len > MAX_MESSAGE_SIZE) {
        // 防止内存分配攻击
        close(sock);
        return;
    }

    // 分配缓冲区并读取数据
    char *buffer = malloc(len);
    n = recv_all(sock, buffer, len);
    if (n == len) {
        process_message(buffer, len);
    }
    free(buffer);
}

/**
 * ✅ 正确做法3: 特殊分隔符
 */
void good_delimiter(int sock) {
    char buffer[4096];
    int buf_len = 0;

    while (1) {
        int n = recv(sock, buffer + buf_len, sizeof(buffer) - buf_len, 0);
        if (n <= 0) return;
        buf_len += n;

        // 查找分隔符(如\n)
        char *newline = memchr(buffer, '\n', buf_len);
        while (newline != NULL) {
            int msg_len = newline - buffer + 1;
            process_message(buffer, msg_len);

            // 移动剩余数据
            memmove(buffer, buffer + msg_len, buf_len - msg_len);
            buf_len -= msg_len;
            newline = memchr(buffer, '\n', buf_len);
        }

        if (buf_len >= sizeof(buffer)) {
            // 缓冲区满但未找到分隔符,错误
            close(sock);
            return;
        }
    }
}
