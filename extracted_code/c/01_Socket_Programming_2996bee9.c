/*
 * Auto-generated from: 03_System_Technology_Domains\15_Network_Programming\01_Socket_Programming.md
 * Line: 2746
 * Language: c
 * Block ID: 2996bee9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例: 资源泄漏
void bad_resource_leak() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    if (connect(sock, ...) < 0) {
        return;  // 错误! sock未关闭
    }

    // ... 使用sock

    if (error_condition) {
        return;  // 错误! sock未关闭
    }

    close(sock);
}

// ✅ 正确做法1: goto清理
void good_goto_cleanup() {
    int sock = -1;
    int ret = -1;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) goto cleanup;

    if (connect(sock, ...) < 0) goto cleanup;

    // ... 使用sock

    ret = 0;
cleanup:
    if (sock >= 0) close(sock);
    return ret;
}

// ✅ 正确做法2: 使用do-while(0)
void good_do_while() {
    int sock = -1;
    int result = -1;

    do {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) break;

        if (connect(sock, ...) < 0) break;

        // ... 使用sock

        result = 0;
    } while (0);

    if (sock >= 0) close(sock);
    return result;
}

// ✅ 正确做法3: 使用__attribute__((cleanup)) (GCC扩展)
#ifdef __GNUC__
void auto_close(int *fd) {
    if (*fd >= 0) {
        close(*fd);
        *fd = -1;
    }
}

void good_cleanup_attribute() {
    int sock __attribute__((cleanup(auto_close))) = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) return;

    if (connect(sock, ...) < 0) return;  // sock会自动关闭

    // ...
}  // 函数退出时sock自动关闭
#endif
