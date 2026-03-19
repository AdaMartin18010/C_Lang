/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\01_BLE_GATT.md
 * Line: 271
 * Language: c
 * Block ID: 4a5f391d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 特征属性 */
#define GATT_PROP_BROADCAST      0x01
#define GATT_PROP_READ           0x02
#define GATT_PROP_WRITE_NO_RESP  0x04
#define GATT_PROP_WRITE          0x08
#define GATT_PROP_NOTIFY         0x10
#define GATT_PROP_INDICATE       0x20
#define GATT_PROP_AUTH_SIGN      0x40
#define GATT_PROP_EXT_PROP       0x80

/* GATT特征结构 */
typedef struct {
    uint16_t    uuid16;
    uint8_t     properties;     /* GATT属性 */
    uint8_t     permissions;    /* ATT权限 */
    uint16_t    value_len;
    uint8_t    *value;
    uint16_t    max_len;

    /* CCCD - 客户端配置 */
    uint16_t    cccd_handle;
    uint16_t    cccd_value;     /* bit0: notify, bit1: indicate */

    /* 回调 */
    void (*on_write)(struct GattCharacteristic *chr,
                     const uint8_t *data, uint16_t len);
    void (*on_read)(struct GattCharacteristic *chr);
} GattCharacteristic;

/* GATT服务结构 */
typedef struct {
    uint16_t            uuid16;
    uint16_t            start_handle;
    uint16_t            end_handle;
    GattCharacteristic *characteristics;
    uint8_t             char_count;
    bool                is_primary;
} GattService;
