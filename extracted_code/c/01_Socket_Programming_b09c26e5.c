/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2566
 * Language: c
 * Block ID: b09c26e5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例: 固定缓冲区可能导致溢出
void bad_recv(int sock) {
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer), 0);
    buffer[n] = '\0';  // 如果n==1024,这里会越界!
    // ...
}

// ✅ 正确做法
void good_recv(int sock) {
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer) - 1, 0);  // 留一个字节给'\0'
    if (n > 0) {
        buffer[n] = '\0';
    }
}

// ❌ 错误示例: 不检查sprintf返回值
void bad_sprintf(char *input) {
    char output[100];
    sprintf(output, "Result: %s", input);  // input可能过长!
}

// ✅ 正确做法
void good_snprintf(const char *input) {
    char output[100];
    snprintf(output, sizeof(output), "Result: %s", input);
    output[sizeof(output) - 1] = '\0';  // 确保终止
}
