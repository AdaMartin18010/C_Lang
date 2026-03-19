/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\02_LoRa_SX1276.md
 * Line: 313
 * Language: c
 * Block ID: b97687c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* LoRa配置结构 */
typedef struct {
    SpreadingFactor sf;
    Bandwidth bw;
    CodingRate cr;
    bool implicit_header;
    bool crc_on;
    uint16_t preamble_len;
    uint8_t payload_len;
    bool tx_continuous;
    bool agc_auto_on;
} LoRaConfig;

/* 应用LoRa配置 */
void sx1276_set_lora_config(const LoRaConfig *cfg) {
    /* Modem Config 1: BW + CR + ImplicitHeaderModeOn */
    uint8_t mc1 = (cfg->bw << 4) | (cfg->cr << 1) | cfg->implicit_header;
    sx1276_write_reg(REG_MODEM_CONFIG_1, mc1);

    /* Modem Config 2: SF + TxContinuousMode + RxPayloadCrcOn + SymbTimeout(9:8) */
    uint8_t mc2 = (cfg->sf << 4) | (cfg->tx_continuous << 3) |
                  (cfg->crc_on << 2) | 0x00;
    sx1276_write_reg(REG_MODEM_CONFIG_2, mc2);

    /* Modem Config 3: 低数据率优化 + AGC */
    uint8_t mc3 = 0x00;
    if ((cfg->bw == BW_125_KHZ && cfg->sf >= SF_11) ||
        (cfg->bw == BW_250_KHZ && cfg->sf == SF_12)) {
        mc3 |= 0x08;  /* LowDataRateOptimizeOn */
    }
    if (cfg->agc_auto_on) {
        mc3 |= 0x04;  /* AgcAutoOn */
    }
    sx1276_write_reg(REG_MODEM_CONFIG_3, mc3);

    /* 前导码长度 */
    sx1276_write_reg(REG_PREAMBLE_MSB, (cfg->preamble_len >> 8) & 0xFF);
    sx1276_write_reg(REG_PREAMBLE_LSB, cfg->preamble_len & 0xFF);

    /* 负载长度 (implicit header模式必需) */
    if (cfg->implicit_header) {
        sx1276_write_reg(REG_PAYLOAD_LENGTH, cfg->payload_len);
    }

    /* SF6特殊优化 */
    if (cfg->sf == SF_6) {
        sx1276_write_reg(REG_DETECTION_OPTIMIZE, 0x05);
        sx1276_write_reg(REG_DETECTION_THRESHOLD, 0x0C);
    } else {
        sx1276_write_reg(REG_DETECTION_OPTIMIZE, 0x03);
        sx1276_write_reg(REG_DETECTION_THRESHOLD, 0x0A);
    }
}

/* 获取当前配置 */
void sx1276_get_lora_config(LoRaConfig *cfg) {
    uint8_t mc1 = sx1276_read_reg(REG_MODEM_CONFIG_1);
    uint8_t mc2 = sx1276_read_reg(REG_MODEM_CONFIG_2);

    cfg->bw = (mc1 >> 4) & 0x0F;
    cfg->cr = (mc1 >> 1) & 0x07;
    cfg->implicit_header = mc1 & 0x01;

    cfg->sf = (mc2 >> 4) & 0x0F;
    cfg->tx_continuous = (mc2 >> 3) & 0x01;
    cfg->crc_on = (mc2 >> 2) & 0x01;

    cfg->preamble_len = (sx1276_read_reg(REG_PREAMBLE_MSB) << 8) |
                         sx1276_read_reg(REG_PREAMBLE_LSB);
}

/* 计算空中时间(毫秒) */
uint32_t sx1276_get_time_on_air(const LoRaConfig *cfg, uint8_t payload_len) {
    /* 符号持续时间: Ts = 2^SF / BW */
    float ts = (1 << cfg->sf) / (125.0 * (1 << cfg->bw));

    /* 前导码符号数 */
    float t_preamble = (cfg->preamble_len + 4.25) * ts;

    /* 有效负载符号数 */
    float de = ((cfg->bw == BW_125_KHZ && cfg->sf >= SF_11) ||
                (cfg->bw == BW_250_KHZ && cfg->sf == SF_12)) ? 1 : 0;

    float payload_symb_nb = 8 + max(
        ceil((8 * payload_len - 4 * cfg->sf + 28 + 16 * cfg->crc_on -
             20 * cfg->implicit_header) / (4 * (cfg->sf - 2 * de))) *
        (cfg->cr + 4), 0);

    float t_payload = payload_symb_nb * ts;

    return (uint32_t)((t_preamble + t_payload) * 1000);
}
