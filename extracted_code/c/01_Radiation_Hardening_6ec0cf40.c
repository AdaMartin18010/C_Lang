/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_Radiation_Hardening.md
 * Line: 295
 * Language: c
 * Block ID: 6ec0cf40
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CRC32计算（硬件加速可用时使用）
uint32_t crc32_table[256];

void crc32_init(void) {
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
        crc32_table[i] = crc;
    }
}

uint32_t crc32(const void *data, size_t len, uint32_t init) {
    const uint8_t *bytes = data;
    uint32_t crc = ~init;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ bytes[i]) & 0xFF];
    }

    return ~crc;
}

// 受保护数据结构
typedef struct {
    uint32_t magic;         // 0xDEADBEEF
    uint32_t version;
    uint32_t seq_number;    // 序列号检测丢失更新
    uint32_t data[16];
    uint32_t crc;           // 覆盖所有前面字段
} ProtectedData;

#define PROTECTED_MAGIC 0xDEADBEEF

bool protected_write(ProtectedData *dest, const ProtectedData *src) {
    // 验证源数据
    if (src->magic != PROTECTED_MAGIC) return false;

    uint32_t computed_crc = crc32(src, sizeof(ProtectedData) - 4, 0);
    if (computed_crc != src->crc) return false;

    // 复制并更新序列号
    ProtectedData tmp = *src;
    tmp.seq_number++;
    tmp.crc = crc32(&tmp, sizeof(ProtectedData) - 4, 0);

    // 原子复制（使用EDAC内存）
    memcpy(dest, &tmp, sizeof(ProtectedData));

    return true;
}

bool protected_read(const ProtectedData *src, ProtectedData *dest) {
    // 三重读取投票
    ProtectedData copies[3];
    memcpy(&copies[0], src, sizeof(ProtectedData));
    memcpy(&copies[1], src, sizeof(ProtectedData));
    memcpy(&copies[2], src, sizeof(ProtectedData));

    // 验证所有副本
    int valid_count = 0;
    ProtectedData *valid = NULL;

    for (int i = 0; i < 3; i++) {
        if (copies[i].magic != PROTECTED_MAGIC) continue;

        uint32_t crc = crc32(&copies[i], sizeof(ProtectedData) - 4, 0);
        if (crc == copies[i].crc) {
            valid = &copies[i];
            valid_count++;
        }
    }

    if (valid_count >= 2) {
        *dest = *valid;
        return true;
    }

    return false;  // 无法确定有效数据
}
