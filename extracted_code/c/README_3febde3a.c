/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\README.md
 * Line: 202
 * Language: c
 * Block ID: 3febde3a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    WIFI_STATE_IDLE,
    WIFI_STATE_SCANNING,
    WIFI_STATE_CONNECTING,
    WIFI_STATE_CONNECTED,
    WIFI_STATE_DISCONNECTING,
} WifiState;

typedef struct {
    WifiState state;
    uint8_t bssid[6];
    uint8_t ssid[32];
    uint8_t channel;
    int16_t rssi;
    uint32_t rx_packets;
    uint32_t tx_packets;
    uint32_t rx_errors;
    uint32_t tx_errors;
} WifiConnection;

// 连接管理
bool wifi_connect(WifiConnection *conn, const char *ssid, const char *psk);
bool wifi_disconnect(WifiConnection *conn);
bool wifi_scan(WifiConnection *conn, void (*on_result)(const char *ssid, int rssi));
