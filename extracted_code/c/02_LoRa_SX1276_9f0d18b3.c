/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 516
 * Language: c
 * Block ID: 9f0d18b3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 启动CAD检测 */
void sx1276_start_cad(void) {
    sx1276_set_mode(MODE_STDBY);

    /* 清除CadDetected中断 */
    sx1276_write_reg(REG_IRQ_FLAGS, 0x01);

    /* 进入CAD模式 */
    sx1276_write_reg(REG_OP_MODE, MODE_CAD | 0x80);
}

/* 检查CAD结果 */
typedef enum {
    CAD_NONE,       /* 检测未完成 */
    CAD_DETECTED,   /* 检测到LoRa前导码 */
    CAD_CLEAR,      /* 信道空闲 */
} CAD_Result;

CAD_Result sx1276_check_cad(void) {
    uint8_t irq = sx1276_read_reg(REG_IRQ_FLAGS);

    if (irq & 0x01) {  /* CadDetected */
        sx1276_write_reg(REG_IRQ_FLAGS, 0x01);
        return CAD_DETECTED;
    }

    /* CAD完成但未检测到 (CadDone=1, CadDetected=0) */
    if (irq & 0x04) {
        sx1276_write_reg(REG_IRQ_FLAGS, 0x04);
        return CAD_CLEAR;
    }

    return CAD_NONE;
}
