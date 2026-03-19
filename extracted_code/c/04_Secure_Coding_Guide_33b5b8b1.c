/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 676
 * Language: c
 * Block ID: 33b5b8b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 安全内存清零 - 防止编译器优化删除
 */

#if defined(__STDC_LIB_EXT1__) && (__STDC_WANT_LIB_EXT1__ == 1)
// 使用C11 memset_s
#define SECURE_ZERO(ptr, len) memset_s((ptr), (len), 0, (len))

#elif defined(__GNUC__) || defined(__clang__)
// GCC/Clang: 使用显式内存屏障
static inline void secure_zero_memory(void *ptr, size_t len) {
    volatile unsigned char *p = ptr;
    while (len--) {
        *p++ = 0;
    }
    __asm__ __volatile__("" ::: "memory");
}
#define SECURE_ZERO(ptr, len) secure_zero_memory((ptr), (len))

#elif defined(_WIN32)
// Windows: 使用SecureZeroMemory
#include <windows.h>
#define SECURE_ZERO(ptr, len) SecureZeroMemory((ptr), (len))

#elif defined(__APPLE__)
// macOS: 使用memset_s
#include <string.h>
#define SECURE_ZERO(ptr, len) memset_s((ptr), (len), 0, (len))

#else
// 通用方案：volatile函数指针
static void *(*volatile secure_memset)(void *, int, size_t) = memset;
#define SECURE_ZERO(ptr, len) secure_memset((ptr), 0, (len))
#endif

// 敏感数据清理示例
void process_password(const char *input) {
    char password[128];
    
    if (strlen(input) >= sizeof(password)) {
        return;
    }
    
    strncpy(password, input, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';
    
    // 使用密码...
    verify_password(password);
    
    // 立即安全清零
    SECURE_ZERO(password, sizeof(password));
}

// 密钥结构自动清理
typedef struct {
    unsigned char key[32];
    unsigned char iv[16];
} crypto_key_t;

void crypto_key_cleanup(crypto_key_t *key) {
    if (key) {
        SECURE_ZERO(key->key, sizeof(key->key));
        SECURE_ZERO(key->iv, sizeof(key->iv));
    }
}

// GCC/Clang cleanup属性
#ifdef __GNUC__
#define AUTO_ZERO(type, name) \
    type name __attribute__((cleanup(cleanup_##type)))

static inline void cleanup_crypto_key(crypto_key_t **k) {
    crypto_key_cleanup(*k);
}
#endif
