/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 1956
 * Language: c
 * Block ID: 2c964fa3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
