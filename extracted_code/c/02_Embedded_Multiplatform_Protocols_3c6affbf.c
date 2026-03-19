/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 1768
 * Language: c
 * Block ID: 3c6affbf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 软件UART (Bit-Bang) 实现
// 适用于只有一个UART或需要特殊引脚的场景
// ============================================================

typedef struct {
    uint32_t tx_pin;
    uint32_t rx_pin;
    uint32_t baudrate;
    uint32_t bit_delay_us;
} SoftUART;

void softuart_init(SoftUART *uart, uint32_t baudrate) {
    uart->baudrate = baudrate;
    uart->bit_delay_us = 1000000 / baudrate;

    gpio_init(uart->tx_pin);
    gpio_init(uart->rx_pin);
    gpio_set_dir(uart->tx_pin, GPIO_OUT);
    gpio_set_dir(uart->rx_pin, GPIO_IN);
    gpio_pull_up(uart->rx_pin);

    gpio_put(uart->tx_pin, 1);  // 空闲高电平
}

// 精确延时
static inline void delay_us(uint32_t us) {
    uint32_t start = time_us_32();
    while (time_us_32() - start < us);
}

// 发送一个字节
void softuart_putc(SoftUART *uart, char c) {
    // 起始位
    gpio_put(uart->tx_pin, 0);
    delay_us(uart->bit_delay_us);

    // 数据位 (LSB first)
    for (int i = 0; i < 8; i++) {
        gpio_put(uart->tx_pin, (c >> i) & 1);
        delay_us(uart->bit_delay_us);
    }

    // 停止位
    gpio_put(uart->tx_pin, 1);
    delay_us(uart->bit_delay_us);
}

// 接收一个字节 (轮询方式)
bool softuart_getc(SoftUART *uart, char *c, uint32_t timeout_us) {
    uint32_t start = time_us_32();

    // 等待起始位
    while (gpio_get(uart->rx_pin)) {
        if (time_us_32() - start > timeout_us) return false;
    }

    // 等待半个bit时间，采样到bit中间
    delay_us(uart->bit_delay_us / 2);

    // 读取8位数据
    char data = 0;
    for (int i = 0; i < 8; i++) {
        delay_us(uart->bit_delay_us);
        data |= (gpio_get(uart->rx_pin) << i);
    }

    // 等待停止位
    delay_us(uart->bit_delay_us);

    *c = data;
    return true;
}

// 发送字符串
void softuart_puts(SoftUART *uart, const char *str) {
    while (*str) {
        softuart_putc(uart, *str++);
    }
}
