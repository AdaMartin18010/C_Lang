/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 560
 * Language: c
 * Block ID: 0bad5380
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pio_blink.c
 * @brief 使用 PIO 控制 LED 闪烁
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// PIO 程序 - LED 闪烁
// 这个程序从 FIFO 获取延时计数，然后翻转 LED
static const uint16_t pio_blink_program[] = {
    // .wrap_target
    0x8080,  // 0: pull   block          ; 从 TX FIFO 获取计数值
    0xa027,  // 1: mov    x, osr          ; 复制到 X 寄存器
    0xe022,  // 2: set    pins, 0          ; LED 熄灭
    // delay_off:
    0x0044,  // 3: jmp    x--, delay_off  ; X--, 如果不为零则跳转
    0x8080,  // 4: pull   block          ; 获取下一个计数值
    0xa027,  // 5: mov    x, osr          ; 复制到 X 寄存器
    0xe023,  // 6: set    pins, 1          ; LED 点亮
    // delay_on:
    0x0047,  // 7: jmp    x--, delay_on   ; X--, 如果不为零则跳转
             // .wrap
};

static const struct pio_program pio_blink_program_struct = {
    .instructions = pio_blink_program,
    .length = 8,
    .origin = -1,
};

int main(void) {
    stdio_init_all();

    PIO pio = pio0;
    uint sm = 0;
    uint offset;

    // 加载程序到 PIO
    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &pio_blink_program_struct,
        &pio,
        &sm,
        &offset,
        25,       // GPIO 25
        1,        // 1 个引脚
        true      // 必需
    );

    if (!success) {
        printf("PIO 程序加载失败\n");
        return -1;
    }

    // 配置状态机
    pio_sm_config c = pio_get_default_sm_config();

    // 设置输出引脚
    sm_config_set_set_pins(&c, 25, 1);

    // 设置时钟分频 (系统时钟 / 2)
    sm_config_set_clkdiv(&c, 2.0f);

    // 初始化引脚
    pio_gpio_init(pio, 25);
    pio_sm_set_consecutive_pindirs(pio, sm, 25, 1, true);

    // 加载配置并启动状态机
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    printf("PIO LED 闪烁启动\n");

    // 主循环 - 向 PIO 发送延时值
    while (1) {
        // 向 PIO 发送熄灭时间 (约 500ms)
        pio_sm_put_blocking(pio, sm, 10000000);

        // 向 PIO 发送点亮时间 (约 500ms)
        pio_sm_put_blocking(pio, sm, 10000000);
    }

    return 0;
}
