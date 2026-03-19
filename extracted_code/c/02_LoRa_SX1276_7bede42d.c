/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 233
 * Language: c
 * Block ID: 7bede42d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 设置载波频率
 * @param freq_hz: 频率(Hz)，范围137-525MHz或860-1020MHz
 */
void sx1276_set_frequency(uint32_t freq_hz) {
    /* FRF = freq * 2^19 / F_XOSC
     * F_XOSC = 32MHz (典型晶振频率)
     */
    uint64_t frf = ((uint64_t)freq_hz << 19) / 32000000;

    sx1276_write_reg(REG_FRF_MSB, (frf >> 16) & 0xFF);
    sx1276_write_reg(REG_FRF_MID, (frf >> 8) & 0xFF);
    sx1276_write_reg(REG_FRF_LSB, frf & 0xFF);
}

/* 获取当前频率 */
uint32_t sx1276_get_frequency(void) {
    uint32_t frf = (sx1276_read_reg(REG_FRF_MSB) << 16) |
                   (sx1276_read_reg(REG_FRF_MID) << 8) |
                   sx1276_read_reg(REG_FRF_LSB);
    return (frf * 32000000) >> 19;
}
