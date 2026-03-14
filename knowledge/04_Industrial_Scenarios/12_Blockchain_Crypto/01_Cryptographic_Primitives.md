# 密码学原语实现

> **目标**: 详解C语言实现密码学算法的安全实践
> 
> **技术栈**: OpenSSL, libsodium
> 
> **难度**: L5 | **预估时间**: 6-8小时

---

## 1. 密码学安全基础

### 1.1 安全编程原则

```
密码学安全黄金法则
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1. 不要自己实现密码学算法
2. 使用经过审核的库 (OpenSSL, libsodium)
3. 密钥使用后立即清零
4. 使用加密安全随机数生成器
5. 使用时序安全函数
```

### 1.2 密钥安全存储

```c
#include <sys/mman.h>

typedef struct {
    unsigned char data[32];
} SecureKey;

SecureKey* secure_key_alloc(void) {
    SecureKey* key = mmap(NULL, sizeof(SecureKey),
                          PROT_READ | PROT_WRITE,
                          MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (key == MAP_FAILED) return NULL;
    mlock(key, sizeof(SecureKey));
    return key;
}

void secure_key_free(SecureKey* key) {
    if (!key) return;
    explicit_memset(key->data, 0, sizeof(key->data));
    munlock(key, sizeof(SecureKey));
    munmap(key, sizeof(SecureKey));
}
```

---

## 2. 哈希函数

### 2.1 SHA-256使用

```c
#include <openssl/evp.h>

int sha256_hash(const unsigned char* data, size_t len,
                unsigned char hash[32]) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;
    
    EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
    EVP_DigestUpdate(ctx, data, len);
    
    unsigned int hash_len;
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
    return 0;
}
```

---

## 3. 对称加密

### 3.1 AES-GCM认证加密

```c
#include <openssl/evp.h>

int aes_gcm_encrypt(const unsigned char key[32],
                    const unsigned char iv[12],
                    const unsigned char* plaintext, size_t pt_len,
                    unsigned char* ciphertext,
                    unsigned char tag[16]) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return -1;
    
    EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 12, NULL);
    EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv);
    
    int len;
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, pt_len);
    EVP_EncryptFinal_ex(ctx, ciphertext + len, &len);
    EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag);
    
    EVP_CIPHER_CTX_free(ctx);
    return 0;
}
```

---

## 4. 非对称加密

### 4.1 RSA操作

```c
#include <openssl/rsa.h>
#include <openssl/pem.h>

RSA* generate_rsa_keypair(int bits) {
    BIGNUM* bne = BN_new();
    BN_set_word(bne, RSA_F4);
    
    RSA* rsa = RSA_new();
    RSA_generate_key_ex(rsa, bits, bne, NULL);
    BN_free(bne);
    return rsa;
}

int rsa_encrypt(RSA* rsa, const unsigned char* in, size_t in_len,
                unsigned char* out) {
    return RSA_public_encrypt(in_len, in, out, rsa, RSA_PKCS1_OAEP_PADDING);
}
```

---

## 5. 随机数生成

```c
#include <sys/random.h>

int secure_random(void* buf, size_t len) {
    return getrandom(buf, len, 0) == (ssize_t)len ? 0 : -1;
}
```

---

## 6. 时序攻击防护

```c
// 常数时间比较
int constant_time_compare(const unsigned char* a, 
                          const unsigned char* b,
                          size_t len) {
    volatile unsigned char result = 0;
    for (size_t i = 0; i < len; i++) {
        result |= a[i] ^ b[i];
    }
    return result;
}
```

---

> **核心洞察**: 密码学安全的核心在于实现细节。时序攻击、侧信道泄漏往往比算法破解更危险。始终使用经过审核的库。

---

*最后更新: 2026-03-14*
