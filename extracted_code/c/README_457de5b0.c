/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 250
 * Language: c
 * Block ID: 457de5b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 控制流检查
#define ASSERT_SIGNATURE(addr, sig) \
    do { \
        if (compute_signature(addr) != sig) { \
            handle_control_flow_error(); \
        } \
    } while(0)

// 数据冗余存储
typedef struct {
    uint32_t data;
    uint32_t inverted;      // 按位取反
    uint32_t checksum;      // 校验和
    uint32_t timestamp;
} HardenedData;

void hardened_write(HardenedData *h, uint32_t value) {
    h->data = value;
    h->inverted = ~value;
    h->checksum = crc32(&value, sizeof(value));
    h->timestamp = get_tick_count();
}

int hardened_read(HardenedData *h, uint32_t *out) {
    // 三重检查
    if (h->data != ~(h->inverted)) {
        return -1;  // 数据损坏
    }

    uint32_t computed_checksum = crc32(&h->data, sizeof(h->data));
    if (computed_checksum != h->checksum) {
        return -1;
    }

    // 检查时效性
    if (get_tick_count() - h->timestamp > TIMEOUT_THRESHOLD) {
        return -2;  // 数据过期
    }

    *out = h->data;
    return 0;
}
