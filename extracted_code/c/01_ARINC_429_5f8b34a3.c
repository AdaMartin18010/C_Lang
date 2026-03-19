/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\01_ARINC_429.md
 * Line: 539
 * Language: c
 * Block ID: 5f8b34a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file arinc429_transceiver.c
 * @brief ARINC 429模拟收发器实现
 * @note 基于软件模拟，实际硬件实现需依赖具体平台
 */

#include <string.h>
#include "arinc429.h"

/* 接收缓冲区大小 */
#define ARINC429_RX_BUFFER_SIZE     (128U)
#define ARINC429_TX_BUFFER_SIZE     (128U)

/* 设备状态 */
typedef enum {
    ARINC429_STATE_RESET = 0,
    ARINC429_STATE_INITIALIZED,
    ARINC429_STATE_RUNNING,
    ARINC429_STATE_ERROR
} ARINC429_State_t;

/* 接收消息结构 */
typedef struct {
    ARINC429_Word_t word;
    uint32_t timestamp;
    uint8_t channel;
} ARINC429_RxMessage_t;

/* 发送消息结构 */
typedef struct {
    ARINC429_Word_t word;
    uint32_t schedule_time;
    uint8_t priority;
} ARINC429_TxMessage_t;

/* 通道配置 */
typedef struct {
    uint8_t channel_id;
    ARINC429_Speed_t speed;
    bool is_enabled;
    uint32_t label_filter[4];  /* 标签过滤位图 */
} ARINC429_ChannelConfig_t;

/* 收发器实例 */
typedef struct {
    ARINC429_State_t state;
    ARINC429_ChannelConfig_t rx_channels[4];
    ARINC429_ChannelConfig_t tx_channels[4];

    /* 接收缓冲区 */
    ARINC429_RxMessage_t rx_buffer[ARINC429_RX_BUFFER_SIZE];
    uint16_t rx_head;
    uint16_t rx_tail;

    /* 发送缓冲区 */
    ARINC429_TxMessage_t tx_buffer[ARINC429_TX_BUFFER_SIZE];
    uint16_t tx_head;
    uint16_t tx_tail;

    /* 统计信息 */
    uint32_t rx_count;
    uint32_t tx_count;
    uint32_t error_count;
    uint32_t parity_errors;
} ARINC429_Transceiver_t;

static ARINC429_Transceiver_t g_arinc429;

/**
 * @brief 初始化收发器
 */
bool ARINC429_Transceiver_Init(void) {
    (void)memset(&g_arinc429, 0, sizeof(g_arinc429));
    g_arinc429.state = ARINC429_STATE_INITIALIZED;
    return true;
}

/**
 * @brief 配置接收通道
 */
bool ARINC429_ConfigureRxChannel(uint8_t channel_id, ARINC429_Speed_t speed,
                                  const uint8_t *filter_labels, uint8_t num_labels) {
    ARINC429_ChannelConfig_t *config;
    uint8_t i;

    if ((channel_id >= 4U) || (g_arinc429.state != ARINC429_STATE_INITIALIZED)) {
        return false;
    }

    config = &g_arinc429.rx_channels[channel_id];
    config->channel_id = channel_id;
    config->speed = speed;

    /* 设置标签过滤 */
    if ((filter_labels != NULL) && (num_labels > 0U)) {
        for (i = 0U; i < num_labels; i++) {
            uint8_t label = filter_labels[i];
            config->label_filter[label / 32U] |= (1U << (label % 32U));
        }
    }

    config->is_enabled = true;
    return true;
}

/**
 * @brief 接收字处理 (中断服务程序调用)
 */
void ARINC429_ReceiveWord_ISR(uint8_t channel_id, ARINC429_Word_t word) {
    ARINC429_ChannelConfig_t *config;
    uint16_t next_head;
    uint8_t label;

    if ((channel_id >= 4U) || (g_arinc429.state != ARINC429_STATE_RUNNING)) {
        return;
    }

    config = &g_arinc429.rx_channels[channel_id];
    if (!config->is_enabled) {
        return;
    }

    /* 校验奇偶校验 */
    if (!ARINC429_ValidateParity(word)) {
        g_arinc429.parity_errors++;
        return;
    }

    /* 提取标签进行过滤 */
    label = (uint8_t)((word & ARINC429_MASK_LABEL) >> ARINC429_SHIFT_LABEL);
    label = reverse_bits(label);

    if (config->label_filter[label / 32U] & (1U << (label % 32U))) {
        /* 标签在过滤列表中，存入缓冲区 */
        next_head = (g_arinc429.rx_head + 1U) % ARINC429_RX_BUFFER_SIZE;

        if (next_head != g_arinc429.rx_tail) {
            g_arinc429.rx_buffer[g_arinc429.rx_head].word = word;
            g_arinc429.rx_buffer[g_arinc429.rx_head].channel = channel_id;
            g_arinc429.rx_head = next_head;
            g_arinc429.rx_count++;
        }
    }
}

/**
 * @brief 发送字
 */
bool ARINC429_TransmitWord(uint8_t channel_id, ARINC429_Word_t word, uint8_t priority) {
    uint16_t next_head;

    if ((channel_id >= 4U) || (!g_arinc429.tx_channels[channel_id].is_enabled)) {
        return false;
    }

    next_head = (g_arinc429.tx_head + 1U) % ARINC429_TX_BUFFER_SIZE;

    if (next_head == g_arinc429.tx_tail) {
        return false;  /* 缓冲区满 */
    }

    g_arinc429.tx_buffer[g_arinc429.tx_head].word = word;
    g_arinc429.tx_buffer[g_arinc429.tx_head].priority = priority;
    g_arinc429.tx_head = next_head;

    return true;
}

/**
 * @brief 主处理循环
 */
void ARINC429_Process(void) {
    /* 处理发送队列 */
    while (g_arinc429.tx_tail != g_arinc429.tx_head) {
        ARINC429_TxMessage_t *msg = &g_arinc429.tx_buffer[g_arinc429.tx_tail];

        /* 实际硬件发送 */
        /* Hardware_Transmit(msg->word); */

        g_arinc429.tx_tail = (g_arinc429.tx_tail + 1U) % ARINC429_TX_BUFFER_SIZE;
        g_arinc429.tx_count++;
    }
}

/**
 * @brief 读取接收到的字
 */
bool ARINC429_ReadWord(ARINC429_Word_t *word, uint8_t *channel) {
    if (g_arinc429.rx_tail == g_arinc429.rx_head) {
        return false;  /* 无数据 */
    }

    if (NULL != word) {
        *word = g_arinc429.rx_buffer[g_arinc429.rx_tail].word;
    }

    if (NULL != channel) {
        *channel = g_arinc429.rx_buffer[g_arinc429.rx_tail].channel;
    }

    g_arinc429.rx_tail = (g_arinc429.rx_tail + 1U) % ARINC429_RX_BUFFER_SIZE;
    return true;
}

/**
 * @brief 获取统计信息
 */
void ARINC429_GetStatistics(uint32_t *rx_count, uint32_t *tx_count,
                             uint32_t *error_count) {
    if (NULL != rx_count) {
        *rx_count = g_arinc429.rx_count;
    }
    if (NULL != tx_count) {
        *tx_count = g_arinc429.tx_count;
    }
    if (NULL != error_count) {
        *error_count = g_arinc429.error_count;
    }
}
