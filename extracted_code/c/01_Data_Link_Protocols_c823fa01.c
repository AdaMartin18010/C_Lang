/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 5265
 * Language: c
 * Block ID: c823fa01
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file lightweight_eth_stack.c
 * @brief Lightweight Ethernet Protocol Stack for Embedded Systems
 * @version 1.0
 *
 * Features:
 * - Ethernet II frame processing
 * - VLAN support (802.1Q)
 * - ARP protocol
 * - CRC32 validation
 * - Frame buffer management
 * - Statistics collection
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * Configuration Constants
 * ============================================================================ */

#define ETH_MAC_LEN             6
#define ETH_MAX_FRAME_SIZE      1518
#define ETH_MTU                 1500
#define ETH_MIN_PAYLOAD         46
#define ETH_FCS_LEN             4

#define VLAN_TAG_LEN            4
#define VLAN_MAX_VID            4094

#define ARP_CACHE_SIZE          16
#define ARP_CACHE_TIMEOUT       600

#define MAX_RX_BUFFERS          8
#define MAX_TX_BUFFERS          4

/* EtherType values */
#define ETH_TYPE_IPv4           0x0800
#define ETH_TYPE_ARP            0x0806
#define ETH_TYPE_VLAN           0x8100
#define ETH_TYPE_IPV6           0x86DD

/* ============================================================================
 * Type Definitions
 * ============================================================================ */

/**
 * @brief MAC address type
 */
typedef uint8_t eth_mac_t[ETH_MAC_LEN];

/**
 * @brief IPv4 address type
 */
typedef uint8_t ipv4_addr_t[4];

/**
 * @brief Ethernet frame buffer
 */
typedef struct {
    uint8_t data[ETH_MAX_FRAME_SIZE];
    uint16_t len;
    bool used;
    time_t timestamp;
} eth_buffer_t;

/**
 * @brief VLAN configuration
 */
typedef struct {
    uint16_t vid;
    bool enabled;
    uint8_t priority;
} vlan_config_t;

/**
 * @brief ARP cache entry
 */
typedef struct {
    ipv4_addr_t ip;
    eth_mac_t mac;
    bool valid;
    bool is_static;
    time_t timestamp;
} arp_entry_t;

/**
 * @brief Protocol stack statistics
 */
typedef struct {
    uint32_t rx_frames;
    uint32_t tx_frames;
    uint32_t rx_bytes;
    uint32_t tx_bytes;
    uint32_t rx_errors;
    uint32_t tx_errors;
    uint32_t dropped_frames;
    uint32_t crc_errors;
    uint32_t arp_requests_rx;
    uint32_t arp_replies_rx;
    uint32_t arp_requests_tx;
    uint32_t arp_replies_tx;
} eth_stats_t;

/**
 * @brief Ethernet protocol stack context
 */
typedef struct {
    /* MAC configuration */
    eth_mac_t local_mac;
    ipv4_addr_t local_ip;
    ipv4_addr_t netmask;
    ipv4_addr_t gateway;

    /* VLAN */
    vlan_config_t vlan;
    bool vlan_enabled;

    /* Buffers */
    eth_buffer_t rx_buffers[MAX_RX_BUFFERS];
    eth_buffer_t tx_buffers[MAX_TX_BUFFERS];

    /* ARP cache */
    arp_entry_t arp_cache[ARP_CACHE_SIZE];

    /* Statistics */
    eth_stats_t stats;

    /* State */
    bool initialized;
    time_t start_time;
} eth_stack_t;

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/**
 * @brief Compare two MAC addresses
 */
static bool mac_equal(const eth_mac_t a, const eth_mac_t b) {
    return memcmp(a, b, ETH_MAC_LEN) == 0;
}

/**
 * @brief Check if MAC is broadcast
 */
static bool is_broadcast_mac(const eth_mac_t mac) {
    static const eth_mac_t broadcast = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    return mac_equal(mac, broadcast);
}

/**
 * @brief Check if MAC is multicast
 */
static bool is_multicast_mac(const eth_mac_t mac) {
    return (mac[0] & 0x01) != 0;
}

/**
 * @brief Compare two IP addresses
 */
static bool ip_equal(const ipv4_addr_t a, const ipv4_addr_t b) {
    return memcmp(a, b, 4) == 0;
}

/**
 * @brief Format MAC to string
 */
