/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 2810
 * Language: c
 * Block ID: 73c7ecbf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 01: 传感器电磁干扰导致的虚假触发
 *============================================================================*/

/* ❌ 错误做法：直接使用原始信号 */
void bad_wheel_speed_handler(void) {
    if (timer_capture - last_capture > threshold) {
        wheel_speed = calculate_speed(timer_capture);
        if (wheel_speed < last_speed - 50.0f) {
            abs_trigger = true;  /* 错误触发！ */
        }
    }
}

/* ✅ 正确做法：多阶段滤波 + 合理性检查 */
void good_wheel_speed_handler(void) {
    static float speed_history[4];
    static uint8_t noise_counter = 0;
    float new_speed;

    /* 1. 硬件滤波：RC滤波器 + 施密特触发 */
    /* 2. 软件滤波 */
    new_speed = low_pass_filter(raw_speed, last_speed, 0.3f);

    /* 3. 中值滤波去除毛刺 */
    new_speed = median_filter_3(speed_history[0],
                                 speed_history[1],
                                 new_speed);

    /* 4. 物理约束检查 */
    if (fabs(new_speed - last_speed) > MAX_PHYSICAL_CHANGE) {
        noise_counter++;
        if (noise_counter > NOISE_THRESHOLD) {
            sensor_status = FAULT_NOISE;
        }
        new_speed = last_speed;  /* 丢弃异常值 */
    } else {
        noise_counter = 0;
    }

    /* 5. 多周期确认 */
    if (new_speed < last_speed - DECEL_THRESHOLD) {
        decel_counter++;
        if (decel_counter >= CONFIRM_CYCLES) {
            abs_trigger = true;  /* 确认触发 */
        }
    }
}
