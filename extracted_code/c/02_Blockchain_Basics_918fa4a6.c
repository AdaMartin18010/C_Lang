/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\02_Blockchain_Basics.md
 * Line: 215
 * Language: c
 * Block ID: 918fa4a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 计算两个哈希的父哈希
void hash_pair(const uint8_t left[32], const uint8_t right[32],
               uint8_t parent[32]) {
    uint8_t combined[64];
    memcpy(combined, left, 32);
    memcpy(combined + 32, right, 32);
    double_sha256(combined, 64, parent);
}

// 构建默克尔根 (简化版 - 假设2^n个交易)
void build_merkle_root(uint8_t** tx_hashes, int tx_count,
                       uint8_t root[32]) {
    if (tx_count == 0) {
        memset(root, 0, 32);
        return;
    }
    if (tx_count == 1) {
        memcpy(root, tx_hashes[0], 32);
        return;
    }

    // 计算下一层哈希数
    int next_count = (tx_count + 1) / 2;
    uint8_t** next_level = malloc(next_count * sizeof(uint8_t*));

    for (int i = 0; i < next_count; i++) {
        next_level[i] = malloc(32);
        uint8_t* left = tx_hashes[i * 2];
        uint8_t* right = (i * 2 + 1 < tx_count) ?
                         tx_hashes[i * 2 + 1] : left;
        hash_pair(left, right, next_level[i]);
    }

    // 递归构建
    build_merkle_root(next_level, next_count, root);

    // 清理
    for (int i = 0; i < next_count; i++) free(next_level[i]);
    free(next_level);
}
