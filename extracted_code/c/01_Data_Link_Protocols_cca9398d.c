/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 3692
 * Language: c
 * Block ID: cca9398d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
