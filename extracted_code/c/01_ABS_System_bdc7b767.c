/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\01_ABS_System.md
 * Line: 2863
 * Language: c
 * Block ID: bdc7b767
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*============================================================================
 * TRAP 02: 控制阀频繁切换导致的抖动
 *============================================================================*/

/* ❌ 错误做法：无滞环的直接控制 */
void bad_pressure_control(float slip) {
    if (slip > 0.15f) {
        valve_state = DECREASE;  /* 边界附近频繁切换 */
    } else {
        valve_state = INCREASE;
    }
}

/* ✅ 正确做法：带死区的滞环控制 */
typedef enum {
    STATE_HOLD = 0,
    STATE_DECREASE,
    STATE_INCREASE
} ValveState;

void good_pressure_control(float slip) {
    static ValveState state = STATE_HOLD;

    /* 滞环控制 - 状态切换需要更大偏差 */
    switch (state) {
        case STATE_HOLD:
            if (slip > SLIP_HIGH_THRESHOLD) {
                state = STATE_DECREASE;
            } else if (slip < SLIP_LOW_THRESHOLD) {
                state = STATE_INCREASE;
            }
            break;

        case STATE_DECREASE:
            /* 退出减压需要滑移率显著降低 */
            if (slip < SLIP_OPTIMAL - 0.05f) {
                state = STATE_HOLD;
            }
            break;

        case STATE_INCREASE:
            /* 退出增压需要滑移率显著升高 */
            if (slip > SLIP_OPTIMAL + 0.05f) {
                state = STATE_HOLD;
            }
            break;
    }

    /* 最小时间限制 */
    if (state != last_state) {
        if (state_timer < MIN_STATE_TIME) {
            state = last_state;  /* 保持原状态 */
        } else {
            state_timer = 0;
        }
    }
}
