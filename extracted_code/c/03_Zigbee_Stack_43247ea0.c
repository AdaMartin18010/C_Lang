/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 1114
 * Language: c
 * Block ID: 43247ea0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 射频状态
#define RX_ACTIVE   0x01
#define TX_ACTIVE   0x02
#define IDLE        0x00

// 初始化射频
void cc2530_radio_init(void) {
    // 软件复位射频
    SFR_RFST = RFST_SRXON;
    delay_us(200);

    // 配置调制解调器
    MDMCTRL0 = 0x0A;   // 自动确认，协调器模式
    MDMCTRL1 = 0x14;   // 相关器阈值

    // 配置频率合成器
    FSCTRL = 0x5A;
    FSCAL1 = 0x00;

    // 配置AGC
    AGCCTRL0 = 0x91;

    // 配置接收控制
    RXCTRL = 0x3F;

    // 设置FIFO阈值
    FIFOPCTRL = 64;    // FIFOP阈值64字节
}

// 发送数据包
void cc2530_tx(uint8_t* data, uint8_t len) {
    // 等待射频空闲
    while (FSMSTAT1 & (RX_ACTIVE | TX_ACTIVE));

    // 写入TX FIFO
    RFD = len;  // 长度字段
    for (int i = 0; i < len; i++) {
        RFD = data[i];
    }

    // 触发发送
    SFR_RFST = RFST_STXON;

    // 等待发送完成
    while (!(RFIRQF1 & IRQ_TXDONE));
    RFIRQF1 &= ~IRQ_TXDONE;  // 清除中断标志
}

// 接收中断处理
#pragma vector = RF_VECTOR
__interrupt void rf_isr(void) {
    if (RFIRQF0 & IRQ_FIFOP) {
        // 接收数据可用
        uint8_t len = RFD;  // 读取长度
        uint8_t rx_buf[128];

        for (int i = 0; i < len; i++) {
            rx_buf[i] = RFD;
        }

        // 处理接收数据
        process_rx_frame(rx_buf, len);

        RFIRQF0 &= ~IRQ_FIFOP;  // 清除中断
    }

    S1CON &= ~0x03;  // 清除CPU中断标志
}
