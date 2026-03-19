/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\README.md
 * Line: 123
 * Language: c
 * Block ID: f794ff69
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// TMR 处理器实现框架
typedef struct {
    uint32_t core_a_result;
    uint32_t core_b_result;
    uint32_t core_c_result;
    uint32_t voted_result;
    int error_count;
} TMR_Unit;

// TMR 表决逻辑
uint32_t tmr_execute(TMR_Unit *tmr,
                      uint32_t (*operation)(uint32_t, uint32_t),
                      uint32_t op1, uint32_t op2) {
    // 三个核心执行相同操作
    tmr->core_a_result = operation(op1, op2);
    tmr->core_b_result = operation(op1, op2);
    tmr->core_c_result = operation(op1, op2);

    // 多数表决
    if (tmr->core_a_result == tmr->core_b_result) {
        tmr->voted_result = tmr->core_a_result;
        if (tmr->core_c_result != tmr->voted_result) {
            tmr->error_count++;
            trigger_core_refresh(CORE_C);
        }
    } else if (tmr->core_a_result == tmr->core_c_result) {
        tmr->voted_result = tmr->core_a_result;
        tmr->error_count++;
        trigger_core_refresh(CORE_B);
    } else if (tmr->core_b_result == tmr->core_c_result) {
        tmr->voted_result = tmr->core_b_result;
        tmr->error_count++;
        trigger_core_refresh(CORE_A);
    } else {
        // 三重不一致 - 严重错误
        tmr->voted_result = 0;
        trigger_system_reset();
    }

    return tmr->voted_result;
}
