/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\01_ARINC_429.md
 * Line: 763
 * Language: c
 * Block ID: 5ae0dbd6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief ARINC 429使用示例
 */
void ARINC429_Example(void) {
    ARINC429_Word_t word;
    uint8_t label, sdi, ssm;
    uint32_t data;
    double altitude;

    /* 初始化 */
    ARINC429_Transceiver_Init();
    ARINC429_ConfigureRxChannel(0, ARINC429_SPEED_HIGH, NULL, 0);

    /* 编码并发送高度数据 (Label 057, 0.5ft分辨率) */
    altitude = 35000.5;  /* 35,000.5 英尺 */
    data = ARINC429_Encode_BNR(altitude, 0.5, 19, true);
    word = ARINC429_EncodeWord(ARINC429_LABEL_BARO_ALTITUDE, 0, data,
                                ARINC429_SSM_PLUS);
    ARINC429_TransmitWord(0, word, 0);

    /* 接收并解码 */
    if (ARINC429_ReadWord(&word, NULL)) {
        if (ARINC429_DecodeWord(word, &label, &sdi, &data, &ssm)) {
            if (label == ARINC429_LABEL_BARO_ALTITUDE) {
                altitude = ARINC429_Decode_BNR(data, 0.5, 19, true);
                /* 处理高度数据 */
            }
        }
    }
}
