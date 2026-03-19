/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 1168
 * Language: c
 * Block ID: 06fd4592
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 固件镜像签名工具代码
 * 用于构建时生成签名镜像
 */

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/sha.h>

/* 镜像签名元数据 */
typedef struct __attribute__((packed)) {
    uint32_t magic;                 /* "SIGM" */
    uint32_t version;
    uint32_t image_type;
    uint32_t security_version;
    uint8_t  image_hash[64];        /* SHA-512 */
    uint32_t hash_alg;              /* 哈希算法ID */
    uint32_t sig_alg;               /* 签名算法ID */
    uint32_t sig_len;
    uint8_t  signature[512];        /* RSA-4096签名 */
    uint32_t cert_chain_len;
    /* 后跟证书链数据 */
} image_signature_block_t;

#define SIG_MAGIC   0x4D474953  /* "SIGM" */

/* 签名镜像生成 */
int sign_firmware_image(const char *input_path,
                        const char *output_path,
                        const char *key_path,
                        uint32_t image_type,
                        uint32_t security_version) {

    /* 1. 读取输入镜像 */
    FILE *in = fopen(input_path, "rb");
    fseek(in, 0, SEEK_END);
    size_t image_size = ftell(in);
    fseek(in, 0, SEEK_SET);

    uint8_t *image_data = malloc(image_size);
    fread(image_data, 1, image_size, in);
    fclose(in);

    /* 2. 计算哈希 */
    uint8_t hash[SHA512_DIGEST_LENGTH];
    SHA512(image_data, image_size, hash);

    /* 3. 加载私钥 */
    FILE *key_file = fopen(key_path, "r");
    RSA *rsa = PEM_read_RSAPrivateKey(key_file, NULL, NULL, NULL);
    fclose(key_file);

    /* 4. 签名 */
    uint8_t signature[RSA_size(rsa)];
    unsigned int sig_len;

    RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH,
             signature, &sig_len, rsa);

    /* 5. 构造签名块 */
    image_signature_block_t sig_block = {0};
    sig_block.magic = SIG_MAGIC;
    sig_block.version = 1;
    sig_block.image_type = image_type;
    sig_block.security_version = security_version;
    memcpy(sig_block.image_hash, hash, SHA256_DIGEST_LENGTH);
    sig_block.hash_alg = HASH_ALG_SHA256;
    sig_block.sig_alg = SIG_ALG_RSA_PSS_SHA256;
    sig_block.sig_len = sig_len;
    memcpy(sig_block.signature, signature, sig_len);

    /* 6. 读取证书链 */
    sig_block.cert_chain_len = load_cert_chain("cert_chain.pem",
                                               ((uint8_t*)&sig_block) +
                                               sizeof(sig_block),
                                               4096);

    /* 7. 写出签名镜像 */
    FILE *out = fopen(output_path, "wb");

    /* 写入原始镜像 */
    fwrite(image_data, 1, image_size, out);

    /* 写入签名块 */
    fwrite(&sig_block, 1, sizeof(sig_block), out);

    /* 写入证书链 */
    /* ... */

    fclose(out);

    RSA_free(rsa);
    free(image_data);

    printf("Signed image written to: %s\n", output_path);
    printf("Security Version: %u\n", security_version);
    printf("Signature: RSA-PSS-SHA256\n");

    return 0;
}

/* 命令行工具入口 */
int main(int argc, char *argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Usage: %s <input> <output> <key> <type> <svn>\n",
                argv[0]);
        return 1;
    }

    return sign_firmware_image(argv[1], argv[2], argv[3],
                               atoi(argv[4]), atoi(argv[5]));
}
