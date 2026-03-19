/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 134
 * Language: c
 * Block ID: ee1fc993
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// USART2 (PA2=TX, PA3=RX)
#define USART2_BASE     0x40004400
#define USART_SR        (*(volatile uint32_t *)(USART2_BASE + 0x00))
#define USART_DR        (*(volatile uint32_t *)(USART2_BASE + 0x04))
#define USART_BRR       (*(volatile uint32_t *)(USART2_BASE + 0x08))
#define USART_CR1       (*(volatile uint32_t *)(USART2_BASE + 0x0C))

// 状态寄存器位
#define USART_SR_TXE    (1 << 7)   // 发送寄存器空
#define USART_SR_RXNE   (1 << 5)   // 接收寄存器非空

void uart2_init(uint32_t baudrate) {
    // 1. 使能GPIOA和USART2时钟
    RCC_AHB1ENR |= (1 << 0);
    RCC_APB1ENR |= (1 << 17);

    // 2. 配置PA2(TX)为复用功能
    GPIOA_MODER &= ~(3 << 4);
    GPIOA_MODER |= (2 << 4);     // 复用模式
    GPIOA_AFRL &= ~(0xF << 8);
    GPIOA_AFRL |= (7 << 8);      // AF7 = USART2

    // 3. 配置波特率 (84MHz APB1)
    uint32_t div = 84000000 / baudrate;
    USART_BRR = div;

    // 4. 使能发送和接收
    USART_CR1 |= (1 << 3) | (1 << 2) | (1 << 13);  // TX, RX, USART使能
}

void uart2_putchar(char c) {
    while (!(USART_SR & USART_SR_TXE));  // 等待发送就绪
    USART_DR = c;
}

char uart2_getchar(void) {
    while (!(USART_SR & USART_SR_RXNE));  // 等待接收数据
    return USART_DR;
}

void uart2_puts(const char *str) {
    while (*str) {
        uart2_putchar(*str++);
    }
}

// 重定向printf
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        uart2_putchar(ptr[i]);
    }
    return len;
}