static void mac_to_str(const eth_mac_t mac, char *str) {
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

/**
 * @brief Format IP to string
 */
static void ip_to_str(const ipv4_addr_t ip, char *str) {
    sprintf(str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
}

/**
 * @brief Parse MAC string
 */
static bool str_to_mac(const char *str, eth_mac_t mac) {
    int bytes[ETH_MAC_LEN];
    if (sscanf(str, "%2x:%2x:%2x:%2x:%2x:%2x",
               &bytes[0], &bytes[1], &bytes[2],
               &bytes[3], &bytes[4], &bytes[5]) != ETH_MAC_LEN) {
        return false;
    }
    for (int i = 0; i < ETH_MAC_LEN; i++) {
        mac[i] = (uint8_t)bytes[i];
    }
    return true;
}

/**
 * @brief Parse IP string
 */
static bool str_to_ip(const char *str, ipv4_addr_t ip) {
    int a, b, c, d;
    if (sscanf(str, "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        return false;
    }
    ip[0] = a; ip[1] = b; ip[2] = c; ip[3] = d;
    return true;
}

/* ============================================================================
 * CRC32 Implementation
 * ============================================================================ */

static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F,
    0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2,
    0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9,
    0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C,
    0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423,
    0xCFBA9599, 0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
    0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D,
    0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457, 0x65B0D9C6, 0x12B7E950,
    0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7,
    0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA,
    0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81,
    0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84,
    0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB,
    0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E,
    0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55,
    0x316E8EEF, 0x4669BE79, 0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28,
    0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
    0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4, 0xF1D4E242,
    0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69,
    0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC,
    0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693,
    0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

/**
 * @brief Calculate CRC32
 */
static uint32_t calc_crc32(const uint8_t *data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc32_table[(crc ^ data[i]) & 0xFF];
    }
    return crc ^ 0xFFFFFFFF;
}

/* ============================================================================
 * Stack Initialization
 * ============================================================================ */

/**
 * @brief Initialize protocol stack
 */
bool eth_stack_init(eth_stack_t *stack, const char *mac_str, const char *ip_str) {
    if (stack == NULL || mac_str == NULL || ip_str == NULL) {
        return false;
    }

    memset(stack, 0, sizeof(eth_stack_t));

    /* Parse addresses */
    if (!str_to_mac(mac_str, stack->local_mac)) {
        return false;
    }
    if (!str_to_ip(ip_str, stack->local_ip)) {
        return false;
    }

    /* Default netmask and gateway */
    stack->netmask[0] = 255; stack->netmask[1] = 255;
    stack->netmask[2] = 255; stack->netmask[3] = 0;
    stack->gateway[0] = stack->local_ip[0];
    stack->gateway[1] = stack->local_ip[1];
    stack->gateway[2] = stack->local_ip[2];
    stack->gateway[3] = 1;

    /* Initialize buffers */
    for (int i = 0; i < MAX_RX_BUFFERS; i++) {
        stack->rx_buffers[i].used = false;
    }
    for (int i = 0; i < MAX_TX_BUFFERS; i++) {
        stack->tx_buffers[i].used = false;
    }

    /* Initialize ARP cache */
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        stack->arp_cache[i].valid = false;
    }

    stack->initialized = true;
    stack->start_time = time(NULL);

    char mac_buf[18], ip_buf[16];
    mac_to_str(stack->local_mac, mac_buf);
    ip_to_str(stack->local_ip, ip_buf);
    printf("EthStack: Initialized\n");
    printf("  MAC: %s\n", mac_buf);
    printf("  IP: %s\n", ip_buf);

    return true;
}

/* ============================================================================
 * Buffer Management
 * ============================================================================ */

/**
 * @brief Allocate receive buffer
 */
static eth_buffer_t* alloc_rx_buffer(eth_stack_t *stack) {
    for (int i = 0; i < MAX_RX_BUFFERS; i++) {
        if (!stack->rx_buffers[i].used) {
            stack->rx_buffers[i].used = true;
            stack->rx_buffers[i].timestamp = time(NULL);
            return &stack->rx_buffers[i];
        }
    }
    return NULL;
}

/**
 * @brief Allocate transmit buffer
 */
static eth_buffer_t* alloc_tx_buffer(eth_stack_t *stack) {
    for (int i = 0; i < MAX_TX_BUFFERS; i++) {
        if (!stack->tx_buffers[i].used) {
            stack->tx_buffers[i].used = true;
            return &stack->tx_buffers[i];
        }
    }
    return NULL;
}

/**
 * @brief Free buffer
 */
static void free_buffer(eth_buffer_t *buf) {
    if (buf) {
        buf->used = false;
        buf->len = 0;
    }
}

/* ============================================================================
 * ARP Implementation
 * ============================================================================ */

/**
 * @brief ARP packet structure
 */
typedef struct __attribute__((packed)) {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8_t hw_len;
    uint8_t proto_len;
    uint16_t opcode;
    eth_mac_t sender_mac;
    ipv4_addr_t sender_ip;
    eth_mac_t target_mac;
    ipv4_addr_t target_ip;
} arp_packet_t;

#define ARP_HW_TYPE_ETH     1
#define ARP_PROTO_IPV4      0x0800
#define ARP_OP_REQUEST      1
#define ARP_OP_REPLY        2

/**
 * @brief Find ARP cache entry
 */
static arp_entry_t* arp_find_entry(eth_stack_t *stack, const ipv4_addr_t ip) {
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (stack->arp_cache[i].valid && ip_equal(stack->arp_cache[i].ip, ip)) {
            return &stack->arp_cache[i];
        }
    }
    return NULL;
}

