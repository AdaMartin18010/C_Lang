/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 93
 * Language: c
 * Block ID: 43f48312
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
