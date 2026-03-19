/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\IEC_61508\01_SIL_Implementation.md
 * Line: 341
 * Language: c
 * Block ID: bb7e072c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * SIL级别输入数据处理
 */

typedef struct {
    int32_t raw_value;
    int32_t filtered_value;
    int32_t min_limit;
    int32_t max_limit;
    uint32_t max_rate_of_change;
    uint32_t timestamp;
    bool valid;
} ProcessedInput;

ProcessedInput process_input(int32_t raw, const ProcessedInput *prev)
{
    ProcessedInput result = {0};
    result.raw_value = raw;
    result.timestamp = get_time_ms();

    /* 1. 范围检查 */
    if (raw < prev->min_limit || raw > prev->max_limit) {
        result.valid = false;
        log_error("Input out of range: %d", raw);
        return result;
    }

    /* 2. 变化率检查 */
    if (prev->valid) {
        int32_t change = abs(raw - prev->raw_value);
        uint32_t time_diff = result.timestamp - prev->timestamp;

        if (time_diff > 0) {
            uint32_t rate = (uint32_t)(change * 1000 / time_diff);
            if (rate > prev->max_rate_of_change) {
                result.valid = false;
                log_error("Rate of change exceeded: %u", rate);
                return result;
            }
        }
    }

    /* 3. 滤波处理 */
    if (prev->valid) {
        /* 简单一阶滤波 */
        result.filtered_value = (prev->filtered_value * 7 + raw) / 8;
    } else {
        result.filtered_value = raw;
    }

    /* 4. 合理性检查 */
    if (prev->valid) {
        int32_t expected_max = prev->filtered_value +
                               (prev->max_rate_of_change * time_diff / 1000);
        int32_t expected_min = prev->filtered_value -
                               (prev->max_rate_of_change * time_diff / 1000);

        if (result.filtered_value < expected_min ||
            result.filtered_value > expected_max) {
            result.valid = false;
            log_error("Plausibility check failed");
            return result;
        }
    }

    result.valid = true;
    return result;
}
