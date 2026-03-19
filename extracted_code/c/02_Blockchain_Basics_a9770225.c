/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\02_Blockchain_Basics.md
 * Line: 512
 * Language: c
 * Block ID: a9770225
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分层确定性钱包 (HD Wallet) 简化版
void derive_key(const uint8_t master_seed[64],
                const uint32_t* path, int depth,
                uint8_t private_key[32]) {
    uint8_t chain_code[32];
    memcpy(private_key, master_seed, 32);
    memcpy(chain_code, master_seed + 32, 32);

    for (int i = 0; i < depth; i++) {
        // HMAC-SHA512(child_index || private_key || chain_code)
        uint8_t data[37];
        data[0] = 0x00;
        memcpy(data + 1, private_key, 32);
        memcpy(data + 33, &path[i], 4);

        uint8_t hmac[64];
        HMAC(EVP_sha512(), chain_code, 32, data, 37, hmac, NULL);

        memcpy(private_key, hmac, 32);
        memcpy(chain_code, hmac + 32, 32);
    }
}
