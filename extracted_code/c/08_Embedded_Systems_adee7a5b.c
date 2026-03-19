/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\08_Embedded_Systems.md
 * Line: 606
 * Language: c
 * Block ID: adee7a5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// uart_driver.c

#define UART_TX_BUF_SIZE 256
#define UART_RX_BUF_SIZE 256

static volatile uint8_t tx_buffer[UART_TX_BUF_SIZE];
static volatile uint8_t rx_buffer[UART_RX_BUF_SIZE];
static volatile uint16_t tx_head = 0, tx_tail = 0;
static volatile uint16_t rx_head = 0, rx_tail = 0;

void UART_Init(USART_TypeDef *USARTx, uint32_t baudrate) {
    // 使能时钟
    if (USARTx == USART1) RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    else if (USARTx == USART2) RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // 配置波特率
    uint32_t apb_clock = (USARTx == USART1) ? 84000000 : 42000000;
    uint32_t usartdiv = (apb_clock + baudrate/2) / baudrate;
    USARTx->BRR = usartdiv;

    // 使能发送、接收、接收中断
    USARTx->CR1 = USART_CR1_TE | USART_CR1_RE | USART_CR1_RXNEIE;

    // 使能USART
    USARTx->CR1 |= USART_CR1_UE;

    // 使能中断
    if (USARTx == USART1) NVIC_EnableIRQ(USART1_IRQn);
    else if (USARTx == USART2) NVIC_EnableIRQ(USART2_IRQn);
}

void UART_SendChar(USART_TypeDef *USARTx, uint8_t c) {
    uint16_t next_head = (tx_head + 1) % UART_TX_BUF_SIZE;

    // 等待缓冲区空间
    while (next_head == tx_tail);

    tx_buffer[tx_head] = c;
    tx_head = next_head;

    // 使能发送中断
    USARTx->CR1 |= USART_CR1_TXEIE;
}

void UART_SendString(USART_TypeDef *USARTx, const char *str) {
    while (*str) {
        UART_SendChar(USARTx, *str++);
    }
}

// 中断服务程序
void USART2_IRQHandler(void) {
    // 接收中断
    if (USART2->SR & USART_SR_RXNE) {
        uint8_t c = USART2->DR;
        uint16_t next_head = (rx_head + 1) % UART_RX_BUF_SIZE;
        if (next_head != rx_tail) {
            rx_buffer[rx_head] = c;
            rx_head = next_head;
        }
    }

    // 发送中断
    if (USART2->SR & USART_SR_TXE) {
        if (tx_head != tx_tail) {
            USART2->DR = tx_buffer[tx_tail];
            tx_tail = (tx_tail + 1) % UART_TX_BUF_SIZE;
        } else {
            // 缓冲区空，禁用发送中断
            USART2->CR1 &= ~USART_CR1_TXEIE;
        }
    }
}
