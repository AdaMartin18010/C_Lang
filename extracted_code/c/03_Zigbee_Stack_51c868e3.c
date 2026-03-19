/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 185
 * Language: c
 * Block ID: 51c868e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 物理层协议数据单元 (PPDU)
typedef struct {
    // 同步头 (SHR)
    uint8_t preamble[4];      // 前导码，用于同步
    uint8_t sfd;              // 帧起始定界符 (0xA7)

    // 物理层头 (PHR)
    uint8_t frame_length;     // PSDU长度 (1-127字节)

    // 物理层服务数据单元 (PSDU) - 即MPDU
    uint8_t psdu[127];
} phy_ppdu_t;

// 物理层状态
typedef enum {
    PHY_IDLE,
    PHY_RX,
    PHY_TX,
    PHY_ED,           // 能量检测
    PHY_CCA,          // 信道空闲评估
} phy_state_t;

// 物理层服务原语
typedef enum {
    PHY_DATA_REQUEST,
    PHY_DATA_CONFIRM,
    PHY_DATA_INDICATION,
    PHY_ED_REQUEST,
    PHY_ED_CONFIRM,
    PHY_CCA_REQUEST,
    PHY_CCA_CONFIRM,
} phy_primitive_t;
