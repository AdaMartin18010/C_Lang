/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 382
 * Language: c
 * Block ID: 527f8897
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RS485 + Modbus RTU 完整实现
// RS485: 差分信号、半双工、1200m距离、32节点
// Modbus: 主从协议、功能码、CRC校验
// ============================================================

#include "driver/uart.h"
#include "driver/gpio.h"

// RS485硬件控制
#define RS485_UART_NUM      UART_NUM_2
#define RS485_TX_PIN        17
#define RS485_RX_PIN        16
#define RS485_DE_RE_PIN     4   // 收发控制引脚

// DE/RE控制宏
#define RS485_TX_MODE()     gpio_set_level(RS485_DE_RE_PIN, 1)  // 发送模式
#define RS485_RX_MODE()     gpio_set_level(RS485_DE_RE_PIN, 0)  // 接收模式

// Modbus帧结构
typedef struct {
    uint8_t slave_addr;
    uint8_t function_code;
    uint8_t data[252];
    uint8_t data_len;
} ModbusFrame;

// 初始化RS485
void rs485_init(void) {
    // 配置DE/RE引脚
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << RS485_DE_RE_PIN),
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io_conf);
    RS485_RX_MODE();  // 默认接收模式

    // 配置UART
    uart_config_t uart_conf = {
        .baud_rate = 9600,  // Modbus标准波特率
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,  // 通常无校验
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    uart_param_config(RS485_UART_NUM, &uart_conf);
    uart_set_pin(RS485_UART_NUM, RS485_TX_PIN, RS485_RX_PIN,
                  UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(RS485_UART_NUM, 256, 256, 0, NULL, 0);

    // 启用RS485模式 (硬件自动控制收发切换，或使用软件控制)
    uart_set_mode(RS485_UART_NUM, UART_MODE_RS485_HALF_DUPLEX);
}

// Modbus CRC16计算
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

// Modbus主站: 读取保持寄存器 (功能码0x03)
bool modbus_master_read_holding(uint8_t slave_addr, uint16_t reg_addr,
                                 uint16_t reg_count, uint16_t *out_data) {
    uint8_t tx_frame[8];
    tx_frame[0] = slave_addr;
    tx_frame[1] = 0x03;  // 功能码
    tx_frame[2] = (reg_addr >> 8) & 0xFF;
    tx_frame[3] = reg_addr & 0xFF;
    tx_frame[4] = (reg_count >> 8) & 0xFF;
    tx_frame[5] = reg_count & 0xFF;

    uint16_t crc = modbus_crc16(tx_frame, 6);
    tx_frame[6] = crc & 0xFF;
    tx_frame[7] = (crc >> 8) & 0xFF;

    // 发送
    RS485_TX_MODE();
    uart_write_bytes(RS485_UART_NUM, tx_frame, 8);
    uart_wait_tx_done(RS485_UART_NUM, 100);
    RS485_RX_MODE();  // 切换到接收模式

    // 接收响应 (3.5字符超时)
    uint8_t rx_frame[256];
    int rx_len = uart_read_bytes(RS485_UART_NUM, rx_frame, sizeof(rx_frame),
                                  pdMS_TO_TICKS(100));

    if (rx_len < 5) return false;  // 帧太短

    // 检查CRC
    uint16_t rx_crc = (rx_frame[rx_len-1] << 8) | rx_frame[rx_len-2];
    if (modbus_crc16(rx_frame, rx_len-2) != rx_crc) return false;

    // 解析数据
    if (rx_frame[0] != slave_addr || rx_frame[1] != 0x03) return false;

    uint8_t byte_count = rx_frame[2];
    if (byte_count != reg_count * 2) return false;

    for (int i = 0; i < reg_count; i++) {
        out_data[i] = (rx_frame[3 + i*2] << 8) | rx_frame[4 + i*2];
    }

    return true;
}

// Modbus从站实现
typedef struct {
    uint8_t slave_addr;
    uint16_t holding_regs[100];  // 保持寄存器
    uint16_t input_regs[100];    // 输入寄存器
    uint8_t coils[16];           // 线圈
    uint8_t discrete_inputs[16]; // 离散输入
} ModbusSlave;

static ModbusSlave mb_slave;

void modbus_slave_init(uint8_t addr) {
    mb_slave.slave_addr = addr;
    // 初始化寄存器...
}

void modbus_slave_handler(void) {
    uint8_t frame[256];
    int len = uart_read_bytes(RS485_UART_NUM, frame, sizeof(frame),
                              pdMS_TO_TICKS(10));
    if (len < 8) return;

    // 检查地址
    if (frame[0] != mb_slave.slave_addr && frame[0] != 0) return;  // 0是广播地址

    // 检查CRC
    uint16_t crc = (frame[len-1] << 8) | frame[len-2];
    if (modbus_crc16(frame, len-2) != crc) return;

    uint8_t func_code = frame[1];
    uint8_t resp[256];
    uint8_t resp_len = 0;

    switch (func_code) {
        case 0x03: {  // 读取保持寄存器
            uint16_t addr = (frame[2] << 8) | frame[3];
            uint16_t count = (frame[4] << 8) | frame[5];
            if (count > 125) count = 125;

            resp[0] = mb_slave.slave_addr;
            resp[1] = 0x03;
            resp[2] = count * 2;  // 字节数

            for (int i = 0; i < count; i++) {
                resp[3 + i*2] = (mb_slave.holding_regs[addr + i] >> 8) & 0xFF;
                resp[4 + i*2] = mb_slave.holding_regs[addr + i] & 0xFF;
            }
            resp_len = 3 + count * 2 + 2;  // +2 for CRC
            break;
        }

        case 0x06: {  // 写单个寄存器
            uint16_t addr = (frame[2] << 8) | frame[3];
            uint16_t value = (frame[4] << 8) | frame[5];
            mb_slave.holding_regs[addr] = value;

            // 响应与请求相同
            memcpy(resp, frame, 6);
            resp_len = 8;
            break;
        }

        case 0x10: {  // 写多个寄存器
            uint16_t addr = (frame[2] << 8) | frame[3];
            uint16_t count = (frame[4] << 8) | frame[5];
            uint8_t byte_count = frame[6];

            for (int i = 0; i < count; i++) {
                mb_slave.holding_regs[addr + i] =
                    (frame[7 + i*2] << 8) | frame[8 + i*2];
            }

            resp[0] = mb_slave.slave_addr;
            resp[1] = 0x10;
            resp[2] = (addr >> 8) & 0xFF;
            resp[3] = addr & 0xFF;
            resp[4] = (count >> 8) & 0xFF;
            resp[5] = count & 0xFF;
            resp_len = 8;
            break;
        }

        default: {  // 非法功能码
            resp[0] = mb_slave.slave_addr;
            resp[1] = 0x80 | func_code;  // 错误响应
            resp[2] = 0x01;  // 非法功能码
            resp_len = 5;
            break;
        }
    }

    // 添加CRC
    uint16_t resp_crc = modbus_crc16(resp, resp_len - 2);
    resp[resp_len - 2] = resp_crc & 0xFF;
    resp[resp_len - 1] = (resp_crc >> 8) & 0xFF;

    // 发送响应
    RS485_TX_MODE();
    uart_write_bytes(RS485_UART_NUM, resp, resp_len);
    uart_wait_tx_done(RS485_UART_NUM, 100);
    RS485_RX_MODE();
}
