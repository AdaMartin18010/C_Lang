/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\08_Hardware_Root_of_Trust.md
 * Line: 1932
 * Language: c
 * Block ID: 5e266cd9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 防回滚保护实现
 */

/* 安全版本存储 */
typedef struct {
    uint32_t firmware_version;
    uint32_t security_version;
    uint32_t rollback_index;
    uint8_t  signature[64];
} rollback_protection_data_t;

/* 单调计数器实现（基于eFuse）*/
typedef struct {
    uint32_t current_value;
    uint32_t num_efuse_blocks;
    uint32_t efuse_base_block;
} monotonic_counter_t;

static monotonic_counter_t g_counter;

/* 初始化单调计数器 */
int monotonic_counter_init(uint32_t base_block, uint32_t num_blocks) {
    g_counter.efuse_base_block = base_block;
    g_counter.num_efuse_blocks = num_blocks;

    /* 读取当前值 */
    g_counter.current_value = read_monotonic_counter_value();

    return 0;
}

/* 读取单调计数器值 */
uint32_t read_monotonic_counter_value(void) {
    uint32_t count = 0;

    /* 计算已烧录的eFuse位 */
    for (uint32_t i = 0; i < g_counter.num_efuse_blocks; i++) {
        uint32_t block_value;
        efuse_read(g_counter.efuse_base_block + i,
                   (uint8_t *)&block_value, sizeof(block_value));

        /* 计算设置的位数 */
        count += __builtin_popcount(block_value);
    }

    return count;
}

/* 增加单调计数器 */
int increment_monotonic_counter(void) {
    /* 找到下一个未烧录的位 */
    for (uint32_t i = 0; i < g_counter.num_efuse_blocks; i++) {
        uint32_t block_value;
        efuse_read(g_counter.efuse_base_block + i,
                   (uint8_t *)&block_value, sizeof(block_value));

        /* 找到第一个0位 */
        for (int bit = 0; bit < 32; bit++) {
            if (!(block_value & (1 << bit))) {
                /* 烧录这个位 */
                uint32_t new_value = block_value | (1 << bit);
                efuse_write(g_counter.efuse_base_block + i,
                           (const uint8_t *)&new_value, sizeof(new_value));

                g_counter.current_value++;
                return 0;
            }
        }
    }

    return -ECOUNTER_EXHAUSTED;
}

/* 检查回滚 */
int check_rollback_protection(uint32_t new_version) {
    rollback_protection_data_t current_data;

    /* 读取当前版本 */
    if (read_rollback_data(&current_data) != 0) {
        /* 首次启动 */
        return 0;
    }

    /* 验证签名 */
    if (verify_rollback_signature(&current_data) != 0) {
        return -ETAMPERED;
    }

    /* 检查版本 */
    if (new_version < current_data.firmware_version) {
        /* 回滚尝试 */
        log_security_event(EVENT_ROLLBACK_ATTEMPT, new_version);
        return -EROLLBACK;
    }

    return 0;
}

/* 更新回滚保护数据 */
int update_rollback_protection(uint32_t new_version) {
    rollback_protection_data_t new_data = {
        .firmware_version = new_version,
        .security_version = get_current_security_version(),
        .rollback_index = g_counter.current_value,
    };

    /* 签名 */
    sign_rollback_data(&new_data);

    /* 增加计数器 */
    increment_monotonic_counter();

    /* 存储 */
    return write_rollback_data(&new_data);
}
