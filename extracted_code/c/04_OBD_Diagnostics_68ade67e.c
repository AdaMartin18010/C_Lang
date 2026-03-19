/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 115
 * Language: c
 * Block ID: 68ade67e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// OBD-II CAN数据帧结构
struct obd_can_frame {
    uint32_t id;           // 11-bit标准ID或29-bit扩展ID
    uint8_t  data[8];      // 数据场 (最大8字节)
    uint8_t  dlc;          // 数据长度码
};

// 标准OBD-II CAN ID
#define OBD_CAN_REQUEST_ID  0x7DF   // 功能请求ID
#define OBD_CAN_RESPONSE_ID 0x7E8   // ECU响应ID起始 (0x7E8-0x7EF)
