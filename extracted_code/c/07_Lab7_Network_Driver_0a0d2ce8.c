/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 140
 * Language: c
 * Block ID: 0a0d2ce8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.h

// 发送描述符 (Legacy模式)
struct tx_desc {
    uint64 addr;       // 数据包缓冲区物理地址
    uint16 length;     // 数据包长度
    uint8  cso;        // 校验和偏移
    uint8  cmd;        // 命令
    uint8  status;     // 状态
    uint8  css;        // 校验和开始
    uint16 special;
};

// TX命令位
#define TX_CMD_EOP    0x01   // End Of Packet
#define TX_CMD_IFCS   0x02   // Insert FCS (CRC)
#define TX_CMD_RS     0x08   // Report Status

// 接收描述符 (Legacy模式)
struct rx_desc {
    uint64 addr;       // 缓冲区物理地址
    uint16 length;     // 接收长度
    uint16 checksum;   // 校验和
    uint8  status;     // 状态
    uint8  errors;     // 错误
    uint16 special;
};

// RX状态位
#define RX_STATUS_DD  0x01   // Descriptor Done
#define RX_STATUS_EOP 0x02   // End Of Packet

#define NTXDESC 32   // 发送描述符数量
#define NRXDESC 128  // 接收描述符数量
