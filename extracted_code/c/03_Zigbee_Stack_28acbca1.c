/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 1045
 * Language: c
 * Block ID: 28acbca1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CC2530寄存器定义

// 射频寄存器
#define FRMCTRL0    0x6180   // 帧控制0
#define FRMCTRL1    0x6181   // 帧控制1
#define RXENABLE    0x6182   // 接收使能
#define RXMASKSET   0x6183   // 接收掩码设置
#define FREQCTRL    0x6189   // 频率控制 (信道: 11 + (FREQCTRL - 11))
#define FSMSTAT1    0x6191   // FSM状态1
#define FIFOPCTRL   0x6194   // FIFOP阈值控制
#define RFIRQM0     0x61A3   // RF中断掩码0
#define RFIRQM1     0x61A4   // RF中断掩码1
#define RFERRM      0x61A5   // RF错误中断掩码
#define RFRND       0x61A7   // RF随机数据
#define MDMCTRL0    0x61A8   // 调制解调控制0
#define MDMCTRL1    0x61A9   // 调制解调控制1
#define RSSI        0x61AB   // RSSI状态
#define RXCTRL      0x61AC   // 接收控制
#define FSCTRL      0x61B0   // 频率合成控制
#define FSCAL1      0x61B2   // 频率合成校准1
#define AGCCTRL0    0x61B4   // AGC控制0
#define ADCTEST0    0x61C5   // ADC测试0
#define ADCTEST1    0x61C6   // ADC测试1
#define ADCTEST2    0x61C7   // ADC测试2

// 配置信道 (2.4GHz, 信道11-26)
void cc2530_set_channel(uint8_t channel) {
    if (channel < 11 || channel > 26) return;
    FREQCTRL = (channel - 11) + 11;
}

// 配置发射功率
typedef enum {
    POWER_MINUS_24_DBM = 0x03,
    POWER_MINUS_18_DBM = 0x2C,
    POWER_MINUS_12_DBM = 0x88,
    POWER_MINUS_6_DBM  = 0x81,
    POWER_0_DBM        = 0x32,
    POWER_4_5_DBM      = 0xF5,
} tx_power_t;

void cc2530_set_tx_power(tx_power_t power) {
    TXPOWER = power;
}

// 配置接收滤波器
void cc2530_set_address_filter(uint16_t pan_id, uint16_t short_addr,
                                uint64_t ext_addr) {
    // 设置PAN ID
    PAN_ID0 = pan_id & 0xFF;
    PAN_ID1 = (pan_id >> 8) & 0xFF;

    // 设置短地址
    SHORT_ADDR0 = short_addr & 0xFF;
    SHORT_ADDR1 = (short_addr >> 8) & 0xFF;

    // 设置扩展地址
    for (int i = 0; i < 8; i++) {
        EXT_ADDR[i] = (ext_addr >> (i * 8)) & 0xFF;
    }

    // 使能地址识别
    FRMCTRL0 |= 0x20;  // SET_RXENMASK_ON_RX
}
