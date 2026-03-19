/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_26262\01_ASIL_Decomposition.md
 * Line: 114
 * Language: c
 * Block ID: b11002f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 示例: 多样化算法实现
 * 两种不同算法计算同一结果，互相验证
 */

/* 算法A - 查表法 - ASIL B */
int32_t calc_speed_lookup(uint32_t pulse_count, uint32_t time_ms)
{
    uint32_t rpm = rpm_table[pulse_count];
    return (rpm * WHEEL_CIRCUMFERENCE) / (time_ms / 1000);
}

/* 算法B - 直接计算 - ASIL A */
int32_t calc_speed_direct(uint32_t pulse_count, uint32_t time_ms)
{
    float pulses_per_sec = (float)pulse_count / (time_ms / 1000.0f);
    float rev_per_sec = pulses_per_sec / PULSES_PER_REV;
    return (int32_t)(rev_per_sec * WHEEL_CIRCUMFERENCE);
}

/* 多样化比较 */
int32_t get_vehicle_speed(uint32_t pulses, uint32_t time)
{
    int32_t speed_a = calc_speed_lookup(pulses, time);
    int32_t speed_b = calc_speed_direct(pulses, time);

    int32_t diff = abs(speed_a - speed_b);
    if (diff > SPEED_TOLERANCE) {
        /* 算法不一致 */
        return DIAGNOSTIC_MODE;
    }

    return (speed_a + speed_b) / 2;
}
