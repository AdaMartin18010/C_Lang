/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2535
 * Language: c
 * Block ID: 6ca4b7bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例: 未处理SIGPIPE,写入已关闭连接会导致进程终止
void bad_example() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, ...);

    // 如果对端关闭了连接,send会触发SIGPIPE,默认行为是终止进程
    send(sock, "data", 4, 0);  // 进程可能被杀死!
}

// ✅ 正确做法1: 忽略SIGPIPE
void good_example1() {
    signal(SIGPIPE, SIG_IGN);  // 全局忽略
    // 或
    // sigaction(SIGPIPE, &(struct sigaction){.sa_handler = SIG_IGN}, NULL);
}

// ✅ 正确做法2: 使用MSG_NOSIGNAL标志
void good_example2(int sock) {
    send(sock, "data", 4, MSG_NOSIGNAL);  // 单次发送忽略SIGPIPE
}

// ✅ 正确做法3: 检查对端关闭状态
void good_example3(int sock) {
    // 使用poll/epoll检测EPOLLRDHUP
    // 或在send前检查连接状态
}
