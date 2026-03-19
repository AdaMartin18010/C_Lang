/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\02_Error_Correction_Coding.md
 * Line: 510
 * Language: c
 * Block ID: d3df696a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// DNA Fountain编码 - Luby Transform码的DNA适配版本
// 来自Erlich & Zielinski 2017
// ============================================================================

#define FOUNTAIN_SEED_SIZE      4
#define FOUNTAIN_DROLET_SIZE    130     // DNA碱基数
#define FOUNTAIN_PAYLOAD_SIZE   120     // 有效载荷
#define FOUNTAIN_REDUNDANCY     1.1     // 额外冗余

// Fountain droplet
typedef struct {
    uint32_t seed;                  // 随机种子
    uint8_t num_blocks;             // 包含的源块数
    uint8_t payload[FOUNTAIN_PAYLOAD_SIZE]; // 异或数据
} FountainDroplet;

// 伪随机数生成器 (xorshift32)
static inline uint32_t xorshift32(uint32_t *seed) {
    uint32_t x = *seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *seed = x;
    return x;
}

// 生成度分布 (Robust Soliton-like)
uint8_t sample_degree(uint32_t *seed, int num_blocks) {
    uint32_t r = xorshift32(seed) % 1000;

    // 简化度分布
    if (r < 100) return 1;          // 10% 度1
    if (r < 550) return 2;          // 45% 度2
    if (r < 850) return 3;          // 30% 度3
    if (r < 950) return 4;          // 10% 度4
    return 5 + (r % 5);             // 5% 度5-9
}

// 选择源块
void select_source_blocks(uint32_t seed, int num_blocks,
                          uint8_t degree, uint8_t *selected) {
    uint32_t rng = seed;

    for (int i = 0; i < degree; i++) {
        selected[i] = xorshift32(&rng) % num_blocks;

        // 去重 (简单方法)
        for (int j = 0; j < i; j++) {
            if (selected[i] == selected[j]) {
                i--;  // 重新选择
                break;
            }
        }
    }
}

// 编码Fountain droplet
void fountain_encode_droplet(const uint8_t *source_data, int num_blocks,
                             uint32_t seed, FountainDroplet *droplet) {
    droplet->seed = seed;

    // 确定度
    uint32_t rng = seed;
    droplet->num_blocks = sample_degree(&rng, num_blocks);

    // 选择源块
    uint8_t selected[32];
    select_source_blocks(seed, num_blocks, droplet->num_blocks, selected);

    // XOR组合
    memset(droplet->payload, 0, FOUNTAIN_PAYLOAD_SIZE);

    for (int i = 0; i < droplet->num_blocks; i++) {
        int block_idx = selected[i];
        const uint8_t *block = source_data + block_idx * FOUNTAIN_PAYLOAD_SIZE;

        for (int j = 0; j < FOUNTAIN_PAYLOAD_SIZE; j++) {
            droplet->payload[j] ^= block[j];
        }
    }
}

// ============================================================================
// Fountain解码 (信念传播)
// ============================================================================

typedef struct {
    FountainDroplet *droplets;
    int num_droplets;
    uint8_t *decoded;
    bool *block_decoded;
    int num_blocks;
    int blocks_remaining;
} FountainDecoder;

int fountain_decode(FountainDecoder *dec) {
    bool progress = true;

    while (dec->blocks_remaining > 0 && progress) {
        progress = false;

        // 寻找度为1的droplet
        for (int d = 0; d < dec->num_droplets; d++) {
            if (dec->droplets[d].num_blocks == 1) {
                // 找到未解码的源块
                uint8_t block_idx;
                select_source_blocks(dec->droplets[d].seed,
                                    dec->num_blocks, 1, &block_idx);

                if (!dec->block_decoded[block_idx]) {
                    // 解码此块
                    memcpy(dec->decoded + block_idx * FOUNTAIN_PAYLOAD_SIZE,
                           dec->droplets[d].payload, FOUNTAIN_PAYLOAD_SIZE);
                    dec->block_decoded[block_idx] = true;
                    dec->blocks_remaining--;
                    progress = true;

                    // 传播: 用此块更新其他droplets
                    for (int d2 = 0; d2 < dec->num_droplets; d2++) {
                        if (dec->droplets[d2].num_blocks > 1) {
                            // 检查是否包含此块
                            uint8_t selected[32];
                            select_source_blocks(dec->droplets[d2].seed,
                                                dec->num_blocks,
                                                dec->droplets[d2].num_blocks,
                                                selected);

                            for (int k = 0; k < dec->droplets[d2].num_blocks; k++) {
                                if (selected[k] == block_idx) {
                                    // XOR消去
                                    for (int j = 0; j < FOUNTAIN_PAYLOAD_SIZE; j++) {
                                        dec->droplets[d2].payload[j] ^=
                                            dec->decoded[block_idx * FOUNTAIN_PAYLOAD_SIZE + j];
                                    }
                                    dec->droplets[d2].num_blocks--;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return dec->blocks_remaining == 0 ? 0 : -1;
}
