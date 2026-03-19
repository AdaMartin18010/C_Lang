/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_EDAC_Memory.md
 * Line: 101
 * Language: c
 * Block ID: dec190dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// SECDED (Single Error Correction, Double Error Detection)
// 汉明码实现 - (72,64) 标准航天编码
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// (72,64) 汉明码参数
#define DATA_BITS       64
#define PARITY_BITS     8
#define CODE_BITS       72
#define CORRECTABLE_ERRORS  1
#define DETECTABLE_ERRORS   2

// 生成矩阵 (简化表示，实际应使用校验矩阵)
// H矩阵定义了哪些数据位影响哪些校验位

// 校验位位置: 1, 2, 4, 8, 16, 32, 64 (2的幂次)
// 数据位分布在非2幂次位置

// 预计算的奇偶校验表 (优化性能)
static const uint8_t parity_table[256] = {
    #include "parity_table.inc"  // 256字节的奇偶校验查找表
};

// 或使用内置函数
static inline uint8_t compute_parity(uint64_t data, uint64_t mask) {
    return __builtin_parityll(data & mask);
}

// ============================================================================
// 编码: 64位数据 -> 72位码字 (64数据 + 8校验)
// ============================================================================

typedef struct {
    uint64_t data;
    uint8_t ecc;
} EDACWord72;

// 生成8位校验码
uint8_t hamming_encode_72(uint64_t data) {
    uint8_t ecc = 0;

    // P1 (bit 0): 覆盖位置1,3,5,7,...的奇偶校验
    ecc |= (__builtin_parityll(data & 0xAAAAAAAAAAAAAAAAULL) << 0);

    // P2 (bit 1): 覆盖位置2,3,6,7,10,11,...的奇偶校验
    ecc |= (__builtin_parityll(data & 0xCCCCCCCCCCCCCCCCULL) << 1);

    // P4 (bit 2): 覆盖位置4-7,12-15,20-23,...的奇偶校验
    ecc |= (__builtin_parityll(data & 0xF0F0F0F0F0F0F0F0ULL) << 2);

    // P8 (bit 3)
    ecc |= (__builtin_parityll(data & 0xFF00FF00FF00FF00ULL) << 3);

    // P16 (bit 4)
    ecc |= (__builtin_parityll(data & 0xFFFF0000FFFF0000ULL) << 4);

    // P32 (bit 5)
    ecc |= (__builtin_parityll(data & 0xFFFFFFFF00000000ULL) << 5);

    // P64 (bit 6): 覆盖所有数据位
    ecc |= (__builtin_parityll(data) << 6);

    // P0 (bit 7): 全体奇偶校验 (用于双错检测)
    ecc |= (__builtin_parityll(data) ^ __builtin_parity(ecc & 0x7F)) << 7;

    return ecc;
}

// 创建EDAC码字
EDACWord72 edac_encode_72(uint64_t data) {
    EDACWord72 word;
    word.data = data;
    word.ecc = hamming_encode_72(data);
    return word;
}

// ============================================================================
// 解码与纠错
// ============================================================================

typedef enum {
    EDAC_NO_ERROR = 0,
    EDAC_SINGLE_ERROR_CORRECTED = 1,
    EDAC_DOUBLE_ERROR_DETECTED = -1,
    EDAC_ECC_ERROR = -2
} EDACResult;

// 解码并纠错
EDACResult hamming_decode_72(EDACWord72 *word, uint64_t *corrected_data) {
    // 重新计算校验码
    uint8_t syndrome = hamming_encode_72(word->data) ^ word->ecc;

    // 检查P0 (全体奇偶校验)
    uint8_t overall_parity = __builtin_parityll(word->data) ^
                             __builtin_parity(word->ecc);

    if (syndrome == 0) {
        if (overall_parity == 0) {
            // 无错误
            *corrected_data = word->data;
            return EDAC_NO_ERROR;
        } else {
            // 校验位本身错误 (P0)
            word->ecc ^= 0x80;
            *corrected_data = word->data;
            return EDAC_SINGLE_ERROR_CORRECTED;
        }
    }

    // syndrome非零
    if (overall_parity == 1) {
        // 单比特错误，可以纠正
        uint8_t error_pos = syndrome & 0x7F;  // 去除P0位

        if (error_pos == 0) {
            // 校验位错误 (非P0)
            word->ecc ^= syndrome;
            *corrected_data = word->data;
        } else if (error_pos <= 64) {
            // 数据位错误
            word->data ^= (1ULL << (error_pos - 1));
            *corrected_data = word->data;
            // 重新计算ECC
            word->ecc = hamming_encode_72(word->data);
        }

        return EDAC_SINGLE_ERROR_CORRECTED;
    } else {
        // 双比特错误 (syndrome非零但overall_parity为0)
        return EDAC_DOUBLE_ERROR_DETECTED;
    }
}

// ============================================================================
// 带EDAC的内存接口
// ============================================================================

typedef struct {
    EDACWord72 *memory;
    size_t size;
    uint32_t correctable_errors;
    uint32_t uncorrectable_errors;
    uint32_t scrub_count;
} EDACMemory;

// 初始化EDAC内存
int edac_memory_init(EDACMemory *mem, size_t num_words) {
    mem->memory = calloc(num_words, sizeof(EDACWord72));
    if (!mem->memory) return -1;

    mem->size = num_words;
    mem->correctable_errors = 0;
    mem->uncorrectable_errors = 0;
    mem->scrub_count = 0;
    return 0;
}

// 安全写入
void edac_write(EDACMemory *mem, size_t address, uint64_t data) {
    if (address >= mem->size) return;

    mem->memory[address] = edac_encode_72(data);
}

// 安全读取 (自动纠错)
EDACResult edac_read(EDACMemory *mem, size_t address, uint64_t *data) {
    if (address >= mem->size) return EDAC_ECC_ERROR;

    EDACWord72 word = mem->memory[address];
    EDACResult result = hamming_decode_72(&word, data);

    // 写回纠正后的数据 (如果需要)
    if (result == EDAC_SINGLE_ERROR_CORRECTED) {
        mem->memory[address] = word;
        mem->correctable_errors++;
    } else if (result == EDAC_DOUBLE_ERROR_DETECTED) {
        mem->uncorrectable_errors++;
    }

    return result;
}
