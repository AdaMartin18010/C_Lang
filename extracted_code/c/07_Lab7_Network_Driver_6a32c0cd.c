/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 92
 * Language: c
 * Block ID: 6a32c0cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.h

// E1000寄存器偏移 (BAR0 + offset)
#define E1000_CTL      0x00000   // 设备控制
#define E1000_STATUS   0x00008   // 设备状态
#define E1000_EERD     0x00014   // EEPROM读
#define E1000_ICR      0x000C0   // 中断原因读
#define E1000_IMS      0x000D0   // 中断掩码设置
#define E1000_RCTL     0x00100   // 接收控制
#define E1000_TCTL     0x00400   // 发送控制

// 接收描述符环
#define E1000_RDBAL    0x02800   // 接收描述符基地址 (低32位)
#define E1000_RDBAH    0x02804   // 接收描述符基地址 (高32位)
#define E1000_RDLEN    0x02808   // 接收描述符环长度
#define E1000_RDH      0x02810   // 接收描述符头
#define E1000_RDT      0x02818   // 接收描述符尾

// 发送描述符环
#define E1000_TDBAL    0x03800   // 发送描述符基地址 (低32位)
#define E1000_TDBAH    0x03804   // 发送描述符基地址 (高32位)
#define E1000_TDLEN    0x03808   // 发送描述符环长度
#define E1000_TDH      0x03810   // 发送描述符头
#define E1000_TDT      0x03818   // 发送描述符尾

// MAC地址
#define E1000_RA       0x05400   // 接收地址 (MAC)
#define E1000_MTA      0x05200   // 多播表数组

// 控制寄存器位
#define E1000_CTL_RST    0x00400000   // 复位
#define E1000_CTL_SLU    0x00000040   // Set Link Up
#define E1000_CTL_ASDE   0x00000020   // Auto-Speed Detection

// 接收控制位
#define E1000_RCTL_EN    0x00000002   // 接收使能
#define E1000_RCTL_BAM   0x00008000   // 广播使能
#define E1000_RCTL_SZ_2048 0x00000000 // 2048字节缓冲区
#define E1000_RCTL_SECRC 0x04000000   // 剥离CRC

// 发送控制位
#define E1000_TCTL_EN    0x00000002   // 发送使能
#define E1000_TCTL_PSP   0x00000008   // Pad Short Packets
