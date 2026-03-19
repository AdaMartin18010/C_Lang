/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 985
 * Language: c
 * Block ID: 7967a77c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 环形振荡器 (RO) PUF实现
 * 利用振荡器频率的制造差异
 */

/* RO PUF参数 */
#define RO_PUF_NUM_PAIRS        64      /* 振荡器对数 */
#define RO_PUF_NUM_STAGES       5       /* 每个环形振荡器的级数 */
#define RO_PUF_COUNTER_BITS     16      /* 计数器位数 */

/* RO PUF配置 */
typedef struct {
    volatile uint32_t *ctrl_reg;        /* 控制寄存器 */
    volatile uint32_t *counter_regs;    /* 计数器寄存器数组 */
    uint32_t num_pairs;
    uint32_t measurement_cycles;        /* 测量周期数 */
} ro_puf_config_t;

static ro_puf_config_t g_ro_puf;

/* 初始化RO PUF */
int ro_puf_init(void *ctrl_reg, void *counter_regs, uint32_t num_pairs) {
    g_ro_puf.ctrl_reg = (volatile uint32_t *)ctrl_reg;
    g_ro_puf.counter_regs = (volatile uint32_t *)counter_regs;
    g_ro_puf.num_pairs = num_pairs;
    g_ro_puf.measurement_cycles = 65536;  /* 默认测量周期 */

    /* 配置RO PUF硬件 */
    *g_ro_puf.ctrl_reg = 0;  /* 复位 */

    return 0;
}

/* 测量单个振荡器对的频率 */
static uint32_t measure_ro_pair_frequency(int pair_idx) {
    /* 1. 选择振荡器对 */
    *g_ro_puf.ctrl_reg = (pair_idx << 8) | 0x01;  /* 选择并启用 */

    /* 2. 等待稳定 */
    delay_us(10);

    /* 3. 启动计数 */
    *g_ro_puf.ctrl_reg |= 0x02;  /* 启动计数 */

    /* 4. 等待测量周期 */
    delay_cycles(g_ro_puf.measurement_cycles);

    /* 5. 停止计数 */
    *g_ro_puf.ctrl_reg &= ~0x02;

    /* 6. 读取计数器 */
    uint32_t count = g_ro_puf.counter_regs[pair_idx];

    /* 7. 禁用振荡器 */
    *g_ro_puf.ctrl_reg &= ~0x01;

    return count;
}

/* 读取RO PUF响应 */
int ro_puf_read_response(uint8_t *response, size_t response_len) {
    uint32_t num_bytes_needed = (g_ro_puf.num_pairs + 7) / 8;
    if (response_len < num_bytes_needed) {
        return -ENOBUFS;
    }

    memset(response, 0, response_len);

    /* 测量所有振荡器对 */
    for (uint32_t i = 0; i < g_ro_puf.num_pairs; i += 2) {
        /* 测量一对 */
        uint32_t freq0 = measure_ro_pair_frequency(i);
        uint32_t freq1 = measure_ro_pair_frequency(i + 1);

        /* 比较并生成响应位 */
        uint8_t bit = (freq0 > freq1) ? 1 : 0;

        response[i / 16] |= (bit << ((i / 2) % 8));
    }

    return 0;
}

/* RO PUF注册 */
int ro_puf_enroll(uint8_t *helper_data, size_t *helper_len) {
    /* 1. 多次测量以确定稳定响应 */
    uint8_t responses[10][(RO_PUF_NUM_PAIRS + 7) / 8];

    for (int i = 0; i < 10; i++) {
        ro_puf_read_response(responses[i], sizeof(responses[0]));
    }

    /* 2. 确定最稳定的响应 */
    uint8_t stable_response[(RO_PUF_NUM_PAIRS + 7) / 8] = {0};
    uint8_t confidence[(RO_PUF_NUM_PAIRS + 7) / 8] = {0};

    for (int byte = 0; byte < (RO_PUF_NUM_PAIRS + 7) / 8; byte++) {
        for (int bit = 0; bit < 8; bit++) {
            int count_1 = 0;
            for (int r = 0; r < 10; r++) {
                if (responses[r][byte] & (1 << bit)) count_1++;
            }

            /* 如果一致性好，则该位稳定 */
            if (count_1 >= 8 || count_1 <= 2) {
                confidence[byte] |= (1 << bit);
                stable_response[byte] |= ((count_1 >= 5) ? 1 : 0) << bit;
            }
        }
    }

    /* 3. 生成Helper Data */
    /* Helper data包含：
     * - 稳定位掩码
     * - BCH纠错码
     */
    *helper_len = 0;
    memcpy(helper_data + *helper_len, confidence, sizeof(confidence));
    *helper_len += sizeof(confidence);

    /* 添加BCH纠错码 */
    uint8_t bch_code[32];
    bch_encode(stable_response, sizeof(stable_response), bch_code);
    memcpy(helper_data + *helper_len, bch_code, sizeof(bch_code));
    *helper_len += sizeof(bch_code);

    return 0;
}
