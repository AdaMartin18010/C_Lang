/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 3039
 * Language: c
 * Block ID: 5a650916
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 05: 转向制动工况的不协调
 *============================================================================*/

/* ❌ 错误做法：不考虑转向角的标准ABS */
void bad_cornering_brake(void) {
    /* 弯道制动时使用标准ABS阈值 */
    /* 外侧轮滑移率计算可能失真 */
    /* 可能导致外侧轮抱死，失去转向能力 */
}

/* ✅ 正确做法：弯道识别 + 非对称控制 */
void good_cornering_brake(void) {
    float steering_angle = get_steering_angle();
    float yaw_rate = get_yaw_rate();
    float lateral_accel = get_lateral_accel();

    /* 1. 弯道识别 */
    bool is_cornering = (fabs(steering_angle) > 30.0f) ||
                        (fabs(lateral_accel) > 0.3f * 9.81f);

    if (is_cornering) {
        /* 2. 计算各轮垂直载荷转移 */
        float load_transfer_front = lateral_accel * CG_HEIGHT / TRACK_WIDTH;

        /* 3. 外侧轮更高的附着能力 */
        float mu_outer = base_mu * (1.0f + load_transfer_front);
        float mu_inner = base_mu * (1.0f - load_transfer_front);

        /* 4. 非对称ABS阈值 */
        if (is_outer_wheel) {
            slip_target = 0.18f;        /* 更高的目标滑移率 */
            decel_threshold = -9.0f;     /* 更迟触发 */
        } else {
            slip_target = 0.10f;        /* 更低的目标滑移率 */
            decel_threshold = -6.0f;     /* 更早触发 */
        }

        /* 5. 优先保持外侧轮抓地力（提供侧向力） */
        if (outer_slip > CRITICAL_SLIP_CORNERING) {
            /* 紧急保护外侧轮 */
            priority_control(OUTER_WHEEL);
        }
    }
}
