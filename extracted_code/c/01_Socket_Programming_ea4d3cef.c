/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3069
 * Language: c
 * Block ID: ea4d3cef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 脆弱点1: 无连接数限制
 */
void vulnerable_no_limit() {
    // 攻击者可以创建大量连接,耗尽文件描述符
    listen(sock, INT_MAX);  // 监听队列过大
    while (1) {
        accept(sock, ...);  // 无上限接受连接
    }
}

// ✅ 防护: 限制并发连接数
void protect_connection_limit() {
    #define MAX_CONNECTIONS 10000
    int connection_count = 0;

    listen(sock, 128);  // 合理的backlog

    while (1) {
        if (connection_count >= MAX_CONNECTIONS) {
            // 拒绝新连接或等待
            sleep(1);
            continue;
        }

        int client = accept(sock, ...);
        if (client >= 0) {
            connection_count++;
            // 处理连接,完成后connection_count--
        }
    }
}

/**
 * ❌ 脆弱点2: Slowloris攻击
 * 攻击者缓慢发送HTTP头,占用连接
 */
// 错误: 无读取超时
void vulnerable_slowloris(int client) {
    char buffer[4096];
    recv(client, buffer, sizeof(buffer), 0);  // 可能永远阻塞!
}

// ✅ 防护: 设置超时
void protect_timeout(int client) {
    struct timeval tv = {5, 0};  // 5秒超时
    setsockopt(client, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    char buffer[4096];
    int total = 0;
    time_t start = time(NULL);

    while (total < sizeof(buffer) - 1) {
        if (time(NULL) - start > 10) {  // 最多10秒
            close(client);
            return;
        }

        int n = recv(client, buffer + total, 1, 0);  // 逐字节读
        if (n <= 0) break;
        total += n;

        if (strstr(buffer, "\r\n\r\n")) break;  // 头结束
    }
}

/**
 * ❌ 脆弱点3: 内存分配攻击
 */
// 错误: 信任对端发送的长度字段
void vulnerable_memory(int sock) {
    uint32_t len;
    recv(sock, &len, sizeof(len), 0);
    len = ntohl(len);

    char *buffer = malloc(len);  // len可能是0xFFFFFFFF!
    recv(sock, buffer, len, 0);
}

// ✅ 防护: 验证长度
void protect_validate_length(int sock) {
    #define MAX_MSG_SIZE (10 * 1024 * 1024)  // 最大10MB

    uint32_t net_len;
    if (recv(sock, &net_len, sizeof(net_len), 0) != sizeof(net_len)) {
        return;
    }

    uint32_t len = ntohl(net_len);
    if (len > MAX_MSG_SIZE) {
        close(sock);  // 断开恶意连接
        return;
    }

    char *buffer = malloc(len);
    // ...
    free(buffer);
}

/**
 * ❌ 脆弱点4: 无速率限制
 */
// 错误: 无流量控制
void vulnerable_no_rate_limit(int sock) {
    while (1) {
        char buffer[8192];
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) break;
        // 处理...
    }
}

// ✅ 防护: 令牌桶算法
#include <time.h>

typedef struct {
    double tokens;      // 当前令牌数
    double rate;        // 令牌产生速率 (字节/秒)
    double capacity;    // 桶容量
    time_t last_time;   // 上次更新时间
} token_bucket_t;

int consume_tokens(token_bucket_t *bucket, int bytes) {
    time_t now = time(NULL);
    double elapsed = difftime(now, bucket->last_time);
    bucket->last_time = now;

    // 添加新产生的令牌
    bucket->tokens += elapsed * bucket->rate;
    if (bucket->tokens > bucket->capacity) {
        bucket->tokens = bucket->capacity;
    }

    // 消费令牌
    if (bucket->tokens < bytes) {
        return -1;  // 超限
    }
    bucket->tokens -= bytes;
    return 0;
}
