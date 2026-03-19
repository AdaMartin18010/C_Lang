/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 604
 * Language: c
 * Block ID: af312226
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// CAN FD: 灵活数据速率，支持64字节数据
// J1939: 重型车辆网络协议 (基于CAN 2.0B)
// ============================================================

typedef struct {
    uint32_t id;           // 标准11位或扩展29位
    uint8_t is_extended;   // 是否扩展帧
    uint8_t is_fd;         // 是否CAN FD
    uint8_t dlc;           // 数据长度码
    uint8_t data[64];      // CAN FD支持最多64字节
} CAN_FD_Frame;

// CAN ID结构 (J1939)
// 29位扩展ID格式:
// [优先级(3bit)][保留(1bit)][数据页(1bit)][PDU格式(8bit)][特定地址(8bit)][源地址(8bit)]
//
// PGN (Parameter Group Number): 用于标识消息类型
// PDU1 (PF < 240): 点对点通信，PS为目标地址
// PDU2 (PF >= 240): 广播，PS为组扩展

#define J1939_PRIORITY_MASK     0x1C000000
#define J1939_PDU_FORMAT_MASK   0x00FF0000
#define J1939_SPECIFIC_MASK     0x0000FF00
#define J1939_SOURCE_MASK       0x000000FF

#define J1939_MAKE_ID(priority, dp, pf, ps, sa) \
    (((priority & 0x7) << 26) | \
     ((dp & 0x1) << 25) | \
     ((pf & 0xFF) << 16) | \
     ((ps & 0xFF) << 8) | \
     (sa & 0xFF))

// J1939 PGN定义
#define PGN_REQUEST             0x00EA00  // 请求消息
#define PGN_ADDRESS_CLAIMED     0x00EE00  // 地址声明
#define PGN_TP_CM               0x00EC00  // 传输协议-连接管理
#define PGN_TP_DT               0x00EB00  // 传输协议-数据传输
#define PGN_ELECTRONIC_ENGINE_CONTROLLER 0x00F000  // 发动机控制

// 发送J1939消息
void j1939_send_message(uint8_t priority, uint32_t pgn, uint8_t *data, uint8_t len) {
    CAN_FD_Frame frame = {0};
    frame.is_extended = 1;
    frame.id = J1939_MAKE_ID(priority, 0, (pgn >> 8) & 0xFF, pgn & 0xFF, MY_SOURCE_ADDRESS);
    frame.dlc = (len <= 8) ? len : 8;
    memcpy(frame.data, data, frame.dlc);

    can_send(&frame);

    // 如果需要传输超过8字节，使用传输协议(TP)
    if (len > 8) {
        j1939_tp_send(pgn, data, len);
    }
}

// J1939传输协议 (多包传输)
void j1939_tp_send(uint32_t pgn, uint8_t *data, uint16_t len) {
    // 1. 发送BAM (广播通知消息) 或 RTS (请求发送)
    uint8_t cm_data[8];
    cm_data[0] = 0x20;  // BAM控制字节
    cm_data[1] = len & 0xFF;
    cm_data[2] = (len >> 8) & 0xFF;
    cm_data[3] = (len + 6) / 7;  // 包数
    cm_data[4] = 0xFF;  // 保留
    cm_data[5] = (pgn >> 16) & 0xFF;
    cm_data[6] = (pgn >> 8) & 0xFF;
    cm_data[7] = pgn & 0xFF;

    j1939_send_message(6, PGN_TP_CM, cm_data, 8);

    // 2. 发送数据包
    uint8_t dt_data[8];
    uint8_t seq = 1;
    for (int i = 0; i < len; i += 7) {
        dt_data[0] = seq++;
        int remaining = len - i;
        int chunk = (remaining > 7) ? 7 : remaining;
        memcpy(&dt_data[1], &data[i], chunk);
        if (chunk < 7) memset(&dt_data[1 + chunk], 0xFF, 7 - chunk);

        j1939_send_message(6, PGN_TP_DT, dt_data, 8);

        vTaskDelay(pdMS_TO_TICKS(10));  // 包间隔
    }
}

// J1939地址声明
void j1939_address_claim(uint8_t preferred_address) {
    uint8_t name[8] = {0};
    // 填充设备名称...

    j1939_send_message(6, PGN_ADDRESS_CLAIMED, name, 8);
}
