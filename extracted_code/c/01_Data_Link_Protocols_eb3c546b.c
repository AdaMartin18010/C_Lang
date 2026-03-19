/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 3091
 * Language: c
 * Block ID: eb3c546b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
