/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 1712
 * Language: c
 * Block ID: 905d0650
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 敏感数据安全处理
 */

// 敏感数据存储类型
typedef struct {
    unsigned char data[1024];
    size_t len;
} sensitive_buffer_t;

// 分配时清零
sensitive_buffer_t *sensitive_alloc(size_t size) {
    sensitive_buffer_t *buf = calloc(1, sizeof(sensitive_buffer_t));
    if (!buf) return NULL;
    
    if (size > sizeof(buf->data)) {
        free(buf);
        return NULL;
    }
    
    buf->len = size;
    return buf;
}

// 释放时清零
void sensitive_free(sensitive_buffer_t **buf) {
    if (!buf || !*buf) return;
    
    SECURE_ZERO((*buf)->data, (*buf)->len);
    (*buf)->len = 0;
    
    free(*buf);
    *buf = NULL;
}

// 防止核心转储
void prevent_core_dump(void) {
    #include <sys/resource.h>
    struct rlimit limit = {0, 0};
    setrlimit(RLIMIT_CORE, &limit);
}

// 锁定内存（防止交换到磁盘）
bool lock_sensitive_memory(void *addr, size_t len) {
    #ifdef __linux__
    #include <sys/mman.h>
    return mlock(addr, len) == 0;
    #else
    (void)addr; (void)len;
    return false;
    #endif
}

// 内存访问保护切换
#ifdef __linux__
#include <sys/mman.h>

typedef struct {
    void *addr;
    size_t len;
} protected_memory_t;

bool protect_memory_readonly(protected_memory_t *pm) {
    return mprotect(pm->addr, pm->len, PROT_READ) == 0;
}

bool protect_memory_readwrite(protected_memory_t *pm) {
    return mprotect(pm->addr, pm->len, PROT_READ | PROT_WRITE) == 0;
}

bool protect_memory_noaccess(protected_memory_t *pm) {
    return mprotect(pm->addr, pm->len, PROT_NONE) == 0;
}
#endif

// 完整的密码处理流程示例
bool process_password_securely(const char *user_input) {
    sensitive_buffer_t *password = sensitive_alloc(128);
    if (!password) return false;
    
    // 复制输入
    size_t input_len = strlen(user_input);
    if (input_len >= password->len) {
        sensitive_free(&password);
        return false;
    }
    memcpy(password->data, user_input, input_len);
    
    // 锁定内存
    lock_sensitive_memory(password->data, password->len);
    
    // 使用密码...
    // hash_password(password->data, ...);
    
    // 清理并释放
    sensitive_free(&password);
    return true;
}
