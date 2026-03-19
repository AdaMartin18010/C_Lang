/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RPi_Pico\01_Pico_Complete_Guide.md
 * Line: 650
 * Language: c
 * Block ID: 2b05d1ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file pio_ws2812.c
 * @brief 使用 PIO 驱动 WS2812B RGB LED 灯带
 */

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// WS2812B 时序要求 (800KHz):
// T0H: 0.4us ±150ns, T0L: 0.85us ±150ns (bit 0)
// T1H: 0.8us ±150ns, T1L: 0.45us ±150ns (bit 1)
// RESET: >50us

// PIO 程序 - WS2812B 驱动
// 使用 side-set 控制输出引脚
static const uint16_t ws2812_program[] = {
    // .wrap_target
    0x6221,  // 0: out    x, 1        side 0 [2]  ; 移出 1 位到 X
    0x1123,  // 1: jmp    !x, 3       side 1 [1]  ; 如果为 0, 跳转到 bit 0
    0x1400,  // 2: jmp    0            side 1 [4]  ; bit 1, 跳回
    0xa242,  // 3: nop                side 0 [4]  ; bit 0
             // .wrap
};

static const struct pio_program ws2812_program_struct = {
    .instructions = ws2812_program,
    .length = 4,
    .origin = -1,
};

#define WS2812_PIN      16
#define NUM_PIXELS      30

// 颜色数据结构
typedef struct {
    uint8_t g;  // WS2812B 使用 GRB 顺序
    uint8_t r;
    uint8_t b;
} ws2812_color_t;

// 像素缓冲区
ws2812_color_t pixel_buffer[NUM_PIXELS];

void ws2812_init(PIO pio, uint sm) {
    uint offset;

    bool success = pio_claim_free_sm_and_add_program_for_gpio_range(
        &ws2812_program_struct, &pio, &sm, &offset,
        WS2812_PIN, 1, true
    );

    if (!success) {
        printf("WS2812 PIO 程序加载失败\n");
        return;
    }

    pio_sm_config c = pio_get_default_sm_config();

    // 设置 side-set 引脚
    sm_config_set_sideset_pins(&c, WS2812_PIN);
    sm_config_set_sideset(&c, 1, false, false);

    // 计算时钟分频以实现 800KHz
    // 每个 bit 需要 1.25us, 8 个指令周期
    // sysclk / (800000 * 8) = 分频值
    float div = (float)clock_get_hz(clk_sys) / (800000.0f * 8.0f);
    sm_config_set_clkdiv(&c, div);

    // 配置移位寄存器 (左移，先发送高位)
    sm_config_set_out_shift(&c, false, true, 24);

    // 配置 FIFO (8 位数据)
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    // 初始化引脚
    pio_gpio_init(pio, WS2812_PIN);
    pio_sm_set_consecutive_pindirs(pio, sm, WS2812_PIN, 1, true);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}

void ws2812_set_pixel(uint index, uint8_t r, uint8_t g, uint8_t b) {
    if (index < NUM_PIXELS) {
        pixel_buffer[index].r = r;
        pixel_buffer[index].g = g;
        pixel_buffer[index].b = b;
    }
}

void ws2812_show(PIO pio, uint sm) {
    // 发送所有像素数据
    for (int i = 0; i < NUM_PIXELS; i++) {
        uint32_t grb = ((uint32_t)pixel_buffer[i].g << 16) |
                       ((uint32_t)pixel_buffer[i].r << 8) |
                       (uint32_t)pixel_buffer[i].b;
        pio_sm_put_blocking(pio, sm, grb);
    }

    // RESET 延时 (>50us)
    sleep_us(60);
}

// HSV 转 RGB 辅助函数
void hsv_to_rgb(uint8_t h, uint8_t s, uint8_t v, uint8_t* r, uint8_t* g, uint8_t* b) {
    uint8_t region, remainder, p, q, t;

    if (s == 0) {
        *r = v; *g = v; *b = v;
        return;
    }

    region = h / 43;
    remainder = (h - (region * 43)) * 6;

    p = (v * (255 - s)) >> 8;
    q = (v * (255 - ((s * remainder) >> 8))) >> 8;
    t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

    switch (region) {
        case 0: *r = v; *g = t; *b = p; break;
        case 1: *r = q; *g = v; *b = p; break;
        case 2: *r = p; *g = v; *b = t; break;
        case 3: *r = p; *g = q; *b = v; break;
        case 4: *r = t; *g = p; *b = v; break;
        default: *r = v; *g = p; *b = q; break;
    }
}

int main(void) {
    stdio_init_all();

    PIO pio = pio0;
    uint sm = 0;

    ws2812_init(pio, sm);

    printf("WS2812B 灯带驱动启动\n");

    uint8_t hue = 0;

    while (1) {
        // 彩虹效果
        for (int i = 0; i < NUM_PIXELS; i++) {
            uint8_t r, g, b;
            uint8_t pixel_hue = (hue + (i * 255 / NUM_PIXELS)) % 256;
            hsv_to_rgb(pixel_hue, 255, 128, &r, &g, &b);
            ws2812_set_pixel(i, r, g, b);
        }

        ws2812_show(pio, sm);

        hue++;
        sleep_ms(20);
    }

    return 0;
}
