/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 3013
 * Language: c
 * Block ID: a92edaae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 问题描述:
 * 连接一方异常终止(断电/崩溃),另一方无法感知
 * 导致连接长时间处于ESTABLISHED状态,资源被占用
 */

// 错误示例: 无心跳检测
void bad_no_keepalive(int sock) {
    while (1) {
        char buffer[1024];
        int n = recv(sock, buffer, sizeof(buffer), 0);
        if (n <= 0) break;  // 对方正常关闭才能检测到
        // 处理数据
    }
}

// ✅ 解决方案1: TCP Keepalive
void good_tcp_keepalive(int sock) {
    int keepalive = 1;      // 启用
    int idle = 60;          // 60秒无数据开始探测
    int interval = 10;      // 探测间隔10秒
    int count = 3;          // 探测3次无响应则断开

    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &idle, sizeof(idle));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof(interval));
    setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &count, sizeof(count));
}

// ✅ 解决方案2: 应用层心跳
void good_application_heartbeat(int sock) {
    // 协议定义心跳消息
    // 每30秒发送一次心跳
    // 90秒未收到响应则断开连接
}

// ✅ 解决方案3: 设置收发超时
void good_socket_timeout(int sock) {
    struct timeval tv;
    tv.tv_sec = 60;   // 60秒超时
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
}

// ✅ 解决方案4: 使用TCP_USER_TIMEOUT (Linux)
void good_tcp_user_timeout(int sock) {
    unsigned int timeout = 20000;  // 20毫秒
    setsockopt(sock, IPPROTO_TCP, TCP_USER_TIMEOUT,
               &timeout, sizeof(timeout));
}
