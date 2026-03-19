/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2894
 * Language: c
 * Block ID: c97821fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * ❌ 问题描述:
 * 主动关闭方会进入TIME_WAIT状态,持续2MSL(通常60秒)
 * 高并发短连接场景下,大量TIME_WAIT会耗尽端口或内存
 */

// 查看TIME_WAIT数量
// netstat -an | grep TIME_WAIT | wc -l

// ✅ 解决方案1: 服务器端启用SO_REUSEADDR
void good_reuseaddr() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 允许重用处于TIME_WAIT状态的地址
}

// ✅ 解决方案2: 启用TCP端口复用(Linux 3.9+)
void good_reuseport() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
    // 多个socket可以绑定到同一端口(用于负载均衡)
}

// ✅ 解决方案3: 调整TCP参数(Linux)
void adjust_tcp_params() {
    // /etc/sysctl.conf:
    // net.ipv4.tcp_tw_reuse = 1      # 允许重用TIME_WAIT连接(仅客户端)
    // net.ipv4.tcp_tw_recycle = 0    # 已废弃,不使用
    // net.ipv4.tcp_fin_timeout = 30  # 减少FIN_WAIT_2时间
    // net.ipv4.tcp_max_tw_buckets = 5000  # 限制TIME_WAIT数量
}

// ✅ 解决方案4: 连接池/长连接
void use_connection_pool() {
    // 避免频繁创建/关闭连接
    // 使用keep-alive维持长连接
    int keepalive = 1;
    setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepalive, sizeof(keepalive));
}

// ✅ 解决方案5: 客户端先关闭(服务器不进入TIME_WAIT)
void server_design_pattern() {
    // 协议设计: 客户端发送请求后主动关闭连接
    // 服务器被动关闭,不进入TIME_WAIT
}
