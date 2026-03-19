/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 3131
 * Language: c
 * Block ID: b9b3a01a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 07: EMC设计不足导致的系统失效
 *============================================================================*/

/* ❌ 错误做法：无EMC防护的代码 */
void bad_emc_protection(void) {
    /* 直接读取ADC值 */
    sensor_value = ADC_Read(CHANNEL_WHEEL_SPEED);
    /* 可能读取到噪声导致的虚假值 */
}

/* ✅ 正确做法：多层EMC防护 */
void good_emc_protection(void) {
    /* 1. 硬件层：硬件RC滤波 */

    /* 2. 软件层：数字滤波 */
    #define ADC_SAMPLES     8
    uint16_t adc_sum = 0;
    uint8_t i;

    for (i = 0; i < ADC_SAMPLES; i++) {
        adc_sum += ADC_Read(CHANNEL_WHEEL_SPEED);
    }
    sensor_value = adc_sum / ADC_SAMPLES;

    /* 3. 合理性检查 */
    if (sensor_value > MAX_VALID_VALUE || sensor_value < MIN_VALID_VALUE) {
        /* 使用上次有效值或默认值 */
        sensor_value = last_valid_value;
        emc_error_counter++;

        if (emc_error_counter > EMC_ERROR_THRESHOLD) {
            /* 报告EMC相关故障 */
            report_fault(FAULT_EMC_INTERFERENCE);
        }
    } else {
        last_valid_value = sensor_value;
        emc_error_counter = 0;
    }

    /* 4. CRC校验关键数据 */
    if (CRC_Check(data, received_crc) != OK) {
        /* 丢弃损坏的数据帧 */
        return;
    }
}
