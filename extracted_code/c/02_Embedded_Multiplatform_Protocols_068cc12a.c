/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 2269
 * Language: c
 * Block ID: 068cc12a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// ESP32低功耗模式管理
// 用于电池供电的传感器节点
// ============================================================

#include "esp_sleep.h"
#include "esp32/ulp.h"
#include "driver/rtc_io.h"

// RTC内存中的数据 (睡眠保持)
RTC_DATA_ATTR static uint32_t boot_count = 0;
RTC_DATA_ATTR static uint32_t total_uptime = 0;

// 深度睡眠唤醒源配置
void setup_wakeup_sources(void) {
    // 1. 定时器唤醒 (每10分钟)
    esp_sleep_enable_timer_wakeup(10 * 60 * 1000000ULL);

    // 2. GPIO唤醒 (按键)
    esp_sleep_enable_ext0_wakeup(GPIO_NUM_0, 0);  // GPIO0低电平唤醒

    // 3. GPIO唤醒 (传感器中断)
    esp_sleep_enable_ext1_wakeup(
        (1ULL << GPIO_NUM_34),  // GPIO34
        ESP_EXT1_WAKEUP_ANY_HIGH  // 高电平唤醒
    );
}

// 进入深度睡眠
void enter_deep_sleep(void) {
    // 保存当前时间
    total_uptime += esp_timer_get_time() / 1000000;
    boot_count++;

    // 配置RTC外设保持 (GPIO状态)
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);

    // 进入深度睡眠
    esp_deep_sleep_start();
}

// 轻度睡眠 (Modem Sleep)
void enter_modem_sleep(void) {
    // 关闭WiFi射频，保持连接
    esp_wifi_set_ps(WIFI_PS_MAX_MODEM);

    // 或者完全关闭WiFi
    // esp_wifi_stop();
}

// ULP协处理器程序 (ULP在深度睡眠时继续运行)
// ULP可以监控GPIO/ADC，在需要时唤醒主CPU
const ulp_insn_t ulp_program[] = {
    I_MOVI(R3, 0),                    // 循环计数器

    // 读取ADC
    I_WR_REG_BIT(RTC_IO_TOUCH_PAD2_REG, RTC_IO_TOUCH_PAD2_MUX_SEL_S, 1),
    I_RD_REG(RTC_IO_TOUCH_PAD2_REG, RTC_IO_TOUCH_PAD2_FUN_IE_S, RTC_IO_TOUCH_PAD2_FUN_IE_S),

    // 与阈值比较
    // ...

    // 唤醒主CPU (如果需要)
    I_WAKE(),

    // 休眠一段时间
    I_HALT()
};
