/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\01_Cryptographic_Primitives.md
 * Line: 127
 * Language: c
 * Block ID: 3a21eb10
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// openssl_sha.c
#include <openssl/evp.h>
#include <openssl/err.h>

typedef enum {
    HASH_SHA256,
    HASH_SHA384,
    HASH_SHA512,
    HASH_SHA3_256
} HashType;

static const EVP_MD* get_md(HashType type) {
    switch (type) {
        case HASH_SHA256: return EVP_sha256();
        case HASH_SHA384: return EVP_sha384();
        case HASH_SHA512: return EVP_sha512();
        case HASH_SHA3_256: return EVP_sha3_256();
        default: return NULL;
    }
}

// 一次性哈希
int hash_digest(HashType type,
                const unsigned char* data, size_t data_len,
                unsigned char* hash, size_t* hash_len) {
    const EVP_MD* md = get_md(type);
    if (!md) return -1;

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) return -1;

    int ret = -1;
    if (EVP_DigestInit_ex(ctx, md, NULL) != 1) goto cleanup;
    if (EVP_DigestUpdate(ctx, data, data_len) != 1) goto cleanup;
    if (EVP_DigestFinal_ex(ctx, hash, (unsigned int*)hash_len) != 1) goto cleanup;

    ret = 0;
cleanup:
    EVP_MD_CTX_free(ctx);
    return ret;
}

// 增量哈希（大文件）
typedef struct {
    EVP_MD_CTX* ctx;
} HashContext;

HashContext* hash_create_context(HashType type) {
    HashContext* ctx = calloc(1, sizeof(HashContext));
    ctx->ctx = EVP_MD_CTX_new();
    if (!ctx->ctx) {
        free(ctx);
        return NULL;
    }

    const EVP_MD* md = get_md(type);
    if (EVP_DigestInit_ex(ctx->ctx, md, NULL) != 1) {
        EVP_MD_CTX_free(ctx->ctx);
        free(ctx);
        return NULL;
    }
    return ctx;
}

int hash_update(HashContext* ctx, const unsigned char* data, size_t len) {
    return EVP_DigestUpdate(ctx->ctx, data, len) == 1 ? 0 : -1;
}

int hash_final(HashContext* ctx, unsigned char* hash, size_t* hash_len) {
    int ret = EVP_DigestFinal_ex(ctx->ctx, hash, (unsigned int*)hash_len);
    EVP_MD_CTX_free(ctx->ctx);
    free(ctx);
    return ret == 1 ? 0 : -1;
}

// 使用示例
void hash_file_example() {
    FILE* fp = fopen("large_file.bin", "rb");
    if (!fp) return;

    HashContext* ctx = hash_create_context(HASH_SHA256);
    if (!ctx) {
        fclose(fp);
        return;
    }

    unsigned char buffer[8192];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        hash_update(ctx, buffer, bytes_read);
    }

    unsigned char hash[32];
    size_t hash_len;
    hash_final(ctx, hash, &hash_len);

    fclose(fp);

    // 打印哈希
    for (size_t i = 0; i < hash_len; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}
