/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\IoT_Protocols\01_IoT_Protocols_Implementation.md
 * Line: 3764
 * Language: c
 * Block ID: c4757fbf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file lorawan_stack.h
 * @brief LoRaWAN协议栈实现
 */

#ifndef LORAWAN_STACK_H
#define LORAWAN_STACK_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* LoRaWAN版本 */
#define LORAWAN_VERSION_MAJOR   1
#define LORAWAN_VERSION_MINOR   0
#define LORAWAN_VERSION_PATCH   4

/* EU868频段定义 */
#define EU868_FREQ_MIN          863000000
#define EU868_FREQ_MAX          870000000
#define EU868_DEFAULT_DR        0
#define EU868_MAX_TX_POWER      14  /* dBm */
#define EU868_MIN_TX_POWER      -4  /* dBm */

/* 设备类别 */
typedef enum {
    LORAWAN_CLASS_A = 0,    /* 仅上行后接收 */
    LORAWAN_CLASS_B = 1,    /* 信标同步接收 */
    LORAWAN_CLASS_C = 2     /* 持续接收 */
} lorawan_class_t;

/* 数据速率 */
typedef enum {
    LORAWAN_DR_0 = 0,       /* SF12, 125kHz */
    LORAWAN_DR_1 = 1,       /* SF11, 125kHz */
    LORAWAN_DR_2 = 2,       /* SF10, 125kHz */
    LORAWAN_DR_3 = 3,       /* SF9, 125kHz */
    LORAWAN_DR_4 = 4,       /* SF8, 125kHz */
    LORAWAN_DR_5 = 5,       /* SF7, 125kHz */
    LORAWAN_DR_6 = 6,       /* SF7, 250kHz */
    LORAWAN_DR_7 = 7        /* FSK */
} lorawan_datarate_t;

/* MAC命令 */
typedef enum {
    LORAWAN_MOTE_MAC_LINK_CHECK_REQ = 0x02,
    LORAWAN_MOTE_MAC_LINK_ADR_ANS = 0x03,
    LORAWAN_MOTE_MAC_DUTY_CYCLE_ANS = 0x04,
    LORAWAN_MOTE_MAC_RX_PARAM_SETUP_ANS = 0x05,
    LORAWAN_MOTE_MAC_DEV_STATUS_ANS = 0x06,
    LORAWAN_MOTE_MAC_NEW_CHANNEL_ANS = 0x07,
    LORAWAN_MOTE_MAC_RX_TIMING_SETUP_ANS = 0x08,
    LORAWAN_MOTE_MAC_TX_PARAM_SETUP_ANS = 0x09,
    LORAWAN_MOTE_MAC_DL_CHANNEL_ANS = 0x0A
} lorawan_mote_mac_cmd_t;

typedef enum {
    LORAWAN_SERV_MAC_LINK_CHECK_ANS = 0x02,
    LORAWAN_SERV_MAC_LINK_ADR_REQ = 0x03,
    LORAWAN_SERV_MAC_DUTY_CYCLE_REQ = 0x04,
    LORAWAN_SERV_MAC_RX_PARAM_SETUP_REQ = 0x05,
    LORAWAN_SERV_MAC_DEV_STATUS_REQ = 0x06,
    LORAWAN_SERV_MAC_NEW_CHANNEL_REQ = 0x07,
    LORAWAN_SERV_MAC_RX_TIMING_SETUP_REQ = 0x08,
    LORAWAN_SERV_MAC_TX_PARAM_SETUP_REQ = 0x09,
    LORAWAN_SERV_MAC_DL_CHANNEL_REQ = 0x0A
} lorawan_server_mac_cmd_t;

/* 消息类型 */
typedef enum {
    LORAWAN_MTYPE_JOIN_REQUEST = 0,
    LORAWAN_MTYPE_JOIN_ACCEPT = 1,
    LORAWAN_MTYPE_UNCONFIRMED_DATA_UP = 2,
    LORAWAN_MTYPE_UNCONFIRMED_DATA_DOWN = 3,
    LORAWAN_MTYPE_CONFIRMED_DATA_UP = 4,
    LORAWAN_MTYPE_CONFIRMED_DATA_DOWN = 5,
    LORAWAN_MTYPE_RFU = 6,
    LORAWAN_MTYPE_PROPRIETARY = 7
} lorawan_mtype_t;

