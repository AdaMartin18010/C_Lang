/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 4040
 * Language: c
 * Block ID: b771d81f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file lorawan_stack.c
 * @brief LoRaWAN协议栈实现
 */

#include "lorawan_stack.h"
#include <string.h>
#include <stdio.h>

/* S-Box用于AES */
static const uint8_t sbox[256] = {
    0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
    0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
    0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
    0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
    0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
    0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
    0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
    0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
    0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
    0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
    0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
    0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
    0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
    0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
    0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
    0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

/* 简化AES实现 (实际项目应使用成熟的加密库) */
void lorawan_aes_encrypt(const uint8_t *key, const uint8_t *in, uint8_t *out) {
    /* 简化的AES实现 - 实际使用mbedtls或其他库 */
    /* 此处仅为示例 */
    (void)key;
    memcpy(out, in, 16);
    for (int i = 0; i < 16; i++) {
        out[i] = sbox[out[i]];
    }
}

void lorawan_aes_decrypt(const uint8_t *key, const uint8_t *in, uint8_t *out) {
    (void)key;
    memcpy(out, in, 16);
    /* 简化实现 */
}

/* 初始化LoRaWAN */
int lorawan_init(lorawan_context_t *ctx, const uint8_t *join_eui,
                  const uint8_t *dev_eui, const uint8_t *app_key,
                  lorawan_class_t dev_class) {
    if (ctx == NULL || join_eui == NULL || dev_eui == NULL || app_key == NULL) {
        return LORAWAN_ERR_INVALID_PARAM;
    }

    memset(ctx, 0, sizeof(lorawan_context_t));

    memcpy(ctx->join_eui, join_eui, LORAWAN_JOIN_EUI_LEN);
    memcpy(ctx->dev_eui, dev_eui, LORAWAN_DEV_EUI_LEN);
    memcpy(ctx->root_keys.app_key, app_key, LORAWAN_APP_KEY_LEN);
    memcpy(ctx->root_keys.nwk_key, app_key, LORAWAN_NWK_KEY_LEN);

    ctx->dev_class = dev_class;
    ctx->joined = false;
    ctx->adr_enabled = true;
    ctx->current_dr = LORAWAN_DR_0;
    ctx->current_tx_power = EU868_MAX_TX_POWER;
    ctx->max_retries = 8;

    /* 初始化EU868默认信道 */
    ctx->channels[0].frequency = 868100000;
    ctx->channels[0].enabled = true;
    ctx->channels[0].dr_range_min = 0;
    ctx->channels[0].dr_range_max = 5;

    ctx->channels[1].frequency = 868300000;
    ctx->channels[1].enabled = true;
    ctx->channels[1].dr_range_min = 0;
    ctx->channels[1].dr_range_max = 5;

    ctx->channels[2].frequency = 868500000;
    ctx->channels[2].enabled = true;
    ctx->channels[2].dr_range_min = 0;
    ctx->channels[2].dr_range_max = 5;

    ctx->num_channels = 3;
    ctx->current_channel = 0;

    /* RX2默认参数 */
    ctx->rx2_freq = 869525000;
    ctx->rx2_dr = LORAWAN_DR_0;

    ctx->counters.up = 0;
    ctx->counters.down = 0;

    return LORAWAN_ERR_OK;
}

/* 生成Join Request */
static int build_join_request(lorawan_context_t *ctx, uint8_t *buffer, size_t *len) {
    int pos = 0;

    /* MHDR */
    buffer[pos++] = LORAWAN_MTYPE_JOIN_REQUEST << 5;

    /* Join EUI (little-endian) */
    for (int i = LORAWAN_JOIN_EUI_LEN - 1; i >= 0; i--) {
        buffer[pos++] = ctx->join_eui[i];
    }

    /* Dev EUI (little-endian) */
    for (int i = LORAWAN_DEV_EUI_LEN - 1; i >= 0; i--) {
        buffer[pos++] = ctx->dev_eui[i];
    }

    /* Dev Nonce (随机生成) */
    static uint16_t dev_nonce = 0;
    if (dev_nonce == 0) {
        dev_nonce = (uint16_t)(ctx->get_time_ms ? ctx->get_time_ms() : 1);
    } else {
        dev_nonce++;
    }
    buffer[pos++] = (uint8_t)(dev_nonce & 0xFF);
    buffer[pos++] = (uint8_t)(dev_nonce >> 8);

    /* MIC (简化 - 实际应计算CMAC) */
    for (int i = 0; i < LORAWAN_MIC_LEN; i++) {
        buffer[pos++] = 0x00;
    }

    *len = pos;
    return LORAWAN_ERR_OK;
}

/* OTAA加入 */
int lorawan_join_otaa(lorawan_context_t *ctx) {
    if (ctx == NULL || ctx->radio_send == NULL || ctx->radio_receive == NULL) {
        return LORAWAN_ERR_INVALID_PARAM;
    }

    if (ctx->joined) {
        return LORAWAN_ERR_OK;
    }

    size_t req_len;
    int err = build_join_request(ctx, ctx->tx_buffer, &req_len);
    if (err != LORAWAN_ERR_OK) return err;

    /* 选择信道 */
    uint32_t freq = ctx->channels[0].frequency;
    lorawan_datarate_t dr = LORAWAN_DR_0;

    /* 发送Join Request */
    err = ctx->radio_send(ctx->radio_ctx, freq, ctx->current_tx_power, dr,
                          ctx->tx_buffer, req_len);
    if (err != 0) {
        return LORAWAN_ERR_BUSY;
    }

    /* 等待Join Accept (RX1窗口) */
    int rx_len = ctx->radio_receive(ctx->radio_ctx, freq, dr, 6000,
                                     ctx->rx_buffer, sizeof(ctx->rx_buffer));

    if (rx_len < 0) {
        /* 尝试RX2窗口 */
        rx_len = ctx->radio_receive(ctx->radio_ctx, ctx->rx2_freq, ctx->rx2_dr,
                                     2000, ctx->rx_buffer, sizeof(ctx->rx_buffer));
    }

    if (rx_len < 0) {
        return LORAWAN_ERR_JOIN_FAIL;
    }

    /* 解析Join Accept (简化) */
    ctx->joined = true;
    ctx->net_params.dev_addr = 0x01020304; /* 实际应从Join Accept解析 */

    /* 生成会话密钥 */
    memcpy(ctx->session_keys.nwk_skey, ctx->root_keys.nwk_key, LORAWAN_NWK_SKEY_LEN);
    memcpy(ctx->session_keys.app_skey, ctx->root_keys.app_key, LORAWAN_APP_SKEY_LEN);

    return LORAWAN_ERR_OK;
}

/* ABP加入 */
int lorawan_join_abp(lorawan_context_t *ctx, uint32_t dev_addr,
                      const uint8_t *nwk_skey, const uint8_t *app_skey) {
    if (ctx == NULL || nwk_skey == NULL || app_skey == NULL) {
        return LORAWAN_ERR_INVALID_PARAM;
    }

    ctx->net_params.dev_addr = dev_addr;
    memcpy(ctx->session_keys.nwk_skey, nwk_skey, LORAWAN_NWK_SKEY_LEN);
    memcpy(ctx->session_keys.app_skey, app_skey, LORAWAN_APP_SKEY_LEN);
    ctx->joined = true;

    return LORAWAN_ERR_OK;
}

/* 构建数据帧 */
static int build_data_frame(lorawan_context_t *ctx, const uint8_t *data, size_t len,
                             uint8_t port, bool confirmed, uint8_t *buffer, size_t *out_len) {
    int pos = 0;

    /* MHDR */
    lorawan_mtype_t mtype = confirmed ? LORAWAN_MTYPE_CONFIRMED_DATA_UP
                                      : LORAWAN_MTYPE_UNCONFIRMED_DATA_UP;
    buffer[pos++] = (mtype << 5) | (LORAWAN_VERSION_MAJOR & 0x03);

    /* FHDR */
    /* DevAddr (little-endian) */
    uint32_t dev_addr = ctx->net_params.dev_addr;
    buffer[pos++] = (uint8_t)(dev_addr & 0xFF);
    buffer[pos++] = (uint8_t)(dev_addr >> 8);
    buffer[pos++] = (uint8_t)(dev_addr >> 16);
    buffer[pos++] = (uint8_t)(dev_addr >> 24);

    /* FCtrl */
    uint8_t fctrl = 0;
    if (ctx->adr_enabled) {
        fctrl |= 0x80; /* ADR */
    }
    if (confirmed && ctx->ack_retries > 0) {
        fctrl |= 0x20; /* ACK */
    }
    if (ctx->mac_cmd_len > 0) {
        fctrl |= (ctx->mac_cmd_len < 16) ? ctx->mac_cmd_len : 15;
    }
    buffer[pos++] = fctrl;

    /* FCnt (little-endian) */
    uint16_t fcnt = (uint16_t)(ctx->counters.up & 0xFFFF);
    buffer[pos++] = (uint8_t)(fcnt & 0xFF);
    buffer[pos++] = (uint8_t)(fcnt >> 8);

    /* FOpts (MAC命令) */
    if (ctx->mac_cmd_len > 0) {
        int opts_len = (ctx->mac_cmd_len < 16) ? ctx->mac_cmd_len : 15;
        memcpy(buffer + pos, ctx->mac_commands, opts_len);
        pos += opts_len;
    }

    /* FPort */
    buffer[pos++] = port;

    /* FRMPayload (加密) */
    if (len > 0) {
        /* 简化: 实际应使用AES-CTR加密 */
        memcpy(buffer + pos, data, len);
        pos += (int)len;
    }

    /* MIC (简化) */
    for (int i = 0; i < LORAWAN_MIC_LEN; i++) {
        buffer[pos++] = 0x00;
    }

    *out_len = pos;
    return LORAWAN_ERR_OK;
}

/* 发送数据 */
int lorawan_send(lorawan_context_t *ctx, const uint8_t *data, size_t len,
                  uint8_t port, bool confirmed) {
    if (ctx == NULL || data == NULL || len == 0 || port == 0) {
        return LORAWAN_ERR_INVALID_PARAM;
    }

    if (!ctx->joined) {
        return LORAWAN_ERR_NOT_JOINED;
    }

    if (len > lorawan_get_payload_max_len(ctx->current_dr)) {
        return LORAWAN_ERR_INVALID_PARAM;
    }

    ctx->confirmed = confirmed;
    ctx->ack_retries = confirmed ? ctx->max_retries : 0;

    size_t frame_len;
    int err = build_data_frame(ctx, data, len, port, confirmed,
                                ctx->tx_buffer, &frame_len);
    if (err != LORAWAN_ERR_OK) return err;

    /* 选择信道 */
    uint32_t freq = ctx->channels[ctx->current_channel].frequency;
    lorawan_datarate_t dr = ctx->current_dr;

    /* 发送 */
    err = ctx->radio_send(ctx->radio_ctx, freq, ctx->current_tx_power, dr,
                          ctx->tx_buffer, frame_len);
    if (err != 0) {
        return LORAWAN_ERR_BUSY;
    }

    ctx->tx_packets++;
    ctx->tx_bytes += len;
    ctx->counters.up++;

    /* 更新信道 */
    ctx->current_channel = (ctx->current_channel + 1) % ctx->num_channels;

    return LORAWAN_ERR_OK;
}

/* 检查是否已加入 */
bool lorawan_is_joined(lorawan_context_t *ctx) {
    return (ctx != NULL) && ctx->joined;
}

/* 获取最大载荷长度 */
int lorawan_get_payload_max_len(lorawan_datarate_t dr) {
    /* EU868最大载荷长度表 */
    static const int max_len[] = {59, 59, 59, 123, 250, 250, 250, 250};
    if (dr <= LORAWAN_DR_7) {
        return max_len[dr];
    }
    return 0;
}

/* 错误码转字符串 */
const char* lorawan_err_str(lorawan_error_t err) {
    switch (err) {
        case LORAWAN_ERR_OK: return "OK";
        case LORAWAN_ERR_INVALID_PARAM: return "Invalid parameter";
        case LORAWAN_ERR_BUSY: return "Busy";
        case LORAWAN_ERR_TX_TIMEOUT: return "TX timeout";
        case LORAWAN_ERR_RX_TIMEOUT: return "RX timeout";
        case LORAWAN_ERR_MIC_FAIL: return "MIC verification failed";
        case LORAWAN_ERR_JOIN_FAIL: return "Join failed";
        case LORAWAN_ERR_NO_CHANNEL: return "No available channel";
        case LORAWAN_ERR_DUTY_CYCLE: return "Duty cycle limit";
        case LORAWAN_ERR_CRYPTO: return "Crypto error";
        case LORAWAN_ERR_NOT_JOINED: return "Not joined";
        default: return "Unknown error";
    }
}
