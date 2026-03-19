/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\IEC_61508\01_SIL_Implementation.md
 * Line: 149
 * Language: c
 * Block ID: 1db96e04
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SIL 3诊断技术要求
 */

/* 1. 程序流监控 */
static uint32_t expected_signature = 0xA5B6C7D8;
static uint32_t actual_signature = 0;

void update_flow_signature(uint32_t block_id)
{
    actual_signature ^= block_id;
}

void check_flow_integrity(void)
{
    if (actual_signature != expected_signature) {
        /* 程序流错误 */
        enter_safe_state();
    }
    /* 重置为下一个周期 */
    actual_signature = 0;
}

/* 2. 数据完整性检查 */
typedef struct {
    int32_t value;
    uint32_t timestamp;
    uint16_t crc;
} ProtectedData;

uint16_t calculate_crc(const ProtectedData *data)
{
    const uint8_t *bytes = (const uint8_t *)data;
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < sizeof(*data) - sizeof(data->crc); i++) {
        crc ^= (uint16_t)bytes[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool verify_data(const ProtectedData *data)
{
    return (calculate_crc(data) == data->crc);
}

/* 3. 双通道比较 (1oo2架构) */
typedef struct {
    int32_t ch_a_value;
    int32_t ch_b_value;
    bool valid;
} DualChannel;

int32_t read_dual_channel(const DualChannel *dc, int32_t *result)
{
    const int32_t TOLERANCE = 10;

    if (abs(dc->ch_a_value - dc->ch_b_value) > TOLERANCE) {
        /* 通道不一致 */
        return ERROR_CHANNEL_MISMATCH;
    }

    /* 输出较保守值 */
    *result = (dc->ch_a_value < dc->ch_b_value) ?
              dc->ch_a_value : dc->ch_b_value;
    return SUCCESS;
}

/* 4. 看门狗监控 */
static volatile uint32_t watchdog_counter = 0;

void feed_watchdog(void)
{
    watchdog_counter++;
    HW_WATCHDOG_REFRESH();
}

void watchdog_task(void)
{
    uint32_t last_counter = 0;

    while (1) {
        sleep(WATCHDOG_CHECK_INTERVAL);

        if (watchdog_counter == last_counter) {
            /* 主程序未喂狗 */
            trigger_emergency_stop();
        }

        last_counter = watchdog_counter;
    }
}
