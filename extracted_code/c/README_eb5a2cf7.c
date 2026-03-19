/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 169
 * Language: c
 * Block ID: eb5a2cf7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 汉明码 ECC 实现
#define ECC_DATA_BITS 64
#define ECC_CHECK_BITS 8
#define ECC_TOTAL_BITS (ECC_DATA_BITS + ECC_CHECK_BITS)

// 生成汉明校验位
uint8_t hamming_encode(uint64_t data) {
    uint8_t ecc = 0;
    // 校验位位置：1, 2, 4, 8, 16, 32, 64, 128
    // 每个校验位覆盖特定数据位

    // P1 覆盖位: 3,5,7,9,11,...
    ecc |= (__builtin_parity(data & 0xAAAAAAAAAAAAAAABULL) << 0);
    // P2 覆盖位: 3,6,7,10,11,...
    ecc |= (__builtin_parity(data & 0xCCCCCCCCCCCCCCCCULL) << 1);
    // P4 覆盖位: 5-7, 12-15, ...
    ecc |= (__builtin_parity(data & 0xF0F0F0F0F0F0F0F0ULL) << 2);
    // P8
    ecc |= (__builtin_parity(data & 0xFF00FF00FF00FF00ULL) << 3);
    // P16
    ecc |= (__builtin_parity(data & 0xFFFF0000FFFF0000ULL) << 4);
    // P32
    ecc |= (__builtin_parity(data & 0xFFFFFFFF00000000ULL) << 5);

    return ecc;
}

// ECC 解码和纠错
int ecc_decode(uint64_t *data, uint8_t received_ecc) {
    uint8_t calculated_ecc = hamming_encode(*data);
    uint8_t syndrome = received_ecc ^ calculated_ecc;

    if (syndrome == 0) {
        return 0;  // 无错误
    }

    // 检查是否是单比特错误
    if ((syndrome & (syndrome - 1)) == 0) {
        // 校验位错误，数据正确
        return 1;
    }

    // 数据位错误，纠正
    int error_bit = syndrome - 1;  // 汉明码错误位编号
    if (error_bit < 64) {
        *data ^= (1ULL << error_bit);
        return 1;  // 纠正成功
    }

    return -1;  // 多位错误，无法纠正
}

// 扩展 ECC 实现 SEC-DED
uint8_t secded_encode(uint64_t data) {
    uint8_t ecc = hamming_encode(data);
    // 添加整体奇偶校验位
    ecc |= (__builtin_parity(data) << 7);
    return ecc;
}

int secded_decode(uint64_t *data, uint8_t received_ecc) {
    uint8_t calculated_ecc = secded_encode(*data);
    uint8_t syndrome = received_ecc ^ calculated_ecc;

    if (syndrome == 0) return 0;

    int parity_bit = (received_ecc >> 7) & 1;
    int data_parity = __builtin_parity(*data);

    if ((syndrome & 0x7F) != 0 && parity_bit == data_parity) {
        // 双比特错误检测
        return -2;
    }

    return ecc_decode(data, received_ecc & 0x7F);
}
