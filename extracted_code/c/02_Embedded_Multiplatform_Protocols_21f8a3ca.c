/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 616
 * Language: c
 * Block ID: 21f8a3ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// PIO (Programmable I/O) 是RP2040最独特的功能
// 可以自定义协议时序，不占用CPU时间
// 4个状态机并行执行，每个最多32条指令
// ============================================================

// ========== 示例1: PIO实现WS2812B LED控制 ==========

// PIO汇编程序
// .pio文件会被pioasm编译成C头文件
/*
.program ws2812
.side_set 1

.define public T1 2
.define public T2 5
.define public T3 3

.lang_opt python sideset_init = pico.PIO.OUT_HIGH
.lang_opt python out_init = pico.PIO.OUT_HIGH
.lang_opt python out_shiftdir = 1

.entry_point
bitloop:
    out x, 1       side 0 [T3 - 1] ; 输出1位到X，同时拉低side pin，延时T3-1
    jmp !x do_zero side 1 [T1 - 1] ; 如果X=0跳转，同时拉高，延时T1-1
    jmp bitloop    side 1 [T2 - 1] ; X=1时继续，保持高电平，延时T2-1
do_zero:
    nop            side 0 [T2 - 1] ; X=0时拉低，延时T2-1

% c-sdk {
static inline void ws2812_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);

    pio_sm_config c = ws2812_program_get_default_config(offset);
    sm_config_set_sideset_pins(&c, pin);
    sm_config_set_out_shift(&c, false, true, 24);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    int cycles_per_bit = T1 + T2 + T3;
    float div = clock_get_hz(clk_sys) / (800000.0f * cycles_per_bit);
    sm_config_set_clkdiv(&c, div);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
%}
*/

// 使用PIO控制WS2812
#include "ws2812.pio.h"  // 由pioasm生成的头文件

#define WS2812_PIN 16
#define NUM_PIXELS 30

void ws2812_init(void) {
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN);
}

void ws2812_put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

// ========== 示例2: PIO实现自定义SPI从机 ==========

/*
.program spi_slave
.side_set 1

; CPHA=0, CPOL=0 SPI从机
; SCK输入，MOSI输入，MISO输出，CS输入

    wait 0 pin 2    ; 等待CS低电平
    wait 0 pin 1    ; 等待SCK下降沿 (CPOL=0)

.wrap_target
    wait 1 pin 1    ; 等待SCK上升沿
    in pins, 1      ; 读取MOSI
    out pin, 1      ; 输出MISO
.wrap

% c-sdk {
static inline void spi_slave_program_init(PIO pio, uint sm, uint offset,
                                          uint mosi_pin, uint miso_pin,
                                          uint sck_pin, uint cs_pin) {
    pio_sm_config c = spi_slave_program_get_default_config(offset);

    sm_config_set_in_pins(&c, mosi_pin);
    sm_config_set_out_pins(&c, miso_pin, 1);
    sm_config_set_sideset_pins(&c, 0);
    sm_config_set_in_shift(&c, false, false, 8);
    sm_config_set_out_shift(&c, false, false, 8);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
}
%}
*/
