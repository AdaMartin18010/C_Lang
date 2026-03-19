/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 729
 * Language: c
 * Block ID: 6bf7ae27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 在ISR中执行耗时操作
void timer_isr(void) {
    process_sensor_data();      // 50us
    update_control_algorithm(); // 200us - 太长了!
    communicate_with_other_ecu(); // 1ms - 绝对不行!
}

// ✅ ISR只处理最紧急的事，其余交给任务
void timer_isr(void) {
    // 仅读取硬件寄存器并记录时间戳
    capture_sensor_timestamp();
    // 通知任务处理
    set_event_flag(EV_SENSOR_READY);
}

void sensor_task(void) {
    while (1) {
        wait_event(EV_SENSOR_READY);
        process_sensor_data();
        update_control_algorithm();
    }
}
