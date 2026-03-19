/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 414
 * Language: c
 * Block ID: d7288661
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 发送数据包 */
int sx1276_send(const uint8_t *data, uint8_t len) {
    /* 1. 进入待机模式 */
    sx1276_set_mode(MODE_STDBY);

    /* 2. 配置FIFO基地址 */
    sx1276_write_reg(REG_FIFO_TX_BASE_ADDR, 0x00);
    sx1276_write_reg(REG_FIFO_ADDR_PTR, 0x00);

    /* 3. 写入数据到FIFO */
    sx1276_write_burst(REG_FIFO, data, len);

    /* 4. 设置负载长度 */
    sx1276_write_reg(REG_PAYLOAD_LENGTH, len);

    /* 5. 启动发送 */
    sx1276_write_reg(REG_OP_MODE, MODE_TX | 0x80);  /* LoRa模式 + TX */

    return 0;
}

/* 检查发送完成 */
bool sx1276_is_tx_done(void) {
    return sx1276_read_reg(REG_IRQ_FLAGS) & 0x08;  /* TxDone */
}

/* 清除发送中断 */
void sx1276_clear_tx_irq(void) {
    sx1276_write_reg(REG_IRQ_FLAGS, 0x08);
}
