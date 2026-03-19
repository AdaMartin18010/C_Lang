/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 444
 * Language: c
 * Block ID: 27fb6e90
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 状态寄存器处理: 检查-清除模式
// ============================================================

// 轮询方式 (简单但阻塞)
void uart_send_poll(uint8_t data) {
    // 等待发送寄存器空 (TXE = Transmit Empty)
    while (!(UART->SR & UART_SR_TXE)) {
        // 忙等待，CPU空转
    }
    UART->DR = data;  // 写入数据
}

// 中断方式 (非阻塞，推荐)
// 发送缓冲区
#define TX_BUF_SIZE 256
static uint8_t tx_buffer[TX_BUF_SIZE];
static volatile uint16_t tx_head = 0;
static volatile uint16_t tx_tail = 0;

// 启动发送
int uart_send_async(uint8_t data) {
    uint16_t next_head = (tx_head + 1) % TX_BUF_SIZE;
    if (next_head == tx_tail) {
        return -1;  // 缓冲区满
    }

    tx_buffer[tx_head] = data;
    tx_head = next_head;

    // 使能发送中断 (如果当前未使能)
    UART->CR1 |= UART_CR1_TXEIE;
    return 0;
}

// 中断服务程序
void UART_IRQHandler(void) {
    uint32_t sr = UART->SR;  // 必须先读SR

    // 检查并处理发送空中断
    if (sr & UART_SR_TXE) {
        if (tx_head != tx_tail) {
            UART->DR = tx_buffer[tx_tail];  // 发送下一个字节
            tx_tail = (tx_tail + 1) % TX_BUF_SIZE;
        } else {
            // 缓冲区空，关闭发送中断
            UART->CR1 &= ~UART_CR1_TXEIE;
        }
    }

    // 检查并处理接收中断
    if (sr & UART_SR_RXNE) {
        uint8_t data = UART->DR;  // 读DR自动清除RXNE
        // 处理接收数据...
    }

    // 错误处理
    if (sr & (UART_SR_ORE | UART_SR_NE | UART_SR_FE)) {
        // 清除错误标志: 先读SR，再读DR
        (void)UART->DR;
    }
}
