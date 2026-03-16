# 数据链路层协议实现指南

## 目录

- [数据链路层协议实现指南](#数据链路层协议实现指南)
  - [目录](#目录)
  - [1. 数据链路层概述](#1-数据链路层概述)
    - [1.1 数据链路层功能](#11-数据链路层功能)
    - [1.2 数据链路层架构](#12-数据链路层架构)
  - [2. HDLC协议实现](#2-hdlc协议实现)
    - [2.1 HDLC帧结构](#21-hdlc帧结构)
    - [2.2 HDLC完整实现](#22-hdlc完整实现)
    - [2.3 编译与运行](#23-编译与运行)
  - [3. PPP协议实现](#3-ppp协议实现)
    - [3.1 PPP帧结构](#31-ppp帧结构)
    - [3.2 PPP完整实现](#32-ppp完整实现)
    - [3.3 编译与运行](#33-编译与运行)
  - [4. 帧中继协议](#4-帧中继协议)
    - [4.1 帧中继帧结构](#41-帧中继帧结构)
    - [4.2 帧中继完整实现](#42-帧中继完整实现)
    - [4.3 编译与运行](#43-编译与运行)
  - [5. 以太网MAC层](#5-以太网mac层)
    - [5.1 以太网帧结构](#51-以太网帧结构)
    - [5.2 MAC地址处理实现](#52-mac地址处理实现)
    - [5.3 编译与运行](#53-编译与运行)
  - [6. VLAN实现](#6-vlan实现)
    - [6.1 VLAN标签格式 (802.1Q)](#61-vlan标签格式-8021q)
    - [6.2 VLAN完整实现](#62-vlan完整实现)
    - [6.3 编译与运行](#63-编译与运行)
  - [7. STP生成树协议](#7-stp生成树协议)
    - [7.1 STP BPDU格式](#71-stp-bpdu格式)
    - [7.2 STP完整实现](#72-stp完整实现)
    - [7.3 编译与运行](#73-编译与运行)
  - [8. ARP协议实现](#8-arp协议实现)
    - [8.1 ARP包格式](#81-arp包格式)
    - [8.2 ARP完整实现](#82-arp完整实现)
    - [8.3 编译与运行](#83-编译与运行)
  - [9. CRC校验算法](#9-crc校验算法)
    - [9.1 CRC算法实现](#91-crc算法实现)
    - [9.3 编译与运行](#93-编译与运行)
  - [10. 流量控制实现](#10-流量控制实现)
    - [10.1 滑动窗口协议](#101-滑动窗口协议)
    - [10.3 编译与运行](#103-编译与运行)

---

## 1. 数据链路层概述

### 1.1 数据链路层功能

数据链路层是OSI模型的第二层，主要功能包括：

- **帧封装与解封装**: 将网络层数据包封装成帧
- **物理寻址**: MAC地址识别与处理
- **差错检测**: CRC校验、奇偶校验等
- **流量控制**: 防止发送方淹没接收方
- **介质访问控制**: 控制对物理介质的访问

### 1.2 数据链路层架构

```
┌─────────────────────────────────────┐
│         网络层 (Network Layer)       │
├─────────────────────────────────────┤
│  LLC子层 │ 数据链路层 │ MAC子层      │
├─────────────────────────────────────┤
│         物理层 (Physical Layer)      │
└─────────────────────────────────────┘
```

---

## 2. HDLC协议实现

### 2.1 HDLC帧结构

```
┌─────┬───────┬──────────┬──────────┬───────┬───────┐
│ Flag│Address│ Control  │  Data    │  FCS  │ Flag  │
│ 0x7E│ 8bit  │  8/16bit │Variable  │16/32bit│ 0x7E │
└─────┴───────┴──────────┴──────────┴───────┴───────┘
```

### 2.2 HDLC完整实现

```c
/**
 * @file hdlc.c
 * @brief HDLC (High-Level Data Link Control) Protocol Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * HDLC Constants and Type Definitions
 * ============================================================================ */

#define HDLC_FLAG           0x7E    /**< Frame delimiter */
#define HDLC_ESCAPE         0x7D    /**< Escape character */
#define HDLC_ESCAPE_MASK    0x20    /**< XOR mask for escaped bytes */
#define HDLC_ADDRESS_BROADCAST 0xFF /**< Broadcast address */
#define HDLC_MAX_FRAME_SIZE 4096    /**< Maximum frame size */
#define HDLC_FCS_SIZE       2       /**< Frame Check Sequence size (CRC16) */

/**
 * @brief HDLC frame types
 */
typedef enum {
    HDLC_FRAME_I    = 0x00,  /**< Information frame */
    HDLC_FRAME_S    = 0x01,  /**< Supervisory frame */
    HDLC_FRAME_U    = 0x03   /**< Unnumbered frame */
} hdlc_frame_type_t;

/**
 * @brief HDLC supervisory frame types
 */
typedef enum {
    HDLC_S_RR   = 0x00,  /**< Receive Ready */
    HDLC_S_RNR  = 0x01,  /**< Receive Not Ready */
    HDLC_S_REJ  = 0x02,  /**< Reject */
    HDLC_S_SREJ = 0x03   /**< Selective Reject */
} hdlc_s_type_t;

/**
 * @brief HDLC unnumbered frame types
 */
typedef enum {
    HDLC_U_SNRM  = 0x00,  /**< Set Normal Response Mode */
    HDLC_U_SABM  = 0x1C,  /**< Set Asynchronous Balanced Mode */
    HDLC_U_SARM  = 0x08,  /**< Set Asynchronous Response Mode */
    HDLC_U_DISC  = 0x04,  /**< Disconnect */
    HDLC_U_UA    = 0x18,  /**< Unnumbered Acknowledgment */
    HDLC_U_DM    = 0x0C,  /**< Disconnected Mode */
    HDLC_U_UI    = 0x00,  /**< Unnumbered Information */
    HDLC_U_FRMR  = 0x24   /**< Frame Reject */
} hdlc_u_type_t;

/**
 * @brief HDLC connection state
 */
typedef enum {
    HDLC_STATE_DISCONNECTED,
    HDLC_STATE_CONNECTING,
    HDLC_STATE_CONNECTED,
    HDLC_STATE_DISCONNECTING
} hdlc_state_t;

/**
 * @brief HDLC configuration structure
 */
typedef struct {
    uint8_t address;           /**< Station address */
    uint8_t mode;              /**< NRM/ARM/ABM mode */
    uint8_t window_size;       /**< Window size for sliding window */
    bool extended;             /**< Extended addressing (16-bit control) */
} hdlc_config_t;

/**
 * @brief HDLC connection context
 */
typedef struct {
    hdlc_config_t config;      /**< Configuration */
    hdlc_state_t state;        /**< Current state */
    uint8_t vs;                /**< Send sequence number */
    uint8_t vr;                /**< Receive sequence number */
    uint8_t va;                /**< Acknowledged sequence number */
    uint32_t tx_seq;           /**< Transmit sequence counter */
    uint32_t rx_seq;           /**< Receive sequence counter */
} hdlc_context_t;

/**
 * @brief HDLC frame structure
 */
typedef struct {
    uint8_t address;           /**< Destination address */
    uint16_t control;          /**< Control field */
    uint8_t *data;             /**< Data payload */
    size_t data_len;           /**< Data length */
    uint16_t fcs;              /**< Frame check sequence */
} hdlc_frame_t;

/* ============================================================================
 * CRC16 Implementation (ITU-T V.41)
 * ============================================================================ */

/**
 * @brief CRC16 lookup table for HDLC FCS calculation
 */
static const uint16_t crc16_table[256] = {
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

/**
 * @brief Calculate CRC16 for HDLC FCS
 * @param data Input data buffer
 * @param len Data length
 * @return Calculated CRC16 value
 */
uint16_t hdlc_crc16(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;  /* Initial value for HDLC */

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc16_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFF;  /* Final XOR for HDLC */
}

/* ============================================================================
 * HDLC Frame Processing
 * ============================================================================ */

/**
 * @brief Initialize HDLC context
 * @param ctx HDLC context to initialize
 * @param config Configuration parameters
 * @return true on success, false on failure
 */
bool hdlc_init(hdlc_context_t *ctx, const hdlc_config_t *config) {
    if (ctx == NULL || config == NULL) {
        return false;
    }

    memset(ctx, 0, sizeof(hdlc_context_t));
    memcpy(&ctx->config, config, sizeof(hdlc_config_t));
    ctx->state = HDLC_STATE_DISCONNECTED;
    ctx->vs = 0;
    ctx->vr = 0;
    ctx->va = 0;

    return true;
}

/**
 * @brief Build HDLC control field for I-frame
 * @param ns Send sequence number
 * @param nr Receive sequence number
 * @param pf Poll/Final bit
 * @return Control field value
 */
uint8_t hdlc_make_i_control(uint8_t ns, uint8_t nr, bool pf) {
    return ((ns & 0x07) << 1) | ((nr & 0x07) << 5) | (pf ? 0x10 : 0x00);
}

/**
 * @brief Build HDLC control field for S-frame
 * @param type Supervisory frame type
 * @param nr Receive sequence number
 * @param pf Poll/Final bit
 * @return Control field value
 */
uint8_t hdlc_make_s_control(hdlc_s_type_t type, uint8_t nr, bool pf) {
    return HDLC_FRAME_S | ((type & 0x03) << 2) | (pf ? 0x10 : 0x00) | ((nr & 0x07) << 5);
}

/**
 * @brief Build HDLC control field for U-frame
 * @param type Unnumbered frame type
 * @param pf Poll/Final bit
 * @return Control field value
 */
uint8_t hdlc_make_u_control(hdlc_u_type_t type, bool pf) {
    return HDLC_FRAME_U | (type & 0xEC) | (pf ? 0x10 : 0x00);
}

/**
 * @brief Parse HDLC control field
 * @param control Control field value
 * @param frame_type Output: frame type
 * @param ns Output: send sequence number (for I-frame)
 * @param nr Output: receive sequence number
 * @param pf Output: poll/final bit
 */
void hdlc_parse_control(uint8_t control, hdlc_frame_type_t *frame_type,
                        uint8_t *ns, uint8_t *nr, bool *pf) {
    *frame_type = (control & 0x01) ?
                  ((control & 0x02) ? HDLC_FRAME_U : HDLC_FRAME_S) :
                  HDLC_FRAME_I;

    *pf = (control & 0x10) ? true : false;

    switch (*frame_type) {
        case HDLC_FRAME_I:
            *ns = (control >> 1) & 0x07;
            *nr = (control >> 5) & 0x07;
            break;
        case HDLC_FRAME_S:
            *ns = 0;
            *nr = (control >> 5) & 0x07;
            break;
        case HDLC_FRAME_U:
            *ns = 0;
            *nr = 0;
            break;
    }
}

/**
 * @brief Encode data with HDLC byte stuffing
 * @param dst Destination buffer
 * @param dst_size Destination buffer size
 * @param src Source data
 * @param src_len Source data length
 * @return Encoded data length, or -1 on error
 */
int hdlc_stuff(uint8_t *dst, size_t dst_size, const uint8_t *src, size_t src_len) {
    size_t j = 0;

    for (size_t i = 0; i < src_len; i++) {
        if (j >= dst_size - 1) {
            return -1;  /* Buffer overflow */
        }

        if (src[i] == HDLC_FLAG || src[i] == HDLC_ESCAPE) {
            dst[j++] = HDLC_ESCAPE;
            dst[j++] = src[i] ^ HDLC_ESCAPE_MASK;
        } else {
            dst[j++] = src[i];
        }
    }

    return (int)j;
}

/**
 * @brief Decode HDLC byte stuffed data
 * @param dst Destination buffer
 * @param dst_size Destination buffer size
 * @param src Source data
 * @param src_len Source data length
 * @return Decoded data length, or -1 on error
 */
int hdlc_unstuff(uint8_t *dst, size_t dst_size, const uint8_t *src, size_t src_len) {
    size_t j = 0;
    bool escape = false;

    for (size_t i = 0; i < src_len; i++) {
        if (j >= dst_size) {
            return -1;  /* Buffer overflow */
        }

        if (escape) {
            dst[j++] = src[i] ^ HDLC_ESCAPE_MASK;
            escape = false;
        } else if (src[i] == HDLC_ESCAPE) {
            escape = true;
        } else {
            dst[j++] = src[i];
        }
    }

    return (int)j;
}

/**
 * @brief Build complete HDLC frame
 * @param ctx HDLC context
 * @param dst Destination buffer
 * @param dst_size Destination buffer size
 * @param data Payload data
 * @param data_len Payload length
 * @param frame_type Frame type
 * @return Frame length, or -1 on error
 */
int hdlc_build_frame(hdlc_context_t *ctx, uint8_t *dst, size_t dst_size,
                     const uint8_t *data, size_t data_len,
                     hdlc_frame_type_t frame_type) {
    if (dst == NULL || dst_size < 6 || (data_len > 0 && data == NULL)) {
        return -1;
    }

    if (data_len > HDLC_MAX_FRAME_SIZE) {
        return -1;  /* Data too long */
    }

    uint8_t temp_buf[HDLC_MAX_FRAME_SIZE + 16];
    size_t pos = 0;

    /* Build unescaped frame */
    temp_buf[pos++] = ctx->config.address;  /* Address field */

    /* Control field */
    uint8_t control;
    switch (frame_type) {
        case HDLC_FRAME_I:
            control = hdlc_make_i_control(ctx->vs, ctx->vr, true);
            ctx->vs = (ctx->vs + 1) & 0x07;
            break;
        case HDLC_FRAME_S:
            control = hdlc_make_s_control(HDLC_S_RR, ctx->vr, true);
            break;
        case HDLC_FRAME_U:
            control = hdlc_make_u_control(HDLC_U_UI, true);
            break;
        default:
            return -1;
    }
    temp_buf[pos++] = control;

    /* Data field (for I-frame) */
    if (frame_type == HDLC_FRAME_I && data_len > 0) {
        memcpy(&temp_buf[pos], data, data_len);
        pos += data_len;
    }

    /* Calculate and append FCS */
    uint16_t fcs = hdlc_crc16(temp_buf, pos);
    temp_buf[pos++] = fcs & 0xFF;
    temp_buf[pos++] = (fcs >> 8) & 0xFF;

    /* Build final frame with flags and stuffing */
    size_t dst_pos = 0;
    dst[dst_pos++] = HDLC_FLAG;  /* Opening flag */

    int stuffed_len = hdlc_stuff(&dst[dst_pos], dst_size - dst_pos - 1, temp_buf, pos);
    if (stuffed_len < 0) {
        return -1;
    }
    dst_pos += stuffed_len;

    dst[dst_pos++] = HDLC_FLAG;  /* Closing flag */

    return (int)dst_pos;
}

/**
 * @brief Parse HDLC frame
 * @param src Frame data (without flags)
 * @param src_len Frame length
 * @param frame Output frame structure
 * @return true on success, false on failure
 */
bool hdlc_parse_frame(const uint8_t *src, size_t src_len, hdlc_frame_t *frame) {
    if (src == NULL || src_len < 4 || frame == NULL) {
        return false;
    }

    /* Unstuff the frame */
    uint8_t unstuffed[HDLC_MAX_FRAME_SIZE];
    int unstuffed_len = hdlc_unstuff(unstuffed, sizeof(unstuffed), src, src_len);
    if (unstuffed_len < 4) {
        return false;
    }

    /* Verify FCS */
    uint16_t recv_fcs = unstuffed[unstuffed_len - 2] |
                        (unstuffed[unstuffed_len - 1] << 8);
    uint16_t calc_fcs = hdlc_crc16(unstuffed, unstuffed_len - 2);

    if (recv_fcs != calc_fcs) {
        printf("HDLC FCS error: received=0x%04X, calculated=0x%04X\n",
               recv_fcs, calc_fcs);
        return false;
    }

    /* Parse fields */
    frame->address = unstuffed[0];
    frame->control = unstuffed[1];

    hdlc_frame_type_t ft;
    uint8_t ns, nr;
    bool pf;
    hdlc_parse_control(frame->control, &ft, &ns, &nr, &pf);

    /* Extract data (if any) */
    if (unstuffed_len > 4) {
        frame->data_len = unstuffed_len - 4;  /* -2 (FCS) -1 (ctrl) -1 (addr) */
        frame->data = (uint8_t *)malloc(frame->data_len);
        if (frame->data == NULL) {
            return false;
        }
        memcpy(frame->data, &unstuffed[2], frame->data_len);
    } else {
        frame->data = NULL;
        frame->data_len = 0;
    }

    return true;
}

/**
 * @brief Free HDLC frame resources
 * @param frame Frame to free
 */
void hdlc_free_frame(hdlc_frame_t *frame) {
    if (frame && frame->data) {
        free(frame->data);
        frame->data = NULL;
        frame->data_len = 0;
    }
}

/**
 * @brief Handle connection establishment
 * @param ctx HDLC context
 * @return true on success, false on failure
 */
bool hdlc_connect(hdlc_context_t *ctx) {
    if (ctx == NULL) {
        return false;
    }

    ctx->state = HDLC_STATE_CONNECTING;

    /* Send SABM frame */
    uint8_t frame_buf[HDLC_MAX_FRAME_SIZE];
    uint8_t control = hdlc_make_u_control(HDLC_U_SABM, true);

    /* Build and send SABM frame */
    (void)control;  /* Used in real implementation */

    printf("HDLC: Sending SABM frame, entering connecting state\n");

    /* Wait for UA response (simplified) */
    ctx->state = HDLC_STATE_CONNECTED;
    ctx->vs = 0;
    ctx->vr = 0;
    ctx->va = 0;

    printf("HDLC: Connection established\n");
    return true;
}

/**
 * @brief Handle disconnection
 * @param ctx HDLC context
 */
void hdlc_disconnect(hdlc_context_t *ctx) {
    if (ctx == NULL) {
        return;
    }

    ctx->state = HDLC_STATE_DISCONNECTING;

    /* Send DISC frame */
    printf("HDLC: Sending DISC frame\n");

    ctx->state = HDLC_STATE_DISCONNECTED;
    printf("HDLC: Disconnected\n");
}

/* ============================================================================
 * HDLC Test/Demo
 * ============================================================================ */

/**
 * @brief HDLC protocol test
 */
void hdlc_test(void) {
    printf("\n=== HDLC Protocol Test ===\n");

    /* Initialize context */
    hdlc_config_t config = {
        .address = 0x03,
        .mode = HDLC_U_SABM,
        .window_size = 7,
        .extended = false
    };

    hdlc_context_t ctx;
    if (!hdlc_init(&ctx, &config)) {
        printf("Failed to initialize HDLC context\n");
        return;
    }

    /* Test CRC16 */
    const char *test_data = "Hello HDLC";
    uint16_t crc = hdlc_crc16((const uint8_t *)test_data, strlen(test_data));
    printf("CRC16 of '%s': 0x%04X\n", test_data, crc);

    /* Test byte stuffing */
    uint8_t stuff_src[] = {0x7E, 0x11, 0x7D, 0x13};
    uint8_t stuff_dst[16];
    int stuffed = hdlc_stuff(stuff_dst, sizeof(stuff_dst), stuff_src, sizeof(stuff_src));
    printf("Byte stuffing: %zu bytes -> %d bytes\n", sizeof(stuff_src), stuffed);

    /* Test frame building */
    uint8_t payload[] = "Test HDLC Frame";
    uint8_t frame_buf[HDLC_MAX_FRAME_SIZE];

    hdlc_connect(&ctx);

    int frame_len = hdlc_build_frame(&ctx, frame_buf, sizeof(frame_buf),
                                     payload, sizeof(payload),
                                     HDLC_FRAME_I);
    if (frame_len > 0) {
        printf("Built I-frame: %d bytes\n", frame_len);
        printf("Frame hex: ");
        for (int i = 0; i < frame_len && i < 32; i++) {
            printf("%02X ", frame_buf[i]);
        }
        printf("...\n");
    }

    /* Test frame parsing */
    /* Skip flags for parsing (frame_len - 2, skip first and last flag) */
    hdlc_frame_t rx_frame;
    if (hdlc_parse_frame(&frame_buf[1], frame_len - 2, &rx_frame)) {
        printf("Parsed frame: addr=0x%02X, control=0x%02X, data_len=%zu\n",
               rx_frame.address, rx_frame.control, rx_frame.data_len);
        if (rx_frame.data_len > 0) {
            printf("Data: ");
            for (size_t i = 0; i < rx_frame.data_len && i < 20; i++) {
                printf("%c", rx_frame.data[i]);
            }
            printf("\n");
        }
        hdlc_free_frame(&rx_frame);
    }

    hdlc_disconnect(&ctx);
    printf("HDLC test completed\n");
}

/* Main entry point for testing */
int main(void) {
    hdlc_test();
    return 0;
}
```

### 2.3 编译与运行

```bash
gcc -o hdlc_test hdlc.c -Wall -Wextra
./hdlc_test
```

---

## 3. PPP协议实现

### 3.1 PPP帧结构

```
┌─────┬───────┬──────────┬──────────┬───────┬─────┐
│Flag │Address│ Control  │ Protocol │ Data  │ FCS │
│0x7E │ 0xFF  │  0x03    │ 2 bytes  │Variable│2/4  │
└─────┴───────┴──────────┴──────────┴───────┴─────┘
```

### 3.2 PPP完整实现

```c
/**
 * @file ppp.c
 * @brief PPP (Point-to-Point Protocol) Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * PPP Constants and Type Definitions
 * ============================================================================ */

#define PPP_FLAG        0x7E    /**< Frame delimiter */
#define PPP_ESCAPE      0x7D    /**< Escape character */
#define PPP_ADDRESS     0xFF    /**< All-stations address */
#define PPP_CONTROL     0x03    /**< Unnumbered information */
#define PPP_MAX_MTU     1500    /**< Maximum transmission unit */
#define PPP_MAX_FRAME   (PPP_MAX_MTU + 32)  /**< Maximum frame size */

/**
 * @brief PPP protocol types
 */
typedef enum {
    PPP_PROTO_IP        = 0x0021,   /**< IPv4 */
    PPP_PROTO_IPCP      = 0x8021,   /**< IP Control Protocol */
    PPP_PROTO_LCP       = 0xC021,   /**< Link Control Protocol */
    PPP_PROTO_PAP       = 0xC023,   /**< Password Authentication Protocol */
    PPP_PROTO_CHAP      = 0xC223,   /**< Challenge Handshake Authentication */
    PPP_PROTO_IPV6      = 0x0057,   /**< IPv6 */
    PPP_PROTO_IPv6CP    = 0x8057    /**< IPv6 Control Protocol */
} ppp_protocol_t;

/**
 * @brief LCP packet types
 */
typedef enum {
    LCP_CONF_REQUEST  = 1,   /**< Configure-Request */
    LCP_CONF_ACK      = 2,   /**< Configure-Ack */
    LCP_CONF_NAK      = 3,   /**< Configure-Nak */
    LCP_CONF_REJECT   = 4,   /**< Configure-Reject */
    LCP_TERM_REQUEST  = 5,   /**< Terminate-Request */
    LCP_TERM_ACK      = 6,   /**< Terminate-Ack */
    LCP_CODE_REJECT   = 7,   /**< Code-Reject */
    LCP_PROTO_REJECT  = 8,   /**< Protocol-Reject */
    LCP_ECHO_REQUEST  = 9,   /**< Echo-Request */
    LCP_ECHO_REPLY    = 10,  /**< Echo-Reply */
    LCP_DISCARD_REQ   = 11   /**< Discard-Request */
} lcp_code_t;

/**
 * @brief LCP configuration options
 */
typedef enum {
    LCP_OPT_MRU           = 1,   /**< Maximum Receive Unit */
    LCP_OPT_ACCM          = 2,   /**< Async Control Character Map */
    LCP_OPT_MAGIC         = 5,   /**< Magic Number */
    LCP_OPT_PFC           = 7,   /**< Protocol Field Compression */
    LCP_OPT_ACFC          = 8,   /**< Address/Control Field Compression */
} lcp_option_t;

/**
 * @brief PPP connection states
 */
typedef enum {
    PPP_STATE_DEAD,
    PPP_STATE_ESTABLISH,
    PPP_STATE_AUTHENTICATE,
    PPP_STATE_NETWORK,
    PPP_STATE_TERMINATE,
    PPP_STATE_OPENED
} ppp_state_t;

/**
 * @brief PPP authentication types
 */
typedef enum {
    PPP_AUTH_NONE,
    PPP_AUTH_PAP,
    PPP_AUTH_CHAP
} ppp_auth_t;

/**
 * @brief PPP configuration structure
 */
typedef struct {
    uint16_t mru;              /**< Maximum Receive Unit */
    uint32_t accm;             /**< Async Control Character Map */
    uint32_t magic_number;     /**< Magic number for loopback detection */
    bool pfc;                  /**< Protocol Field Compression enabled */
    bool acfc;                 /**< Address/Control Field Compression */
    ppp_auth_t auth;           /**< Authentication method */
    char username[64];         /**< Username for PAP/CHAP */
    char password[64];         /**< Password for PAP/CHAP */
} ppp_config_t;

/**
 * @brief PPP connection context
 */
typedef struct {
    ppp_state_t state;         /**< Current state */
    ppp_config_t local_cfg;    /**< Local configuration */
    ppp_config_t peer_cfg;     /**< Peer configuration */
    uint8_t identifier;        /**< Packet identifier */
    uint32_t tx_bytes;         /**< Transmitted bytes */
    uint32_t rx_bytes;         /**< Received bytes */
    uint32_t tx_packets;       /**< Transmitted packets */
    uint32_t rx_packets;       /**< Received packets */
} ppp_context_t;

/**
 * @brief PPP frame structure
 */
typedef struct {
    uint8_t address;           /**< Address field */
    uint8_t control;           /**< Control field */
    uint16_t protocol;         /**< Protocol field */
    uint8_t *data;             /**< Data payload */
    size_t data_len;           /**< Data length */
} ppp_frame_t;

/* ============================================================================
 * PPP FCS Calculation (CRC16-CCITT)
 * ============================================================================ */

/**
 * @brief CRC16 lookup table for PPP FCS
 */
static const uint16_t ppp_crc16_table[256] = {
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

/**
 * @brief Calculate PPP FCS (CRC16)
 * @param data Input data
 * @param len Data length
 * @return Calculated FCS
 */
uint16_t ppp_calc_fcs(const uint8_t *data, size_t len) {
    uint16_t fcs = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        fcs = (fcs >> 8) ^ ppp_crc16_table[(fcs ^ data[i]) & 0xFF];
    }

    return fcs ^ 0xFFFF;
}

/* ============================================================================
 * PPP Byte Stuffing
 * ============================================================================ */

/**
 * @brief Encode data with PPP byte stuffing
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @param src Source data
 * @param src_len Source length
 * @return Encoded length, or -1 on error
 */
int ppp_stuff(uint8_t *dst, size_t dst_size, const uint8_t *src, size_t src_len) {
    size_t j = 0;

    for (size_t i = 0; i < src_len; i++) {
        if (j >= dst_size - 1) {
            return -1;
        }

        if (src[i] == PPP_FLAG || src[i] == PPP_ESCAPE) {
            dst[j++] = PPP_ESCAPE;
            dst[j++] = src[i] ^ 0x20;
        } else if (src[i] < 0x20) {
            /* Check ACCM for control characters */
            dst[j++] = PPP_ESCAPE;
            dst[j++] = src[i] ^ 0x20;
        } else {
            dst[j++] = src[i];
        }
    }

    return (int)j;
}

/**
 * @brief Decode PPP stuffed data
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @param src Source data
 * @param src_len Source length
 * @return Decoded length, or -1 on error
 */
int ppp_unstuff(uint8_t *dst, size_t dst_size, const uint8_t *src, size_t src_len) {
    size_t j = 0;
    bool escape = false;

    for (size_t i = 0; i < src_len; i++) {
        if (j >= dst_size) {
            return -1;
        }

        if (escape) {
            dst[j++] = src[i] ^ 0x20;
            escape = false;
        } else if (src[i] == PPP_ESCAPE) {
            escape = true;
        } else {
            dst[j++] = src[i];
        }
    }

    return (int)j;
}

/* ============================================================================
 * PPP Frame Operations
 * ============================================================================ */

/**
 * @brief Initialize PPP context with default configuration
 * @param ctx PPP context
 * @return true on success, false on failure
 */
bool ppp_init(ppp_context_t *ctx) {
    if (ctx == NULL) {
        return false;
    }

    memset(ctx, 0, sizeof(ppp_context_t));
    ctx->state = PPP_STATE_DEAD;

    /* Default configuration */
    ctx->local_cfg.mru = 1500;
    ctx->local_cfg.accm = 0xFFFFFFFF;
    ctx->local_cfg.magic_number = (uint32_t)rand();
    ctx->local_cfg.pfc = false;
    ctx->local_cfg.acfc = false;
    ctx->local_cfg.auth = PPP_AUTH_NONE;

    ctx->identifier = 1;

    return true;
}

/**
 * @brief Build PPP frame
 * @param ctx PPP context
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @param protocol PPP protocol
 * @param data Payload data
 * @param data_len Payload length
 * @return Frame length, or -1 on error
 */
int ppp_build_frame(ppp_context_t *ctx, uint8_t *dst, size_t dst_size,
                    uint16_t protocol, const uint8_t *data, size_t data_len) {
    if (dst == NULL || dst_size < 8 || data_len > PPP_MAX_MTU) {
        return -1;
    }

    uint8_t temp_buf[PPP_MAX_FRAME];
    size_t pos = 0;

    /* Address and Control fields (may be compressed) */
    if (!ctx->local_cfg.acfc || protocol == PPP_PROTO_LCP) {
        temp_buf[pos++] = PPP_ADDRESS;
        temp_buf[pos++] = PPP_CONTROL;
    }

    /* Protocol field (may be compressed for 1 byte if < 0xFF) */
    if (ctx->local_cfg.pfc && (protocol & 0xFF00) == 0) {
        temp_buf[pos++] = protocol & 0xFF;
    } else {
        temp_buf[pos++] = (protocol >> 8) & 0xFF;
        temp_buf[pos++] = protocol & 0xFF;
    }

    /* Data payload */
    if (data_len > 0 && data != NULL) {
        memcpy(&temp_buf[pos], data, data_len);
        pos += data_len;
    }

    /* Calculate FCS */
    uint16_t fcs = ppp_calc_fcs(temp_buf, pos);
    temp_buf[pos++] = fcs & 0xFF;
    temp_buf[pos++] = (fcs >> 8) & 0xFF;

    /* Build final frame with flag and stuffing */
    size_t dst_pos = 0;
    dst[dst_pos++] = PPP_FLAG;

    int stuffed = ppp_stuff(&dst[dst_pos], dst_size - dst_pos - 1, temp_buf, pos);
    if (stuffed < 0) {
        return -1;
    }
    dst_pos += stuffed;

    dst[dst_pos++] = PPP_FLAG;

    ctx->tx_bytes += data_len;
    ctx->tx_packets++;

    return (int)dst_pos;
}

/**
 * @brief Parse PPP frame
 * @param ctx PPP context
 * @param src Frame data
 * @param src_len Frame length
 * @param frame Output frame structure
 * @return true on success, false on failure
 */
bool ppp_parse_frame(ppp_context_t *ctx, const uint8_t *src, size_t src_len,
                     ppp_frame_t *frame) {
    if (src == NULL || src_len < 4 || frame == NULL) {
        return false;
    }

    /* Find frame boundaries (skip flags) */
    size_t start = 0;
    size_t end = src_len;

    if (src[0] == PPP_FLAG) start++;
    if (src[src_len - 1] == PPP_FLAG) end--;

    if (end <= start) {
        return false;
    }

    /* Unstuff the frame */
    uint8_t unstuffed[PPP_MAX_FRAME];
    int unstuffed_len = ppp_unstuff(unstuffed, sizeof(unstuffed),
                                    &src[start], end - start);
    if (unstuffed_len < 4) {
        return false;
    }

    /* Verify FCS */
    uint16_t recv_fcs = unstuffed[unstuffed_len - 2] |
                        (unstuffed[unstuffed_len - 1] << 8);
    uint16_t calc_fcs = ppp_calc_fcs(unstuffed, unstuffed_len - 2);

    if (recv_fcs != calc_fcs) {
        printf("PPP FCS error: received=0x%04X, calculated=0x%04X\n",
               recv_fcs, calc_fcs);
        return false;
    }

    /* Parse fields */
    size_t pos = 0;

    /* Address and Control (check for compression) */
    if (unstuffed[0] == PPP_ADDRESS) {
        frame->address = unstuffed[pos++];
        frame->control = unstuffed[pos++];
    } else {
        frame->address = PPP_ADDRESS;
        frame->control = PPP_CONTROL;
    }

    /* Protocol (check for compression) */
    if ((unstuffed[pos] & 0x01) == 0x01) {
        /* Compressed protocol (1 byte) */
        frame->protocol = unstuffed[pos++];
    } else {
        frame->protocol = (unstuffed[pos] << 8) | unstuffed[pos + 1];
        pos += 2;
    }

    /* Extract data (excluding FCS) */
    if (pos < (size_t)(unstuffed_len - 2)) {
        frame->data_len = unstuffed_len - 2 - pos;
        frame->data = (uint8_t *)malloc(frame->data_len);
        if (frame->data == NULL) {
            return false;
        }
        memcpy(frame->data, &unstuffed[pos], frame->data_len);
    } else {
        frame->data = NULL;
        frame->data_len = 0;
    }

    ctx->rx_bytes += frame->data_len;
    ctx->rx_packets++;

    return true;
}

/**
 * @brief Free PPP frame resources
 * @param frame Frame to free
 */
void ppp_free_frame(ppp_frame_t *frame) {
    if (frame && frame->data) {
        free(frame->data);
        frame->data = NULL;
        frame->data_len = 0;
    }
}

/* ============================================================================
 * LCP (Link Control Protocol) Operations
 * ============================================================================ */

/**
 * @brief Build LCP Configure-Request packet
 * @param ctx PPP context
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @return Packet length, or -1 on error
 */
int lcp_build_conf_request(ppp_context_t *ctx, uint8_t *dst, size_t dst_size) {
    if (dst == NULL || dst_size < 32) {
        return -1;
    }

    size_t pos = 0;

    /* LCP header */
    dst[pos++] = LCP_CONF_REQUEST;   /* Code */
    dst[pos++] = ctx->identifier++;  /* Identifier */
    uint16_t *length_field = (uint16_t *)&dst[pos];
    pos += 2;  /* Length (filled later) */

    /* MRU option */
    dst[pos++] = LCP_OPT_MRU;
    dst[pos++] = 4;  /* Length */
    dst[pos++] = (ctx->local_cfg.mru >> 8) & 0xFF;
    dst[pos++] = ctx->local_cfg.mru & 0xFF;

    /* Magic Number option */
    dst[pos++] = LCP_OPT_MAGIC;
    dst[pos++] = 6;  /* Length */
    dst[pos++] = (ctx->local_cfg.magic_number >> 24) & 0xFF;
    dst[pos++] = (ctx->local_cfg.magic_number >> 16) & 0xFF;
    dst[pos++] = (ctx->local_cfg.magic_number >> 8) & 0xFF;
    dst[pos++] = ctx->local_cfg.magic_number & 0xFF;

    /* Update length field */
    *length_field = htons(pos);

    return (int)pos;
}

/**
 * @brief Process received LCP packet
 * @param ctx PPP context
 * @param data LCP packet data
 * @param len Packet length
 */
void lcp_process(ppp_context_t *ctx, const uint8_t *data, size_t len) {
    if (len < 4) return;

    uint8_t code = data[0];
    uint8_t id = data[1];
    uint16_t pkt_len = (data[2] << 8) | data[3];

    (void)id;
    (void)pkt_len;

    switch (code) {
        case LCP_CONF_REQUEST:
            printf("LCP: Received Configure-Request\n");
            /* Send Configure-Ack */
            break;
        case LCP_CONF_ACK:
            printf("LCP: Received Configure-Ack\n");
            ctx->state = PPP_STATE_AUTHENTICATE;
            break;
        case LCP_CONF_NAK:
            printf("LCP: Received Configure-Nak\n");
            break;
        case LCP_TERM_REQUEST:
            printf("LCP: Received Terminate-Request\n");
            break;
        case LCP_TERM_ACK:
            printf("LCP: Received Terminate-Ack\n");
            break;
        case LCP_ECHO_REQUEST:
            printf("LCP: Received Echo-Request\n");
            break;
        default:
            printf("LCP: Unknown code %d\n", code);
    }
}

/* ============================================================================
 * PPP Connection Management
 * ============================================================================ */

/**
 * @brief Start PPP connection establishment
 * @param ctx PPP context
 */
void ppp_connect(ppp_context_t *ctx) {
    if (ctx == NULL) return;

    printf("PPP: Starting connection establishment\n");
    ctx->state = PPP_STATE_ESTABLISH;

    /* Send LCP Configure-Request */
    uint8_t lcp_pkt[128];
    int lcp_len = lcp_build_conf_request(ctx, lcp_pkt, sizeof(lcp_pkt));

    if (lcp_len > 0) {
        printf("PPP: Sending LCP Configure-Request (%d bytes)\n", lcp_len);
        /* In real implementation, send the packet */
    }
}

/**
 * @brief Disconnect PPP connection
 * @param ctx PPP context
 */
void ppp_disconnect(ppp_context_t *ctx) {
    if (ctx == NULL) return;

    printf("PPP: Disconnecting\n");
    ctx->state = PPP_STATE_TERMINATE;

    /* Send LCP Terminate-Request */

    ctx->state = PPP_STATE_DEAD;
    printf("PPP: Disconnected\n");
}

/**
 * @brief Get PPP state string
 * @param state PPP state
 * @return State name string
 */
const char* ppp_state_string(ppp_state_t state) {
    switch (state) {
        case PPP_STATE_DEAD:          return "DEAD";
        case PPP_STATE_ESTABLISH:     return "ESTABLISH";
        case PPP_STATE_AUTHENTICATE:  return "AUTHENTICATE";
        case PPP_STATE_NETWORK:       return "NETWORK";
        case PPP_STATE_TERMINATE:     return "TERMINATE";
        case PPP_STATE_OPENED:        return "OPENED";
        default:                      return "UNKNOWN";
    }
}

/* ============================================================================
 * PPP Test/Demo
 * ============================================================================ */

/**
 * @brief PPP protocol test
 */
void ppp_test(void) {
    printf("\n=== PPP Protocol Test ===\n");

    /* Initialize PPP context */
    ppp_context_t ctx;
    if (!ppp_init(&ctx)) {
        printf("Failed to initialize PPP context\n");
        return;
    }

    printf("PPP state: %s\n", ppp_state_string(ctx.state));
    printf("Local MRU: %d\n", ctx.local_cfg.mru);
    printf("Magic number: 0x%08X\n", ctx.local_cfg.magic_number);

    /* Test FCS calculation */
    const char *test_data = "Hello PPP";
    uint16_t fcs = ppp_calc_fcs((const uint8_t *)test_data, strlen(test_data));
    printf("FCS of '%s': 0x%04X\n", test_data, fcs);

    /* Test frame building */
    uint8_t payload[] = "Test PPP Data";
    uint8_t frame_buf[PPP_MAX_FRAME];

    ppp_connect(&ctx);

    int frame_len = ppp_build_frame(&ctx, frame_buf, sizeof(frame_buf),
                                    PPP_PROTO_IP, payload, sizeof(payload));
    if (frame_len > 0) {
        printf("Built PPP frame: %d bytes\n", frame_len);
        printf("Frame hex dump:\n");
        for (int i = 0; i < frame_len; i++) {
            printf("%02X ", frame_buf[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }

    /* Test frame parsing */
    ppp_frame_t rx_frame;
    if (ppp_parse_frame(&ctx, frame_buf, frame_len, &rx_frame)) {
        printf("Parsed PPP frame:\n");
        printf("  Address: 0x%02X\n", rx_frame.address);
        printf("  Control: 0x%02X\n", rx_frame.control);
        printf("  Protocol: 0x%04X\n", rx_frame.protocol);
        printf("  Data length: %zu\n", rx_frame.data_len);
        if (rx_frame.data_len > 0) {
            printf("  Data: ");
            for (size_t i = 0; i < rx_frame.data_len; i++) {
                printf("%c", rx_frame.data[i]);
            }
            printf("\n");
        }
        ppp_free_frame(&rx_frame);
    }

    /* Test LCP processing */
    uint8_t lcp_conf_req[32];
    int lcp_len = lcp_build_conf_request(&ctx, lcp_conf_req, sizeof(lcp_conf_req));
    if (lcp_len > 0) {
        printf("\nLCP Configure-Request (%d bytes):\n", lcp_len);
        for (int i = 0; i < lcp_len; i++) {
            printf("%02X ", lcp_conf_req[i]);
        }
        printf("\n");
    }

    ppp_disconnect(&ctx);
    printf("\nPPP statistics:\n");
    printf("  TX packets: %u, bytes: %u\n", ctx.tx_packets, ctx.tx_bytes);
    printf("  RX packets: %u, bytes: %u\n", ctx.rx_packets, ctx.rx_bytes);

    printf("PPP test completed\n");
}

/* Main entry point */
int main(void) {
    ppp_test();
    return 0;
}
```

### 3.3 编译与运行

```bash
gcc -o ppp_test ppp.c -Wall -Wextra
./ppp_test
```

---

## 4. 帧中继协议

### 4.1 帧中继帧结构

```
┌─────────┬──────────┬───────────┬──────────┬───────┐
│  Flags  │  DLCI    │  Control  │  Data    │  FCS  │
│ 0x7E/7E │(10/16bit)│  2 bytes  │Variable  │ 2byte │
└─────────┴──────────┴───────────┴──────────┴───────┘
```

### 4.2 帧中继完整实现

```c
/**
 * @file framerelay.c
 * @brief Frame Relay Protocol Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Frame Relay Constants
 * ============================================================================ */

#define FR_FLAG         0x7E    /**< Frame delimiter */
#define FR_ESCAPE       0x7D    /**< Escape character */
#define FR_MAX_FRAME    4096    /**< Maximum frame size */
#define FR_HEADER_LEN   2       /**< Minimum header length */
#define FR_FCS_LEN      2       /**< FCS length */

/**
 * @brief Frame Relay LMI (Local Management Interface) types
 */
typedef enum {
    FR_LMI_NONE,       /**< No LMI */
    FR_LMI_ANSI,       /**< ANSI T1.617 Annex D */
    FR_LMI_CISCO,      /**< Cisco Gang of Four */
    FR_LMI_Q933A       /**< Q.933 Annex A */
} fr_lmi_type_t;

/**
 * @brief Frame Relay PVC status
 */
typedef enum {
    FR_PVC_INACTIVE,   /**< PVC not active */
    FR_PVC_ACTIVE,     /**< PVC active */
    FR_PVC_NEW         /**< PVC newly discovered */
} fr_pvc_status_t;

/**
 * @brief Frame Relay PVC entry
 */
typedef struct {
    uint16_t dlci;           /**< DLCI number */
    fr_pvc_status_t status;  /**< PVC status */
    uint32_t tx_frames;      /**< Transmitted frames */
    uint32_t rx_frames;      /**< Received frames */
    uint32_t tx_bytes;       /**< Transmitted bytes */
    uint32_t rx_bytes;       /**< Received bytes */
    uint32_t last_update;    /**< Last status update time */
} fr_pvc_t;

/**
 * @brief Frame Relay context
 */
typedef struct {
    fr_lmi_type_t lmi_type;   /**< LMI type */
    uint8_t lmi_sequence;     /**< LMI sequence number */
    fr_pvc_t *pvc_table;      /**< PVC table */
    uint16_t num_pvcs;        /**< Number of PVCs */
    uint16_t max_pvcs;        /**< Maximum PVCs in table */
    bool lmi_enabled;         /**< LMI enabled flag */
} fr_context_t;

/**
 * @brief Frame Relay frame structure
 */
typedef struct {
    uint16_t dlci;            /**< Data Link Connection Identifier */
    uint8_t cr;               /**< Command/Response bit */
    uint8_t ea;               /**< Address Extension */
    uint8_t fecn;             /**< Forward Explicit Congestion Notification */
    uint8_t becn;             /**< Backward Explicit Congestion Notification */
    uint8_t de;               /**< Discard Eligibility */
    uint8_t *data;            /**< Payload data */
    size_t data_len;          /**< Data length */
} fr_frame_t;

/* ============================================================================
 * Frame Relay Address Parsing
 * ============================================================================ */

/**
 * @brief Parse 2-byte Frame Relay address
 * @param addr Address bytes
 * @param dlci Output DLCI
 * @param cr Output C/R bit
 * @param ea Output EA bit
 * @param fecn Output FECN bit
 * @param becn Output BECN bit
 * @param de Output DE bit
 * @return true if 2-byte address, false if extended
 */
bool fr_parse_address_2byte(const uint8_t *addr, uint16_t *dlci, uint8_t *cr,
                            uint8_t *ea, uint8_t *fecn, uint8_t *becn, uint8_t *de) {
    *dlci = ((addr[0] & 0xFC) << 2) | ((addr[1] & 0xF0) >> 4);
    *cr = (addr[0] >> 1) & 0x01;
    *ea = addr[1] & 0x01;
    *fecn = (addr[1] >> 3) & 0x01;
    *becn = (addr[1] >> 2) & 0x01;
    *de = (addr[1] >> 1) & 0x01;

    return (*ea == 1);
}

/**
 * @brief Parse 4-byte extended Frame Relay address
 * @param addr Address bytes
 * @param dlci Output DLCI
 * @param cr Output C/R bit
 * @param ea Output EA bit
 * @param fecn Output FECN bit
 * @param becn Output BECN bit
 * @param de Output DE bit
 * @return true on success
 */
bool fr_parse_address_4byte(const uint8_t *addr, uint16_t *dlci, uint8_t *cr,
                            uint8_t *ea, uint8_t *fecn, uint8_t *becn, uint8_t *de) {
    /* 2-byte format first */
    *dlci = ((addr[0] & 0xFC) << 2) | ((addr[1] & 0xF0) >> 4);
    *cr = (addr[0] >> 1) & 0x01;

    /* Check for extension */
    if ((addr[1] & 0x01) == 0) {
        /* Extended address - add high bits */
        *dlci |= (addr[2] & 0x7F) << 6;
        *fecn = (addr[3] >> 3) & 0x01;
        *becn = (addr[3] >> 2) & 0x01;
        *de = (addr[3] >> 1) & 0x01;
        *ea = addr[3] & 0x01;
    } else {
        *fecn = (addr[1] >> 3) & 0x01;
        *becn = (addr[1] >> 2) & 0x01;
        *de = (addr[1] >> 1) & 0x01;
        *ea = 1;
    }

    return true;
}

/**
 * @brief Build 2-byte Frame Relay address
 * @param dlci DLCI number (0-1023)
 * @param cr Command/Response bit
 * @param fecn FECN bit
 * @param becn BECN bit
 * @param de DE bit
 * @param addr Output address bytes
 */
void fr_build_address_2byte(uint16_t dlci, uint8_t cr, uint8_t fecn,
                            uint8_t becn, uint8_t de, uint8_t *addr) {
    addr[0] = 0xFC | ((dlci >> 6) & 0x03) | (cr << 1);
    addr[1] = ((dlci & 0x0F) << 4) | (fecn << 3) | (becn << 2) | (de << 1) | 0x01;
}

/* ============================================================================
 * Frame Relay FCS (CRC16)
 * ============================================================================ */

/**
 * @brief CRC16 lookup table for Frame Relay
 */
static const uint16_t fr_crc16_table[256] = {
    0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
    0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
    0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
    0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
    0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
    0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
    0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
    0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
    0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
    0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
    0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
    0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
    0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
    0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
    0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
    0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
    0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
    0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
    0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
    0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
    0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
    0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
    0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
    0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
    0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
    0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
    0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
    0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
    0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
    0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
    0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
    0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
};

/**
 * @brief Calculate CRC16 for Frame Relay FCS
 * @param data Input data
 * @param len Data length
 * @return Calculated CRC16
 */
uint16_t fr_calc_crc(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ fr_crc16_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFF;
}

/* ============================================================================
 * Frame Relay Context Management
 * ============================================================================ */

/**
 * @brief Initialize Frame Relay context
 * @param ctx Context to initialize
 * @param lmi_type LMI type
 * @param max_pvcs Maximum number of PVCs
 * @return true on success, false on failure
 */
bool fr_init(fr_context_t *ctx, fr_lmi_type_t lmi_type, uint16_t max_pvcs) {
    if (ctx == NULL || max_pvcs == 0) {
        return false;
    }

    memset(ctx, 0, sizeof(fr_context_t));

    ctx->lmi_type = lmi_type;
    ctx->lmi_enabled = (lmi_type != FR_LMI_NONE);
    ctx->max_pvcs = max_pvcs;

    ctx->pvc_table = (fr_pvc_t *)calloc(max_pvcs, sizeof(fr_pvc_t));
    if (ctx->pvc_table == NULL) {
        return false;
    }

    ctx->lmi_sequence = 0;

    return true;
}

/**
 * @brief Cleanup Frame Relay context
 * @param ctx Context to cleanup
 */
void fr_cleanup(fr_context_t *ctx) {
    if (ctx == NULL) return;

    if (ctx->pvc_table != NULL) {
        free(ctx->pvc_table);
        ctx->pvc_table = NULL;
    }

    ctx->num_pvcs = 0;
    ctx->max_pvcs = 0;
}

/**
 * @brief Add PVC to table
 * @param ctx Frame Relay context
 * @param dlci DLCI number
 * @return true on success, false on failure
 */
bool fr_add_pvc(fr_context_t *ctx, uint16_t dlci) {
    if (ctx == NULL || ctx->pvc_table == NULL) {
        return false;
    }

    if (ctx->num_pvcs >= ctx->max_pvcs) {
        return false;
    }

    /* Check for duplicate */
    for (uint16_t i = 0; i < ctx->num_pvcs; i++) {
        if (ctx->pvc_table[i].dlci == dlci) {
            return false;
        }
    }

    ctx->pvc_table[ctx->num_pvcs].dlci = dlci;
    ctx->pvc_table[ctx->num_pvcs].status = FR_PVC_INACTIVE;
    ctx->num_pvcs++;

    printf("Frame Relay: Added PVC with DLCI %d\n", dlci);
    return true;
}

/**
 * @brief Find PVC by DLCI
 * @param ctx Frame Relay context
 * @param dlci DLCI number
 * @return PVC pointer, or NULL if not found
 */
fr_pvc_t* fr_find_pvc(fr_context_t *ctx, uint16_t dlci) {
    if (ctx == NULL || ctx->pvc_table == NULL) {
        return NULL;
    }

    for (uint16_t i = 0; i < ctx->num_pvcs; i++) {
        if (ctx->pvc_table[i].dlci == dlci) {
            return &ctx->pvc_table[i];
        }
    }

    return NULL;
}

/**
 * @brief Update PVC status
 * @param ctx Frame Relay context
 * @param dlci DLCI number
 * @param status New status
 * @return true on success, false on failure
 */
bool fr_update_pvc_status(fr_context_t *ctx, uint16_t dlci, fr_pvc_status_t status) {
    fr_pvc_t *pvc = fr_find_pvc(ctx, dlci);
    if (pvc == NULL) {
        return false;
    }

    pvc->status = status;
    pvc->last_update = (uint32_t)time(NULL);

    return true;
}

/* ============================================================================
 * Frame Relay Frame Operations
 * ============================================================================ */

/**
 * @brief Build Frame Relay frame
 * @param ctx Frame Relay context
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @param dlci DLCI number
 * @param data Payload data
 * @param data_len Payload length
 * @return Frame length, or -1 on error
 */
int fr_build_frame(fr_context_t *ctx, uint8_t *dst, size_t dst_size,
                   uint16_t dlci, const uint8_t *data, size_t data_len) {
    if (dst == NULL || dst_size < 6 || data_len > FR_MAX_FRAME - 6) {
        return -1;
    }

    /* Find PVC */
    fr_pvc_t *pvc = fr_find_pvc(ctx, dlci);
    if (pvc == NULL) {
        printf("Frame Relay: PVC with DLCI %d not found\n", dlci);
        return -1;
    }

    if (pvc->status != FR_PVC_ACTIVE) {
        printf("Frame Relay: PVC with DLCI %d is not active\n", dlci);
        return -1;
    }

    size_t pos = 0;

    /* Opening flag */
    dst[pos++] = FR_FLAG;

    /* Address field (2 bytes) */
    fr_build_address_2byte(dlci, 0, 0, 0, 0, &dst[pos]);
    pos += 2;

    /* Data payload */
    memcpy(&dst[pos], data, data_len);
    pos += data_len;

    /* Calculate and append FCS */
    uint16_t fcs = fr_calc_crc(&dst[1], pos - 1);  /* Exclude flag */
    dst[pos++] = fcs & 0xFF;
    dst[pos++] = (fcs >> 8) & 0xFF;

    /* Closing flag */
    dst[pos++] = FR_FLAG;

    /* Update statistics */
    pvc->tx_frames++;
    pvc->tx_bytes += data_len;

    return (int)pos;
}

/**
 * @brief Parse Frame Relay frame
 * @param ctx Frame Relay context
 * @param src Frame data
 * @param src_len Frame length
 * @param frame Output frame structure
 * @return true on success, false on failure
 */
bool fr_parse_frame(fr_context_t *ctx, const uint8_t *src, size_t src_len,
                    fr_frame_t *frame) {
    if (src == NULL || src_len < 6 || frame == NULL) {
        return false;
    }

    /* Check flags */
    if (src[0] != FR_FLAG || src[src_len - 1] != FR_FLAG) {
        printf("Frame Relay: Invalid frame flags\n");
        return false;
    }

    /* Verify FCS */
    uint16_t recv_fcs = src[src_len - 3] | (src[src_len - 2] << 8);
    uint16_t calc_fcs = fr_calc_crc(&src[1], src_len - 4);  /* Exclude flags and FCS */

    if (recv_fcs != calc_fcs) {
        printf("Frame Relay: FCS error (received=0x%04X, calculated=0x%04X)\n",
               recv_fcs, calc_fcs);
        return false;
    }

    /* Parse address */
    uint8_t ea;
    bool addr_2byte = fr_parse_address_2byte(&src[1], &frame->dlci, &frame->cr,
                                             &ea, &frame->fecn, &frame->becn,
                                             &frame->de);
    (void)addr_2byte;

    /* Extract data */
    size_t header_len = (ea == 1) ? 2 : 4;
    frame->data_len = src_len - header_len - 4;  /* -header -flags -fcs */

    if (frame->data_len > 0) {
        frame->data = (uint8_t *)malloc(frame->data_len);
        if (frame->data == NULL) {
            return false;
        }
        memcpy(frame->data, &src[1 + header_len], frame->data_len);
    } else {
        frame->data = NULL;
    }

    /* Update statistics */
    fr_pvc_t *pvc = fr_find_pvc(ctx, frame->dlci);
    if (pvc != NULL) {
        pvc->rx_frames++;
        pvc->rx_bytes += frame->data_len;
    }

    return true;
}

/**
 * @brief Free Frame Relay frame
 * @param frame Frame to free
 */
void fr_free_frame(fr_frame_t *frame) {
    if (frame && frame->data) {
        free(frame->data);
        frame->data = NULL;
        frame->data_len = 0;
    }
}

/* ============================================================================
 * Frame Relay Test
 * ============================================================================ */

/**
 * @brief Frame Relay protocol test
 */
void fr_test(void) {
    printf("\n=== Frame Relay Protocol Test ===\n");

    /* Initialize context */
    fr_context_t ctx;
    if (!fr_init(&ctx, FR_LMI_CISCO, 16)) {
        printf("Failed to initialize Frame Relay context\n");
        return;
    }

    printf("LMI type: Cisco\n");
    printf("LMI enabled: %s\n", ctx.lmi_enabled ? "yes" : "no");

    /* Add PVCs */
    fr_add_pvc(&ctx, 16);
    fr_add_pvc(&ctx, 17);
    fr_add_pvc(&ctx, 100);

    /* Activate PVC */
    fr_update_pvc_status(&ctx, 16, FR_PVC_ACTIVE);
    fr_update_pvc_status(&ctx, 17, FR_PVC_ACTIVE);

    /* Test address parsing */
    uint8_t test_addr[2];
    fr_build_address_2byte(100, 1, 0, 1, 0, test_addr);
    printf("\nAddress for DLCI 100: %02X %02X\n", test_addr[0], test_addr[1]);

    uint16_t dlci;
    uint8_t cr, ea, fecn, becn, de;
    fr_parse_address_2byte(test_addr, &dlci, &cr, &ea, &fecn, &becn, &de);
    printf("Parsed: DLCI=%d, C/R=%d, FECN=%d, BECN=%d, DE=%d\n",
           dlci, cr, fecn, becn, de);

    /* Test frame building */
    uint8_t payload[] = "Hello Frame Relay!";
    uint8_t frame_buf[FR_MAX_FRAME];

    int frame_len = fr_build_frame(&ctx, frame_buf, sizeof(frame_buf), 16,
                                   payload, sizeof(payload));
    if (frame_len > 0) {
        printf("\nBuilt frame (%d bytes):\n", frame_len);
        for (int i = 0; i < frame_len; i++) {
            printf("%02X ", frame_buf[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }

    /* Test frame parsing */
    fr_frame_t rx_frame;
    if (fr_parse_frame(&ctx, frame_buf, frame_len, &rx_frame)) {
        printf("\nParsed frame:\n");
        printf("  DLCI: %d\n", rx_frame.dlci);
        printf("  C/R: %d\n", rx_frame.cr);
        printf("  FECN: %d, BECN: %d, DE: %d\n", rx_frame.fecn, rx_frame.becn, rx_frame.de);
        printf("  Data (%zu bytes): ", rx_frame.data_len);
        for (size_t i = 0; i < rx_frame.data_len; i++) {
            printf("%c", rx_frame.data[i]);
        }
        printf("\n");
        fr_free_frame(&rx_frame);
    }

    /* Print PVC statistics */
    printf("\nPVC Statistics:\n");
    for (uint16_t i = 0; i < ctx.num_pvcs; i++) {
        printf("  DLCI %d: status=%d, TX=%u/%u, RX=%u/%u\n",
               ctx.pvc_table[i].dlci,
               ctx.pvc_table[i].status,
               ctx.pvc_table[i].tx_frames, ctx.pvc_table[i].tx_bytes,
               ctx.pvc_table[i].rx_frames, ctx.pvc_table[i].rx_bytes);
    }

    fr_cleanup(&ctx);
    printf("\nFrame Relay test completed\n");
}

/* Main entry point */
int main(void) {
    fr_test();
    return 0;
}
```

### 4.3 编译与运行

```bash
gcc -o framerelay_test framerelay.c -Wall -Wextra
./framerelay_test
```

---

## 5. 以太网MAC层

### 5.1 以太网帧结构

```
┌───────────┬───────────┬─────────┬───────────┬──────────┬───────┬──────────┐
│ Preamble  │   SFD     │   DA    │    SA     │  Length/ │ Data  │   FCS    │
│ (7 bytes) │ (1 byte)  │ 6 bytes │  6 bytes  │  Type    │46-1500│ 4 bytes  │
│  0x55...  │   0xD5    │         │           │ 2 bytes  │       │  CRC32   │
└───────────┴───────────┴─────────┴───────────┴──────────┴───────┴──────────┘
```

### 5.2 MAC地址处理实现

```c
/**
 * @file ethernet_mac.c
 * @brief Ethernet MAC Layer Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * Ethernet Constants
 * ============================================================================ */

#define ETH_ADDR_LEN        6       /**< MAC address length */
#define ETH_MIN_FRAME_LEN   64      /**< Minimum frame length */
#define ETH_MAX_FRAME_LEN   1518    /**< Maximum frame length (without VLAN) */
#define ETH_MTU             1500    /**< Maximum transmission unit */
#define ETH_FCS_LEN         4       /**< FCS length */

/**
 * @brief Ethernet protocol types
 */
typedef enum {
    ETH_TYPE_IPv4       = 0x0800,  /**< IPv4 */
    ETH_TYPE_ARP        = 0x0806,  /**< ARP */
    ETH_TYPE_WOL        = 0x0842,  /**< Wake-on-LAN */
    ETH_TYPE_RARP       = 0x8035,  /**< Reverse ARP */
    ETH_TYPE_VLAN       = 0x8100,  /**< VLAN Tag (802.1Q) */
    ETH_TYPE_IPV6       = 0x86DD,  /**< IPv6 */
    ETH_TYPE_MPLS_UC    = 0x8847,  /**< MPLS Unicast */
    ETH_TYPE_MPLS_MC    = 0x8848,  /**< MPLS Multicast */
    ETH_TYPE_PPPoE_D    = 0x8863,  /**< PPPoE Discovery */
    ETH_TYPE_PPPoE_S    = 0x8864,  /**< PPPoE Session */
    ETH_TYPE_LLDP       = 0x88CC,  /**< LLDP */
    ETH_TYPE_8021AD     = 0x88A8,  /**< 802.1ad (QinQ) */
    ETH_TYPE_8021AH     = 0x88E7,  /**< 802.1ah (Provider Backbone Bridge) */
} eth_type_t;

/**
 * @brief Ethernet address types
 */
typedef enum {
    ETH_ADDR_UNICAST,       /**< Unicast address */
    ETH_ADDR_MULTICAST,     /**< Multicast address */
    ETH_ADDR_BROADCAST      /**< Broadcast address */
} eth_addr_type_t;

/**
 * @brief MAC address structure
 */
typedef struct {
    uint8_t addr[ETH_ADDR_LEN];  /**< MAC address bytes */
} eth_addr_t;

/**
 * @brief Ethernet frame structure
 */
typedef struct {
    eth_addr_t dst;          /**< Destination MAC */
    eth_addr_t src;          /**< Source MAC */
    uint16_t type;           /**< EtherType */
    uint8_t *payload;        /**< Payload data */
    size_t payload_len;      /**< Payload length */
} eth_frame_t;

/* ============================================================================
 * MAC Address Operations
 * ============================================================================ */

/**
 * @brief Broadcast MAC address
 */
static const eth_addr_t ETH_ADDR_BROADCAST_VAL = {
    .addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
};

/**
 * @brief Null MAC address
 */
static const eth_addr_t ETH_ADDR_NULL = {
    .addr = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

/**
 * @brief Parse MAC address string to binary
 * @param str MAC address string (e.g., "00:11:22:33:44:55")
 * @param addr Output MAC address structure
 * @return true on success, false on failure
 */
bool eth_parse_mac(const char *str, eth_addr_t *addr) {
    if (str == NULL || addr == NULL) {
        return false;
    }

    unsigned int bytes[ETH_ADDR_LEN];
    if (sscanf(str, "%2x:%2x:%2x:%2x:%2x:%2x",
               &bytes[0], &bytes[1], &bytes[2],
               &bytes[3], &bytes[4], &bytes[5]) != ETH_ADDR_LEN) {
        return false;
    }

    for (int i = 0; i < ETH_ADDR_LEN; i++) {
        addr->addr[i] = (uint8_t)bytes[i];
    }

    return true;
}

/**
 * @brief Format MAC address to string
 * @param addr MAC address
 * @param str Output string buffer (at least 18 bytes)
 * @return str pointer
 */
char* eth_mac_to_string(const eth_addr_t *addr, char *str) {
    if (addr == NULL || str == NULL) {
        return NULL;
    }

    snprintf(str, 18, "%02X:%02X:%02X:%02X:%02X:%02X",
             addr->addr[0], addr->addr[1], addr->addr[2],
             addr->addr[3], addr->addr[4], addr->addr[5]);

    return str;
}

/**
 * @brief Compare two MAC addresses
 * @param a First MAC address
 * @param b Second MAC address
 * @return 0 if equal, negative if a < b, positive if a > b
 */
int eth_mac_compare(const eth_addr_t *a, const eth_addr_t *b) {
    return memcmp(a->addr, b->addr, ETH_ADDR_LEN);
}

/**
 * @brief Check if MAC address is broadcast
 * @param addr MAC address
 * @return true if broadcast, false otherwise
 */
bool eth_is_broadcast(const eth_addr_t *addr) {
    return eth_mac_compare(addr, &ETH_ADDR_BROADCAST_VAL) == 0;
}

/**
 * @brief Check if MAC address is multicast
 * @param addr MAC address
 * @return true if multicast, false otherwise
 */
bool eth_is_multicast(const eth_addr_t *addr) {
    return (addr->addr[0] & 0x01) != 0;
}

/**
 * @brief Check if MAC address is unicast
 * @param addr MAC address
 * @return true if unicast, false otherwise
 */
bool eth_is_unicast(const eth_addr_t *addr) {
    return !eth_is_multicast(addr);
}

/**
 * @brief Get address type
 * @param addr MAC address
 * @return Address type
 */
eth_addr_type_t eth_get_addr_type(const eth_addr_t *addr) {
    if (eth_is_broadcast(addr)) {
        return ETH_ADDR_BROADCAST;
    } else if (eth_is_multicast(addr)) {
        return ETH_ADDR_MULTICAST;
    }
    return ETH_ADDR_UNICAST;
}

/**
 * @brief Copy MAC address
 * @param dst Destination
 * @param src Source
 */
void eth_mac_copy(eth_addr_t *dst, const eth_addr_t *src) {
    if (dst && src) {
        memcpy(dst->addr, src->addr, ETH_ADDR_LEN);
    }
}

/**
 * @brief Generate random MAC address
 * @param addr Output MAC address
 * @param locally_administered true for locally administered address
 */
void eth_generate_mac(eth_addr_t *addr, bool locally_administered) {
    if (addr == NULL) return;

    for (int i = 0; i < ETH_ADDR_LEN; i++) {
        addr->addr[i] = (uint8_t)(rand() & 0xFF);
    }

    /* Set locally administered bit */
    if (locally_administered) {
        addr->addr[0] |= 0x02;
    } else {
        addr->addr[0] &= ~0x02;
    }

    /* Clear multicast bit */
    addr->addr[0] &= ~0x01;
}

/**
 * @brief Check if MAC address is null
 * @param addr MAC address
 * @return true if null, false otherwise
 */
bool eth_is_null(const eth_addr_t *addr) {
    return eth_mac_compare(addr, &ETH_ADDR_NULL) == 0;
}

/* ============================================================================
 * CRC32 Implementation (Ethernet FCS)
 * ============================================================================ */

/**
 * @brief CRC32 lookup table
 */
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/**
 * @brief Calculate CRC32 for Ethernet FCS
 * @param data Input data
 * @param len Data length
 * @return Calculated CRC32
 */
uint32_t eth_calc_crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/* ============================================================================
 * Ethernet Frame Operations
 * ============================================================================ */

/**
 * @brief Build Ethernet frame
 * @param dst Destination MAC
 * @param src Source MAC
 * @param type EtherType
 * @param payload Payload data
 * @param payload_len Payload length
 * @param frame_buf Output frame buffer
 * @param frame_buf_size Frame buffer size
 * @return Frame length, or -1 on error
 */
int eth_build_frame(const eth_addr_t *dst, const eth_addr_t *src, uint16_t type,
                    const uint8_t *payload, size_t payload_len,
                    uint8_t *frame_buf, size_t frame_buf_size) {
    if (dst == NULL || src == NULL || frame_buf == NULL) {
        return -1;
    }

    if (payload_len > ETH_MTU) {
        return -1;  /* Payload too large */
    }

    size_t frame_len = 2 * ETH_ADDR_LEN + 2 + payload_len + ETH_FCS_LEN;

    /* Check minimum frame size */
    if (payload_len < 46) {
        frame_len = 2 * ETH_ADDR_LEN + 2 + 46 + ETH_FCS_LEN;
    }

    if (frame_buf_size < frame_len) {
        return -1;  /* Buffer too small */
    }

    size_t pos = 0;

    /* Destination MAC */
    memcpy(&frame_buf[pos], dst->addr, ETH_ADDR_LEN);
    pos += ETH_ADDR_LEN;

    /* Source MAC */
    memcpy(&frame_buf[pos], src->addr, ETH_ADDR_LEN);
    pos += ETH_ADDR_LEN;

    /* EtherType (big-endian) */
    frame_buf[pos++] = (type >> 8) & 0xFF;
    frame_buf[pos++] = type & 0xFF;

    /* Payload */
    if (payload_len > 0 && payload != NULL) {
        memcpy(&frame_buf[pos], payload, payload_len);
        pos += payload_len;
    }

    /* Pad to minimum frame size */
    while (pos < 2 * ETH_ADDR_LEN + 2 + 46) {
        frame_buf[pos++] = 0;
    }

    /* Calculate and append FCS */
    uint32_t fcs = eth_calc_crc32(frame_buf, pos);
    frame_buf[pos++] = fcs & 0xFF;
    frame_buf[pos++] = (fcs >> 8) & 0xFF;
    frame_buf[pos++] = (fcs >> 16) & 0xFF;
    frame_buf[pos++] = (fcs >> 24) & 0xFF;

    return (int)pos;
}

/**
 * @brief Parse Ethernet frame
 * @param frame_buf Frame buffer
 * @param frame_len Frame length
 * @param frame Output frame structure
 * @return true on success, false on failure
 */
bool eth_parse_frame(const uint8_t *frame_buf, size_t frame_len, eth_frame_t *frame) {
    if (frame_buf == NULL || frame_len < ETH_MIN_FRAME_LEN || frame == NULL) {
        return false;
    }

    size_t pos = 0;

    /* Destination MAC */
    memcpy(frame->dst.addr, &frame_buf[pos], ETH_ADDR_LEN);
    pos += ETH_ADDR_LEN;

    /* Source MAC */
    memcpy(frame->src.addr, &frame_buf[pos], ETH_ADDR_LEN);
    pos += ETH_ADDR_LEN;

    /* EtherType */
    frame->type = ((uint16_t)frame_buf[pos] << 8) | frame_buf[pos + 1];
    pos += 2;

    /* Payload length (excluding FCS) */
    frame->payload_len = frame_len - pos - ETH_FCS_LEN;

    if (frame->payload_len > 0) {
        frame->payload = (uint8_t *)malloc(frame->payload_len);
        if (frame->payload == NULL) {
            return false;
        }
        memcpy(frame->payload, &frame_buf[pos], frame->payload_len);
    } else {
        frame->payload = NULL;
    }

    /* Verify FCS */
    uint32_t recv_fcs = frame_buf[frame_len - 4] |
                       (frame_buf[frame_len - 3] << 8) |
                       (frame_buf[frame_len - 2] << 16) |
                       (frame_buf[frame_len - 1] << 24);
    uint32_t calc_fcs = eth_calc_crc32(frame_buf, frame_len - 4);

    if (recv_fcs != calc_fcs) {
        printf("Ethernet: FCS error (received=0x%08X, calculated=0x%08X)\n",
               recv_fcs, calc_fcs);
        /* Note: In real implementation, might want to return false here */
    }

    return true;
}

/**
 * @brief Free Ethernet frame resources
 * @param frame Frame to free
 */
void eth_free_frame(eth_frame_t *frame) {
    if (frame && frame->payload) {
        free(frame->payload);
        frame->payload = NULL;
        frame->payload_len = 0;
    }
}

/**
 * @brief Get EtherType name
 * @param type EtherType value
 * @return Type name string
 */
const char* eth_type_name(uint16_t type) {
    switch (type) {
        case ETH_TYPE_IPv4:     return "IPv4";
        case ETH_TYPE_ARP:      return "ARP";
        case ETH_TYPE_RARP:     return "RARP";
        case ETH_TYPE_VLAN:     return "VLAN";
        case ETH_TYPE_IPV6:     return "IPv6";
        case ETH_TYPE_PPPoE_D:  return "PPPoE Discovery";
        case ETH_TYPE_PPPoE_S:  return "PPPoE Session";
        case ETH_TYPE_LLDP:     return "LLDP";
        default:                return "Unknown";
    }
}

/* ============================================================================
 * Ethernet Test
 * ============================================================================ */

/**
 * @brief Ethernet MAC layer test
 */
void eth_test(void) {
    printf("\n=== Ethernet MAC Layer Test ===\n");

    /* Test MAC address parsing */
    eth_addr_t mac1, mac2;
    char mac_str[18];

    printf("\nMAC Address Tests:\n");

    eth_parse_mac("00:11:22:33:44:55", &mac1);
    eth_mac_to_string(&mac1, mac_str);
    printf("Parsed MAC: %s\n", mac_str);

    /* Test MAC type detection */
    eth_parse_mac("FF:FF:FF:FF:FF:FF", &mac1);
    printf("Broadcast: %s\n", eth_is_broadcast(&mac1) ? "yes" : "no");

    eth_parse_mac("01:00:5E:00:00:01", &mac1);
    printf("Multicast: %s\n", eth_is_multicast(&mac1) ? "yes" : "no");

    eth_parse_mac("00:11:22:33:44:55", &mac1);
    printf("Unicast: %s\n", eth_is_unicast(&mac1) ? "yes" : "no");

    /* Test random MAC generation */
    eth_generate_mac(&mac1, true);
    eth_mac_to_string(&mac1, mac_str);
    printf("Generated MAC: %s\n", mac_str);

    /* Test CRC32 */
    const char *test_data = "Hello Ethernet";
    uint32_t crc = eth_calc_crc32((const uint8_t *)test_data, strlen(test_data));
    printf("\nCRC32 of '%s': 0x%08X\n", test_data, crc);

    /* Test frame building */
    eth_parse_mac("00:11:22:33:44:55", &mac1);  /* Source */
    eth_parse_mac("AA:BB:CC:DD:EE:FF", &mac2);  /* Destination */

    uint8_t payload[] = "Hello Ethernet Frame!";
    uint8_t frame_buf[ETH_MAX_FRAME_LEN];

    int frame_len = eth_build_frame(&mac2, &mac1, ETH_TYPE_IPv4,
                                    payload, sizeof(payload),
                                    frame_buf, sizeof(frame_buf));
    if (frame_len > 0) {
        printf("\nBuilt frame (%d bytes):\n", frame_len);
        printf("  Destination: ");
        for (int i = 0; i < ETH_ADDR_LEN; i++) {
            printf("%02X:", frame_buf[i]);
        }
        printf("\b \n");

        printf("  Source: ");
        for (int i = 0; i < ETH_ADDR_LEN; i++) {
            printf("%02X:", frame_buf[ETH_ADDR_LEN + i]);
        }
        printf("\b \n");

        uint16_t type = (frame_buf[12] << 8) | frame_buf[13];
        printf("  EtherType: 0x%04X (%s)\n", type, eth_type_name(type));

        printf("  Hex dump:\n");
        for (int i = 0; i < frame_len && i < 64; i++) {
            printf("%02X ", frame_buf[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        if (frame_len % 16 != 0) printf("\n");
    }

    /* Test frame parsing */
    eth_frame_t rx_frame;
    if (eth_parse_frame(frame_buf, frame_len, &rx_frame)) {
        printf("\nParsed frame:\n");
        eth_mac_to_string(&rx_frame.dst, mac_str);
        printf("  Destination: %s\n", mac_str);
        eth_mac_to_string(&rx_frame.src, mac_str);
        printf("  Source: %s\n", mac_str);
        printf("  EtherType: 0x%04X (%s)\n", rx_frame.type, eth_type_name(rx_frame.type));
        printf("  Payload (%zu bytes): ", rx_frame.payload_len);
        for (size_t i = 0; i < rx_frame.payload_len && i < 32; i++) {
            printf("%c", rx_frame.payload[i]);
        }
        printf("\n");
        eth_free_frame(&rx_frame);
    }

    printf("\nEthernet MAC test completed\n");
}

/* Main entry point */
int main(void) {
    eth_test();
    return 0;
}
```

### 5.3 编译与运行

```bash
gcc -o ethernet_mac_test ethernet_mac.c -Wall -Wextra
./ethernet_mac_test
```

---

## 6. VLAN实现

### 6.1 VLAN标签格式 (802.1Q)

```
┌──────────┬──────────┬──────────┬──────────┐
│ TPID     │  PCP     │  CFI     │  VID     │
│0x8100    │3 bits    │1 bit     │12 bits   │
│(2 bytes) │          │          │          │
└──────────┴──────────┴──────────┴──────────┘
```

### 6.2 VLAN完整实现

```c
/**
 * @file vlan.c
 * @brief VLAN (802.1Q) Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * VLAN Constants
 * ============================================================================ */

#define VLAN_TPID           0x8100  /**< VLAN Tag Protocol Identifier */
#define VLAN_VID_MASK       0x0FFF  /**< VLAN ID mask */
#define VLAN_PCP_MASK       0xE000  /**< Priority Code Point mask */
#define VLAN_CFI_MASK       0x1000  /**< Canonical Format Indicator mask */
#define VLAN_MAX_VID        4094    /**< Maximum valid VLAN ID */
#define VLAN_MIN_VID        1       /**< Minimum valid VLAN ID */
#define VLAN_TAG_LEN        4       /**< VLAN tag length */

/**
 * @brief VLAN priority levels (802.1p)
 */
typedef enum {
    VLAN_PRIO_BK = 0,      /**< Background (lowest) */
    VLAN_PRIO_BE = 1,      /**< Best Effort */
    VLAN_PRIO_EE = 2,      /**< Excellent Effort */
    VLAN_PRIO_CA = 3,      /**< Critical Applications */
    VLAN_PRIO_VI = 4,      /**< Video, < 100ms latency */
    VLAN_PRIO_VO = 5,      /**< Voice, < 10ms latency */
    VLAN_PRIO_IC = 6,      /**< Internetwork Control */
    VLAN_PRIO_NC = 7       /**< Network Control (highest) */
} vlan_priority_t;

/**
 * @brief VLAN tag structure
 */
typedef struct {
    uint16_t tci;            /**< Tag Control Information (PCP|CFI|VID) */
    uint16_t inner_type;     /**< Inner EtherType */
} vlan_tag_t;

/**
 * @brief VLAN membership port types
 */
typedef enum {
    VLAN_PORT_NONE,          /**< Not a member */
    VLAN_PORT_UNTAGGED,      /**< Untagged member */
    VLAN_PORT_TAGGED,        /**< Tagged member */
    VLAN_PORT_HYBRID         /**< Hybrid port */
} vlan_port_mode_t;

/**
 * @brief VLAN port configuration
 */
typedef struct {
    uint8_t port_id;         /**< Port identifier */
    vlan_port_mode_t mode;   /**< Port VLAN mode */
    uint16_t pvid;           /**< Port VLAN ID (native VLAN) */
    uint8_t prio;            /**< Default priority */
} vlan_port_t;

/**
 * @brief VLAN entry
 */
typedef struct {
    uint16_t vid;            /**< VLAN ID */
    char name[32];           /**< VLAN name */
    bool active;             /**< VLAN active flag */
    uint8_t *port_bitmap;    /**< Port membership bitmap */
    uint16_t num_ports;      /**< Number of ports */
} vlan_entry_t;

/**
 * @brief VLAN database
 */
typedef struct {
    vlan_entry_t *vlans;     /**< VLAN entries */
    uint16_t max_vlans;      /**< Maximum VLANs */
    uint16_t num_vlans;      /**< Current VLAN count */
    vlan_port_t *ports;      /**< Port configurations */
    uint16_t num_ports;      /**< Number of ports */
} vlan_database_t;

/* ============================================================================
 * VLAN Tag Operations
 * ============================================================================ */

/**
 * @brief Build VLAN TCI field
 * @param prio Priority (0-7)
 * @param cfi Canonical Format Indicator
 * @param vid VLAN ID
 * @return TCI value
 */
uint16_t vlan_build_tci(uint8_t prio, bool cfi, uint16_t vid) {
    return ((prio & 0x07) << 13) | (cfi ? 0x1000 : 0) | (vid & VLAN_VID_MASK);
}

/**
 * @brief Parse VLAN TCI field
 * @param tci TCI value
 * @param prio Output priority
 * @param cfi Output CFI
 * @param vid Output VLAN ID
 */
void vlan_parse_tci(uint16_t tci, uint8_t *prio, bool *cfi, uint16_t *vid) {
    *prio = (tci >> 13) & 0x07;
    *cfi = (tci & VLAN_CFI_MASK) != 0;
    *vid = tci & VLAN_VID_MASK;
}

/**
 * @brief Check if VLAN ID is valid
 * @param vid VLAN ID
 * @return true if valid, false otherwise
 */
bool vlan_is_valid_id(uint16_t vid) {
    return (vid >= VLAN_MIN_VID && vid <= VLAN_MAX_VID);
}

/**
 * @brief Insert VLAN tag into frame
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @param src Original frame
 * @param src_len Original frame length
 * @param tag VLAN tag
 * @return New frame length, or -1 on error
 */
int vlan_insert_tag(uint8_t *dst, size_t dst_size, const uint8_t *src,
                    size_t src_len, const vlan_tag_t *tag) {
    if (dst == NULL || src == NULL || tag == NULL) {
        return -1;
    }

    if (src_len < 12 || dst_size < src_len + VLAN_TAG_LEN) {
        return -1;
    }

    size_t pos = 0;

    /* Copy destination and source MAC (12 bytes) */
    memcpy(&dst[pos], src, 12);
    pos += 12;

    /* Insert VLAN tag */
    dst[pos++] = (VLAN_TPID >> 8) & 0xFF;
    dst[pos++] = VLAN_TPID & 0xFF;
    dst[pos++] = (tag->tci >> 8) & 0xFF;
    dst[pos++] = tag->tci & 0xFF;

    /* Copy rest of original frame */
    memcpy(&dst[pos], &src[12], src_len - 12);
    pos += src_len - 12;

    return (int)pos;
}

/**
 * @brief Remove VLAN tag from frame
 * @param dst Destination buffer
 * @param dst_size Destination size
 * @param src Tagged frame
 * @param src_len Tagged frame length
 * @param tag Output VLAN tag (can be NULL)
 * @return New frame length, or -1 on error
 */
int vlan_remove_tag(uint8_t *dst, size_t dst_size, const uint8_t *src,
                    size_t src_len, vlan_tag_t *tag) {
    if (dst == NULL || src == NULL) {
        return -1;
    }

    if (src_len < 16 || dst_size < src_len - VLAN_TAG_LEN) {
        return -1;
    }

    /* Check for VLAN tag */
    uint16_t tpid = ((uint16_t)src[12] << 8) | src[13];
    if (tpid != VLAN_TPID) {
        return -1;  /* Not a VLAN tagged frame */
    }

    /* Extract tag info if requested */
    if (tag != NULL) {
        tag->tci = ((uint16_t)src[14] << 8) | src[15];
    }

    size_t pos = 0;

    /* Copy destination and source MAC (12 bytes) */
    memcpy(&dst[pos], src, 12);
    pos += 12;

    /* Copy rest of frame (skip VLAN tag) */
    memcpy(&dst[pos], &src[16], src_len - 16);
    pos += src_len - 16;

    return (int)pos;
}

/**
 * @brief Get priority name
 * @param prio Priority value
 * @return Priority name string
 */
const char* vlan_prio_name(uint8_t prio) {
    switch (prio) {
        case VLAN_PRIO_BK: return "Background";
        case VLAN_PRIO_BE: return "Best Effort";
        case VLAN_PRIO_EE: return "Excellent Effort";
        case VLAN_PRIO_CA: return "Critical Apps";
        case VLAN_PRIO_VI: return "Video";
        case VLAN_PRIO_VO: return "Voice";
        case VLAN_PRIO_IC: return "Internetwork Control";
        case VLAN_PRIO_NC: return "Network Control";
        default: return "Unknown";
    }
}

/* ============================================================================
 * VLAN Database Operations
 * ============================================================================ */

/**
 * @brief Initialize VLAN database
 * @param db VLAN database
 * @param max_vlans Maximum number of VLANs
 * @param num_ports Number of switch ports
 * @return true on success, false on failure
 */
bool vlan_db_init(vlan_database_t *db, uint16_t max_vlans, uint16_t num_ports) {
    if (db == NULL || max_vlans == 0 || num_ports == 0) {
        return false;
    }

    memset(db, 0, sizeof(vlan_database_t));

    db->vlans = (vlan_entry_t *)calloc(max_vlans, sizeof(vlan_entry_t));
    if (db->vlans == NULL) {
        return false;
    }

    db->ports = (vlan_port_t *)calloc(num_ports, sizeof(vlan_port_t));
    if (db->ports == NULL) {
        free(db->vlans);
        return false;
    }

    db->max_vlans = max_vlans;
    db->num_ports = num_ports;

    /* Initialize default VLAN (VLAN 1) */
    db->vlans[0].vid = 1;
    strcpy(db->vlans[0].name, "default");
    db->vlans[0].active = true;
    db->vlans[0].num_ports = num_ports;
    db->vlans[0].port_bitmap = (uint8_t *)calloc((num_ports + 7) / 8, 1);
    if (db->vlans[0].port_bitmap == NULL) {
        free(db->vlans);
        free(db->ports);
        return false;
    }

    /* All ports are members of default VLAN initially */
    for (uint16_t i = 0; i < num_ports; i++) {
        db->vlans[0].port_bitmap[i / 8] |= (1 << (i % 8));
        db->ports[i].port_id = i;
        db->ports[i].pvid = 1;
        db->ports[i].mode = VLAN_PORT_UNTAGGED;
    }

    db->num_vlans = 1;

    return true;
}

/**
 * @brief Cleanup VLAN database
 * @param db VLAN database
 */
void vlan_db_cleanup(vlan_database_t *db) {
    if (db == NULL) return;

    if (db->vlans != NULL) {
        for (uint16_t i = 0; i < db->num_vlans; i++) {
            if (db->vlans[i].port_bitmap != NULL) {
                free(db->vlans[i].port_bitmap);
            }
        }
        free(db->vlans);
        db->vlans = NULL;
    }

    if (db->ports != NULL) {
        free(db->ports);
        db->ports = NULL;
    }

    db->num_vlans = 0;
    db->num_ports = 0;
}

/**
 * @brief Create VLAN
 * @param db VLAN database
 * @param vid VLAN ID
 * @param name VLAN name
 * @return true on success, false on failure
 */
bool vlan_create(vlan_database_t *db, uint16_t vid, const char *name) {
    if (db == NULL || !vlan_is_valid_id(vid)) {
        return false;
    }

    /* Check if VLAN already exists */
    for (uint16_t i = 0; i < db->num_vlans; i++) {
        if (db->vlans[i].vid == vid) {
            return false;  /* VLAN already exists */
        }
    }

    if (db->num_vlans >= db->max_vlans) {
        return false;  /* VLAN table full */
    }

    vlan_entry_t *vlan = &db->vlans[db->num_vlans];
    vlan->vid = vid;
    strncpy(vlan->name, name, sizeof(vlan->name) - 1);
    vlan->name[sizeof(vlan->name) - 1] = '\0';
    vlan->active = true;
    vlan->num_ports = db->num_ports;
    vlan->port_bitmap = (uint8_t *)calloc((db->num_ports + 7) / 8, 1);
    if (vlan->port_bitmap == NULL) {
        return false;
    }

    db->num_vlans++;

    printf("VLAN %d '%s' created\n", vid, name);
    return true;
}

/**
 * @brief Add port to VLAN
 * @param db VLAN database
 * @param vid VLAN ID
 * @param port_id Port ID
 * @return true on success, false on failure
 */
bool vlan_add_port(vlan_database_t *db, uint16_t vid, uint8_t port_id) {
    if (db == NULL || port_id >= db->num_ports) {
        return false;
    }

    vlan_entry_t *vlan = NULL;
    for (uint16_t i = 0; i < db->num_vlans; i++) {
        if (db->vlans[i].vid == vid) {
            vlan = &db->vlans[i];
            break;
        }
    }

    if (vlan == NULL) {
        return false;  /* VLAN not found */
    }

    vlan->port_bitmap[port_id / 8] |= (1 << (port_id % 8));

    return true;
}

/**
 * @brief Check if port is member of VLAN
 * @param vlan VLAN entry
 * @param port_id Port ID
 * @return true if member, false otherwise
 */
bool vlan_is_port_member(const vlan_entry_t *vlan, uint8_t port_id) {
    if (vlan == NULL || vlan->port_bitmap == NULL) {
        return false;
    }
    return (vlan->port_bitmap[port_id / 8] & (1 << (port_id % 8))) != 0;
}

/**
 * @brief Print VLAN database
 * @param db VLAN database
 */
void vlan_db_print(const vlan_database_t *db) {
    if (db == NULL) return;

    printf("\nVLAN Database:\n");
    printf("%-6s %-20s %-8s %s\n", "VID", "Name", "Active", "Ports");
    printf("-------------------------------------------------\n");

    for (uint16_t i = 0; i < db->num_vlans; i++) {
        vlan_entry_t *vlan = &db->vlans[i];
        printf("%-6d %-20s %-8s ", vlan->vid, vlan->name,
               vlan->active ? "Yes" : "No");

        for (uint16_t j = 0; j < db->num_ports; j++) {
            if (vlan_is_port_member(vlan, j)) {
                printf("%d ", j);
            }
        }
        printf("\n");
    }
}

/* ============================================================================
 * VLAN Test
 * ============================================================================ */

/**
 * @brief VLAN test
 */
void vlan_test(void) {
    printf("\n=== VLAN (802.1Q) Test ===\n");

    /* Test TCI building and parsing */
    printf("\nVLAN Tag Tests:\n");

    uint16_t vid = 100;
    uint8_t prio = VLAN_PRIO_VO;
    bool cfi = false;

    uint16_t tci = vlan_build_tci(prio, cfi, vid);
    printf("TCI for VID=%d, Prio=%d: 0x%04X\n", vid, prio, tci);

    uint8_t parsed_prio;
    bool parsed_cfi;
    uint16_t parsed_vid;
    vlan_parse_tci(tci, &parsed_prio, &parsed_cfi, &parsed_vid);
    printf("Parsed: VID=%d, Prio=%d (%s), CFI=%d\n",
           parsed_vid, parsed_prio, vlan_prio_name(parsed_prio), parsed_cfi);

    /* Test tag insertion/removal */
    uint8_t frame[] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  /* Destination MAC */
        0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF,  /* Source MAC */
        0x08, 0x00,                          /* EtherType (IPv4) */
        0x45, 0x00, 0x00, 0x20,              /* IP header... */
    };

    uint8_t tagged[64];
    vlan_tag_t tag = {
        .tci = vlan_build_tci(VLAN_PRIO_VI, false, 100)
    };

    int tagged_len = vlan_insert_tag(tagged, sizeof(tagged), frame, sizeof(frame), &tag);
    if (tagged_len > 0) {
        printf("\nTagged frame (%d bytes):\n", tagged_len);
        printf("  TPID: 0x%02X%02X\n", tagged[12], tagged[13]);
        uint16_t tci_rx = (tagged[14] << 8) | tagged[15];
        printf("  TCI: 0x%04X\n", tci_rx);
    }

    uint8_t untagged[64];
    vlan_tag_t removed_tag;
    int untagged_len = vlan_remove_tag(untagged, sizeof(untagged), tagged, tagged_len, &removed_tag);
    if (untagged_len > 0) {
        printf("\nUntagged frame (%d bytes)\n", untagged_len);
        vlan_parse_tci(removed_tag.tci, &parsed_prio, &parsed_cfi, &parsed_vid);
        printf("  Removed tag: VID=%d, Prio=%d\n", parsed_vid, parsed_prio);
    }

    /* Test VLAN database */
    vlan_database_t db;
    if (vlan_db_init(&db, 16, 8)) {
        printf("\nVLAN database initialized with 8 ports\n");

        vlan_create(&db, 10, "Sales");
        vlan_create(&db, 20, "Engineering");
        vlan_create(&db, 30, "Management");

        vlan_add_port(&db, 10, 0);
        vlan_add_port(&db, 10, 1);
        vlan_add_port(&db, 10, 2);

        vlan_add_port(&db, 20, 2);
        vlan_add_port(&db, 20, 3);
        vlan_add_port(&db, 20, 4);

        vlan_add_port(&db, 30, 4);
        vlan_add_port(&db, 30, 5);
        vlan_add_port(&db, 30, 6);
        vlan_add_port(&db, 30, 7);

        vlan_db_print(&db);

        vlan_db_cleanup(&db);
    }

    printf("\nVLAN test completed\n");
}

/* Main entry point */
int main(void) {
    vlan_test();
    return 0;
}
```

### 6.3 编译与运行

```bash
gcc -o vlan_test vlan.c -Wall -Wextra
./vlan_test
```

---

## 7. STP生成树协议

### 7.1 STP BPDU格式

```
┌──────────┬──────────┬──────────┬──────────┬──────────┐
│Protocol  │Version   │BPDU Type │Flags     │Root ID   │
│ID (2)    │(1)       │(1)       │(1)       │(8)       │
├──────────┼──────────┼──────────┼──────────┼──────────┤
│Root Path │Bridge ID │Port ID   │Message   │Max Age   │
│Cost (4)  │(8)       │(2)       │Age (2)   │(2)       │
├──────────┼──────────┼──────────┼──────────┴──────────┤
│Hello Time│Fwd Delay │
│(2)       │(2)       │
└──────────┴──────────┘
```

### 7.2 STP完整实现

```c
/**
 * @file stp.c
 * @brief Spanning Tree Protocol (STP 802.1D) Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * STP Constants
 * ============================================================================ */

#define STP_PROTOCOL_ID     0x0000  /**< STP Protocol Identifier */
#define STP_VERSION_ID      0x00    /**< STP Version */
#define STP_BPDU_TYPE_CFG   0x00    /**< Configuration BPDU */
#define STP_BPDU_TYPE_TCN   0x80    /**< Topology Change Notification BPDU */

#define STP_PORT_STATE_DISABLED     0
#define STP_PORT_STATE_BLOCKING     1
#define STP_PORT_STATE_LISTENING    2
#define STP_PORT_STATE_LEARNING     3
#define STP_PORT_STATE_FORWARDING   4

#define STP_DEFAULT_MAX_AGE         20
#define STP_DEFAULT_HELLO_TIME      2
#define STP_DEFAULT_FWD_DELAY       15
#define STP_DEFAULT_PRIORITY        32768
#define STP_DEFAULT_PATH_COST       19

/**
 * @brief STP port roles
 */
typedef enum {
    STP_ROLE_UNKNOWN,       /**< Unknown role */
    STP_ROLE_ROOT,          /**< Root port */
    STP_ROLE_DESIGNATED,    /**< Designated port */
    STP_ROLE_ALTERNATE,     /**< Alternate port (blocked) */
    STP_ROLE_BACKUP         /**< Backup port */
} stp_port_role_t;

/**
 * @brief STP BPDU flags
 */
typedef enum {
    STP_FLAG_TC     = 0x01,  /**< Topology Change */
    STP_FLAG_TC_ACK = 0x80   /**< Topology Change Acknowledgment */
} stp_bpdu_flag_t;

/**
 * @brief Bridge Identifier structure
 */
typedef struct {
    uint16_t priority;       /**< Bridge priority */
    uint8_t mac[6];          /**< Bridge MAC address */
} __attribute__((packed)) stp_bridge_id_t;

/**
 * @brief STP BPDU structure
 */
typedef struct {
    uint16_t protocol_id;    /**< Protocol identifier */
    uint8_t version;         /**< Version */
    uint8_t bpdu_type;       /**< BPDU type */
    uint8_t flags;           /**< Flags */
    stp_bridge_id_t root_id; /**< Root bridge ID */
    uint32_t root_path_cost; /**< Root path cost */
    stp_bridge_id_t bridge_id; /**< Bridge ID */
    uint16_t port_id;        /**< Port ID */
    uint16_t message_age;    /**< Message age */
    uint16_t max_age;        /**< Max age */
    uint16_t hello_time;     /**< Hello time */
    uint16_t forward_delay;  /**< Forward delay */
} __attribute__((packed)) stp_bpdu_t;

/**
 * @brief STP port structure
 */
typedef struct {
    uint16_t port_id;        /**< Port ID (priority + number) */
    uint8_t state;           /**< Port state */
    stp_port_role_t role;    /**< Port role */
    uint32_t path_cost;      /**< Path cost */

    /* BPDU timing */
    uint16_t designated_priority;
    uint32_t designated_cost;
    stp_bridge_id_t designated_bridge;
    uint16_t designated_port;

    /* Timers */
    time_t last_bpdu_time;
    uint16_t fwd_delay_timer;
} stp_port_t;

/**
 * @brief STP bridge structure
 */
typedef struct {
    stp_bridge_id_t bridge_id;   /**< Bridge ID */
    uint32_t root_path_cost;     /**< Root path cost */
    stp_bridge_id_t root_id;     /**< Root bridge ID */
    uint16_t root_port;          /**< Root port */

    /* Configuration */
    uint16_t max_age;
    uint16_t hello_time;
    uint16_t forward_delay;
    uint8_t max_hops;

    /* Topology change */
    bool topology_change;
    bool topology_change_detected;
    time_t topology_change_time;

    /* Ports */
    stp_port_t *ports;
    uint16_t num_ports;

    /* State */
    bool is_root;
} stp_bridge_t;

/* ============================================================================
 * Bridge ID Operations
 * ============================================================================ */

/**
 * @brief Build bridge ID
 * @param priority Bridge priority
 * @param mac MAC address
 * @param id Output bridge ID
 */
void stp_build_bridge_id(uint16_t priority, const uint8_t *mac,
                         stp_bridge_id_t *id) {
    id->priority = htons(priority);
    memcpy(id->mac, mac, 6);
}

/**
 * @brief Compare two bridge IDs
 * @param a First bridge ID
 * @param b Second bridge ID
 * @return negative if a < b, 0 if equal, positive if a > b
 */
int stp_bridge_id_compare(const stp_bridge_id_t *a, const stp_bridge_id_t *b) {
    int cmp = memcmp(a, b, sizeof(stp_bridge_id_t));
    return cmp;
}

/**
 * @brief Check if two bridge IDs are equal
 * @param a First bridge ID
 * @param b Second bridge ID
 * @return true if equal, false otherwise
 */
bool stp_bridge_id_equal(const stp_bridge_id_t *a, const stp_bridge_id_t *b) {
    return stp_bridge_id_compare(a, b) == 0;
}

/**
 * @brief Format bridge ID to string
 * @param id Bridge ID
 * @param str Output string buffer (min 24 bytes)
 * @return str pointer
 */
char* stp_bridge_id_to_string(const stp_bridge_id_t *id, char *str) {
    snprintf(str, 24, "%04X.%02X%02X.%02X%02X.%02X%02X",
             ntohs(id->priority),
             id->mac[0], id->mac[1], id->mac[2],
             id->mac[3], id->mac[4], id->mac[5]);
    return str;
}

/* ============================================================================
 * STP Bridge Operations
 * ============================================================================ */

/**
 * @brief Initialize STP bridge
 * @param bridge Bridge structure
 * @param priority Bridge priority
 * @param mac Bridge MAC address
 * @param num_ports Number of ports
 * @return true on success, false on failure
 */
bool stp_init_bridge(stp_bridge_t *bridge, uint16_t priority,
                     const uint8_t *mac, uint16_t num_ports) {
    if (bridge == NULL || mac == NULL || num_ports == 0) {
        return false;
    }

    memset(bridge, 0, sizeof(stp_bridge_t));

    /* Initialize bridge ID */
    stp_build_bridge_id(priority, mac, &bridge->bridge_id);

    /* Initialize root information */
    bridge->root_id = bridge->bridge_id;
    bridge->root_path_cost = 0;
    bridge->root_port = 0;
    bridge->is_root = true;

    /* Set default timers */
    bridge->max_age = STP_DEFAULT_MAX_AGE;
    bridge->hello_time = STP_DEFAULT_HELLO_TIME;
    bridge->forward_delay = STP_DEFAULT_FWD_DELAY;
    bridge->max_hops = 20;

    /* Allocate ports */
    bridge->ports = (stp_port_t *)calloc(num_ports, sizeof(stp_port_t));
    if (bridge->ports == NULL) {
        return false;
    }

    /* Initialize ports */
    for (uint16_t i = 0; i < num_ports; i++) {
        bridge->ports[i].port_id = (0x80 << 8) | (i + 1);  /* Default priority 128 */
        bridge->ports[i].state = STP_PORT_STATE_BLOCKING;
        bridge->ports[i].role = STP_ROLE_DESIGNATED;
        bridge->ports[i].path_cost = STP_DEFAULT_PATH_COST;
        bridge->ports[i].designated_priority = priority;
        bridge->ports[i].designated_cost = 0;
        bridge->ports[i].designated_bridge = bridge->bridge_id;
        bridge->ports[i].designated_port = bridge->ports[i].port_id;
    }

    bridge->num_ports = num_ports;

    return true;
}

/**
 * @brief Cleanup STP bridge
 * @param bridge Bridge structure
 */
void stp_cleanup_bridge(stp_bridge_t *bridge) {
    if (bridge == NULL) return;

    if (bridge->ports != NULL) {
        free(bridge->ports);
        bridge->ports = NULL;
    }
    bridge->num_ports = 0;
}

/**
 * @brief Get port state string
 * @param state Port state
 * @return State string
 */
const char* stp_port_state_name(uint8_t state) {
    switch (state) {
        case STP_PORT_STATE_DISABLED:   return "disabled";
        case STP_PORT_STATE_BLOCKING:   return "blocking";
        case STP_PORT_STATE_LISTENING:  return "listening";
        case STP_PORT_STATE_LEARNING:   return "learning";
        case STP_PORT_STATE_FORWARDING: return "forwarding";
        default:                        return "unknown";
    }
}

/**
 * @brief Get port role string
 * @param role Port role
 * @return Role string
 */
const char* stp_port_role_name(stp_port_role_t role) {
    switch (role) {
        case STP_ROLE_ROOT:        return "Root";
        case STP_ROLE_DESIGNATED:  return "Designated";
        case STP_ROLE_ALTERNATE:   return "Alternate";
        case STP_ROLE_BACKUP:      return "Backup";
        default:                   return "Unknown";
    }
}

/* ============================================================================
 * BPDU Operations
 * ============================================================================ */

/**
 * @brief Build configuration BPDU
 * @param bridge STP bridge
 * @param port Port number
 * @param bpdu Output BPDU
 */
void stp_build_bpdu(const stp_bridge_t *bridge, uint16_t port, stp_bpdu_t *bpdu) {
    bpdu->protocol_id = htons(STP_PROTOCOL_ID);
    bpdu->version = STP_VERSION_ID;
    bpdu->bpdu_type = STP_BPDU_TYPE_CFG;
    bpdu->flags = bridge->topology_change ? STP_FLAG_TC : 0;

    /* Root information */
    bpdu->root_id = bridge->root_id;
    bpdu->root_path_cost = htonl(bridge->root_path_cost);

    /* Bridge information */
    bpdu->bridge_id = bridge->bridge_id;
    bpdu->port_id = htons(bridge->ports[port].port_id);

    /* Timers */
    bpdu->message_age = htons(0);  /* Will be updated when relaying */
    bpdu->max_age = htons(bridge->max_age);
    bpdu->hello_time = htons(bridge->hello_time);
    bpdu->forward_delay = htons(bridge->forward_delay);
}

/**
 * @brief Parse BPDU
 * @param data BPDU data
 * @param len Data length
 * @param bpdu Output BPDU structure
 * @return true on success, false on failure
 */
bool stp_parse_bpdu(const uint8_t *data, size_t len, stp_bpdu_t *bpdu) {
    if (data == NULL || len < sizeof(stp_bpdu_t) || bpdu == NULL) {
        return false;
    }

    memcpy(bpdu, data, sizeof(stp_bpdu_t));

    /* Convert from network to host byte order */
    bpdu->protocol_id = ntohs(bpdu->protocol_id);
    bpdu->root_id.priority = ntohs(bpdu->root_id.priority);
    bpdu->root_path_cost = ntohl(bpdu->root_path_cost);
    bpdu->bridge_id.priority = ntohs(bpdu->bridge_id.priority);
    bpdu->port_id = ntohs(bpdu->port_id);
    bpdu->message_age = ntohs(bpdu->message_age);
    bpdu->max_age = ntohs(bpdu->max_age);
    bpdu->hello_time = ntohs(bpdu->hello_time);
    bpdu->forward_delay = ntohs(bpdu->forward_delay);

    return true;
}

/**
 * @brief Print BPDU information
 * @param bpdu BPDU structure
 */
void stp_print_bpdu(const stp_bpdu_t *bpdu) {
    char id_str[24];

    printf("BPDU:\n");
    printf("  Protocol ID: 0x%04X\n", bpdu->protocol_id);
    printf("  Version: %d\n", bpdu->version);
    printf("  Type: %s\n", bpdu->bpdu_type == STP_BPDU_TYPE_TCN ?
           "TCN" : "Configuration");
    printf("  Flags: 0x%02X%s%s\n", bpdu->flags,
           bpdu->flags & STP_FLAG_TC ? " TC" : "",
           bpdu->flags & STP_FLAG_TC_ACK ? " TC_ACK" : "");
    printf("  Root ID: %s\n", stp_bridge_id_to_string(&bpdu->root_id, id_str));
    printf("  Root Path Cost: %u\n", bpdu->root_path_cost);
    printf("  Bridge ID: %s\n", stp_bridge_id_to_string(&bpdu->bridge_id, id_str));
    printf("  Port ID: 0x%04X\n", bpdu->port_id);
    printf("  Message Age: %d\n", bpdu->message_age);
    printf("  Max Age: %d\n", bpdu->max_age);
    printf("  Hello Time: %d\n", bpdu->hello_time);
    printf("  Forward Delay: %d\n", bpdu->forward_delay);
}

/* ============================================================================
 * STP State Machine
 * ============================================================================ */

/**
 * @brief Process received BPDU
 * @param bridge STP bridge
 * @param port Port number
 * @param bpdu Received BPDU
 * @return true if topology changed, false otherwise
 */
bool stp_process_bpdu(stp_bridge_t *bridge, uint16_t port, const stp_bpdu_t *bpdu) {
    if (bridge == NULL || port >= bridge->num_ports || bpdu == NULL) {
        return false;
    }

    stp_port_t *p = &bridge->ports[port];
    bool topology_changed = false;

    /* Update last BPDU time */
    p->last_bpdu_time = time(NULL);

    /* Check if received BPDU is superior */
    int root_cmp = stp_bridge_id_compare(&bpdu->root_id, &bridge->root_id);

    if (root_cmp < 0 ||
        (root_cmp == 0 && bpdu->root_path_cost < bridge->root_path_cost) ||
        (root_cmp == 0 && bpdu->root_path_cost == bridge->root_path_cost &&
         stp_bridge_id_compare(&bpdu->bridge_id, &bridge->bridge_id) < 0)) {

        /* Superior BPDU received - update root information */
        bridge->root_id = bpdu->root_id;
        bridge->root_path_cost = bpdu->root_path_cost + p->path_cost;
        bridge->root_port = port;
        bridge->is_root = false;

        p->role = STP_ROLE_ROOT;

        printf("Port %d: New root discovered\n", port + 1);
        topology_changed = true;
    }

    /* Check topology change flag */
    if (bpdu->flags & STP_FLAG_TC) {
        bridge->topology_change_detected = true;
        bridge->topology_change_time = time(NULL);
    }

    return topology_changed;
}

/**
 * @brief Run STP state machine
 * @param bridge STP bridge
 */
void stp_run_state_machine(stp_bridge_t *bridge) {
    if (bridge == NULL) return;

    /* Update port states based on roles */
    for (uint16_t i = 0; i < bridge->num_ports; i++) {
        stp_port_t *port = &bridge->ports[i];

        switch (port->state) {
            case STP_PORT_STATE_BLOCKING:
                if (port->role == STP_ROLE_ROOT || port->role == STP_ROLE_DESIGNATED) {
                    port->state = STP_PORT_STATE_LISTENING;
                    port->fwd_delay_timer = bridge->forward_delay;
                }
                break;

            case STP_PORT_STATE_LISTENING:
                if (port->fwd_delay_timer > 0) {
                    port->fwd_delay_timer--;
                } else {
                    port->state = STP_PORT_STATE_LEARNING;
                    port->fwd_delay_timer = bridge->forward_delay;
                }
                break;

            case STP_PORT_STATE_LEARNING:
                if (port->fwd_delay_timer > 0) {
                    port->fwd_delay_timer--;
                } else {
                    port->state = STP_PORT_STATE_FORWARDING;
                    printf("Port %d: Forwarding\n", i + 1);
                }
                break;

            case STP_PORT_STATE_FORWARDING:
                if (port->role == STP_ROLE_ALTERNATE || port->role == STP_ROLE_BACKUP) {
                    port->state = STP_PORT_STATE_BLOCKING;
                }
                break;
        }
    }
}

/**
 * @brief Print bridge information
 * @param bridge STP bridge
 */
void stp_print_bridge(const stp_bridge_t *bridge) {
    char id_str[24];

    printf("\n=== STP Bridge Information ===\n");
    printf("Bridge ID: %s\n", stp_bridge_id_to_string(&bridge->bridge_id, id_str));
    printf("Root ID: %s\n", stp_bridge_id_to_string(&bridge->root_id, id_str));
    printf("Root Path Cost: %u\n", bridge->root_path_cost);
    printf("Root Port: %d\n", bridge->is_root ? 0 : bridge->root_port + 1);
    printf("Is Root: %s\n", bridge->is_root ? "Yes" : "No");
    printf("Topology Change: %s\n", bridge->topology_change ? "Yes" : "No");
    printf("\nPort Information:\n");
    printf("%-6s %-12s %-15s %-10s\n", "Port", "State", "Role", "Path Cost");
    printf("-------------------------------------------\n");

    for (uint16_t i = 0; i < bridge->num_ports; i++) {
        stp_port_t *port = &bridge->ports[i];
        printf("%-6d %-12s %-15s %-10u\n",
               i + 1,
               stp_port_state_name(port->state),
               stp_port_role_name(port->role),
               port->path_cost);
    }
}

/* ============================================================================
 * STP Test
 * ============================================================================ */

/**
 * @brief STP test
 */
void stp_test(void) {
    printf("\n=== Spanning Tree Protocol Test ===\n");

    /* Create bridge */
    stp_bridge_t bridge;
    uint8_t mac[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    if (!stp_init_bridge(&bridge, STP_DEFAULT_PRIORITY, mac, 4)) {
        printf("Failed to initialize bridge\n");
        return;
    }

    stp_print_bridge(&bridge);

    /* Simulate receiving BPDU from another bridge with better priority */
    stp_bpdu_t superior_bpdu;
    uint8_t better_mac[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x00};  /* Lower MAC */

    memset(&superior_bpdu, 0, sizeof(superior_bpdu));
    superior_bpdu.protocol_id = STP_PROTOCOL_ID;
    superior_bpdu.version = STP_VERSION_ID;
    superior_bpdu.bpdu_type = STP_BPDU_TYPE_CFG;
    stp_build_bridge_id(STP_DEFAULT_PRIORITY, better_mac, &superior_bpdu.root_id);
    superior_bpdu.root_path_cost = 0;
    stp_build_bridge_id(STP_DEFAULT_PRIORITY, better_mac, &superior_bpdu.bridge_id);
    superior_bpdu.port_id = 0x8001;
    superior_bpdu.max_age = STP_DEFAULT_MAX_AGE;
    superior_bpdu.hello_time = STP_DEFAULT_HELLO_TIME;
    superior_bpdu.forward_delay = STP_DEFAULT_FWD_DELAY;

    printf("\n--- Received BPDU on Port 1 ---\n");
    stp_print_bpdu(&superior_bpdu);

    /* Process BPDU */
    stp_process_bpdu(&bridge, 0, &superior_bpdu);

    /* Run state machine a few times */
    for (int i = 0; i < 20; i++) {
        stp_run_state_machine(&bridge);
    }

    stp_print_bridge(&bridge);

    /* Test BPDU serialization */
    uint8_t bpdu_data[sizeof(stp_bpdu_t)];
    stp_build_bpdu(&bridge, 1, &superior_bpdu);
    memcpy(bpdu_data, &superior_bpdu, sizeof(stp_bpdu_t));

    printf("\nSerialized BPDU (%zu bytes):\n", sizeof(bpdu_data));
    for (size_t i = 0; i < sizeof(bpdu_data); i++) {
        printf("%02X ", bpdu_data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");

    stp_cleanup_bridge(&bridge);
    printf("\nSTP test completed\n");
}

/* Helper for htons/htonl if not available */
#ifndef htons
    #define htons(x) ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))
    #define ntohs(x) htons(x)
    #define htonl(x) ((((x) >> 24) & 0xFF) | (((x) >> 8) & 0xFF00) | \
                    (((x) & 0xFF00) << 8) | (((x) & 0xFF) << 24))
    #define ntohl(x) htonl(x)
#endif

/* Main entry point */
int main(void) {
    stp_test();
    return 0;
}
```

### 7.3 编译与运行

```bash
gcc -o stp_test stp.c -Wall -Wextra
./stp_test
```

---

## 8. ARP协议实现

### 8.1 ARP包格式

```
┌───────────┬───────────┬───────────┬───────────┐
│HW Type    │Protocol   │HW Len     │Prot Len   │
│(2 bytes)  │Type(2)    │(1 byte)   │(1 byte)   │
├───────────┼───────────┼───────────┼───────────┤
│Operation  │Sender HW  │Sender Prot│Target HW  │
│(2 bytes)  │Address    │Address    │Address    │
├───────────┼───────────┼───────────┼───────────┤
│Target Prot│
│Address    │
└───────────┘
```

### 8.2 ARP完整实现

```c
/**
 * @file arp.c
 * @brief ARP (Address Resolution Protocol) Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * ARP Constants
 * ============================================================================ */

#define ARP_HW_TYPE_ETHERNET    1       /**< Ethernet hardware type */
#define ARP_PROTO_TYPE_IP       0x0800  /**< IPv4 protocol type */

#define ARP_OP_REQUEST          1       /**< ARP Request */
#define ARP_OP_REPLY            2       /**< ARP Reply */
#define ARP_OP_RARP_REQUEST     3       /**< RARP Request */
#define ARP_OP_RARP_REPLY       4       /**< RARP Reply */

#define ARP_HW_ADDR_LEN         6       /**< MAC address length */
#define ARP_PROTO_ADDR_LEN      4       /**< IPv4 address length */

#define ARP_CACHE_SIZE          64      /**< ARP cache size */
#define ARP_CACHE_TIMEOUT       600     /**< Cache entry timeout (seconds) */
#define ARP_MAX_RETRIES         3       /**< Maximum ARP retries */
#define ARP_RETRY_INTERVAL      1       /**< Retry interval (seconds) */

/**
 * @brief ARP packet structure
 */
typedef struct __attribute__((packed)) {
    uint16_t hw_type;        /**< Hardware type */
    uint16_t proto_type;     /**< Protocol type */
    uint8_t  hw_addr_len;    /**< Hardware address length */
    uint8_t  proto_addr_len; /**< Protocol address length */
    uint16_t opcode;         /**< Operation code */
    uint8_t  sender_hw[ARP_HW_ADDR_LEN];     /**< Sender hardware address */
    uint8_t  sender_proto[ARP_PROTO_ADDR_LEN]; /**< Sender protocol address */
    uint8_t  target_hw[ARP_HW_ADDR_LEN];     /**< Target hardware address */
    uint8_t  target_proto[ARP_PROTO_ADDR_LEN]; /**< Target protocol address */
} arp_packet_t;

/**
 * @brief ARP cache entry states
 */
typedef enum {
    ARP_ENTRY_FREE,        /**< Entry is free */
    ARP_ENTRY_INCOMPLETE,  /**< Resolution in progress */
    ARP_ENTRY_COMPLETE,    /**< Resolution complete */
    ARP_ENTRY_STATIC       /**< Static entry */
} arp_entry_state_t;

/**
 * @brief ARP cache entry
 */
typedef struct {
    uint8_t ip[ARP_PROTO_ADDR_LEN];   /**< IP address */
    uint8_t mac[ARP_HW_ADDR_LEN];     /**< MAC address */
    arp_entry_state_t state;          /**< Entry state */
    time_t timestamp;                 /**< Last update time */
    uint8_t retries;                  /**< Retry count */
} arp_cache_entry_t;

/**
 * @brief ARP context
 */
typedef struct {
    uint8_t local_ip[ARP_PROTO_ADDR_LEN];    /**< Local IP address */
    uint8_t local_mac[ARP_HW_ADDR_LEN];      /**< Local MAC address */
    arp_cache_entry_t cache[ARP_CACHE_SIZE]; /**< ARP cache */
    uint32_t requests_sent;                  /**< Statistics */
    uint32_t replies_sent;
    uint32_t requests_received;
    uint32_t replies_received;
} arp_context_t;

/* ============================================================================
 * ARP Packet Operations
 * ============================================================================ */

/**
 * @brief Build ARP packet
 * @param pkt Output ARP packet
 * @param opcode Operation code
 * @param sender_mac Sender MAC address
 * @param sender_ip Sender IP address
 * @param target_mac Target MAC address
 * @param target_ip Target IP address
 */
void arp_build_packet(arp_packet_t *pkt, uint16_t opcode,
                      const uint8_t *sender_mac, const uint8_t *sender_ip,
                      const uint8_t *target_mac, const uint8_t *target_ip) {
    pkt->hw_type = htons(ARP_HW_TYPE_ETHERNET);
    pkt->proto_type = htons(ARP_PROTO_TYPE_IP);
    pkt->hw_addr_len = ARP_HW_ADDR_LEN;
    pkt->proto_addr_len = ARP_PROTO_ADDR_LEN;
    pkt->opcode = htons(opcode);

    memcpy(pkt->sender_hw, sender_mac, ARP_HW_ADDR_LEN);
    memcpy(pkt->sender_proto, sender_ip, ARP_PROTO_ADDR_LEN);
    memcpy(pkt->target_hw, target_mac, ARP_HW_ADDR_LEN);
    memcpy(pkt->target_proto, target_ip, ARP_PROTO_ADDR_LEN);
}

/**
 * @brief Parse ARP packet
 * @param data Raw packet data
 * @param len Data length
 * @param pkt Output ARP packet structure
 * @return true on success, false on failure
 */
bool arp_parse_packet(const uint8_t *data, size_t len, arp_packet_t *pkt) {
    if (data == NULL || len < sizeof(arp_packet_t) || pkt == NULL) {
        return false;
    }

    memcpy(pkt, data, sizeof(arp_packet_t));

    /* Convert from network to host byte order */
    pkt->hw_type = ntohs(pkt->hw_type);
    pkt->proto_type = ntohs(pkt->proto_type);
    pkt->opcode = ntohs(pkt->opcode);

    /* Validate */
    if (pkt->hw_type != ARP_HW_TYPE_ETHERNET ||
        pkt->proto_type != ARP_PROTO_TYPE_IP ||
        pkt->hw_addr_len != ARP_HW_ADDR_LEN ||
        pkt->proto_addr_len != ARP_PROTO_ADDR_LEN) {
        return false;
    }

    return true;
}

/**
 * @brief Get operation code name
 * @param opcode Operation code
 * @return Name string
 */
const char* arp_opcode_name(uint16_t opcode) {
    switch (opcode) {
        case ARP_OP_REQUEST:      return "REQUEST";
        case ARP_OP_REPLY:        return "REPLY";
        case ARP_OP_RARP_REQUEST: return "RARP REQUEST";
        case ARP_OP_RARP_REPLY:   return "RARP REPLY";
        default:                  return "UNKNOWN";
    }
}

/**
 * @brief Format IP address to string
 * @param ip IP address bytes
 * @param str Output string buffer
 * @return str pointer
 */
char* arp_ip_to_string(const uint8_t *ip, char *str) {
    sprintf(str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return str;
}

/**
 * @brief Format MAC address to string
 * @param mac MAC address bytes
 * @param str Output string buffer
 * @return str pointer
 */
char* arp_mac_to_string(const uint8_t *mac, char *str) {
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return str;
}

/**
 * @brief Parse IP address string
 * @param str IP string
 * @param ip Output IP bytes
 * @return true on success, false on failure
 */
bool arp_parse_ip(const char *str, uint8_t *ip) {
    int a, b, c, d;
    if (sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        return false;
    }
    ip[0] = a; ip[1] = b; ip[2] = c; ip[3] = d;
    return true;
}

/**
 * @brief Compare IP addresses
 * @param a First IP
 * @param b Second IP
 * @return 0 if equal, non-zero otherwise
 */
int arp_ip_compare(const uint8_t *a, const uint8_t *b) {
    return memcmp(a, b, ARP_PROTO_ADDR_LEN);
}

/**
 * @brief Compare MAC addresses
 * @param a First MAC
 * @param b Second MAC
 * @return 0 if equal, non-zero otherwise
 */
int arp_mac_compare(const uint8_t *a, const uint8_t *b) {
    return memcmp(a, b, ARP_HW_ADDR_LEN);
}

/**
 * @brief Print ARP packet
 * @param pkt ARP packet
 */
void arp_print_packet(const arp_packet_t *pkt) {
    char ip_str[16], mac_str[18];

    printf("ARP %s:\n", arp_opcode_name(pkt->opcode));
    printf("  Sender: %s / %s\n",
           arp_mac_to_string(pkt->sender_hw, mac_str),
           arp_ip_to_string(pkt->sender_proto, ip_str));
    printf("  Target: %s / %s\n",
           arp_mac_to_string(pkt->target_hw, mac_str),
           arp_ip_to_string(pkt->target_proto, ip_str));
}

/* ============================================================================
 * ARP Cache Operations
 * ============================================================================ */

/**
 * @brief Initialize ARP context
 * @param ctx ARP context
 * @param local_ip Local IP address
 * @param local_mac Local MAC address
 * @return true on success, false on failure
 */
bool arp_init(arp_context_t *ctx, const uint8_t *local_ip, const uint8_t *local_mac) {
    if (ctx == NULL || local_ip == NULL || local_mac == NULL) {
        return false;
    }

    memset(ctx, 0, sizeof(arp_context_t));
    memcpy(ctx->local_ip, local_ip, ARP_PROTO_ADDR_LEN);
    memcpy(ctx->local_mac, local_mac, ARP_HW_ADDR_LEN);

    /* Initialize cache entries as free */
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        ctx->cache[i].state = ARP_ENTRY_FREE;
    }

    return true;
}

/**
 * @brief Find cache entry by IP
 * @param ctx ARP context
 * @param ip IP address to find
 * @return Pointer to entry, or NULL if not found
 */
arp_cache_entry_t* arp_cache_find(arp_context_t *ctx, const uint8_t *ip) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (ctx->cache[i].state != ARP_ENTRY_FREE &&
            arp_ip_compare(ctx->cache[i].ip, ip) == 0) {
            return &ctx->cache[i];
        }
    }
    return NULL;
}

/**
 * @brief Find free cache entry
 * @param ctx ARP context
 * @return Pointer to free entry, or NULL if cache full
 */
arp_cache_entry_t* arp_cache_find_free(arp_context_t *ctx) {
    /* First, look for truly free entry */
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (ctx->cache[i].state == ARP_ENTRY_FREE) {
            return &ctx->cache[i];
        }
    }

    /* No free entry - look for oldest complete entry */
    time_t oldest = time(NULL);
    arp_cache_entry_t *oldest_entry = NULL;

    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (ctx->cache[i].state == ARP_ENTRY_COMPLETE &&
            ctx->cache[i].timestamp < oldest) {
            oldest = ctx->cache[i].timestamp;
            oldest_entry = &ctx->cache[i];
        }
    }

    return oldest_entry;
}

/**
 * @brief Add entry to ARP cache
 * @param ctx ARP context
 * @param ip IP address
 * @param mac MAC address
 * @param is_static Static entry flag
 * @return true on success, false on failure
 */
bool arp_cache_add(arp_context_t *ctx, const uint8_t *ip, const uint8_t *mac,
                   bool is_static) {
    if (ctx == NULL || ip == NULL || mac == NULL) {
        return false;
    }

    /* Check if entry already exists */
    arp_cache_entry_t *entry = arp_cache_find(ctx, ip);

    if (entry == NULL) {
        /* Find free entry */
        entry = arp_cache_find_free(ctx);
        if (entry == NULL) {
            return false;  /* Cache full */
        }
    }

    /* Update entry */
    memcpy(entry->ip, ip, ARP_PROTO_ADDR_LEN);
    memcpy(entry->mac, mac, ARP_HW_ADDR_LEN);
    entry->state = is_static ? ARP_ENTRY_STATIC : ARP_ENTRY_COMPLETE;
    entry->timestamp = time(NULL);
    entry->retries = 0;

    char ip_str[16], mac_str[18];
    printf("ARP cache: Added %s -> %s (%s)\n",
           arp_ip_to_string(ip, ip_str),
           arp_mac_to_string(mac, mac_str),
           is_static ? "static" : "dynamic");

    return true;
}

/**
 * @brief Remove entry from ARP cache
 * @param ctx ARP context
 * @param ip IP address to remove
 * @return true on success, false on failure
 */
bool arp_cache_remove(arp_context_t *ctx, const uint8_t *ip) {
    arp_cache_entry_t *entry = arp_cache_find(ctx, ip);
    if (entry == NULL) {
        return false;
    }

    entry->state = ARP_ENTRY_FREE;
    return true;
}

/**
 * @brief Lookup MAC address for IP
 * @param ctx ARP context
 * @param ip IP address to resolve
 * @param mac Output MAC address
 * @return true if found, false otherwise
 */
bool arp_lookup(arp_context_t *ctx, const uint8_t *ip, uint8_t *mac) {
    if (ctx == NULL || ip == NULL || mac == NULL) {
        return false;
    }

    /* Check for broadcast */
    uint8_t broadcast_ip[4] = {255, 255, 255, 255};
    if (arp_ip_compare(ip, broadcast_ip) == 0) {
        memset(mac, 0xFF, ARP_HW_ADDR_LEN);
        return true;
    }

    /* Check cache */
    arp_cache_entry_t *entry = arp_cache_find(ctx, ip);
    if (entry != NULL && entry->state == ARP_ENTRY_COMPLETE) {
        memcpy(mac, entry->mac, ARP_HW_ADDR_LEN);
        return true;
    }

    return false;
}

/**
 * @brief Create ARP request
 * @param ctx ARP context
 * @param target_ip Target IP to resolve
 * @param pkt Output ARP packet
 */
void arp_create_request(arp_context_t *ctx, const uint8_t *target_ip,
                        arp_packet_t *pkt) {
    uint8_t zero_mac[ARP_HW_ADDR_LEN] = {0};
    arp_build_packet(pkt, ARP_OP_REQUEST,
                     ctx->local_mac, ctx->local_ip,
                     zero_mac, target_ip);
    ctx->requests_sent++;
}

/**
 * @brief Create ARP reply
 * @param ctx ARP context
 * @param request Received ARP request
 * @param pkt Output ARP packet
 */
void arp_create_reply(arp_context_t *ctx, const arp_packet_t *request,
                      arp_packet_t *pkt) {
    arp_build_packet(pkt, ARP_OP_REPLY,
                     ctx->local_mac, ctx->local_ip,
                     request->sender_hw, request->sender_proto);
    ctx->replies_sent++;
}

/**
 * @brief Process received ARP packet
 * @param ctx ARP context
 * @param pkt Received ARP packet
 */
void arp_process_packet(arp_context_t *ctx, const arp_packet_t *pkt) {
    if (ctx == NULL || pkt == NULL) return;

    char ip_str[16];

    switch (pkt->opcode) {
        case ARP_OP_REQUEST:
            ctx->requests_received++;
            printf("ARP: Received request for %s\n",
                   arp_ip_to_string(pkt->target_proto, ip_str));

            /* Check if request is for us */
            if (arp_ip_compare(pkt->target_proto, ctx->local_ip) == 0) {
                printf("ARP: Request is for us - would send reply\n");
                /* Add sender to cache */
                arp_cache_add(ctx, pkt->sender_proto, pkt->sender_hw, false);
            }
            break;

        case ARP_OP_REPLY:
            ctx->replies_received++;
            printf("ARP: Received reply from %s\n",
                   arp_ip_to_string(pkt->sender_proto, ip_str));
            /* Add to cache */
            arp_cache_add(ctx, pkt->sender_proto, pkt->sender_hw, false);
            break;
    }
}

/**
 * @brief Print ARP cache
 * @param ctx ARP context
 */
void arp_print_cache(const arp_context_t *ctx) {
    char ip_str[16], mac_str[18];
    const char *state_names[] = {"FREE", "INCOMPLETE", "COMPLETE", "STATIC"};

    printf("\n=== ARP Cache ===\n");
    printf("%-16s %-18s %-12s %s\n", "IP Address", "MAC Address", "State", "Age");
    printf("---------------------------------------------------------\n");

    time_t now = time(NULL);

    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (ctx->cache[i].state != ARP_ENTRY_FREE) {
            printf("%-16s %-18s %-12s %lds\n",
                   arp_ip_to_string(ctx->cache[i].ip, ip_str),
                   arp_mac_to_string(ctx->cache[i].mac, mac_str),
                   state_names[ctx->cache[i].state],
                   (long)(now - ctx->cache[i].timestamp));
        }
    }
}

/**
 * @brief Print ARP statistics
 * @param ctx ARP context
 */
void arp_print_stats(const arp_context_t *ctx) {
    printf("\n=== ARP Statistics ===\n");
    printf("Requests sent: %u\n", ctx->requests_sent);
    printf("Replies sent: %u\n", ctx->replies_sent);
    printf("Requests received: %u\n", ctx->requests_received);
    printf("Replies received: %u\n", ctx->replies_received);
}

/* ============================================================================
 * ARP Test
 * ============================================================================ */

/**
 * @brief ARP test
 */
void arp_test(void) {
    printf("\n=== ARP Protocol Test ===\n");

    /* Initialize ARP context */
    arp_context_t ctx;
    uint8_t local_ip[4] = {192, 168, 1, 10};
    uint8_t local_mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

    if (!arp_init(&ctx, local_ip, local_mac)) {
        printf("Failed to initialize ARP context\n");
        return;
    }

    char ip_str[16], mac_str[18];
    printf("Local IP: %s\n", arp_ip_to_string(local_ip, ip_str));
    printf("Local MAC: %s\n", arp_mac_to_string(local_mac, mac_str));

    /* Test ARP packet building */
    arp_packet_t pkt;
    uint8_t target_ip[4] = {192, 168, 1, 1};

    printf("\n--- Creating ARP Request ---\n");
    arp_create_request(&ctx, target_ip, &pkt);
    arp_print_packet(&pkt);

    /* Test ARP reply */
    printf("\n--- Creating ARP Reply ---\n");
    arp_create_reply(&ctx, &pkt, &pkt);
    arp_print_packet(&pkt);

    /* Test ARP cache operations */
    printf("\n--- ARP Cache Operations ---\n");

    uint8_t ip1[4] = {192, 168, 1, 1};
    uint8_t mac1[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
    arp_cache_add(&ctx, ip1, mac1, false);

    uint8_t ip2[4] = {192, 168, 1, 2};
    uint8_t mac2[6] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    arp_cache_add(&ctx, ip2, mac2, true);  /* Static entry */

    arp_print_cache(&ctx);

    /* Test lookup */
    printf("\n--- ARP Lookup Test ---\n");
    uint8_t resolved_mac[6];
    if (arp_lookup(&ctx, ip1, resolved_mac)) {
        printf("Resolved %s -> %s\n",
               arp_ip_to_string(ip1, ip_str),
               arp_mac_to_string(resolved_mac, mac_str));
    }

    /* Test processing received packet */
    printf("\n--- Processing ARP Request ---\n");
    arp_packet_t request;
    uint8_t sender_ip[4] = {192, 168, 1, 100};
    uint8_t sender_mac[6] = {0x99, 0x88, 0x77, 0x66, 0x55, 0x44};
    arp_build_packet(&request, ARP_OP_REQUEST,
                     sender_mac, sender_ip,
                     local_mac, local_ip);  /* Request for us */
    arp_process_packet(&ctx, &request);

    arp_print_cache(&ctx);
    arp_print_stats(&ctx);

    printf("\nARP test completed\n");
}

/* Byte order conversion helpers */
#ifndef htons
    #define htons(x) ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))
    #define ntohs(x) htons(x)
    #define htonl(x) ((((x) >> 24) & 0xFF) | (((x) >> 8) & 0xFF00) | \
                    (((x) & 0xFF00) << 8) | (((x) & 0xFF) << 24))
    #define ntohl(x) htonl(x)
#endif

/* Main entry point */
int main(void) {
    arp_test();
    return 0;
}
```

### 8.3 编译与运行

```bash
gcc -o arp_test arp.c -Wall -Wextra
./arp_test
```

---

## 9. CRC校验算法

### 9.1 CRC算法实现

```c
/**
 * @file crc_algorithms.c
 * @brief CRC Algorithms Implementation
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * CRC-8 Implementations
 * ============================================================================ */

/**
 * @brief Calculate CRC-8 (ATM/SDH)
 * Polynomial: x^8 + x^2 + x + 1 (0x07)
 * @param data Input data
 * @param len Data length
 * @return CRC-8 value
 */
uint8_t crc8_sdh(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x07;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Calculate CRC-8 (CRC-8-CCITT)
 * Polynomial: x^8 + x^7 + x^3 + x^2 + 1 (0x8D)
 * @param data Input data
 * @param len Data length
 * @return CRC-8 value
 */
uint8_t crc8_ccitt(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80) {
                crc = (crc << 1) ^ 0x8D;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Calculate CRC-8 (Dallas/Maxim 1-Wire)
 * Polynomial: x^8 + x^5 + x^4 + 1 (0x31)
 * @param data Input data
 * @param len Data length
 * @return CRC-8 value
 */
uint8_t crc8_dallas(const uint8_t *data, size_t len) {
    uint8_t crc = 0x00;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x8C;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/* ============================================================================
 * CRC-16 Implementations
 * ============================================================================ */

/**
 * @brief CRC-16 lookup table (CCITT)
 */
static uint16_t crc16_ccitt_table[256];
static bool crc16_ccitt_table_initialized = false;

/**
 * @brief Initialize CRC-16 CCITT lookup table
 */
void crc16_ccitt_init_table(void) {
    if (crc16_ccitt_table_initialized) return;

    const uint16_t polynomial = 0x1021;  /* x^16 + x^12 + x^5 + 1 */

    for (int i = 0; i < 256; i++) {
        uint16_t crc = (uint16_t)(i << 8);
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
        crc16_ccitt_table[i] = crc;
    }

    crc16_ccitt_table_initialized = true;
}

/**
 * @brief Calculate CRC-16 CCITT (0xFFFF initial)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_ccitt(const uint8_t *data, size_t len) {
    crc16_ccitt_init_table();

    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = (crc << 8) ^ crc16_ccitt_table[((crc >> 8) ^ data[i]) & 0xFF];
    }

    return crc;
}

/**
 * @brief Calculate CRC-16 CCITT (0x1D0F initial for Modbus)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_ccitt_modbus(const uint8_t *data, size_t len) {
    crc16_ccitt_init_table();

    uint16_t crc = 0x1D0F;

    for (size_t i = 0; i < len; i++) {
        crc = (crc << 8) ^ crc16_ccitt_table[((crc >> 8) ^ data[i]) & 0xFF];
    }

    return crc;
}

/**
 * @brief Calculate CRC-16 (IBM/ARC/SDLC)
 * Polynomial: x^16 + x^15 + x^2 + 1 (0x8005)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_ibm(const uint8_t *data, size_t len) {
    uint16_t crc = 0x0000;

    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x8005;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/**
 * @brief Calculate CRC-16 (Modbus)
 * @param data Input data
 * @param len Data length
 * @return CRC-16 value
 */
uint16_t crc16_modbus(const uint8_t *data, size_t len) {
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/* ============================================================================
 * CRC-32 Implementations
 * ============================================================================ */

/**
 * @brief CRC-32 lookup table (Ethernet)
 */
static uint32_t crc32_table[256];
static bool crc32_table_initialized = false;

/**
 * @brief Initialize CRC-32 lookup table
 */
void crc32_init_table(void) {
    if (crc32_table_initialized) return;

    const uint32_t polynomial = 0xEDB88320;  /* Reversed polynomial */

    for (uint32_t i = 0; i < 256; i++) {
        uint32_t crc = i;
        for (int j = 0; j < 8; j++) {
            if (crc & 1) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
        crc32_table[i] = crc;
    }

    crc32_table_initialized = true;
}

/**
 * @brief Calculate CRC-32 (Ethernet/IEEE 802.3)
 * @param data Input data
 * @param len Data length
 * @return CRC-32 value
 */
uint32_t crc32_ethernet(const uint8_t *data, size_t len) {
    crc32_init_table();

    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/**
 * @brief Calculate CRC-32 (POSIX cksum)
 * @param data Input data
 * @param len Data length
 * @return CRC-32 value
 */
uint32_t crc32_posix(const uint8_t *data, size_t len) {
    crc32_init_table();

    uint32_t crc = 0x00000000;

    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }

    return crc ^ 0xFFFFFFFF;
}

/**
 * @brief Calculate CRC-32 (MPEG-2)
 * @param data Input data
 * @param len Data length
 * @return CRC-32 value
 */
uint32_t crc32_mpeg2(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    const uint32_t polynomial = 0x04C11DB7;

    for (size_t i = 0; i < len; i++) {
        crc ^= ((uint32_t)data[i]) << 24;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ polynomial;
            } else {
                crc <<= 1;
            }
        }
    }

    return crc;
}

/* ============================================================================
 * CRC Test
 * ============================================================================ */

/**
 * @brief CRC test
 */
void crc_test(void) {
    printf("\n=== CRC Algorithms Test ===\n");

    const char *test_data = "123456789";
    size_t len = strlen(test_data);

    printf("Test data: \"%s\" (%zu bytes)\n\n", test_data, len);

    /* CRC-8 tests */
    printf("CRC-8 (SDH/ATM):     0x%02X (expected: 0xF4)\n", crc8_sdh((const uint8_t *)test_data, len));
    printf("CRC-8 (CCITT):       0x%02X\n", crc8_ccitt((const uint8_t *)test_data, len));
    printf("CRC-8 (Dallas):      0x%02X (expected: 0xA1)\n", crc8_dallas((const uint8_t *)test_data, len));

    /* CRC-16 tests */
    printf("\nCRC-16 (CCITT):      0x%04X (expected: 0xE5CC)\n", crc16_ccitt((const uint8_t *)test_data, len));
    printf("CRC-16 (CCITT Modbus):0x%04X\n", crc16_ccitt_modbus((const uint8_t *)test_data, len));
    printf("CRC-16 (IBM):        0x%04X (expected: 0xBB3D)\n", crc16_ibm((const uint8_t *)test_data, len));
    printf("CRC-16 (Modbus):     0x%04X (expected: 0x4B37)\n", crc16_modbus((const uint8_t *)test_data, len));

    /* CRC-32 tests */
    printf("\nCRC-32 (Ethernet):   0x%08X (expected: 0xCBF43926)\n", crc32_ethernet((const uint8_t *)test_data, len));
    printf("CRC-32 (POSIX):      0x%08X\n", crc32_posix((const uint8_t *)test_data, len));
    printf("CRC-32 (MPEG-2):     0x%08X (expected: 0x0376E6E7)\n", crc32_mpeg2((const uint8_t *)test_data, len));

    /* Verify with appended CRC */
    printf("\n--- Verification Test ---\n");
    uint32_t crc = crc32_ethernet((const uint8_t *)test_data, len);
    uint8_t data_with_crc[64];
    memcpy(data_with_crc, test_data, len);
    data_with_crc[len] = crc & 0xFF;
    data_with_crc[len + 1] = (crc >> 8) & 0xFF;
    data_with_crc[len + 2] = (crc >> 16) & 0xFF;
    data_with_crc[len + 3] = (crc >> 24) & 0xFF;

    uint32_t verify = crc32_ethernet(data_with_crc, len + 4);
    printf("Verification CRC: 0x%08X (should be 0x2144DF1C for Ethernet)\n", verify);

    printf("\nCRC test completed\n");
}

/* Main entry point */
int main(void) {
    crc_test();
    return 0;
}
```

### 9.3 编译与运行

```bash
gcc -o crc_algorithms_test crc_algorithms.c -Wall -Wextra
./crc_algorithms_test
```

---

## 10. 流量控制实现

### 10.1 滑动窗口协议

```c
/**
 * @file flow_control.c
 * @brief Data Link Layer Flow Control Implementations
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * Sliding Window Protocol
 * ============================================================================ */

#define WINDOW_SIZE         8       /**< Window size */
#define MAX_SEQ_NUM         16      /**< Maximum sequence number */
#define FRAME_TIMEOUT_MS    1000    /**< Frame timeout in milliseconds */
#define MAX_FRAME_SIZE      256     /**< Maximum frame size */
#define BUFFER_SIZE         (WINDOW_SIZE * MAX_FRAME_SIZE)

/**
 * @brief Frame structure
 */
typedef struct {
    uint8_t seq_num;         /**< Sequence number */
    uint8_t ack_num;         /**< Acknowledgment number */
    uint16_t len;            /**< Data length */
    uint8_t data[MAX_FRAME_SIZE]; /**< Data payload */
} window_frame_t;

/**
 * @brief Sliding window transmitter state
 */
typedef struct {
    uint8_t base;            /**< Base of window */
    uint8_t next_seq;        /**< Next sequence number to use */
    uint8_t window_size;     /**< Current window size */
    bool acked[WINDOW_SIZE]; /**< Acknowledgment status */
    window_frame_t frames[WINDOW_SIZE]; /**< Buffered frames */
    clock_t send_time[WINDOW_SIZE]; /**< Send timestamps */
} sw_transmitter_t;

/**
 * @brief Sliding window receiver state
 */
typedef struct {
    uint8_t expected_seq;    /**< Next expected sequence number */
    uint8_t window_size;     /**< Window size */
    bool received[WINDOW_SIZE]; /**< Reception status */
    window_frame_t frames[WINDOW_SIZE]; /**< Received frames buffer */
} sw_receiver_t;

/**
 * @brief Initialize sliding window transmitter
 * @param tx Transmitter state
 * @param window_size Window size
 */
void sw_init_transmitter(sw_transmitter_t *tx, uint8_t window_size) {
    if (tx == NULL) return;

    memset(tx, 0, sizeof(sw_transmitter_t));
    tx->base = 0;
    tx->next_seq = 0;
    tx->window_size = window_size < WINDOW_SIZE ? window_size : WINDOW_SIZE;

    for (int i = 0; i < WINDOW_SIZE; i++) {
        tx->acked[i] = true;  /* Initially all slots are free */
    }
}

/**
 * @brief Initialize sliding window receiver
 * @param rx Receiver state
 * @param window_size Window size
 */
void sw_init_receiver(sw_receiver_t *rx, uint8_t window_size) {
    if (rx == NULL) return;

    memset(rx, 0, sizeof(sw_receiver_t));
    rx->expected_seq = 0;
    rx->window_size = window_size < WINDOW_SIZE ? window_size : WINDOW_SIZE;

    for (int i = 0; i < WINDOW_SIZE; i++) {
        rx->received[i] = false;
    }
}

/**
 * @brief Check if transmitter can send
 * @param tx Transmitter state
 * @return true if can send, false otherwise
 */
bool sw_can_send(const sw_transmitter_t *tx) {
    if (tx == NULL) return false;

    uint8_t available = (tx->base + tx->window_size - tx->next_seq) % MAX_SEQ_NUM;
    return available < tx->window_size;
}

/**
 * @brief Send frame using sliding window
 * @param tx Transmitter state
 * @param data Data to send
 * @param len Data length
 * @return Sequence number, or -1 on error
 */
int sw_send(sw_transmitter_t *tx, const uint8_t *data, uint16_t len) {
    if (tx == NULL || data == NULL || len > MAX_FRAME_SIZE) {
        return -1;
    }

    if (!sw_can_send(tx)) {
        printf("Window full, cannot send\n");
        return -1;
    }

    uint8_t seq = tx->next_seq;
    uint8_t idx = seq % WINDOW_SIZE;

    tx->frames[idx].seq_num = seq;
    tx->frames[idx].len = len;
    memcpy(tx->frames[idx].data, data, len);
    tx->acked[idx] = false;
    tx->send_time[idx] = clock();

    tx->next_seq = (tx->next_seq + 1) % MAX_SEQ_NUM;

    printf("TX: Sent frame with seq=%d, len=%d\n", seq, len);
    return seq;
}

/**
 * @brief Process acknowledgment
 * @param tx Transmitter state
 * @param ack_num Acknowledgment number
 */
void sw_process_ack(sw_transmitter_t *tx, uint8_t ack_num) {
    if (tx == NULL) return;

    /* Cumulative acknowledgment - mark all up to ack_num as acked */
    while (tx->base != ack_num) {
        uint8_t idx = tx->base % WINDOW_SIZE;
        tx->acked[idx] = true;
        printf("TX: Frame %d acknowledged\n", tx->base);
        tx->base = (tx->base + 1) % MAX_SEQ_NUM;
    }
}

/**
 * @brief Check for timeouts and return timed out frames
 * @param tx Transmitter state
 * @param timed_out_seqs Output array for timed out sequence numbers
 * @param max_count Maximum number to return
 * @return Number of timed out frames
 */
int sw_check_timeouts(sw_transmitter_t *tx, uint8_t *timed_out_seqs, int max_count) {
    if (tx == NULL || timed_out_seqs == NULL) return 0;

    int count = 0;
    clock_t now = clock();

    for (uint8_t seq = tx->base; seq != tx->next_seq; seq = (seq + 1) % MAX_SEQ_NUM) {
        uint8_t idx = seq % WINDOW_SIZE;

        if (!tx->acked[idx]) {
            double elapsed_ms = ((double)(now - tx->send_time[idx]) / CLOCKS_PER_SEC) * 1000;

            if (elapsed_ms > FRAME_TIMEOUT_MS) {
                if (count < max_count) {
                    timed_out_seqs[count++] = seq;
                    printf("TX: Frame %d timed out (%.0f ms)\n", seq, elapsed_ms);
                }
            }
        }
    }

    return count;
}

/**
 * @brief Receive frame using sliding window
 * @param rx Receiver state
 * @param frame Received frame
 * @param output_data Output buffer for in-order data
 * @param output_max Maximum output buffer size
 * @return Number of bytes delivered, or -1 on error
 */
int sw_receive(sw_receiver_t *rx, const window_frame_t *frame,
               uint8_t *output_data, uint16_t output_max) {
    if (rx == NULL || frame == NULL || output_data == NULL) return -1;

    uint8_t seq = frame->seq_num;
    uint8_t idx = seq % WINDOW_SIZE;

    /* Check if frame is within receive window */
    uint8_t window_end = (rx->expected_seq + rx->window_size) % MAX_SEQ_NUM;
    bool in_window;

    if (rx->expected_seq < window_end) {
        in_window = (seq >= rx->expected_seq && seq < window_end);
    } else {
        in_window = (seq >= rx->expected_seq || seq < window_end);
    }

    if (!in_window) {
        printf("RX: Frame %d outside window (expected %d)\n", seq, rx->expected_seq);
        return 0;  /* Still need to send ACK */
    }

    /* Store frame */
    rx->frames[idx] = *frame;
    rx->received[idx] = true;
    printf("RX: Received frame %d\n", seq);

    /* Deliver in-order frames */
    uint16_t delivered = 0;
    while (rx->received[rx->expected_seq % WINDOW_SIZE]) {
        idx = rx->expected_seq % WINDOW_SIZE;
        window_frame_t *f = &rx->frames[idx];

        if (delivered + f->len <= output_max) {
            memcpy(&output_data[delivered], f->data, f->len);
            delivered += f->len;
        }

        rx->received[idx] = false;
        printf("RX: Delivered frame %d\n", rx->expected_seq);
        rx->expected_seq = (rx->expected_seq + 1) % MAX_SEQ_NUM;
    }

    return delivered;
}

/**
 * @brief Get expected sequence number for ACK
 * @param rx Receiver state
 * @return Expected sequence number
 */
uint8_t sw_get_ack_num(const sw_receiver_t *rx) {
    return rx ? rx->expected_seq : 0;
}

/* ============================================================================
 * Stop-and-Wait Protocol
 * ============================================================================ */

/**
 * @brief Stop-and-Wait transmitter state
 */
typedef struct {
    uint8_t seq_num;         /**< Current sequence number */
    bool waiting_for_ack;    /**< Waiting for acknowledgment */
    clock_t send_time;       /**< Send timestamp */
    window_frame_t last_frame; /**< Last sent frame for retransmission */
} saw_transmitter_t;

/**
 * @brief Stop-and-Wait receiver state
 */
typedef struct {
    uint8_t expected_seq;    /**< Expected sequence number */
} saw_receiver_t;

/**
 * @brief Initialize Stop-and-Wait transmitter
 * @param tx Transmitter state
 */
void saw_init_transmitter(saw_transmitter_t *tx) {
    if (tx == NULL) return;

    memset(tx, 0, sizeof(saw_transmitter_t));
    tx->seq_num = 0;
    tx->waiting_for_ack = false;
}

/**
 * @brief Initialize Stop-and-Wait receiver
 * @param rx Receiver state
 */
void saw_init_receiver(saw_receiver_t *rx) {
    if (rx == NULL) return;

    memset(rx, 0, sizeof(saw_receiver_t));
    rx->expected_seq = 0;
}

/**
 * @brief Send frame using Stop-and-Wait
 * @param tx Transmitter state
 * @param data Data to send
 * @param len Data length
 * @return true on success, false if waiting for ACK
 */
bool saw_send(saw_transmitter_t *tx, const uint8_t *data, uint16_t len) {
    if (tx == NULL || data == NULL || len > MAX_FRAME_SIZE) {
        return false;
    }

    if (tx->waiting_for_ack) {
        printf("S&W TX: Still waiting for ACK of frame %d\n", tx->seq_num ^ 1);
        return false;
    }

    tx->last_frame.seq_num = tx->seq_num;
    tx->last_frame.len = len;
    memcpy(tx->last_frame.data, data, len);
    tx->send_time = clock();
    tx->waiting_for_ack = true;

    printf("S&W TX: Sent frame with seq=%d, len=%d\n", tx->seq_num, len);
    return true;
}

/**
 * @brief Process ACK in Stop-and-Wait
 * @param tx Transmitter state
 * @param ack_num Acknowledgment number
 * @return true if valid ACK, false otherwise
 */
bool saw_process_ack(saw_transmitter_t *tx, uint8_t ack_num) {
    if (tx == NULL || !tx->waiting_for_ack) return false;

    /* In S&W, ACK contains expected sequence number */
    if (ack_num == ((tx->seq_num + 1) % 2)) {
        printf("S&W TX: ACK %d received, toggle sequence\n", ack_num);
        tx->seq_num = ack_num;
        tx->waiting_for_ack = false;
        return true;
    }

    printf("S&W TX: Unexpected ACK %d (expected %d)\n",
           ack_num, (tx->seq_num + 1) % 2);
    return false;
}

/**
 * @brief Check for timeout and retransmit if needed
 * @param tx Transmitter state
 * @return true if retransmission needed, false otherwise
 */
bool saw_check_timeout(saw_transmitter_t *tx) {
    if (tx == NULL || !tx->waiting_for_ack) return false;

    clock_t now = clock();
    double elapsed_ms = ((double)(now - tx->send_time) / CLOCKS_PER_SEC) * 1000;

    if (elapsed_ms > FRAME_TIMEOUT_MS) {
        printf("S&W TX: Timeout! Retransmitting frame %d\n", tx->seq_num);
        tx->send_time = now;
        return true;
    }

    return false;
}

/**
 * @brief Receive frame using Stop-and-Wait
 * @param rx Receiver state
 * @param frame Received frame
 * @param output_data Output buffer
 * @param output_max Maximum output size
 * @return Number of bytes delivered, or -1 on error
 */
int saw_receive(saw_receiver_t *rx, const window_frame_t *frame,
                uint8_t *output_data, uint16_t output_max) {
    if (rx == NULL || frame == NULL || output_data == NULL) return -1;

    if (frame->seq_num == rx->expected_seq) {
        /* Correct frame received */
        uint16_t len = frame->len < output_max ? frame->len : output_max;
        memcpy(output_data, frame->data, len);

        printf("S&W RX: Received expected frame %d, delivering %d bytes\n",
               frame->seq_num, len);

        /* Toggle expected sequence */
        rx->expected_seq = (rx->expected_seq + 1) % 2;
        return len;
    } else {
        /* Duplicate or out-of-order frame */
        printf("S&W RX: Unexpected frame %d (expected %d)\n",
               frame->seq_num, rx->expected_seq);
        return 0;  /* Still need to ACK */
    }
}

/**
 * @brief Get expected sequence number for ACK in S&W
 * @param rx Receiver state
 * @return Expected sequence number
 */
uint8_t saw_get_ack_num(const saw_receiver_t *rx) {
    return rx ? rx->expected_seq : 0;
}

/* ============================================================================
 * Rate-Based Flow Control
 * ============================================================================ */

/**
 * @brief Token bucket for rate control
 */
typedef struct {
    uint32_t tokens;         /**< Current tokens */
    uint32_t bucket_size;    /**< Maximum bucket size */
    uint32_t rate;           /**< Token refill rate (tokens per second) */
    clock_t last_update;     /**< Last update time */
} token_bucket_t;

/**
 * @brief Initialize token bucket
 * @param bucket Token bucket
 * @param rate Token rate (tokens per second)
 * @param burst_size Maximum bucket size (burst capacity)
 */
void token_bucket_init(token_bucket_t *bucket, uint32_t rate, uint32_t burst_size) {
    if (bucket == NULL) return;

    bucket->tokens = burst_size;  /* Start with full bucket */
    bucket->bucket_size = burst_size;
    bucket->rate = rate;
    bucket->last_update = clock();
}

/**
 * @brief Add tokens to bucket based on elapsed time
 * @param bucket Token bucket
 */
void token_bucket_refill(token_bucket_t *bucket) {
    if (bucket == NULL) return;

    clock_t now = clock();
    double elapsed_sec = (double)(now - bucket->last_update) / CLOCKS_PER_SEC;

    uint32_t new_tokens = (uint32_t)(elapsed_sec * bucket->rate);
    bucket->tokens = (bucket->tokens + new_tokens > bucket->bucket_size) ?
                     bucket->bucket_size : bucket->tokens + new_tokens;
    bucket->last_update = now;
}

/**
 * @brief Try to consume tokens from bucket
 * @param bucket Token bucket
 * @param tokens Number of tokens to consume
 * @return true if tokens consumed, false otherwise
 */
bool token_bucket_consume(token_bucket_t *bucket, uint32_t tokens) {
    if (bucket == NULL) return false;

    token_bucket_refill(bucket);

    if (bucket->tokens >= tokens) {
        bucket->tokens -= tokens;
        return true;
    }

    return false;
}

/**
 * @brief Get current token count
 * @param bucket Token bucket
 * @return Current tokens
 */
uint32_t token_bucket_get_tokens(token_bucket_t *bucket) {
    if (bucket == NULL) return 0;
    token_bucket_refill(bucket);
    return bucket->tokens;
}

/* ============================================================================
 * Flow Control Test
 * ============================================================================ */

/**
 * @brief Flow control test
 */
void flow_control_test(void) {
    printf("\n=== Flow Control Test ===\n");

    /* Test sliding window */
    printf("\n--- Sliding Window Protocol ---\n");
    sw_transmitter_t sw_tx;
    sw_receiver_t sw_rx;

    sw_init_transmitter(&sw_tx, 4);
    sw_init_receiver(&sw_rx, 4);

    /* Send frames */
    uint8_t data1[] = "Hello";
    uint8_t data2[] = "World";
    uint8_t data3[] = "Sliding";
    uint8_t data4[] = "Window";
    uint8_t data5[] = "Test";  /* This should fail (window full) */

    sw_send(&sw_tx, data1, sizeof(data1));
    sw_send(&sw_tx, data2, sizeof(data2));
    sw_send(&sw_tx, data3, sizeof(data3));
    sw_send(&sw_tx, data4, sizeof(data4));
    sw_send(&sw_tx, data5, sizeof(data5));  /* Window full */

    printf("TX: Window base=%d, next=%d\n", sw_tx.base, sw_tx.next_seq);

    /* Simulate frame reception and ACKs */
    window_frame_t frame;
    uint8_t output[256];
    int delivered;

    /* Process frame 0 */
    frame.seq_num = 0;
    frame.len = sizeof(data1);
    memcpy(frame.data, data1, sizeof(data1));
    delivered = sw_receive(&sw_rx, &frame, output, sizeof(output));
    printf("RX: Delivered %d bytes from frame 0\n", delivered);

    /* Send ACK for frame 1 (cumulative) */
    printf("RX: Sending ACK %d\n", sw_get_ack_num(&sw_rx));
    sw_process_ack(&sw_tx, sw_get_ack_num(&sw_rx));
    printf("TX: After ACK, window base=%d, next=%d\n", sw_tx.base, sw_tx.next_seq);

    /* Process frame 2 (out of order) */
    frame.seq_num = 2;
    frame.len = sizeof(data3);
    memcpy(frame.data, data3, sizeof(data3));
    delivered = sw_receive(&sw_rx, &frame, output, sizeof(output));
    printf("RX: Delivered %d bytes from frame 2 (buffered, out of order)\n", delivered);

    /* Process frame 1 (completes sequence) */
    frame.seq_num = 1;
    frame.len = sizeof(data2);
    memcpy(frame.data, data2, sizeof(data2));
    delivered = sw_receive(&sw_rx, &frame, output, sizeof(output));
    printf("RX: Delivered %d bytes total (frames 1,2 delivered)\n", delivered);

    /* Test Stop-and-Wait */
    printf("\n--- Stop-and-Wait Protocol ---\n");
    saw_transmitter_t saw_tx;
    saw_receiver_t saw_rx;

    saw_init_transmitter(&saw_tx);
    saw_init_receiver(&saw_rx);

    uint8_t saw_data1[] = "S&W Test 1";
    uint8_t saw_data2[] = "S&W Test 2";

    saw_send(&saw_tx, saw_data1, sizeof(saw_data1));
    saw_send(&saw_tx, saw_data2, sizeof(saw_data2));  /* Should fail, waiting for ACK */

    /* Simulate ACK */
    saw_process_ack(&saw_tx, 1);
    saw_send(&saw_tx, saw_data2, sizeof(saw_data2));  /* Now should succeed */

    /* Test token bucket */
    printf("\n--- Token Bucket Rate Control ---\n");
    token_bucket_t bucket;
    token_bucket_init(&bucket, 100, 200);  /* 100 tokens/sec, 200 burst */

    printf("Initial tokens: %u\n", token_bucket_get_tokens(&bucket));

    /* Consume tokens */
    for (int i = 0; i < 5; i++) {
        if (token_bucket_consume(&bucket, 50)) {
            printf("Consumed 50 tokens, remaining: %u\n", bucket.tokens);
        } else {
            printf("Not enough tokens (have %u)\n", bucket.tokens);
        }
    }

    /* Try to consume more than available */
    if (!token_bucket_consume(&bucket, 100)) {
        printf("Correctly rejected consumption (only %u tokens)\n", bucket.tokens);
    }

    printf("\nFlow control test completed\n");
}

/* Byte order helpers */
#ifndef htons
    #define htons(x) ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))
    #define ntohs(x) htons(x)
#endif

/* Main entry point */
int main(void) {
    flow_control_test();
    return 0;
}
```

### 10.3 编译与运行

```bash
gcc -o flow_control_test flow_control.c -Wall -Wextra
./flow_control_test
```

---
