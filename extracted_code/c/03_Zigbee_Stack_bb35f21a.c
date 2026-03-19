/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 300
 * Language: c
 * Block ID: bb35f21a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// MAC帧控制字段
typedef union {
    uint16_t value;
    struct {
        uint16_t frame_type : 3;      // 帧类型
        uint16_t security_en : 1;      // 安全使能
        uint16_t frame_pending : 1;    // 帧待传
        uint16_t ar : 1;               // 确认请求
        uint16_t pan_id_compress : 1;  // PAN ID压缩
        uint16_t reserved : 3;
        uint16_t dest_addr_mode : 2;   // 目的地址模式
        uint16_t frame_version : 2;    // 帧版本
        uint16_t src_addr_mode : 2;    // 源地址模式
    } bits;
} mac_frame_ctrl_t;

// MAC帧头
typedef struct {
    mac_frame_ctrl_t frame_ctrl;
    uint8_t seq_num;
    uint16_t dest_pan_id;
    union {
        uint16_t short_addr;
        uint64_t ext_addr;
    } dest_addr;
    uint16_t src_pan_id;
    union {
        uint16_t short_addr;
        uint64_t ext_addr;
    } src_addr;
} mac_header_t;

// MAC帧
typedef struct {
    mac_header_t header;
    uint8_t payload[118];
    uint8_t payload_len;
    uint16_t fcs;
} mac_frame_t;

// MAC PIB属性
typedef struct {
    uint64_t ext_addr;          // 64位扩展地址
    uint16_t short_addr;        // 16位短地址
    uint16_t pan_id;            // PAN ID
    uint8_t channel;            // 当前信道
    uint8_t beacon_order;       // 信标阶
    uint8_t superframe_order;   // 超帧阶
    uint8_t rx_on_when_idle;    // 空闲时接收使能
    uint8_t ack_wait_duration;  // 确认等待时间
    uint8_t max_csma_backoffs;  // 最大CSMA退避次数
    uint8_t min_be;             // 最小退避指数
    uint8_t max_be;             // 最大退避指数
} mac_pib_t;
