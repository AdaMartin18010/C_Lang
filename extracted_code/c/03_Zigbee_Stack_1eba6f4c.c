/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 222
 * Language: c
 * Block ID: 1eba6f4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 物理层接口函数

/**
 * 设置射频信道
 * @param channel: 信道号 (11-26)
 * @return: 状态码
 */
int8_t phy_set_channel(uint8_t channel);

/**
 * 发送数据
 * @param data: 数据缓冲区
 * @param length: 数据长度
 * @return: 发送结果
 */
int8_t phy_tx(uint8_t* data, uint8_t length);

/**
 * 接收数据（中断处理）
 */
void phy_rx_isr(void);

/**
 * 能量检测扫描
 * @param channel: 要扫描的信道
 * @return: 能量值 (0-255)
 */
uint8_t phy_ed_scan(uint8_t channel);

/**
 * 信道空闲评估 (CCA)
 * @return: CCA结果
 */
typedef enum {
    PHY_CCA_IDLE = 0,     // 信道空闲
    PHY_CCA_BUSY = 1,     // 信道忙
    PHY_CCA_TRX_OFF = 2,  // 收发器关闭
} phy_cca_status_t;

phy_cca_status_t phy_cca(void);
