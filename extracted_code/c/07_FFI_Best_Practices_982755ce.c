/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\07_FFI_Best_Practices.md
 * Line: 1223
 * Language: c
 * Block ID: 982755ce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// c-example/main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crypto_ffi.h"

int main() {
    // 方法1: 使用便捷函数
    const char* message = "Hello, Rust FFI!";
    char* hash1 = NULL;

    int result = crypto_sha256_hex(
        (const uint8_t*)message,
        strlen(message),
        &hash1
    );

    if (result == 0) {
        printf("SHA256: %s\n", hash1);
        crypto_free_string(hash1);
    }

    // 方法2: 使用流式API
    HasherHandle* hasher = NULL;
    result = crypto_hasher_create(&hasher);
    if (result != 0) {
        fprintf(stderr, "Failed to create hasher\n");
        return 1;
    }

    // 分块更新
    crypto_hasher_update(hasher, (const uint8_t*)"Hello, ", 7);
    crypto_hasher_update(hasher, (const uint8_t*)"World!", 6);

    char* hash2 = NULL;
    result = crypto_hasher_finalize(hasher, &hash2);  // hasher被消耗

    if (result == 0) {
        printf("SHA256 (stream): %s\n", hash2);
        crypto_free_string(hash2);
    }

    return 0;
}
