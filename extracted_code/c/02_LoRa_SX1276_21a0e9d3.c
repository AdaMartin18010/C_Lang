/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 259
 * Language: c
 * Block ID: 21a0e9d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 设置发射功率
 * @param power: dBm值，范围-4到+20
 * @param use_rfo: 使用RFO引脚(低功率)或PA_BOOST(高功率)
 */
void sx1276_set_tx_power(int8_t power, bool use_rfo) {
    if (use_rfo) {
        /* RFO引脚，范围-4到+15dBm */
        if (power > 15) power = 15;
        if (power < -4) power = -4;

        /* PA配置: RFO模式，max power=2 (即10.8+0.6*2=12dBm max) */
        sx1276_write_reg(REG_PA_CONFIG,
                        (2 << 4) | ((power + 4) & 0x0F));
        sx1276_write_reg(REG_PA_DAC, 0x84);  /* default */
    } else {
        /* PA_BOOST引脚，范围+2到+20dBm */
        if (power > 20) power = 20;
        if (power < 2) power = 2;

        if (power > 17) {
            /* +20dBm需要特殊DAC设置 */
            sx1276_write_reg(REG_PA_DAC, 0x87);  /* 高功率模式 */
            power = 15;  /* 寄存器值对应+20dBm */
        } else {
            sx1276_write_reg(REG_PA_DAC, 0x84);  /* 默认 */
        }

        sx1276_write_reg(REG_PA_CONFIG,
                        0x80 | (power - 2));  /* PA_BOOST bit = 1 */
    }
}

/* 过流保护配置 */
void sx1276_set_ocp(uint8_t current_ma) {
    uint8_t ocp_trim;
    if (current_ma <= 120) {
        ocp_trim = (current_ma - 45) / 5;
    } else if (current_ma <= 240) {
        ocp_trim = (current_ma + 30) / 10;
    } else {
        ocp_trim = 27;  /* max 240mA */
    }

    sx1276_write_reg(REG_OCP, 0x20 | (ocp_trim & 0x1F));
}
