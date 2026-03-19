/*
 * Auto-generated from: 03_System_Technology_Domains\03_Zigbee_Stack.md
 * Line: 746
 * Language: c
 * Block ID: a80e34d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 端点定义
#define ZDO_ENDPOINT        0       // ZDO保留端点
#define AF_BROADCAST_EP     255     // 广播端点
#define AF_MIN_ENDPOINT     1       // 最小应用端点
#define AF_MAX_ENDPOINT     240     // 最大应用端点

// 简单描述符
typedef struct {
    uint8_t endpoint;              // 端点号
    uint16_t profile_id;           // 应用配置文件ID
    uint16_t device_id;            // 设备ID
    uint8_t device_version;        // 设备版本
    uint8_t num_in_clusters;       // 输入簇数量
    uint16_t in_cluster_list[16];  // 输入簇列表
    uint8_t num_out_clusters;      // 输出簇数量
    uint16_t out_cluster_list[16]; // 输出簇列表
} simple_desc_t;

// 应用配置文件示例
typedef enum {
    // 标准配置文件
    ZDO_PROFILE_ID = 0x0000,
    HA_PROFILE_ID = 0x0104,      // Home Automation
    ZSE_PROFILE_ID = 0x0109,     // Smart Energy
    ZLL_PROFILE_ID = 0xC05E,     // Light Link
    // ...
} profile_id_t;

// 簇ID定义
typedef enum {
    // 通用簇
    ZCL_CLUSTER_ID_GEN_BASIC = 0x0000,
    ZCL_CLUSTER_ID_GEN_POWER_CFG = 0x0001,
    ZCL_CLUSTER_ID_GEN_DEVICE_TEMP_CFG = 0x0002,
    ZCL_CLUSTER_ID_GEN_IDENTIFY = 0x0003,
    ZCL_CLUSTER_ID_GEN_GROUPS = 0x0004,
    ZCL_CLUSTER_ID_GEN_SCENES = 0x0005,
    ZCL_CLUSTER_ID_GEN_ON_OFF = 0x0006,
    ZCL_CLUSTER_ID_GEN_LEVEL_CTRL = 0x0008,
    ZCL_CLUSTER_ID_GEN_ALARMS = 0x0009,
    ZCL_CLUSTER_ID_GEN_TIME = 0x000A,
    // ...
} cluster_id_t;
