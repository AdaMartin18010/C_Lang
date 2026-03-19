/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 777
 * Language: c
 * Block ID: 89c914ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SRAM PUF实现
 * 利用SRAM上电初始值的随机性
 */

#include <stdint.h>
#include <string.h>

/* SRAM PUF参数 */
#define SRAM_PUF_SIZE           4096    /* PUF区域大小 */
#define SRAM_PUF_START_ADDR     0x20000000  /* SRAM起始地址 */
#define SRAM_PUF_HELPER_SIZE    256     /* Helper data大小 */

/* PUF状态 */
typedef enum {
    PUF_STATE_UNINITIALIZED = 0,
    PUF_STATE_ENROLLING,
    PUF_STATE_ENROLLED,
    PUF_STATE_ERROR,
} puf_state_t;

/* SRAM PUF上下文 */
typedef struct {
    volatile uint8_t *sram_base;
    size_t sram_size;
    puf_state_t state;
    uint8_t helper_data[SRAM_PUF_HELPER_SIZE];
    size_t helper_data_len;
} sram_puf_t;

static sram_puf_t g_sram_puf;

/* 初始化SRAM PUF */
int sram_puf_init(void *sram_base, size_t sram_size) {
    g_sram_puf.sram_base = (volatile uint8_t *)sram_base;
    g_sram_puf.sram_size = sram_size;
    g_sram_puf.state = PUF_STATE_UNINITIALIZED;

    /* 检查是否已注册 */
    if (is_puf_enrolled()) {
        g_sram_puf.state = PUF_STATE_ENROLLED;
        load_helper_data(g_sram_puf.helper_data, &g_sram_puf.helper_data_len);
    }

    return 0;
}

/* 读取SRAM PUF响应 */
int sram_puf_read_response(uint8_t *response, size_t response_len) {
    /* 1. 确保SRAM处于未初始化状态 */
    /* 复位SRAM控制器或执行全零写入 */
    memset((void *)g_sram_puf.sram_base, 0, g_sram_puf.sram_size);

    /* 2. 等待SRAM稳定 */
    delay_us(100);

    /* 3. 读取SRAM值 */
    /* 这些值现在应该是随机的（由制造差异决定）*/
    uint8_t sram_data[SRAM_PUF_SIZE];
    for (size_t i = 0; i < SRAM_PUF_SIZE; i++) {
        sram_data[i] = g_sram_puf.sram_base[i];
    }

    /* 4. 提取稳定位 */
    /* 只使用那些在不同启动中保持稳定的位 */
    extract_stable_bits(response, response_len, sram_data, SRAM_PUF_SIZE);

    return 0;
}

/* 注册SRAM PUF（一次性）*/
int sram_puf_enroll(void) {
    if (g_sram_puf.state == PUF_STATE_ENROLLED) {
        return -EALREADY;  /* 已注册 */
    }

    g_sram_puf.state = PUF_STATE_ENROLLING;

    /* 1. 多次读取以识别稳定位 */
    uint8_t responses[10][128];

    for (int i = 0; i < 10; i++) {
        /* 每次读取前需要重新初始化SRAM */
        system_reset_sram_controller();
        delay_ms(10);

        sram_puf_read_response(responses[i], 128);
    }

    /* 2. 识别稳定位 */
    uint8_t stable_mask[128] = {0};
    uint8_t reference[128] = {0};

    for (int byte = 0; byte < 128; byte++) {
        for (int bit = 0; bit < 8; bit++) {
            int count_1 = 0;

            for (int r = 0; r < 10; r++) {
                if (responses[r][byte] & (1 << bit)) {
                    count_1++;
                }
            }

            /* 如果该位在大多数读取中相同，则为稳定位 */
            if (count_1 >= 8 || count_1 <= 2) {
                stable_mask[byte] |= (1 << bit);
                reference[byte] |= ((count_1 >= 5) ? 1 : 0) << bit;
            }
        }
    }

    /* 3. 生成Helper Data */
    /* Helper data = 码偏移（Code Offset）Fuzzy Extractor */
    generate_helper_data(g_sram_puf.helper_data, &g_sram_puf.helper_data_len,
                         reference, stable_mask, 128);

    /* 4. 安全存储Helper Data */
    save_helper_data(g_sram_puf.helper_data, g_sram_puf.helper_data_len);

    g_sram_puf.state = PUF_STATE_ENROLLED;
    mark_puf_enrolled();

    return 0;
}

/* 从PUF重构密钥 */
int sram_puf_reconstruct_key(uint8_t *key, size_t key_len) {
    if (g_sram_puf.state != PUF_STATE_ENROLLED) {
        return -ENOTENROLLED;
    }

    /* 1. 读取当前PUF响应 */
    uint8_t response[128];
    sram_puf_read_response(response, 128);

    /* 2. 使用Helper Data重构 */
    uint8_t reconstructed[128];
    if (reconstruct_with_helper(reconstructed, response,
                                g_sram_puf.helper_data,
                                g_sram_puf.helper_data_len) != 0) {
        return -ERECONSTRUCT_FAIL;
    }

    /* 3. 派生密钥 */
    /* 使用HKDF从重构的响应派生密钥 */
    hkdf_sha256(reconstructed, 128,
                NULL, 0,
                (const uint8_t *)"sram_puf_key", 12,
                key, key_len);

    /* 4. 清除敏感数据 */
    secure_memzero(response, sizeof(response));
    secure_memzero(reconstructed, sizeof(reconstructed));

    return 0;
}

/* PUF质量评估 */
typedef struct {
    float uniqueness;       /* 唯一性 (0-1, 理想0.5) */
    float reliability;      /* 可靠性 (0-1, 理想>0.95) */
    float randomness;       /* 随机性 (0-1, 理想0.5) */
    float bias;             /* 偏置 (0-1, 理想0.5) */
} puf_quality_metrics_t;

int sram_puf_quality_assessment(puf_quality_metrics_t *metrics) {
    /* 1. 多次读取 */
    uint8_t responses[100][128];

    for (int i = 0; i < 100; i++) {
        system_reset_sram_controller();
        delay_ms(10);
        sram_puf_read_response(responses[i], 128);
    }

    /* 2. 计算可靠性（同一芯片内）*/
    float reliability_sum = 0;
    for (int byte = 0; byte < 128; byte++) {
        for (int bit = 0; bit < 8; bit++) {
            int count_1 = 0;
            for (int r = 0; r < 100; r++) {
                if (responses[r][byte] & (1 << bit)) count_1++;
            }
            float p1 = count_1 / 100.0;
            reliability_sum += (p1 > 0.5 ? p1 : 1 - p1);
        }
    }
    metrics->reliability = reliability_sum / (128 * 8);

    /* 3. 计算随机性 */
    int total_1 = 0;
    for (int r = 0; r < 100; r++) {
        for (int i = 0; i < 128; i++) {
            total_1 += __builtin_popcount(responses[r][i]);
        }
    }
    metrics->randomness = total_1 / (100.0 * 128 * 8);

    /* 4. 偏置 */
    metrics->bias = metrics->randomness;

    return 0;
}
