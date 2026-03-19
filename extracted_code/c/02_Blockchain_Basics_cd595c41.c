/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\02_Blockchain_Basics.md
 * Line: 351
 * Language: c
 * Block ID: cd595c41
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// RIPEMD160(SHA256(pubkey))
void hash160(const uint8_t* data, size_t len, uint8_t hash[20]) {
    uint8_t sha256_hash[32];
    SHA256(data, len, sha256_hash);

    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_ripemd160(), NULL);
    EVP_DigestUpdate(ctx, sha256_hash, 32);

    unsigned int hash_len;
    EVP_DigestFinal_ex(ctx, hash, &hash_len);
    EVP_MD_CTX_free(ctx);
}

// Base58编码
static const char BASE58_CHARS[] =
    "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

int base58_encode(const uint8_t* data, size_t len, char* out, size_t out_len) {
    // 计算前导零
    int zeros = 0;
    while (zeros < len && data[zeros] == 0) zeros++;

    // 大数转换
    uint8_t* buf = malloc(len * 2);  // 足够大
    memset(buf, 0, len * 2);

    for (size_t i = zeros; i < len; i++) {
        uint16_t carry = data[i];
        for (size_t j = 0; j < len * 2; j++) {
            carry += (uint16_t)buf[j] << 8;
            buf[j] = carry % 58;
            carry /= 58;
        }
    }

    // 跳过前导零
    size_t j = len * 2;
    while (j > 0 && buf[j-1] == 0) j--;

    // 编码
    if (out_len < (size_t)zeros + j + 1) {
        free(buf);
        return -1;
    }

    size_t k = 0;
    for (int i = 0; i < zeros; i++) out[k++] = '1';
    while (j > 0) out[k++] = BASE58_CHARS[buf[--j]];
    out[k] = '\0';

    free(buf);
    return 0;
}

// 生成P2PKH地址
void generate_address(const uint8_t public_key[33], char address[35]) {
    // 1. Hash160
    uint8_t hash[20];
    hash160(public_key, 33, hash);

    // 2. 添加版本字节 (0x00 for mainnet)
    uint8_t versioned[21];
    versioned[0] = 0x00;
    memcpy(versioned + 1, hash, 20);

    // 3. 计算校验和 (双SHA256前4字节)
    uint8_t checksum[32];
    double_sha256(versioned, 21, checksum);

    // 4. 组合
    uint8_t binary_addr[25];
    memcpy(binary_addr, versioned, 21);
    memcpy(binary_addr + 21, checksum, 4);

    // 5. Base58编码
    base58_encode(binary_addr, 25, address, 35);
}
