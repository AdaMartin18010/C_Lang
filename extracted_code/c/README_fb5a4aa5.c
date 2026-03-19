/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\README.md
 * Line: 142
 * Language: c
 * Block ID: fb5a4aa5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    STATE_IDLE,
    STATE_ADVERTISING,
    STATE_SCANNING,
    STATE_INITIATING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,
} BleState;

typedef struct {
    BleState state;
    uint16_t conn_handle;
    uint8_t  peer_addr[6];
    uint16_t conn_interval;
    uint16_t slave_latency;
} BleConnection;
