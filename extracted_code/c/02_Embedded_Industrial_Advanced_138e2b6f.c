/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 1738
 * Language: c
 * Block ID: 138e2b6f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// EMC设计在软件层面的考虑
// - 看门狗防止程序跑飞产生干扰
// - GPIO状态初始化防止浮空
// - 通信协议错误处理
// ============================================================

// GPIO安全初始化 (防止浮空产生噪声)
void gpio_safe_init(void) {
    // 所有未使用GPIO设为输出低电平或输入上拉
    for (int pin = 0; pin < GPIO_MAX; pin++) {
        if (!is_pin_used(pin)) {
            // 设为输出低电平 (减少辐射)
            gpio_set_direction(pin, GPIO_MODE_OUTPUT);
            gpio_set_level(pin, 0);

            // 或设为输入上拉 (减少敏感)
            // gpio_set_pull_mode(pin, GPIO_PULLUP_ONLY);
        }
    }
}

// 通信错误计数与恢复
#define MAX_ERROR_COUNT 10

static int comm_error_count = 0;

void robust_communication(void) {
    if (send_data() != SUCCESS) {
        comm_error_count++;

        if (comm_error_count >= MAX_ERROR_COUNT) {
            // 错误过多，复位通信接口
            reinit_comm_interface();
            comm_error_count = 0;
        }
    } else {
        comm_error_count = 0;
    }
}