/**
 * @brief Add ARP cache entry
 */
static void arp_add_entry(eth_stack_t *stack, const ipv4_addr_t ip,
                          const eth_mac_t mac, bool is_static) {
    /* Check for existing entry */
    arp_entry_t *entry = arp_find_entry(stack, ip);

    if (entry == NULL) {
        /* Find free slot */
        for (int i = 0; i < ARP_CACHE_SIZE; i++) {
            if (!stack->arp_cache[i].valid) {
                entry = &stack->arp_cache[i];
                break;
            }
        }
    }

    if (entry) {
        memcpy(entry->ip, ip, 4);
        memcpy(entry->mac, mac, ETH_MAC_LEN);
        entry->valid = true;
        entry->is_static = is_static;
        entry->timestamp = time(NULL);

        char ip_buf[16], mac_buf[18];
        ip_to_str(ip, ip_buf);
        mac_to_str(mac, mac_buf);
        printf("ARP: Cache updated %s -> %s\n", ip_buf, mac_buf);
    }
}

/**
 * @brief Send ARP request
 */
static bool arp_send_request(eth_stack_t *stack, const ipv4_addr_t target_ip) {
    eth_buffer_t *buf = alloc_tx_buffer(stack);
    if (!buf) return false;

    /* Build Ethernet header */
    uint8_t broadcast[ETH_MAC_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    memcpy(buf->data, broadcast, ETH_MAC_LEN);  /* DST */
    memcpy(&buf->data[ETH_MAC_LEN], stack->local_mac, ETH_MAC_LEN);  /* SRC */
    buf->data[12] = 0x08;  /* ARP EtherType */
    buf->data[13] = 0x06;

    /* Build ARP packet */
    arp_packet_t *arp = (arp_packet_t *)&buf->data[14];
    arp->hw_type = htons(ARP_HW_TYPE_ETH);
    arp->proto_type = htons(ARP_PROTO_IPV4);
    arp->hw_len = ETH_MAC_LEN;
    arp->proto_len = 4;
    arp->opcode = htons(ARP_OP_REQUEST);
    memcpy(arp->sender_mac, stack->local_mac, ETH_MAC_LEN);
    memcpy(arp->sender_ip, stack->local_ip, 4);
    memset(arp->target_mac, 0, ETH_MAC_LEN);
    memcpy(arp->target_ip, target_ip, 4);

    buf->len = 14 + sizeof(arp_packet_t);

    /* Add FCS */
    uint32_t crc = calc_crc32(buf->data, buf->len);
    buf->data[buf->len++] = crc & 0xFF;
    buf->data[buf->len++] = (crc >> 8) & 0xFF;
    buf->data[buf->len++] = (crc >> 16) & 0xFF;
    buf->data[buf->len++] = (crc >> 24) & 0xFF;

    stack->stats.arp_requests_tx++;
    stack->stats.tx_frames++;
    stack->stats.tx_bytes += buf->len;

    char ip_buf[16];
    ip_to_str(target_ip, ip_buf);
    printf("ARP: Sent request for %s\n", ip_buf);

    free_buffer(buf);
    return true;
}

/**
 * @brief Send ARP reply
 */
static bool arp_send_reply(eth_stack_t *stack, const arp_packet_t *request) {
    eth_buffer_t *buf = alloc_tx_buffer(stack);
    if (!buf) return false;

    /* Ethernet header */
    memcpy(buf->data, request->sender_mac, ETH_MAC_LEN);
    memcpy(&buf->data[ETH_MAC_LEN], stack->local_mac, ETH_MAC_LEN);
    buf->data[12] = 0x08;
    buf->data[13] = 0x06;

    /* ARP reply */
    arp_packet_t *arp = (arp_packet_t *)&buf->data[14];
    arp->hw_type = htons(ARP_HW_TYPE_ETH);
    arp->proto_type = htons(ARP_PROTO_IPV4);
    arp->hw_len = ETH_MAC_LEN;
    arp->proto_len = 4;
    arp->opcode = htons(ARP_OP_REPLY);
    memcpy(arp->sender_mac, stack->local_mac, ETH_MAC_LEN);
    memcpy(arp->sender_ip, stack->local_ip, 4);
    memcpy(arp->target_mac, request->sender_mac, ETH_MAC_LEN);
    memcpy(arp->target_ip, request->sender_ip, 4);

    buf->len = 14 + sizeof(arp_packet_t);

    uint32_t crc = calc_crc32(buf->data, buf->len);
    buf->data[buf->len++] = crc & 0xFF;
    buf->data[buf->len++] = (crc >> 8) & 0xFF;
    buf->data[buf->len++] = (crc >> 16) & 0xFF;
    buf->data[buf->len++] = (crc >> 24) & 0xFF;

    stack->stats.arp_replies_tx++;
    stack->stats.tx_frames++;

    char ip_buf[16];
    ip_to_str(request->sender_ip, ip_buf);
    printf("ARP: Sent reply to %s\n", ip_buf);

    free_buffer(buf);
    return true;
}

/**
 * @brief Process ARP packet
 */
static void arp_process(eth_stack_t *stack, const uint8_t *data, uint16_t len) {
    if (len < sizeof(arp_packet_t)) return;

    arp_packet_t arp;
    memcpy(&arp, data, sizeof(arp_packet_t));
    arp.hw_type = ntohs(arp.hw_type);
    arp.proto_type = ntohs(arp.proto_type);
    arp.opcode = ntohs(arp.opcode);

    if (arp.hw_type != ARP_HW_TYPE_ETH || arp.proto_type != ARP_PROTO_IPV4) {
        return;
    }

    /* Update cache with sender info */
    arp_add_entry(stack, arp.sender_ip, arp.sender_mac, false);

    switch (arp.opcode) {
        case ARP_OP_REQUEST:
            stack->stats.arp_requests_rx++;
            /* Check if request is for us */
            if (ip_equal(arp.target_ip, stack->local_ip)) {
                arp_send_reply(stack, &arp);
            }
            break;

        case ARP_OP_REPLY:
            stack->stats.arp_replies_rx++;
            printf("ARP: Received reply\n");
            break;
    }
}

/* ============================================================================
 * Frame Processing
 * ============================================================================ */

/**
 * @brief Process received Ethernet frame
 */
void eth_process_frame(eth_stack_t *stack, const uint8_t *data, uint16_t len) {
    if (!stack || !stack->initialized || !data || len < 14) {
        return;
    }

    /* Validate FCS */
    if (len >= 18) {
        uint32_t recv_crc = data[len-4] | (data[len-3] << 8) |
                           (data[len-2] << 16) | (data[len-1] << 24);
        uint32_t calc_crc = calc_crc32(data, len - 4);
        if (recv_crc != calc_crc) {
            stack->stats.crc_errors++;
            stack->stats.rx_errors++;
            return;
        }
        len -= 4;  /* Remove FCS from processing */
    }

    /* Parse Ethernet header */
    eth_mac_t dst_mac, src_mac;
    memcpy(dst_mac, data, ETH_MAC_LEN);
    memcpy(src_mac, &data[ETH_MAC_LEN], ETH_MAC_LEN);
    uint16_t ethertype = (data[12] << 8) | data[13];

    /* Check if frame is for us */
    if (!is_broadcast_mac(dst_mac) && !mac_equal(dst_mac, stack->local_mac)) {
        return;  /* Not for us */
    }

    stack->stats.rx_frames++;
    stack->stats.rx_bytes += len;

    /* Process based on EtherType */
    switch (ethertype) {
        case ETH_TYPE_ARP:
            arp_process(stack, &data[14], len - 14);
            break;

        case ETH_TYPE_VLAN:
            /* VLAN tagged frame */
            if (len >= 18) {
                uint16_t vlan_tci = (data[14] << 8) | data[15];
                uint16_t inner_type = (data[16] << 8) | data[17];
                uint16_t vid = vlan_tci & 0x0FFF;
                uint8_t prio = (vlan_tci >> 13) & 0x07;
                printf("VLAN: VID=%d, Prio=%d, Type=0x%04X\n", vid, prio, inner_type);

                /* Check VLAN membership */
                if (stack->vlan_enabled && stack->vlan.vid == vid) {
                    /* Process inner frame */
                    if (inner_type == ETH_TYPE_ARP) {
                        arp_process(stack, &data[18], len - 18);
                    }
                }
            }
            break;

        case ETH_TYPE_IPv4:
            printf("IPv4: Received %d bytes\n", len - 14);
            break;

        default:
            printf("Unknown EtherType: 0x%04X\n", ethertype);
            break;
    }
}

/* ============================================================================
 * VLAN Configuration
 * ============================================================================ */

/**
 * @brief Enable VLAN
 */
bool eth_vlan_enable(eth_stack_t *stack, uint16_t vid, uint8_t priority) {
    if (!stack || vid < 1 || vid > VLAN_MAX_VID) {
        return false;
    }

    stack->vlan.vid = vid;
    stack->vlan.priority = priority & 0x07;
    stack->vlan.enabled = true;
    stack->vlan_enabled = true;

    printf("VLAN: Enabled with VID=%d, Priority=%d\n", vid, priority);
    return true;
}

/**
 * @brief Disable VLAN
 */
void eth_vlan_disable(eth_stack_t *stack) {
    if (stack) {
        stack->vlan_enabled = false;
        stack->vlan.enabled = false;
        printf("VLAN: Disabled\n");
    }
}

/* ============================================================================
 * Statistics and Info
 * ============================================================================ */

/**
 * @brief Print statistics
 */
void eth_print_stats(const eth_stack_t *stack) {
    if (!stack) return;

    printf("\n=== Ethernet Stack Statistics ===\n");
    printf("RX Frames:  %u\n", stack->stats.rx_frames);
    printf("RX Bytes:   %u\n", stack->stats.rx_bytes);
    printf("TX Frames:  %u\n", stack->stats.tx_frames);
    printf("TX Bytes:   %u\n", stack->stats.tx_bytes);
    printf("RX Errors:  %u\n", stack->stats.rx_errors);
    printf("CRC Errors: %u\n", stack->stats.crc_errors);
    printf("Dropped:    %u\n", stack->stats.dropped_frames);
    printf("\nARP Statistics:\n");
    printf("  Requests RX: %u, TX: %u\n",
           stack->stats.arp_requests_rx, stack->stats.arp_requests_tx);
    printf("  Replies  RX: %u, TX: %u\n",
           stack->stats.arp_replies_rx, stack->stats.arp_replies_tx);

    time_t uptime = time(NULL) - stack->start_time;
    printf("\nUptime: %ld seconds\n", (long)uptime);
}

/**
 * @brief Print ARP cache
 */
void eth_print_arp_cache(const eth_stack_t *stack) {
    if (!stack) return;

    printf("\n=== ARP Cache ===\n");
    printf("%-16s %-18s %-8s\n", "IP Address", "MAC Address", "Type");
    printf("-------------------------------------------\n");

    char ip_buf[16], mac_buf[18];
    for (int i = 0; i < ARP_CACHE_SIZE; i++) {
        if (stack->arp_cache[i].valid) {
            ip_to_str(stack->arp_cache[i].ip, ip_buf);
            mac_to_str(stack->arp_cache[i].mac, mac_buf);
            printf("%-16s %-18s %-8s\n", ip_buf, mac_buf,
                   stack->arp_cache[i].is_static ? "static" : "dynamic");
        }
    }
}

/* ============================================================================
 * Test and Demonstration
 * ============================================================================ */

/**
 * @brief Test the lightweight Ethernet stack
 */
void eth_stack_test(void) {
    printf("\n");
    printf("========================================\n");
    printf("Lightweight Ethernet Protocol Stack Test\n");
    printf("========================================\n\n");

    eth_stack_t stack;

    /* Initialize stack */
    if (!eth_stack_init(&stack, "00:11:22:33:44:55", "192.168.1.100")) {
        printf("Failed to initialize stack\n");
        return;
    }

    /* Enable VLAN */
    eth_vlan_enable(&stack, 100, 3);

    /* Test 1: Receive broadcast ARP request */
    printf("\n--- Test 1: ARP Request ---\n");
    uint8_t arp_request[] = {
        /* DST: Broadcast */
        0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        /* SRC: 00:AA:BB:CC:DD:EE */
        0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
        /* EtherType: ARP */
        0x08, 0x06,
        /* ARP Packet */
        0x00, 0x01,  /* HW Type: Ethernet */
        0x08, 0x00,  /* Protocol: IPv4 */
        0x06,        /* HW Len: 6 */
        0x04,        /* Protocol Len: 4 */
        0x00, 0x01,  /* Opcode: Request */
        /* Sender MAC */
        0x00, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE,
        /* Sender IP: 192.168.1.1 */
        192, 168, 1, 1,
        /* Target MAC: 00:00:00:00:00:00 */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        /* Target IP: 192.168.1.100 (our IP) */
        192, 168, 1, 100,
    };

    uint32_t crc = calc_crc32(arp_request, sizeof(arp_request));
    uint8_t frame[64];
    memcpy(frame, arp_request, sizeof(arp_request));
    frame[sizeof(arp_request)] = crc & 0xFF;
    frame[sizeof(arp_request) + 1] = (crc >> 8) & 0xFF;
    frame[sizeof(arp_request) + 2] = (crc >> 16) & 0xFF;
    frame[sizeof(arp_request) + 3] = (crc >> 24) & 0xFF;

    eth_process_frame(&stack, frame, sizeof(arp_request) + 4);

    /* Test 2: Receive ARP reply */
    printf("\n--- Test 2: ARP Reply ---\n");
    uint8_t arp_reply[] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  /* DST: Our MAC */
        0x00, 0x11, 0x22, 0x33, 0x44, 0x01,  /* SRC */
        0x08, 0x06,  /* ARP */
        0x00, 0x01, 0x08, 0x00, 0x06, 0x04,
        0x00, 0x02,  /* Reply */
        0x00, 0x11, 0x22, 0x33, 0x44, 0x01,  /* Sender MAC */
        192, 168, 1, 1,  /* Sender IP */
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,  /* Target MAC */
        192, 168, 1, 100,  /* Target IP */
    };

    crc = calc_crc32(arp_reply, sizeof(arp_reply));
    memcpy(frame, arp_reply, sizeof(arp_reply));
    frame[sizeof(arp_reply)] = crc & 0xFF;
    frame[sizeof(arp_reply) + 1] = (crc >> 8) & 0xFF;
    frame[sizeof(arp_reply) + 2] = (crc >> 16) & 0xFF;
    frame[sizeof(arp_reply) + 3] = (crc >> 24) & 0xFF;

    eth_process_frame(&stack, frame, sizeof(arp_reply) + 4);

    /* Test 3: VLAN tagged frame */
    printf("\n--- Test 3: VLAN Tagged Frame ---\n");
    uint8_t vlan_frame[] = {
        0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x01,
        0x81, 0x00,  /* VLAN */
        0x00, 0x64,  /* TCI: VID=100, Prio=0 */
        0x08, 0x06,  /* Inner: ARP */
        /* ARP Request */
        0x00, 0x01, 0x08, 0x00, 0x06, 0x04, 0x00, 0x01,
        0x00, 0x11, 0x22, 0x33, 0x44, 0x01,
        192, 168, 1, 1,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        192, 168, 1, 100,
    };

    crc = calc_crc32(vlan_frame, sizeof(vlan_frame));
    memcpy(frame, vlan_frame, sizeof(vlan_frame));
    frame[sizeof(vlan_frame)] = crc & 0xFF;
    frame[sizeof(vlan_frame) + 1] = (crc >> 8) & 0xFF;
    frame[sizeof(vlan_frame) + 2] = (crc >> 16) & 0xFF;
    frame[sizeof(vlan_frame) + 3] = (crc >> 24) & 0xFF;

    eth_process_frame(&stack, frame, sizeof(vlan_frame) + 4);

    /* Print results */
    eth_print_arp_cache(&stack);
    eth_print_stats(&stack);

    printf("\n=== Test Completed ===\n");
}

/* Byte order helpers */
#ifndef htons
    #define htons(x) ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))
    #define ntohs(x) htons(x)
#endif

/* Main entry point */
int main(void) {
    eth_stack_test();
    return 0;
}
