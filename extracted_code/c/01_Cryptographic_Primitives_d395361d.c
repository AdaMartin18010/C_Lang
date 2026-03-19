/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 633
 * Language: c
 * Block ID: d395361d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux getrandom (内核3.17+)
#include <sys/random.h>

int secure_random(void* buf, size_t len) {
    size_t total = 0;
    while (total < len) {
        ssize_t ret = getrandom((char*)buf + total, len - total, 0);
        if (ret < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        total += ret;
    }
    return 0;
}

// OpenSSL CSPRNG
int openssl_random(void* buf, size_t len) {
    return RAND_bytes(buf, len) == 1 ? 0 : -1;
}

// libsodium随机
void sodium_random_example(void) {
    // 生成随机32字节
    unsigned char random_bytes[32];
    randombytes_buf(random_bytes, sizeof(random_bytes));

    // 生成随机32位整数
    uint32_t random_int = randombytes_random();

    // 生成范围内的随机数
    uint32_t bounded = randombytes_uniform(100);  // 0-99
}
