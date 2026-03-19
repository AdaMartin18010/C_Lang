/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 1644
 * Language: c
 * Block ID: 650498f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ UNSAFE: 返回局部变量地址 */
char *get_message_bad(void) {
    char msg[] = "Hello";    /* 局部数组，栈上分配 */
    return msg;              /* 返回悬挂指针！ */
}

/* ❌ UNSAFE: 返回局部指针 */
int **get_matrix_bad(void) {
    int *rows[10];           /* 局部数组 */
    for (int i = 0; i < 10; i++) {
        rows[i] = malloc(sizeof(int) * 10);
    }
    return rows;             /* rows 数组本身在栈上！ */
}

/* ✅ SAFE: 使用静态存储期 */
const char *get_status_message(int code) {
    static const char *messages[] = {
        "OK",
        "Error",
        "Not Found"
    };
    if (code >= 0 && code < 3) {
        return messages[code];
    }
    return "Unknown";
}

/* ✅ SAFE: 返回堆内存 */
char *get_message_heap(void) {
    char *msg = malloc(100);
    if (msg) {
        strcpy(msg, "Hello");
    }
    return msg;  /* 调用者负责释放 */
}

/* ✅ SAFE: 调用者提供缓冲区 */
void get_message_safe(char *buf, size_t bufsize) {
    if (buf && bufsize > 0) {
        strncpy(buf, "Hello", bufsize - 1);
        buf[bufsize - 1] = '\0';
    }
}
