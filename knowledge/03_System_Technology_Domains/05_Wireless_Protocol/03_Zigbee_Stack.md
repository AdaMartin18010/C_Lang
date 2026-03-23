# Zigbee协议栈C实现


---

## 📑 目录

- [Zigbee协议栈C实现](#zigbee协议栈c实现)
  - [📑 目录](#-目录)
  - [1. Zigbee协议栈概述](#1-zigbee协议栈概述)
    - [1.1 协议栈架构](#11-协议栈架构)
    - [1.2 关键特性](#12-关键特性)
  - [2. 核心数据结构定义](#2-核心数据结构定义)
  - [3. MAC层实现](#3-mac层实现)
  - [4. 网络层实现](#4-网络层实现)
  - [5. 简化的Zigbee节点实现](#5-简化的zigbee节点实现)
  - [6. 总结](#6-总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. Zigbee协议栈概述

Zigbee是一种基于IEEE 802.15.4标准的低功耗、短距离无线通信协议，广泛应用于智能家居、工业控制和物联网领域。

### 1.1 协议栈架构

```text
┌─────────────────────────────────────────────────────────┐
│              应用层 (APL) - 应用对象/配置文件             │
├─────────────────────────────────────────────────────────┤
│         应用支持子层 (APS) - 数据服务和安全               │
├─────────────────────────────────────────────────────────┤
│              网络层 (NWK) - 路由和寻址                    │
├─────────────────────────────────────────────────────────┤
│        MAC层 (802.15.4) - 信道接入和帧结构                │
├─────────────────────────────────────────────────────────┤
│           物理层 (PHY) - 射频收发和调制                   │
└─────────────────────────────────────────────────────────┘
```

### 1.2 关键特性

| 特性 | 参数 |
|------|------|
| 频段 | 2.4 GHz / 915 MHz / 868 MHz |
| 数据速率 | 250 kbps (2.4 GHz) |
| 传输范围 | 10-100 米 |
| 网络节点数 | 最多 65,535 个 |
| 拓扑结构 | 星型、树型、网状 |

---

## 2. 核心数据结构定义

```c
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// IEEE 802.15.4 地址长度
#define IEEE_ADDR_LEN       8
#define SHORT_ADDR_LEN      2
#define MAX_PAYLOAD_LEN     127
#define MAX_DEVICES         32
#define MAX_CHILDREN        8
#define CHANNEL_PAGE        0
#define DEFAULT_CHANNEL     11

// PAN ID (Personal Area Network Identifier)
typedef uint16_t panid_t;

// 短地址类型
typedef uint16_t short_addr_t;

// 扩展地址 (64位IEEE地址)
typedef struct {
    uint8_t addr[IEEE_ADDR_LEN];
} ieee_addr_t;

// 地址模式
typedef enum {
    ADDR_MODE_NONE      = 0,
    ADDR_MODE_RESERVED  = 1,
    ADDR_MODE_SHORT     = 2,    // 16位短地址
    ADDR_MODE_EXTENDED  = 3,    // 64位扩展地址
} addr_mode_t;

// 设备类型
typedef enum {
    DEVICE_COORDINATOR  = 0,    // 协调器
    DEVICE_ROUTER       = 1,    // 路由器
    DEVICE_END_DEVICE   = 2,    // 终端设备
} device_type_t;

// 网络状态
typedef enum {
    NETWORK_DOWN        = 0,
    NETWORK_JOINING     = 1,
    NETWORK_JOINED      = 2,
    NETWORK_LEAVING     = 3,
    NETWORK_REJOINING   = 4,
} network_state_t;

// MAC帧控制字段
typedef struct {
    uint16_t frame_type     : 3;    // 帧类型
    uint16_t security       : 1;    // 安全使能
    uint16_t frame_pending  : 1;    // 帧挂起
    uint16_t ar             : 1;    // 确认请求
    uint16_t panid_comp     : 1;    // PAN ID压缩
    uint16_t reserved       : 3;    // 保留
    uint16_t dest_addr_mode : 2;    // 目的地址模式
    uint16_t frame_version  : 2;    // 帧版本
    uint16_t src_addr_mode  : 2;    // 源地址模式
} mac_fcf_t;

// MAC层帧头
typedef struct {
    mac_fcf_t   fcf;
    uint8_t     seq_num;
    panid_t     dest_panid;
    ieee_addr_t dest_addr;
    panid_t     src_panid;
    ieee_addr_t src_addr;
} mac_header_t;

// 网络层帧头
typedef struct {
    uint16_t    frame_control;
    uint8_t     dest_addr[SHORT_ADDR_LEN];
    uint8_t     src_addr[SHORT_ADDR_LEN];
    uint8_t     radius;
    uint8_t     seq_num;
    uint8_t     dest_ieee_addr[IEEE_ADDR_LEN];  // 可选
    uint8_t     src_ieee_addr[IEEE_ADDR_LEN];   // 可选
    uint16_t    multicast_control;               // 可选
} nwk_header_t;

// 设备表项
typedef struct {
    ieee_addr_t     ieee_addr;
    short_addr_t    short_addr;
    device_type_t   device_type;
    uint8_t         lqi;            // 链路质量
    int8_t          rssi;           // 信号强度
    bool            rx_on_when_idle;
    uint32_t        last_seen;
    bool            valid;
} device_entry_t;

// 邻居表
typedef struct {
    device_entry_t  devices[MAX_DEVICES];
    uint8_t         count;
} neighbor_table_t;

// 子节点表
typedef struct {
    short_addr_t    short_addr;
    ieee_addr_t     ieee_addr;
    device_type_t   type;
    bool            valid;
} child_entry_t;

// Zigbee节点上下文
typedef struct {
    // 设备标识
    ieee_addr_t         ieee_addr;
    short_addr_t        short_addr;
    panid_t             pan_id;
    device_type_t       device_type;

    // 网络状态
    network_state_t     network_state;
    uint8_t             channel;

    // 父节点信息 (终端设备和路由器)
    short_addr_t        parent_short_addr;
    ieee_addr_t         parent_ieee_addr;
    uint8_t             depth;          // 网络深度

    // 表管理
    neighbor_table_t    neighbors;
    child_entry_t       children[MAX_CHILDREN];
    uint8_t             child_count;

    // 序列号
    uint8_t             mac_seq_num;
    uint8_t             nwk_seq_num;
    uint8_t             aps_counter;

    // 安全密钥 (简化)
    uint8_t             network_key[16];
    bool                security_enabled;
} zigbee_node_t;

// APS层帧
typedef struct {
    uint8_t     frame_control;
    uint8_t     dest_endpoint;
    uint8_t     cluster_id[2];
    uint8_t     profile_id[2];
    uint8_t     src_endpoint;
    uint8_t     aps_counter;
    uint8_t     payload[MAX_PAYLOAD_LEN];
    uint8_t     payload_len;
} aps_frame_t;

// 回调函数类型
typedef void (*data_received_cb_t)(short_addr_t src,
                                    const uint8_t *data,
                                    uint8_t len);
typedef void (*network_status_cb_t)(network_state_t state);
```

---

## 3. MAC层实现

```c
/**
 * 构建MAC层帧头
 */
static uint8_t build_mac_header(uint8_t *buffer, const mac_header_t *hdr)
{
    uint8_t pos = 0;

    // 帧控制字段 (2字节)
    uint16_t fcf = 0;
    fcf |= (hdr->fcf.frame_type & 0x07);
    fcf |= (hdr->fcf.security & 0x01) << 3;
    fcf |= (hdr->fcf.frame_pending & 0x01) << 4;
    fcf |= (hdr->fcf.ar & 0x01) << 5;
    fcf |= (hdr->fcf.panid_comp & 0x01) << 6;
    fcf |= (hdr->fcf.dest_addr_mode & 0x03) << 10;
    fcf |= (hdr->fcf.frame_version & 0x03) << 12;
    fcf |= (hdr->fcf.src_addr_mode & 0x03) << 14;

    buffer[pos++] = fcf & 0xFF;
    buffer[pos++] = (fcf >> 8) & 0xFF;

    // 序列号
    buffer[pos++] = hdr->seq_num;

    // 目的PAN ID (如果存在)
    if (hdr->fcf.dest_addr_mode != ADDR_MODE_NONE) {
        buffer[pos++] = hdr->dest_panid & 0xFF;
        buffer[pos++] = (hdr->dest_panid >> 8) & 0xFF;
    }

    // 目的地址
    if (hdr->fcf.dest_addr_mode == ADDR_MODE_SHORT) {
        // 16位短地址
        buffer[pos++] = hdr->dest_addr.addr[0];
        buffer[pos++] = hdr->dest_addr.addr[1];
    } else if (hdr->fcf.dest_addr_mode == ADDR_MODE_EXTENDED) {
        // 64位扩展地址
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            buffer[pos++] = hdr->dest_addr.addr[IEEE_ADDR_LEN - 1 - i];
        }
    }

    // 源PAN ID (如果未压缩且源地址存在)
    if (hdr->fcf.src_addr_mode != ADDR_MODE_NONE &&
        !hdr->fcf.panid_comp) {
        buffer[pos++] = hdr->src_panid & 0xFF;
        buffer[pos++] = (hdr->src_panid >> 8) & 0xFF;
    }

    // 源地址
    if (hdr->fcf.src_addr_mode == ADDR_MODE_SHORT) {
        buffer[pos++] = hdr->src_addr.addr[0];
        buffer[pos++] = hdr->src_addr.addr[1];
    } else if (hdr->fcf.src_addr_mode == ADDR_MODE_EXTENDED) {
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            buffer[pos++] = hdr->src_addr.addr[IEEE_ADDR_LEN - 1 - i];
        }
    }

    return pos;
}

/**
 * 解析MAC层帧头
 */
static uint8_t parse_mac_header(const uint8_t *buffer, mac_header_t *hdr)
{
    uint8_t pos = 0;

    // 解析帧控制字段
    uint16_t fcf = buffer[pos] | (buffer[pos + 1] << 8);
    pos += 2;

    hdr->fcf.frame_type = fcf & 0x07;
    hdr->fcf.security = (fcf >> 3) & 0x01;
    hdr->fcf.frame_pending = (fcf >> 4) & 0x01;
    hdr->fcf.ar = (fcf >> 5) & 0x01;
    hdr->fcf.panid_comp = (fcf >> 6) & 0x01;
    hdr->fcf.dest_addr_mode = (fcf >> 10) & 0x03;
    hdr->fcf.frame_version = (fcf >> 12) & 0x03;
    hdr->fcf.src_addr_mode = (fcf >> 14) & 0x03;

    // 序列号
    hdr->seq_num = buffer[pos++];

    // 目的PAN ID
    if (hdr->fcf.dest_addr_mode != ADDR_MODE_NONE) {
        hdr->dest_panid = buffer[pos] | (buffer[pos + 1] << 8);
        pos += 2;
    }

    // 目的地址
    if (hdr->fcf.dest_addr_mode == ADDR_MODE_SHORT) {
        hdr->dest_addr.addr[0] = buffer[pos++];
        hdr->dest_addr.addr[1] = buffer[pos++];
    } else if (hdr->fcf.dest_addr_mode == ADDR_MODE_EXTENDED) {
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            hdr->dest_addr.addr[IEEE_ADDR_LEN - 1 - i] = buffer[pos++];
        }
    }

    // 源PAN ID
    if (hdr->fcf.src_addr_mode != ADDR_MODE_NONE &&
        !hdr->fcf.panid_comp) {
        hdr->src_panid = buffer[pos] | (buffer[pos + 1] << 8);
        pos += 2;
    }

    // 源地址
    if (hdr->fcf.src_addr_mode == ADDR_MODE_SHORT) {
        hdr->src_addr.addr[0] = buffer[pos++];
        hdr->src_addr.addr[1] = buffer[pos++];
    } else if (hdr->fcf.src_addr_mode == ADDR_MODE_EXTENDED) {
        for (int i = 0; i < IEEE_ADDR_LEN; i++) {
            hdr->src_addr.addr[IEEE_ADDR_LEN - 1 - i] = buffer[pos++];
        }
    }

    return pos;
}

/**
 * 发送MAC层数据帧
 */
int mac_data_request(zigbee_node_t *node,
                      short_addr_t dest_addr,
                      const uint8_t *payload,
                      uint8_t payload_len)
{
    uint8_t frame[256];
    mac_header_t hdr = {0};

    // 配置帧控制字段
    hdr.fcf.frame_type = 1;     // 数据帧
    hdr.fcf.security = 0;       // 无安全
    hdr.fcf.frame_pending = 0;
    hdr.fcf.ar = 1;             // 请求确认
    hdr.fcf.panid_comp = 1;     // PAN ID压缩
    hdr.fcf.dest_addr_mode = ADDR_MODE_SHORT;
    hdr.fcf.frame_version = 1;
    hdr.fcf.src_addr_mode = ADDR_MODE_SHORT;

    hdr.seq_num = node->mac_seq_num++;
    hdr.dest_panid = node->pan_id;
    hdr.dest_addr.addr[0] = dest_addr & 0xFF;
    hdr.dest_addr.addr[1] = (dest_addr >> 8) & 0xFF;
    hdr.src_panid = node->pan_id;
    hdr.src_addr.addr[0] = node->short_addr & 0xFF;
    hdr.src_addr.addr[1] = (node->short_addr >> 8) & 0xFF;

    // 构建帧头
    uint8_t hdr_len = build_mac_header(frame, &hdr);

    // 添加payload
    if (payload_len > MAX_PAYLOAD_LEN - hdr_len - 2) {  // 预留FCS
        payload_len = MAX_PAYLOAD_LEN - hdr_len - 2;
    }
    memcpy(frame + hdr_len, payload, payload_len);

    // 计算并添加FCS (简化 - 实际应为CRC16)
    uint16_t fcs = 0x1234;  // 占位
    frame[hdr_len + payload_len] = fcs & 0xFF;
    frame[hdr_len + payload_len + 1] = (fcs >> 8) & 0xFF;

    // 调用PHY层发送 (模拟)
    printf("[MAC] 发送数据帧到 0x%04X, 长度: %d\n", dest_addr,
           hdr_len + payload_len + 2);

    // 实际硬件调用: phy_transmit(frame, hdr_len + payload_len + 2);

    return 0;
}
```

---

## 4. 网络层实现

```c
// 网络层命令ID
typedef enum {
    NWK_CMD_ROUTE_REQUEST    = 0x01,
    NWK_CMD_ROUTE_REPLY      = 0x02,
    NWK_CMD_NETWORK_STATUS   = 0x03,
    NWK_CMD_LEAVE            = 0x04,
    NWK_CMD_ROUTE_RECORD     = 0x05,
    NWK_CMD_REJOIN_REQUEST   = 0x06,
    NWK_CMD_REJOIN_RESPONSE  = 0x07,
    NWK_CMD_LINK_STATUS      = 0x08,
    NWK_CMD_NETWORK_REPORT   = 0x09,
    NWK_CMD_NETWORK_UPDATE   = 0x0A,
    NWK_CMD_END_DEVICE_TIMEOUT_REQUEST  = 0x0B,
    NWK_CMD_END_DEVICE_TIMEOUT_RESPONSE = 0x0C,
} nwk_command_id_t;

/**
 * 构建网络层数据帧
 */
static uint8_t build_nwk_header(uint8_t *buffer,
                                 const nwk_header_t *hdr,
                                 uint8_t frame_type)
{
    uint8_t pos = 0;

    // 帧控制字段 (2字节)
    uint16_t fcf = 0;
    fcf |= (frame_type & 0x03);         // 帧类型
    fcf |= (0 << 2);                     // 协议版本
    fcf |= (0 << 5);                     // 发现路由
    fcf |= (0 << 6);                     // 多播标志
    fcf |= (0 << 7);                     // 安全
    fcf |= (0 << 8);                     // 源路由
    fcf |= (0 << 9);                     // 目的IEEE地址
    fcf |= (0 << 10);                    // 源IEEE地址

    buffer[pos++] = fcf & 0xFF;
    buffer[pos++] = (fcf >> 8) & 0xFF;

    // 目的地址
    buffer[pos++] = hdr->dest_addr[0];
    buffer[pos++] = hdr->dest_addr[1];

    // 源地址
    buffer[pos++] = hdr->src_addr[0];
    buffer[pos++] = hdr->src_addr[1];

    // 半径
    buffer[pos++] = hdr->radius;

    // 序列号
    buffer[pos++] = hdr->seq_num;

    return pos;
}

/**
 * 网络层加入请求处理
 */
int nwk_handle_join_request(zigbee_node_t *node,
                             const ieee_addr_t *device_addr,
                             device_type_t device_type,
                             uint8_t capability)
{
    if (node->device_type == DEVICE_END_DEVICE) {
        // 终端设备不能处理加入请求
        return -1;
    }

    if (node->child_count >= MAX_CHILDREN) {
        printf("[NWK] 子节点表已满\n");
        return -1;
    }

    // 分配短地址
    short_addr_t new_addr = 0x0001 + node->child_count;
    if (node->device_type == DEVICE_ROUTER) {
        new_addr |= (node->short_addr & 0xFF00);
    }

    // 添加到子节点表
    int idx = node->child_count++;
    node->children[idx].short_addr = new_addr;
    node->children[idx].ieee_addr = *device_addr;
    node->children[idx].type = device_type;
    node->children[idx].valid = true;

    // 添加到邻居表
    if (node->neighbors.count < MAX_DEVICES) {
        device_entry_t *entry = &node->neighbors.devices[node->neighbors.count++];
        entry->ieee_addr = *device_addr;
        entry->short_addr = new_addr;
        entry->device_type = device_type;
        entry->valid = true;
        entry->last_seen = 0;  // 应使用系统时间
    }

    printf("[NWK] 设备 %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X 加入网络, "
           "分配地址: 0x%04X\n",
           device_addr->addr[0], device_addr->addr[1],
           device_addr->addr[2], device_addr->addr[3],
           device_addr->addr[4], device_addr->addr[5],
           device_addr->addr[6], device_addr->addr[7],
           new_addr);

    return 0;
}

/**
 * 发送网络层数据
 */
int nwk_send_data(zigbee_node_t *node,
                   short_addr_t dest_addr,
                   uint8_t radius,
                   const uint8_t *payload,
                   uint8_t payload_len)
{
    if (node->network_state != NETWORK_JOINED) {
        fprintf(stderr, "[NWK] 网络未连接\n");
        return -1;
    }

    uint8_t nwk_frame[256];
    nwk_header_t hdr = {0};

    // 设置网络层头部
    hdr.dest_addr[0] = dest_addr & 0xFF;
    hdr.dest_addr[1] = (dest_addr >> 8) & 0xFF;
    hdr.src_addr[0] = node->short_addr & 0xFF;
    hdr.src_addr[1] = (node->short_addr >> 8) & 0xFF;
    hdr.radius = radius > 0 ? radius : 30;
    hdr.seq_num = node->nwk_seq_num++;

    // 构建网络层帧
    uint8_t hdr_len = build_nwk_header(nwk_frame, &hdr, 0);  // 数据帧
    memcpy(nwk_frame + hdr_len, payload, payload_len);

    // 确定下一跳地址
    short_addr_t next_hop;
    if (dest_addr == 0xFFFC || dest_addr == 0xFFFD || dest_addr == 0xFFFF) {
        // 广播地址
        next_hop = 0xFFFF;
    } else if (dest_addr == node->parent_short_addr) {
        next_hop = node->parent_short_addr;
    } else {
        // 查找路由表或沿树路由
        next_hop = node->parent_short_addr;  // 简化：发往父节点
    }

    // 调用MAC层发送
    return mac_data_request(node, next_hop, nwk_frame, hdr_len + payload_len);
}

/**
 * 处理接收到的网络层帧
 */
void nwk_process_incoming(zigbee_node_t *node,
                          const uint8_t *data,
                          uint8_t len)
{
    if (len < 8) return;

    uint8_t pos = 0;
    uint16_t fcf = data[pos] | (data[pos + 1] << 8);
    pos += 2;

    uint8_t frame_type = fcf & 0x03;

    // 解析地址
    short_addr_t dest_addr = data[pos] | (data[pos + 1] << 8);
    pos += 2;
    short_addr_t src_addr = data[pos] | (data[pos + 1] << 8);
    pos += 2;

    uint8_t radius = data[pos++];
    uint8_t seq_num = data[pos++];

    printf("[NWK] 接收帧: 类型=%d, 源=0x%04X, 目的=0x%04X, 序列=%d\n",
           frame_type, src_addr, dest_addr, seq_num);

    // 检查是否为目的地址
    if (dest_addr != node->short_addr &&
        dest_addr != 0xFFFC &&
        dest_addr != 0xFFFD &&
        dest_addr != 0xFFFF) {
        // 需要转发
        if (radius > 1) {
            // 递减radius并转发
            printf("[NWK] 转发帧到 0x%04X\n", dest_addr);
        }
        return;
    }

    // 处理数据帧
    if (frame_type == 0) {
        // 提取payload并传递给APS层
        printf("[NWK] 数据帧payload长度: %d\n", len - pos);
    } else if (frame_type == 1) {
        // 命令帧
        uint8_t cmd_id = data[pos++];
        printf("[NWK] 命令帧, ID=0x%02X\n", cmd_id);
    }
}
```

---

## 5. 简化的Zigbee节点实现

```c
/**
 * 初始化Zigbee节点
 */
void zigbee_node_init(zigbee_node_t *node,
                       device_type_t type,
                       const ieee_addr_t *addr)
{
    memset(node, 0, sizeof(*node));

    node->ieee_addr = *addr;
    node->device_type = type;
    node->network_state = NETWORK_DOWN;
    node->channel = DEFAULT_CHANNEL;
    node->short_addr = 0xFFFF;  // 未分配

    // 协调器分配0x0000
    if (type == DEVICE_COORDINATOR) {
        node->short_addr = 0x0000;
        node->pan_id = 0x1234;  // 示例PAN ID
        node->network_state = NETWORK_JOINED;
    }

    // 初始化序列号
    node->mac_seq_num = 0;
    node->nwk_seq_num = 0;
    node->aps_counter = 0;

    printf("[Zigbee] 节点初始化完成, 类型=%s\n",
           type == DEVICE_COORDINATOR ? "协调器" :
           type == DEVICE_ROUTER ? "路由器" : "终端设备");
}

/**
 * 网络加入过程 (终端设备/路由器)
 */
int zigbee_join_network(zigbee_node_t *node,
                         panid_t pan_id,
                         uint8_t channel)
{
    if (node->device_type == DEVICE_COORDINATOR) {
        printf("[Zigbee] 协调器已创建网络\n");
        return 0;
    }

    node->network_state = NETWORK_JOINING;
    node->pan_id = pan_id;
    node->channel = channel;

    printf("[Zigbee] 开始加入网络 PAN=0x%04X, 信道=%d\n", pan_id, channel);

    // 步骤1: 扫描网络
    printf("[Zigbee] 执行主动扫描...\n");
    // 模拟发现协调器

    // 步骤2: 发送关联请求
    printf("[Zigbee] 发送关联请求...\n");

    // 步骤3: 等待关联响应
    // 模拟接收到父节点地址
    node->parent_short_addr = 0x0000;
    node->parent_ieee_addr = (ieee_addr_t){{0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77}};
    node->short_addr = 0x0001;  // 父节点分配
    node->depth = 1;

    node->network_state = NETWORK_JOINED;
    printf("[Zigbee] 成功加入网络, 短地址=0x%04X\n", node->short_addr);

    return 0;
}

/**
 * 发送应用数据
 */
int zigbee_send_data(zigbee_node_t *node,
                      short_addr_t dest_addr,
                      uint8_t dest_ep,
                      uint16_t cluster_id,
                      uint16_t profile_id,
                      const uint8_t *data,
                      uint8_t len)
{
    if (node->network_state != NETWORK_JOINED) {
        fprintf(stderr, "[Zigbee] 网络未连接, 无法发送数据\n");
        return -1;
    }

    // 构建APS帧
    uint8_t aps_frame[128];
    uint8_t pos = 0;

    // APS帧控制
    aps_frame[pos++] = 0x00;  // 数据帧, 无安全
    aps_frame[pos++] = dest_ep;
    aps_frame[pos++] = cluster_id & 0xFF;
    aps_frame[pos++] = (cluster_id >> 8) & 0xFF;
    aps_frame[pos++] = profile_id & 0xFF;
    aps_frame[pos++] = (profile_id >> 8) & 0xFF;
    aps_frame[pos++] = 1;  // 源端点
    aps_frame[pos++] = node->aps_counter++;

    // 组地址 (如果适用)
    // 序列号
    aps_frame[pos++] = 0;

    // Payload
    memcpy(aps_frame + pos, data, len);
    pos += len;

    printf("[Zigbee] 发送数据到 0x%04X:%d, Cluster=0x%04X, 长度=%d\n",
           dest_addr, dest_ep, cluster_id, len);

    // 通过网络层发送
    return nwk_send_data(node, dest_addr, 30, aps_frame, pos);
}

/**
 * 打印节点信息
 */
void zigbee_print_node_info(const zigbee_node_t *node)
{
    const char *state_str[] = {"DOWN", "JOINING", "JOINED", "LEAVING", "REJOINING"};
    const char *type_str[] = {"协调器", "路由器", "终端设备"};

    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║                 Zigbee 节点信息                          ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ IEEE地址:  %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X                    ║\n",
           node->ieee_addr.addr[0], node->ieee_addr.addr[1],
           node->ieee_addr.addr[2], node->ieee_addr.addr[3],
           node->ieee_addr.addr[4], node->ieee_addr.addr[5],
           node->ieee_addr.addr[6], node->ieee_addr.addr[7]);
    printf("║ 短地址:    0x%04X                                        ║\n",
           node->short_addr);
    printf("║ PAN ID:    0x%04X                                        ║\n",
           node->pan_id);
    printf("║ 设备类型:  %-12s                                  ║\n",
           type_str[node->device_type]);
    printf("║ 网络状态:  %-12s                                  ║\n",
           state_str[node->network_state]);
    printf("║ 信道:      %d                                             ║\n",
           node->channel);
    printf("║ 网络深度:  %d                                             ║\n",
           node->depth);
    printf("║ 邻居数量:  %d/%d                                          ║\n",
           node->neighbors.count, MAX_DEVICES);
    printf("║ 子节点数:  %d/%d                                          ║\n",
           node->child_count, MAX_CHILDREN);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}

/**
 * 主函数示例
 */
int main(void)
{
    // 创建协调器
    zigbee_node_t coordinator;
    ieee_addr_t coord_addr = {{0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}};
    zigbee_node_init(&coordinator, DEVICE_COORDINATOR, &coord_addr);
    zigbee_print_node_info(&coordinator);

    // 创建终端设备
    zigbee_node_t end_device;
    ieee_addr_t ed_addr = {{0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11}};
    zigbee_node_init(&end_device, DEVICE_END_DEVICE, &ed_addr);

    // 终端设备加入网络
    zigbee_join_network(&end_device, 0x1234, 11);
    zigbee_print_node_info(&end_device);

    // 模拟设备加入处理
    nwk_handle_join_request(&coordinator, &ed_addr,
                            DEVICE_END_DEVICE, 0x8E);

    // 发送数据
    uint8_t payload[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    zigbee_send_data(&end_device, 0x0000, 1, 0x0006, 0x0104,
                     payload, sizeof(payload));

    return 0;
}
```

---

## 6. 总结

本文档展示了一个简化的Zigbee协议栈实现，包含：

1. **MAC层**: IEEE 802.15.4帧结构和传输
2. **网络层**: 地址分配、路由和加入过程
3. **APS层**: 应用支持子层框架
4. **设备类型**: 协调器、路由器、终端设备

实际商用实现需要完整的IEEE 802.15.4 PHY支持、安全加密(AES-128)、更复杂的路由算法和更完善的网络管理。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
