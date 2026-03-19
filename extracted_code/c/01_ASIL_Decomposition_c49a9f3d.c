/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_26262\01_ASIL_Decomposition.md
 * Line: 74
 * Language: c
 * Block ID: c49a9f3d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 示例: 带监控的计算
 * ASIL C执行主要功能，ASIL A监控其正确性
 */

/* 主功能 - ASIL C */
int32_t calculate_steering_angle(int32_t sensor_raw)
{
    /* 复杂的信号处理和滤波 */
    int32_t filtered = apply_filter(sensor_raw);
    int32_t compensated = apply_compensation(filtered);
    return apply_limits(compensated);
}

/* 监控功能 - ASIL A */
bool check_steering_plausibility(int32_t input, int32_t output)
{
    /* 简化的合理性检查 */
    int32_t expected_range = abs(input) * MAX_GAIN;
    return (abs(output) <= expected_range);
}

/* 组合调用 */
int32_t safe_steering_calculation(int32_t sensor_raw)
{
    int32_t result = calculate_steering_angle(sensor_raw);

    if (!check_steering_plausibility(sensor_raw, result)) {
        /* 监控检测到异常 */
        log_error("Steering monitoring failed");
        return USE_DEFAULT_VALUE;
    }

    return result;
}
