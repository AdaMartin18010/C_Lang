/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 687
 * Language: c
 * Block ID: 4864dd8a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
