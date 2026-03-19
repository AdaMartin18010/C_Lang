/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 359
 * Language: c
 * Block ID: a7cc4b8b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * CSMA-CA (载波侦听多路访问/冲突避免) 算法
 * 非时隙版本（用于非信标网络）
 */
bool mac_csma_ca(void) {
    uint8_t nb = 0;           // 退避次数
    uint8_t cw = 2;           // 竞争窗口 (2或1)
    uint8_t be = mac_pib.min_be;  // 退避指数

    while (nb <= mac_pib.max_csma_backoffs) {
        // 生成随机退避时间: (0 to 2^BE - 1) 个退避周期
        uint8_t backoff = rand() % (1 << be);
        uint16_t backoff_us = backoff * MAC_UNIT_BACKOFF_PERIOD; // 320us

        // 等待退避时间
        delay_us(backoff_us);

        // 执行CCA (信道空闲评估)
        phy_cca_status_t cca_result = phy_cca();

        if (cca_result == PHY_CCA_IDLE) {
            // 信道空闲
            cw--;
            if (cw == 0) {
                return true;  // 可以发送
            }
        } else {
            // 信道忙
            cw = 2;  // 重置竞争窗口
            nb++;
            be++;    // 增加退避指数
            if (be > mac_pib.max_be) {
                be = mac_pib.max_be;
            }
        }
    }

    return false;  // 达到最大退避次数，发送失败
}

// 时隙CSMA-CA（用于信标网络）
bool mac_csma_ca_slotted(void) {
    // 类似算法，但退避周期与信标边界对齐
    // ...
    return true;
}
