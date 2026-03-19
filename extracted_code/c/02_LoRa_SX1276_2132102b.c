/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 449
 * Language: c
 * Block ID: 2132102b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 进入接收模式 */
void sx1276_set_rx_mode(bool continuous) {
    sx1276_set_mode(MODE_STDBY);

    /* 配置FIFO基地址 */
    sx1276_write_reg(REG_FIFO_RX_BASE_ADDR, 0x00);
    sx1276_write_reg(REG_FIFO_ADDR_PTR, 0x00);

    /* 清除所有中断 */
    sx1276_write_reg(REG_IRQ_FLAGS, 0xFF);

    /* 启动接收 */
    RadioMode rx_mode = continuous ? MODE_RXCONTINUOUS : MODE_RXSINGLE;
    sx1276_write_reg(REG_OP_MODE, rx_mode | 0x80);
}

/* 读取接收数据 */
int sx1276_receive(uint8_t *buffer, uint8_t max_len) {
    /* 检查接收完成标志 */
    uint8_t irq_flags = sx1276_read_reg(REG_IRQ_FLAGS);

    if (!(irq_flags & 0x40)) {  /* RxDone */
        return 0;  /* 尚未收到数据 */
    }

    /* 清除中断 */
    sx1276_write_reg(REG_IRQ_FLAGS, 0x40);

    /* 检查CRC错误 */
    if (irq_flags & 0x20) {  /* PayloadCrcError */
        sx1276_write_reg(REG_IRQ_FLAGS, 0x20);
        return -1;  /* CRC错误 */
    }

    /* 获取当前FIFO指针位置 */
    uint8_t fifo_ptr = sx1276_read_reg(REG_FIFO_RX_CURR_ADDR);
    sx1276_write_reg(REG_FIFO_ADDR_PTR, fifo_ptr);

    /* 读取接收字节数 */
    uint8_t rx_nb_bytes = sx1276_read_reg(REG_RX_NB_BYTES);
    if (rx_nb_bytes > max_len) {
        rx_nb_bytes = max_len;
    }

    /* 从FIFO读取数据 */
    sx1276_read_burst(REG_FIFO, buffer, rx_nb_bytes);

    return rx_nb_bytes;
}

/* 获取RSSI和SNR */
void sx1276_get_packet_stats(int16_t *rssi, float *snr) {
    *rssi = sx1276_read_reg(REG_PKT_RSSI_VALUE) - 164;  /* HF模式校正 */

    int8_t raw_snr = (int8_t)sx1276_read_reg(REG_PKT_SNR_VALUE);
    *snr = raw_snr * 0.25f;

    /* RSSI精度增强（基于SNR） */
    if (*snr < 0) {
        *rssi = *rssi + *snr;
    }
}