/* 最大载荷长度 */
#define LORAWAN_MAX_PAYLOAD_LEN     242
#define LORAWAN_MAX_FOPTS_LEN       15
#define LORAWAN_MIC_LEN             4
#define LORAWAN_FHDR_LEN_MIN        7
#define LORAWAN_JOIN_EUI_LEN        8
#define LORAWAN_DEV_EUI_LEN         8
#define LORAWAN_APP_KEY_LEN         16
#define LORAWAN_NWK_KEY_LEN         16
#define LORAWAN_APP_SKEY_LEN        16
#define LORAWAN_NWK_SKEY_LEN        16
#define LORAWAN_DEV_NONCE_LEN       2
#define LORAWAN_APP_NONCE_LEN       3
#define LORAWAN_NET_ID_LEN          3

/* 错误码 */
typedef enum {
    LORAWAN_ERR_OK = 0,
    LORAWAN_ERR_INVALID_PARAM = -1,
    LORAWAN_ERR_BUSY = -2,
    LORAWAN_ERR_TX_TIMEOUT = -3,
    LORAWAN_ERR_RX_TIMEOUT = -4,
    LORAWAN_ERR_MIC_FAIL = -5,
    LORAWAN_ERR_JOIN_FAIL = -6,
    LORAWAN_ERR_NO_CHANNEL = -7,
    LORAWAN_ERR_DUTY_CYCLE = -8,
    LORAWAN_ERR_CRYPTO = -9,
    LORAWAN_ERR_NOT_JOINED = -10
} lorawan_error_t;

/* 射频接口回调 */
typedef int (*lorawan_radio_send_t)(void *ctx, uint32_t freq, int8_t power,
                                     lorawan_datarate_t dr,
                                     const uint8_t *data, size_t len);
typedef int (*lorawan_radio_receive_t)(void *ctx, uint32_t freq, lorawan_datarate_t dr,
                                        uint32_t timeout_ms, uint8_t *data, size_t max_len);
typedef uint32_t (*lorawan_get_time_ms_t)(void);
typedef void (*lorawan_delay_ms_t)(uint32_t ms);

/* 密钥结构 */
typedef struct {
    uint8_t app_key[LORAWAN_APP_KEY_LEN];
    uint8_t nwk_key[LORAWAN_NWK_KEY_LEN];
} lorawan_root_keys_t;

/* 会话密钥 */
typedef struct {
    uint8_t nwk_skey[LORAWAN_NWK_SKEY_LEN];
    uint8_t app_skey[LORAWAN_APP_SKEY_LEN];
} lorawan_session_keys_t;

/* 网络参数 */
typedef struct {
    uint32_t net_id;
    uint32_t dev_addr;
    uint16_t rx1_delay_ms;
    uint8_t rx1_dr_offset;
    lorawan_datarate_t rx2_dr;
    uint32_t rx2_freq;
} lorawan_network_params_t;

/* 帧计数器 */
typedef struct {
    uint32_t up;
    uint32_t down;
    uint32_t up_32bit;      /* 完整32位计数器 */
    uint32_t down_32bit;
} lorawan_frame_counters_t;

/* 信道配置 */
typedef struct {
    uint32_t frequency;
    bool enabled;
    uint8_t dr_range_min;
    uint8_t dr_range_max;
} lorawan_channel_t;

