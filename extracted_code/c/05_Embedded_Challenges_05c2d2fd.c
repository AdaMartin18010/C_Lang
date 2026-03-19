/*
 * Auto-generated from: 12_Practice_Exercises\05_Embedded_Challenges.md
 * Line: 192
 * Language: c
 * Block ID: 05c2d2fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 环形缓冲区
typedef struct {
    uint8_t buffer[256];
    volatile uint8_t head;
    volatile uint8_t tail;
} RingBuffer;

RingBuffer rx_buffer = {0};

void uart2_init_interrupt(void) {
    uart2_init(115200);

    // 使能接收中断
    USART_CR1 |= (1 << 5);  // RXNEIE

    // NVIC配置
    NVIC_ISER1 |= (1 << 6);  // USART2_IRQn = 38
}

// 中断服务程序
void USART2_IRQHandler(void) {
    if (USART_SR & USART_SR_RXNE) {
        uint8_t data = USART_DR;
        uint8_t next_head = (rx_buffer.head + 1) % sizeof(rx_buffer.buffer);
        if (next_head != rx_buffer.tail) {  // 缓冲区未满
            rx_buffer.buffer[rx_buffer.head] = data;
            rx_buffer.head = next_head;
        }
    }
}

// 非阻塞读取
int uart2_read_nonblock(uint8_t *data) {
    if (rx_buffer.head == rx_buffer.tail) {
        return 0;  // 无数据
    }
    *data = rx_buffer.buffer[rx_buffer.tail];
    rx_buffer.tail = (rx_buffer.tail + 1) % sizeof(rx_buffer.buffer);
    return 1;
}
