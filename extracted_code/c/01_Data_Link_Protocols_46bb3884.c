/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 2553
 * Language: c
 * Block ID: 46bb3884
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
