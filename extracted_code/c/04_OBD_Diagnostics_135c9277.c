/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\04_OBD_Diagnostics.md
 * Line: 202
 * Language: c
 * Block ID: 135c9277
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DTC格式: [类型][系统][子系统][故障]
// P0XXX - 动力系统
// B0XXX - 车身系统
// C0XXX - 底盘系统
// U0XXX - 网络通信

// 示例: P0171
// P = 动力系统 (Powertrain)
// 0 = 标准故障码 (SAE)
// 1 = 燃油和空气计量
// 71 = 具体故障 (系统过稀)

// DTC编码结构
typedef struct {
    char prefix;      // P/B/C/U
    uint8_t system;   // 0-3
    uint8_t subtype;  // 0-F
    uint8_t fault;    // 00-FF
} dtc_t;

// 解析DTC原始数据
void parse_dtc(uint16_t raw_dtc, dtc_t* dtc) {
    char prefixes[] = {'P', 'C', 'B', 'U'};

    dtc->prefix = prefixes[(raw_dtc >> 14) & 0x03];
    dtc->system = (raw_dtc >> 12) & 0x03;
    dtc->subtype = (raw_dtc >> 8) & 0x0F;
    dtc->fault = raw_dtc & 0xFF;
}

// 格式化DTC为字符串
void format_dtc(const dtc_t* dtc, char* buffer, size_t size) {
    snprintf(buffer, size, "%c%d%X%02X",
             dtc->prefix,
             dtc->system,
             dtc->subtype,
             dtc->fault);
}
