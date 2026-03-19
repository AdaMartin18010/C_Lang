/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 1371
 * Language: c
 * Block ID: 856e49b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 看门狗: 检测程序跑飞，自动复位
// 必须在设定时间内"喂狗"
// ============================================================

// 独立看门狗 (IWDG): 使用独立RC振荡器，即使主时钟失效也能工作
void iwdg_init(uint32_t timeout_ms) {
    // 使能寄存器访问
    IWDG->KR = 0x5555;

    // 配置预分频 (40kHz / 分频)
    uint32_t prescaler;
    if (timeout_ms < 100) {
        prescaler = IWDG_PR_PR_0;  // /4
    } else if (timeout_ms < 1000) {
        prescaler = IWDG_PR_PR_2;  // /16
    } else {
        prescaler = IWDG_PR_PR_3;  // /32
    }
    IWDG->PR = prescaler;

    // 配置重载值
    // 超时时间 = (重载值 + 1) * 分频 / 40000
    uint32_t reload = timeout_ms * 40 / (4 << prescaler);
    if (reload > 0xFFF) reload = 0xFFF;
    IWDG->RLR = reload;

    // 启动看门狗
    IWDG->KR = 0xCCCC;
}

// 喂狗
void iwdg_feed(void) {
    IWDG->KR = 0xAAAA;  // 写入重载值到计数器
}

// 窗口看门狗 (WWDG): 必须在窗口内喂狗，不能太早也不能太晚
void wwdg_init(uint8_t window, uint8_t counter) {
    // 使能WWDG时钟
    RCC->APB1ENR |= RCC_APB1ENR_WWDGEN;

    // 配置窗口值
    WWDG->CFR = window;

    // 启动并设置计数器
    WWDG->CR = WWDG_CR_WDGA | counter;
}

// 喂窗口看门狗
void wwdg_feed(uint8_t counter) {
    // 检查是否在窗口内 (由硬件自动检查)
    WWDG->CR = counter;
}

// 安全的喂狗模式: 只在主循环喂，不在ISR中喂
void main_loop_safe(void) {
    while (1) {
        // 执行所有任务
        process_all_tasks();

        // 只有正常执行完才喂狗
        iwdg_feed();
    }
}
