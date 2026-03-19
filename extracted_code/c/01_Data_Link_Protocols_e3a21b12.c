/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 1376
 * Language: c
 * Block ID: e3a21b12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