/* LoRaWAN上下文 */
typedef struct {
    /* 设备标识 */
    uint8_t join_eui[LORAWAN_JOIN_EUI_LEN];
    uint8_t dev_eui[LORAWAN_DEV_EUI_LEN];
    lorawan_root_keys_t root_keys;

    /* 会话状态 */
    bool joined;
    lorawan_class_t dev_class;
    lorawan_session_keys_t session_keys;
    lorawan_network_params_t net_params;
    lorawan_frame_counters_t counters;

    /* 信道 */
    lorawan_channel_t channels[16];
    uint8_t num_channels;
    uint8_t current_channel;

    /* ADR */
    bool adr_enabled;
    lorawan_datarate_t current_dr;
    int8_t current_tx_power;
    uint8_t redundancy;

    /* 射频接口 */
    void *radio_ctx;
    lorawan_radio_send_t radio_send;
    lorawan_radio_receive_t radio_receive;
    lorawan_get_time_ms_t get_time_ms;
    lorawan_delay_ms_t delay_ms;

    /* 缓冲区 */
    uint8_t tx_buffer[LORAWAN_MAX_PAYLOAD_LEN + 64];
    uint8_t rx_buffer[LORAWAN_MAX_PAYLOAD_LEN + 64];

    /* 接收窗口配置 */
    uint32_t rx1_freq;
    lorawan_datarate_t rx1_dr;
    uint32_t rx2_freq;
    lorawan_datarate_t rx2_dr;

    /* MAC命令缓冲区 */
    uint8_t mac_commands[16];
    uint8_t mac_cmd_len;

    /* 确认重传 */
    uint8_t ack_retries;
    uint8_t max_retries;
    bool confirmed;

    /* 统计 */
    uint32_t tx_packets;
    uint32_t rx_packets;
    uint32_t tx_bytes;
    uint32_t rx_bytes;
} lorawan_context_t;

/* API函数 */

/* 初始化和加入 */
int lorawan_init(lorawan_context_t *ctx, const uint8_t *join_eui,
                  const uint8_t *dev_eui, const uint8_t *app_key,
                  lorawan_class_t dev_class);
int lorawan_join_otaa(lorawan_context_t *ctx);
int lorawan_join_abp(lorawan_context_t *ctx, uint32_t dev_addr,
                      const uint8_t *nwk_skey, const uint8_t *app_skey);

/* 发送和接收 */
int lorawan_send(lorawan_context_t *ctx, const uint8_t *data, size_t len,
                  uint8_t port, bool confirmed);
int lorawan_send_with_callback(lorawan_context_t *ctx, const uint8_t *data, size_t len,
                                 uint8_t port, bool confirmed,
                                 void (*ack_callback)(bool success, void *user_data),
                                 void *user_data);
int lorawan_receive(lorawan_context_t *ctx, uint8_t *data, size_t max_len,
                     uint8_t *port, int32_t timeout_ms);

/* 处理 */
int lorawan_process(lorawan_context_t *ctx);
void lorawan_process_downlink(lorawan_context_t *ctx, const uint8_t *data, size_t len);

/* 配置 */
int lorawan_set_data_rate(lorawan_context_t *ctx, lorawan_datarate_t dr);
int lorawan_set_tx_power(lorawan_context_t *ctx, int8_t power);
int lorawan_enable_adr(lorawan_context_t *ctx, bool enable);
int lorawan_set_channel(lorawan_context_t *ctx, uint8_t idx, uint32_t freq,
                         uint8_t dr_min, uint8_t dr_max);

/* MAC命令 */
int lorawan_send_link_check(lorawan_context_t *ctx);
int lorawan_request_device_status(lorawan_context_t *ctx);

/* 状态查询 */
bool lorawan_is_joined(lorawan_context_t *ctx);
lorawan_datarate_t lorawan_get_data_rate(lorawan_context_t *ctx);
int8_t lorawan_get_tx_power(lorawan_context_t *ctx);

/* 工具函数 */
int lorawan_get_payload_max_len(lorawan_datarate_t dr);
const char* lorawan_err_str(lorawan_error_t err);

/* AES加密辅助函数 */
void lorawan_aes_encrypt(const uint8_t *key, const uint8_t *in, uint8_t *out);
void lorawan_aes_decrypt(const uint8_t *key, const uint8_t *in, uint8_t *out);
void lorawan_cmac(const uint8_t *key, const uint8_t *data, size_t len, uint8_t *cmac);

#ifdef __cplusplus
}
#endif

#endif /* LORAWAN_STACK_H */
