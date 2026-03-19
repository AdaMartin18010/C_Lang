/*
 * Auto-generated from: 04_Industrial_Scenarios\12_Blockchain_Crypto\02_Blockchain_Basics.md
 * Line: 436
 * Language: c
 * Block ID: ca1ec314
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简化版区块链
#define MAX_BLOCKS 1000

typedef struct {
    Block blocks[MAX_BLOCKS];
    int count;
    uint32_t difficulty;
} Blockchain;

// 初始化创世区块
void init_blockchain(Blockchain* chain) {
    chain->count = 0;
    chain->difficulty = 0x1d00ffff; // 比特币创世难度

    // 创建创世区块
    Block* genesis = &chain->blocks[0];
    memset(&genesis->header, 0, sizeof(BlockHeader));
    genesis->header.version = 1;
    genesis->header.timestamp = 1231006505; // 2009-01-03
    genesis->header.bits = chain->difficulty;

    // 设置创世交易
    genesis->tx_count = 1;
    genesis->transactions = malloc(sizeof(Transaction));
    // ... 初始化 coinbase 交易

    chain->count = 1;
}

// 添加新区块
int add_block(Blockchain* chain, Block* new_block) {
    if (chain->count >= MAX_BLOCKS) return -1;

    // 验证前一区块哈希
    Block* prev = &chain->blocks[chain->count - 1];
    uint8_t prev_hash[32];
    block_hash(&prev->header, prev_hash);

    if (memcmp(new_block->header.prev_hash, prev_hash, 32) != 0) {
        return -1; // 前一区块哈希不匹配
    }

    // 验证工作量证明
    uint8_t new_hash[32];
    block_hash(&new_block->header, new_hash);
    if (!check_proof_of_work(new_hash, new_block->header.bits)) {
        return -1; // 工作量证明无效
    }

    // 添加区块
    memcpy(&chain->blocks[chain->count], new_block, sizeof(Block));
    chain->count++;

    return 0;
}
