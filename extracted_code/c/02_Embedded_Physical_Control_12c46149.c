/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Physical_Control.md
 * Line: 820
 * Language: c
 * Block ID: 12c46149
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// Modbus RTU: 工业控制常用协议
// 帧格式: [地址][功能码][数据][CRC16]
// ============================================================

#define MODBUS_SLAVE_ADDR   0x01
#define MODBUS_FUNC_READ_HOLDING  0x03
#define MODBUS_FUNC_WRITE_SINGLE  0x06

// CRC16计算 (Modbus标准多项式: 0xA001)
uint16_t modbus_crc16(const uint8_t *data, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// 构造读保持寄存器请求
int modbus_build_read_holding(uint8_t *frame, uint16_t addr, uint16_t count) {
    frame[0] = MODBUS_SLAVE_ADDR;
    frame[1] = MODBUS_FUNC_READ_HOLDING;
    frame[2] = (addr >> 8) & 0xFF;      // 起始地址高字节
    frame[3] = addr & 0xFF;              // 起始地址低字节
    frame[4] = (count >> 8) & 0xFF;      // 寄存器数量高字节
    frame[5] = count & 0xFF;             // 寄存器数量低字节

    uint16_t crc = modbus_crc16(frame, 6);
    frame[6] = crc & 0xFF;               // CRC低字节先传
    frame[7] = (crc >> 8) & 0xFF;

    return 8;  // 帧长度
}

// 解析响应
int modbus_parse_response(const uint8_t *frame, uint16_t len,
                          uint16_t *out_data, uint16_t max_count) {
    if (len < 5) return -1;  // 太短

    // CRC检查
    uint16_t rx_crc = (frame[len-1] << 8) | frame[len-2];
    if (modbus_crc16(frame, len-2) != rx_crc) return -2;  // CRC错误

    // 地址检查
    if (frame[0] != MODBUS_SLAVE_ADDR) return -3;

    // 异常检查
    if (frame[1] & 0x80) {
        return -frame[2];  // 返回异常码
    }

    // 提取数据
    uint8_t byte_count = frame[2];
    if (byte_count > max_count * 2) return -4;

    for (int i = 0; i < byte_count / 2; i++) {
        out_data[i] = (frame[3 + i*2] << 8) | frame[4 + i*2];
    }

    return byte_count / 2;  // 返回寄存器数量
}
