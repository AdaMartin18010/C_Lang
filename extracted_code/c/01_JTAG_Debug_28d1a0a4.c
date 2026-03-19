/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\19_Hardware_Debugging\01_JTAG_Debug.md
 * Line: 647
 * Language: c
 * Block ID: 28d1a0a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// TAP 状态机控制实现

typedef enum {
    TEST_LOGIC_RESET = 0x0F,
    RUN_TEST_IDLE    = 0x00,
    SELECT_DR_SCAN   = 0x01,
    CAPTURE_DR       = 0x02,
    SHIFT_DR         = 0x03,
    EXIT1_DR         = 0x04,
    PAUSE_DR         = 0x05,
    EXIT2_DR         = 0x06,
    UPDATE_DR        = 0x07,
    SELECT_IR_SCAN   = 0x09,
    CAPTURE_IR       = 0x0A,
    SHIFT_IR         = 0x0B,
    EXIT1_IR         = 0x0C,
    PAUSE_IR         = 0x0D,
    EXIT2_IR         = 0x0E,
    UPDATE_IR        = 0x08
} tap_state_t;

// 状态转换表
static const uint8_t state_transition[16][2] = {
    // [当前状态][TMS=0/TMS=1] = 下一状态
    /* RUN_TEST_IDLE    [00] */ {RUN_TEST_IDLE,    SELECT_DR_SCAN},
    /* SELECT_DR_SCAN   [01] */ {CAPTURE_DR,       SELECT_IR_SCAN},
    /* CAPTURE_DR       [02] */ {SHIFT_DR,         EXIT1_DR},
    /* SHIFT_DR         [03] */ {SHIFT_DR,         EXIT1_DR},
    /* EXIT1_DR         [04] */ {PAUSE_DR,         UPDATE_DR},
    /* PAUSE_DR         [05] */ {PAUSE_DR,         EXIT2_DR},
    /* EXIT2_DR         [06] */ {SHIFT_DR,         UPDATE_DR},
    /* UPDATE_DR        [07] */ {RUN_TEST_IDLE,    SELECT_DR_SCAN},
    /* UPDATE_IR        [08] */ {RUN_TEST_IDLE,    SELECT_DR_SCAN},
    /* SELECT_IR_SCAN   [09] */ {CAPTURE_IR,       TEST_LOGIC_RESET},
    /* CAPTURE_IR       [0A] */ {SHIFT_IR,         EXIT1_IR},
    /* SHIFT_IR         [0B] */ {SHIFT_IR,         EXIT1_IR},
    /* EXIT1_IR         [0C] */ {PAUSE_IR,         UPDATE_IR},
    /* PAUSE_IR         [0D] */ {PAUSE_IR,         EXIT2_IR},
    /* EXIT2_IR         [0E] */ {SHIFT_IR,         UPDATE_IR},
    /* TEST_LOGIC_RESET [0F] */ {RUN_TEST_IDLE,    TEST_LOGIC_RESET}
};

// TAP 时钟周期
void tap_cycle(int tms, int *tdo) {
    set_tms(tms);
    set_tdi(*tdo);

    delay_ns(TCK_HALF_PERIOD);  // 半周期延迟
    set_tck(1);                 // TCK 上升沿
    delay_ns(TCK_HALF_PERIOD);

    *tdo = get_tdo();           // 采样 TDO

    set_tck(0);                 // TCK 下降沿
}

// 状态转移函数
void tap_goto_state(tap_state_t target_state) {
    static tap_state_t current_state = TEST_LOGIC_RESET;

    while (current_state != target_state) {
        // 计算需要的 TMS 值
        int tms = (state_transition[current_state][0] == target_state) ? 0 : 1;

        // 如果两个路径都不到达目标，选择更接近的路径
        if (state_transition[current_state][0] != target_state &&
            state_transition[current_state][1] != target_state) {
            // 使用启发式选择路径
            tms = calculate_best_path(current_state, target_state);
        }

        int dummy_tdo;
        tap_cycle(tms, &dummy_tdo);
        current_state = state_transition[current_state][tms];
    }
}
