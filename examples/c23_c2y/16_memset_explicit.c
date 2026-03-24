/*
 * C23 memset_explicit 安全内存清零示例
 * 编译: gcc -std=c23 -o 16_memset_explicit 16_memset_explicit.c
 *       clang -std=c23 -o 16_memset_explicit 16_memset_explicit.c
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 安全密码处理
void process_password(const char *input) {
    char password[128];
    
    // 复制密码
    strncpy(password, input, sizeof(password) - 1);
    password[sizeof(password) - 1] = '\0';
    
    printf("Processing password...\n");
    // ... 验证密码 ...
    
    // 安全清零（不会被优化掉）
    memset_explicit(password, 0, sizeof(password));
    printf("Password cleared securely\n");
}

// 密钥处理示例
typedef struct {
    unsigned char key[32];
    size_t key_len;
} SecureKey;

void generate_key(SecureKey *sk) {
    // 模拟密钥生成
    for (size_t i = 0; i < sizeof(sk->key); i++) {
        sk->key[i] = (unsigned char)(i * 7 + 13);
    }
    sk->key_len = 32;
}

void destroy_key(SecureKey *sk) {
    if (sk) {
        // 安全清零密钥
        memset_explicit(sk->key, 0, sizeof(sk->key));
        sk->key_len = 0;
    }
}

// 对比：普通memset可能被优化
void unsafe_clear(void *ptr, size_t len) {
    // 编译器可能优化掉这个调用
    memset(ptr, 0, len);
}

void safe_clear(void *ptr, size_t len) {
    // 保证不会被优化
    memset_explicit(ptr, 0, len);
}

// 堆内存安全清零
void* secure_allocate(size_t size) {
    void *ptr = malloc(size);
    if (ptr) {
        // 初始化为随机值或零
        memset(ptr, 0, size);
    }
    return ptr;
}

void secure_free(void *ptr, size_t size) {
    if (ptr) {
        memset_explicit(ptr, 0, size);
        free(ptr);
    }
}

// 敏感数据结构
typedef struct {
    char username[64];
    char password_hash[64];
    char session_token[128];
} UserSession;

void cleanup_session(UserSession *session) {
    if (!session) return;
    
    printf("Cleaning up session for: %s\n", session->username);
    
    // 安全清零所有敏感字段
    memset_explicit(session->password_hash, 0, sizeof(session->password_hash));
    memset_explicit(session->session_token, 0, sizeof(session->session_token));
    
    // 用户名可以普通清零（非敏感）
    memset(session->username, 0, sizeof(session->username));
}

int main(void) {
    printf("C23 memset_explicit Demo\n");
    printf("========================\n\n");
    
    printf("--- Password Processing ---\n");
    process_password("my_secret_password_123");
    
    printf("\n--- Secure Key Management ---\n");
    SecureKey key;
    generate_key(&key);
    printf("Key generated (len=%zu)\n", key.key_len);
    printf("First byte: 0x%02X\n", key.key[0]);
    
    destroy_key(&key);
    printf("After destroy, first byte: 0x%02X (should be 0)\n", key.key[0]);
    
    printf("\n--- Heap Memory ---\n");
    size_t secret_size = 256;
    unsigned char *secret = secure_allocate(secret_size);
    if (secret) {
        // 使用secret...
        for (size_t i = 0; i < secret_size; i++) {
            secret[i] = (unsigned char)(i + 100);
        }
        printf("Secret first byte: 0x%02X\n", secret[0]);
        
        secure_free(secret, secret_size);
        printf("Memory securely freed\n");
    }
    
    printf("\n--- User Session ---\n");
    UserSession session = {
        .username = "john_doe",
        .password_hash = "hashed_password_here",
        .session_token = "abc123xyz_session_token"
    };
    
    printf("Session token before: %s\n", session.session_token);
    cleanup_session(&session);
    printf("Session token after: %s (cleared)\n", session.session_token);
    
    printf("\n--- Comparison ---\n");
    printf("memset:      May be optimized away by compiler\n");
    printf("memset_explicit: Guaranteed NOT to be optimized\n");
    printf("explicit_bzero:  Non-standard alternative (BSD)\n");
    printf("SecureZeroMemory: Windows specific\n");
    
    printf("\nmemset_explicit demo complete!\n");
    
    return 0;
}
