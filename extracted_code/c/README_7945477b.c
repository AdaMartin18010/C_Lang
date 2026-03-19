/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\README.md
 * Line: 451
 * Language: c
 * Block ID: 7945477b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#include "modbus.h"

int main() {
    modbus_t ctx;

    // 初始化 Modbus RTU
    modbus_init_rtu(&ctx, "/dev/ttyUSB0", 9600, 'N', 8, 1);
    modbus_set_slave(&ctx, 1);

    // 连接设备
    if (modbus_connect(&ctx) == 0) {
        uint16_t regs[10];

        // 读取保持寄存器
        int rc = modbus_read_registers(&ctx, 0, 10, regs);
        if (rc > 0) {
            for (int i = 0; i < rc; i++) {
                printf("Register %d: %d\n", i, regs[i]);
            }
        }
    }

    modbus_close(&ctx);
    return 0;
}
