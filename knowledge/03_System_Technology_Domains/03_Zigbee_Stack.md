# Zigbee协议栈实现

## 目录

- [Zigbee协议栈实现](#zigbee协议栈实现)
  - [目录](#目录)
  - [概述](#概述)
    - [Zigbee核心特性](#zigbee核心特性)
    - [Zigbee与其他协议对比](#zigbee与其他协议对比)
  - [Zigbee协议架构](#zigbee协议架构)
    - [协议栈架构图](#协议栈架构图)
    - [协议栈组件关系](#协议栈组件关系)
  - [物理层(PHY)](#物理层phy)
    - [2.4GHz频段信道分布](#24ghz频段信道分布)
    - [物理层数据结构](#物理层数据结构)
    - [物理层关键操作](#物理层关键操作)
  - [MAC层](#mac层)
    - [MAC帧格式](#mac帧格式)
    - [MAC层数据结构](#mac层数据结构)
    - [CSMA-CA算法](#csma-ca算法)
    - [MAC层服务原语](#mac层服务原语)
  - [网络层(NWK)](#网络层nwk)
    - [网络拓扑结构](#网络拓扑结构)
    - [网络层帧格式](#网络层帧格式)
    - [网络层数据结构](#网络层数据结构)
    - [路由算法](#路由算法)
  - [应用层(APS/AF)](#应用层apsaf)
    - [APS层帧格式](#aps层帧格式)
    - [APS层服务](#aps层服务)
    - [应用框架(AF)](#应用框架af)
    - [ZDO（Zigbee设备对象）](#zdozigbee设备对象)
  - [安全机制](#安全机制)
    - [安全架构](#安全架构)
    - [密钥类型](#密钥类型)
    - [安全处理流程](#安全处理流程)
    - [信任中心](#信任中心)
  - [CC2530实现](#cc2530实现)
    - [CC2530硬件架构](#cc2530硬件架构)
    - [CC2530寄存器配置](#cc2530寄存器配置)
    - [CC2530射频操作](#cc2530射频操作)
    - [CC2530 AES加密](#cc2530-aes加密)
  - [802.15.4基础](#802154基础)
    - [802.15.4协议特点](#802154协议特点)
    - [O-QPSK调制（2.4GHz）](#o-qpsk调制24ghz)
    - [超帧结构（信标使能网络）](#超帧结构信标使能网络)
    - [网络建立流程](#网络建立流程)
  - [总结](#总结)

---

## 概述

Zigbee是一种基于IEEE 802.15.4标准的低功耗、短距离无线通信协议，专为低数据率、低功耗、高可靠性的物联网应用设计。
它广泛应用于智能家居、工业控制、医疗监护、智能农业等领域。

### Zigbee核心特性

| 特性 | 参数 |
|------|------|
| **频段** | 2.4GHz (全球)、915MHz (北美)、868MHz (欧洲) |
| **数据率** | 250kbps (2.4GHz)、40kbps (915MHz)、20kbps (868MHz) |
| **传输距离** | 室内30-50米，室外100-300米 |
| **网络节点数** | 理论65000+节点 |
| **功耗** | 休眠电流μA级，发送电流mA级 |
| **协议栈大小** | 4-32KB |

### Zigbee与其他协议对比

| 协议 | 频段 | 数据率 | 功耗 | 应用场景 |
|------|------|--------|------|----------|
| Zigbee | 2.4GHz | 250kbps | 低 | 传感器网络、控制 |
| WiFi | 2.4/5GHz | 11-600Mbps | 高 | 高速数据传输 |
| Bluetooth | 2.4GHz | 1-3Mbps | 中 | 音频、短距离 |
| LoRa | Sub-GHz | 0.3-50kbps | 极低 | 远距离、低速率 |
| NB-IoT | 授权频段 | <100kbps | 低 | 广域覆盖 |

---

## Zigbee协议架构

Zigbee协议栈采用分层架构，基于OSI模型设计，各层之间通过标准接口通信。

### 协议栈架构图

```
┌─────────────────────────────────────────────────────────────────┐
│                         应用层 (APL)                             │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │         应用支持子层 (APS - Application Support)          │  │
│  │   - 应用对象绑定    - 组管理    - 端到端安全               │  │
│  ├──────────────────────────────────────────────────────────┤  │
│  │      Zigbee设备对象 (ZDO - Zigbee Device Object)          │  │
│  │   - 设备发现    - 网络管理    - 安全配置                   │  │
│  ├──────────────────────────────────────────────────────────┤  │
│  │           应用框架 (AF - Application Framework)            │  │
│  │   - 应用配置文件(Profile)    - 簇(Cluster)                 │  │
│  └──────────────────────────────────────────────────────────┘  │
├─────────────────────────────────────────────────────────────────┤
│                         网络层 (NWK)                             │
│   - 网络建立与维护    - 路由发现与管理    - 邻居表管理           │
│   - 地址分配    - 数据包转发    - 网络安全                       │
├─────────────────────────────────────────────────────────────────┤
│                      MAC层 (IEEE 802.15.4)                       │
│   - CSMA-CA信道访问    - 帧封装/解析    - 确认机制               │
│   - 信标管理    - 关联/分离    - 加密(AES-CCM)                   │
├─────────────────────────────────────────────────────────────────┤
│                      物理层 (IEEE 802.15.4)                      │
│   - 射频收发    - 信道选择与扫描    - 能量检测    - 链路质量     │
└─────────────────────────────────────────────────────────────────┘
```

### 协议栈组件关系

```
┌─────────┐   ┌─────────┐   ┌─────────┐
│  应用1  │   │  应用2  │   │  应用3  │
│ Endpoint│   │ Endpoint│   │ Endpoint│
│  1-240  │   │  1-240  │   │  1-240  │
└────┬────┘   └────┬────┘   └────┬────┘
     │             │             │
     └─────────────┼─────────────┘
                   │ AF (应用框架)
                   │
          ┌────────┴────────┐
          │    APS (应用    │
          │   支持子层)     │
          └────────┬────────┘
                   │
          ┌────────┴────────┐
          │  ZDO (Zigbee    │
          │   设备对象)     │
          └────────┬────────┘
                   │
          ┌────────┴────────┐
          │   NWK (网络层)   │
          └────────┬────────┘
                   │
          ┌────────┴────────┐
          │ MAC (介质访问   │
          │   控制层)       │
          └────────┬────────┘
                   │
          ┌────────┴────────┐
          │   PHY (物理层)   │
          └─────────────────┘
```

---

## 物理层(PHY)

物理层负责射频信号的收发、信道选择和能量检测。

### 2.4GHz频段信道分布

```
Frequency (MHz)
│
2485 ─┬───────────────────────────────────────────────────────────
      │
      │     Ch16 Ch17 Ch18 Ch19 Ch20 Ch21 Ch22 Ch23 Ch24 Ch25 Ch26
      │      │    │    │    │    │    │    │    │    │    │    │
      │      ▼    ▼    ▼    ▼    ▼    ▼    ▼    ▼    ▼    ▼    ▼
      │    ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐
      │    │██│ │██│ │██│ │██│ │██│ │██│ │██│ │██│ │██│ │██│ │██│
      │    └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘
2405 ─┤ ◄──┬──► 2MHz带宽，5MHz信道间隔
      │
      │  Ch11 Ch12 Ch13 Ch14 Ch15
      │   │    │    │    │    │
      │   ▼    ▼    ▼    ▼    ▼
      │ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐
      │ │██│ │██│ │██│ │██│ │██│
      │ └──┘ └──┘ └──┘ └──┘ └──┘
2400 ─┴───────────────────────────────────────────────────────────

注：信道11-26可用，与WiFi信道有部分重叠
     WiFi Ch1: 2412MHz (影响Zigbee Ch11-14)
     WiFi Ch6: 2437MHz (影响Zigbee Ch16-19)
     WiFi Ch11: 2462MHz (影响Zigbee Ch21-26)
```

### 物理层数据结构

```c
// 物理层协议数据单元 (PPDU)
typedef struct {
    // 同步头 (SHR)
    uint8_t preamble[4];      // 前导码，用于同步
    uint8_t sfd;              // 帧起始定界符 (0xA7)

    // 物理层头 (PHR)
    uint8_t frame_length;     // PSDU长度 (1-127字节)

    // 物理层服务数据单元 (PSDU) - 即MPDU
    uint8_t psdu[127];
} phy_ppdu_t;

// 物理层状态
typedef enum {
    PHY_IDLE,
    PHY_RX,
    PHY_TX,
    PHY_ED,           // 能量检测
    PHY_CCA,          // 信道空闲评估
} phy_state_t;

// 物理层服务原语
typedef enum {
    PHY_DATA_REQUEST,
    PHY_DATA_CONFIRM,
    PHY_DATA_INDICATION,
    PHY_ED_REQUEST,
    PHY_ED_CONFIRM,
    PHY_CCA_REQUEST,
    PHY_CCA_CONFIRM,
} phy_primitive_t;
```

### 物理层关键操作

```c
// 物理层接口函数

/**
 * 设置射频信道
 * @param channel: 信道号 (11-26)
 * @return: 状态码
 */
int8_t phy_set_channel(uint8_t channel);

/**
 * 发送数据
 * @param data: 数据缓冲区
 * @param length: 数据长度
 * @return: 发送结果
 */
int8_t phy_tx(uint8_t* data, uint8_t length);

/**
 * 接收数据（中断处理）
 */
void phy_rx_isr(void);

/**
 * 能量检测扫描
 * @param channel: 要扫描的信道
 * @return: 能量值 (0-255)
 */
uint8_t phy_ed_scan(uint8_t channel);

/**
 * 信道空闲评估 (CCA)
 * @return: CCA结果
 */
typedef enum {
    PHY_CCA_IDLE = 0,     // 信道空闲
    PHY_CCA_BUSY = 1,     // 信道忙
    PHY_CCA_TRX_OFF = 2,  // 收发器关闭
} phy_cca_status_t;

phy_cca_status_t phy_cca(void);
```

---

## MAC层

MAC层负责介质访问控制、帧封装、确认机制和信道访问。

### MAC帧格式

```
┌─────────────────────────────────────────────────────────────────────┐
│                         MAC帧 (MPDU)                                 │
├─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┤
│ 帧控制   │ 序列号  │ 目的PAN  │ 目的地址│ 源PAN   │ 源地址  │ 安全头  │
│2字节    │ 1字节   │ 0/2字节 │0/2/8字节│ 0/2字节 │0/2/8字节│ 0/14字节│
├─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┤
│                            帧载荷 (0-118字节)                        │
├─────────────────────────────────────────────────────────────────────┤
│                           FCS (2字节)                                │
└─────────────────────────────────────────────────────────────────────┘

帧控制字段 (16位):
┌────┬────┬──────┬──────┬────────┬────────┬──────────┬──────────┐
│帧类型│安全│帧待传│AR请求│PAN ID压缩│保留    │目的地址模式│帧版本  │
│ 3bit│1bit│ 1bit│1bit │  1bit  │ 1bit   │  2bit    │ 2bit    │
├────┴────┴──────┴──────┴────────┴────────┼──────────┼──────────┤
│                 源地址模式                │          │          │
│                  2bit                     │          │          │
└───────────────────────────────────────────┴──────────┴──────────┘

帧类型: 000=信标, 001=数据, 010=确认, 011=MAC命令, 其他=保留
地址模式: 00=无, 01=保留, 10=16位短地址, 11=64位扩展地址
```

### MAC层数据结构

```c
// MAC帧控制字段
typedef union {
    uint16_t value;
    struct {
        uint16_t frame_type : 3;      // 帧类型
        uint16_t security_en : 1;      // 安全使能
        uint16_t frame_pending : 1;    // 帧待传
        uint16_t ar : 1;               // 确认请求
        uint16_t pan_id_compress : 1;  // PAN ID压缩
        uint16_t reserved : 3;
        uint16_t dest_addr_mode : 2;   // 目的地址模式
        uint16_t frame_version : 2;    // 帧版本
        uint16_t src_addr_mode : 2;    // 源地址模式
    } bits;
} mac_frame_ctrl_t;

// MAC帧头
typedef struct {
    mac_frame_ctrl_t frame_ctrl;
    uint8_t seq_num;
    uint16_t dest_pan_id;
    union {
        uint16_t short_addr;
        uint64_t ext_addr;
    } dest_addr;
    uint16_t src_pan_id;
    union {
        uint16_t short_addr;
        uint64_t ext_addr;
    } src_addr;
} mac_header_t;

// MAC帧
typedef struct {
    mac_header_t header;
    uint8_t payload[118];
    uint8_t payload_len;
    uint16_t fcs;
} mac_frame_t;

// MAC PIB属性
typedef struct {
    uint64_t ext_addr;          // 64位扩展地址
    uint16_t short_addr;        // 16位短地址
    uint16_t pan_id;            // PAN ID
    uint8_t channel;            // 当前信道
    uint8_t beacon_order;       // 信标阶
    uint8_t superframe_order;   // 超帧阶
    uint8_t rx_on_when_idle;    // 空闲时接收使能
    uint8_t ack_wait_duration;  // 确认等待时间
    uint8_t max_csma_backoffs;  // 最大CSMA退避次数
    uint8_t min_be;             // 最小退避指数
    uint8_t max_be;             // 最大退避指数
} mac_pib_t;
```

### CSMA-CA算法

```c
/**
 * CSMA-CA (载波侦听多路访问/冲突避免) 算法
 * 非时隙版本（用于非信标网络）
 */
bool mac_csma_ca(void) {
    uint8_t nb = 0;           // 退避次数
    uint8_t cw = 2;           // 竞争窗口 (2或1)
    uint8_t be = mac_pib.min_be;  // 退避指数

    while (nb <= mac_pib.max_csma_backoffs) {
        // 生成随机退避时间: (0 to 2^BE - 1) 个退避周期
        uint8_t backoff = rand() % (1 << be);
        uint16_t backoff_us = backoff * MAC_UNIT_BACKOFF_PERIOD; // 320us

        // 等待退避时间
        delay_us(backoff_us);

        // 执行CCA (信道空闲评估)
        phy_cca_status_t cca_result = phy_cca();

        if (cca_result == PHY_CCA_IDLE) {
            // 信道空闲
            cw--;
            if (cw == 0) {
                return true;  // 可以发送
            }
        } else {
            // 信道忙
            cw = 2;  // 重置竞争窗口
            nb++;
            be++;    // 增加退避指数
            if (be > mac_pib.max_be) {
                be = mac_pib.max_be;
            }
        }
    }

    return false;  // 达到最大退避次数，发送失败
}

// 时隙CSMA-CA（用于信标网络）
bool mac_csma_ca_slotted(void) {
    // 类似算法，但退避周期与信标边界对齐
    // ...
    return true;
}
```

### MAC层服务原语

```c
// MAC数据服务
void mcps_data_request(mac_data_req_t* req);
void mcps_data_confirm(mac_status_t status);
void mcps_data_indication(mac_data_ind_t* ind);
void mcps_purge_request(uint8_t msdu_handle);

// MAC管理服务
void mlme_associate_request(mac_assoc_req_t* req);
void mlme_associate_confirm(mac_assoc_conf_t* conf);
void mlme_associate_indication(mac_assoc_ind_t* ind);
void mlme_associate_response(mac_assoc_rsp_t* rsp);

void mlme_disassociate_request(mac_disassoc_req_t* req);
void mlme_disassociate_confirm(mac_status_t status);
void mlme_disassociate_indication(mac_disassoc_ind_t* ind);

void mlme_beacon_notify_indication(mac_beacon_ind_t* ind);
void mlme_get_request(mac_pib_attr_t attr);
void mlme_get_confirm(mac_status_t status, mac_pib_attr_t attr, void* value);
void mlme_set_request(mac_pib_attr_t attr, void* value);
void mlme_set_confirm(mac_status_t status, mac_pib_attr_t attr);

void mlme_scan_request(mac_scan_req_t* req);
void mlme_scan_confirm(mac_scan_conf_t* conf);

void mlme_start_request(mac_start_req_t* req);
void mlme_start_confirm(mac_status_t status);

void mlme_sync_request(uint8_t logical_channel);
void mlme_sync_loss_indication(mac_sync_loss_reason_t reason);
```

---

## 网络层(NWK)

网络层负责网络的建立、维护、路由和数据转发。

### 网络拓扑结构

```
星型网络 (Star):
                    ┌──────────┐
                    │ 协调器   │
                    │ Coordinator│
                    └────┬─────┘
                         │
         ┌───────────────┼───────────────┐
         │               │               │
    ┌────┴────┐     ┌────┴────┐     ┌────┴────┐
    │ 终端设备│     │ 终端设备│     │ 终端设备│
    │ Device  │     │ Device  │     │ Device  │
    └─────────┘     └─────────┘     └─────────┘

树型网络 (Tree):
                    ┌──────────┐
                    │ 协调器   │
                    └────┬─────┘
                         │
              ┌──────────┼──────────┐
              │          │          │
         ┌────┴────┐ ┌───┴───┐ ┌───┴───┐
         │ 路由器  │ │ 路由器│ │ 路由器│
         └────┬────┘ └───┬───┘ └───┬───┘
              │          │         │
        ┌─────┼─────┐   ...      ...
        │     │     │
    ┌───┴┐ ┌──┴┐ ┌─┴─┐
    │终端│ │终端│ │终端│
    └────┘ └───┘ └───┘

网状网络 (Mesh):
              ┌─────────┐
              │ 协调器  │
              └────┬────┘
                   │
         ┌─────────┼─────────┐
         │         │         │
    ┌────┴────┐ ┌──┴───┐ ┌───┴───┐
    │ 路由器  │ │路由器│ │ 路由器│
    └────┬────┘ └──┬───┘ └───┬───┘
         │         │         │
    ┌────┴───┐   ┌─┴───┐   ┌─┴────┐
    │ 路由器 │───│路由器│───│路由器│
    └────┬───┘   └──┬──┘   └──┬───┘
         │          │         │
        ...        ...       ...
```

### 网络层帧格式

```
┌─────────────────────────────────────────────────────────────────────┐
│                      NWK层帧 (NPDU)                                  │
├─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┤
│ 帧控制  │ 目的地址│ 源地址  │ 半径    │ 序列号  │ 目的IEEE│ 源IEEE  │
│ 2字节   │ 2字节   │ 2字节   │ 1字节   │ 1字节   │ 0/8字节 │ 0/8字节 │
├─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┤
│                      安全头 (0/14字节，可选)                         │
├─────────────────────────────────────────────────────────────────────┤
│                      路由域 (可选，多播源路由)                       │
├─────────────────────────────────────────────────────────────────────┤
│                      NWK载荷 (上层数据)                              │
└─────────────────────────────────────────────────────────────────────┘

帧控制字段 (16位):
┌──────┬──────┬────────┬──────┬────────┬────────┬──────────┬──────────┐
│帧类型│协议版本│发现路由│多播 │安全   │源路由  │目的IEEE  │源IEEE    │
│2bit  │4bit  │2bit   │1bit  │1bit   │1bit    │1bit      │1bit      │
│      │(通常2)│       │      │       │        │          │          │
└──────┴──────┴────────┴──────┴────────┴────────┴──────────┴──────────┘

帧类型: 00=数据, 01=NWK命令, 10=保留, 11=保留
发现路由: 00=抑制, 01=使能, 10=强制, 11=保留
```

### 网络层数据结构

```c
// 网络地址分配
// 16位网络地址格式 (Cskip路由)
// Cm: 子节点最大数, Rm: 子路由器最大数, Lm: 网络最大深度

typedef uint16_t nwk_addr_t;

// 网络层帧控制
typedef union {
    uint16_t value;
    struct {
        uint16_t frame_type : 2;
        uint16_t protocol_version : 4;
        uint16_t discover_route : 2;
        uint16_t multicast : 1;
        uint16_t security : 1;
        uint16_t source_route : 1;
        uint16_t dest_ieee_addr : 1;
        uint16_t src_ieee_addr : 1;
        uint16_t reserved : 3;
    } bits;
} nwk_frame_ctrl_t;

// 网络层帧头
typedef struct {
    nwk_frame_ctrl_t frame_ctrl;
    nwk_addr_t dest_addr;
    nwk_addr_t src_addr;
    uint8_t radius;           // 最大跳数，每转发一次减1
    uint8_t seq_num;          // 序列号，用于去重
    uint64_t dest_ieee_addr;  // 可选
    uint64_t src_ieee_addr;   // 可选
} nwk_header_t;

// 邻居表条目
typedef struct {
    uint64_t ext_addr;        // 扩展地址
    nwk_addr_t nwk_addr;      // 网络地址
    uint8_t device_type;      // 0=协调器, 1=路由器, 2=终端
    uint8_t rx_on_when_idle;  // 空闲时接收
    uint8_t relationship;     // 关系
    uint8_t depth;            // 网络深度
    uint8_t lqi;              // 链路质量
    uint8_t outgoing_cost;    // 出链路成本
    uint8_t age;              // 老化计数
} neighbor_entry_t;

// 路由表条目
typedef struct {
    nwk_addr_t dest_addr;     // 目的地址
    uint8_t status;           // 状态: 0=激活, 1=发现中, 2=发现失败, 3=不活跃
    uint8_t no_route_cache;   // 不缓存路由
    uint8_t many_to_one;      // 多对一标志
    uint8_t route_record_req; // 需要路由记录
    nwk_addr_t next_hop_addr; // 下一跳地址
} route_entry_t;

// NWK PIB属性
typedef struct {
    uint64_t ext_pan_id;      // 扩展PAN ID
    nwk_addr_t nwk_addr;      // 网络地址
    nwk_addr_t parent_addr;   // 父节点地址
    uint8_t depth;            // 网络深度
    uint8_t channel;          // 当前信道
    uint8_t protocol_version; // 协议版本
    uint8_t stack_profile;    // 协议栈配置文件
    uint8_t max_children;     // 最大子节点数
    uint8_t max_routers;      // 最大路由器数
    uint8_t max_depth;        // 最大网络深度
    uint8_t beacon_order;     // 信标阶
    uint8_t tx_total;         // 传输总数
    uint8_t tx_fail;          // 传输失败数
} nwk_pib_t;
```

### 路由算法

```c
/**
 * 按需距离矢量路由 (AODV简化版)
 * Zigbee使用簇树路由 + AODV按需路由
 */

// 簇树路由计算下一跳
nwk_addr_t nwk_ctree_route(nwk_addr_t dest_addr) {
    // 如果目的地址是自身子节点
    if (is_child(dest_addr)) {
        return dest_addr;  // 直接发送
    }

    // 如果目的地址是父节点或父节点的后代
    if (is_parent_descendant(dest_addr)) {
        return nwk_pib.parent_addr;  // 发给父节点
    }

    // 计算公共祖先，路由到对应子路由器
    nwk_addr_t next_hop = find_common_ancestor_next_hop(dest_addr);
    return next_hop;
}

// Cskip计算 (确定子路由器地址空间大小)
uint16_t nwk_cskip(uint8_t d) {
    // d: 当前深度
    // Cm: 最大子节点数, Rm: 最大路由器数, Lm: 最大深度
    if (d >= nwk_pib.max_depth) {
        return 0;
    }

    uint16_t cm = nwk_pib.max_children;
    uint16_t rm = nwk_pib.max_routers;
    uint16_t lm = nwk_pib.max_depth;

    if (rm == 0) {
        return 1 + cm * (lm - d - 1);
    } else {
        return (1 + cm - rm - cm * pow(rm, lm - d - 1)) / (1 - rm);
    }
}

// 路由发现
void nwk_route_discovery(nwk_addr_t dest_addr) {
    // 创建路由请求命令帧
    nwk_route_req_cmd_t route_req;
    route_req.command_frame_id = NWK_CMD_ROUTE_REQUEST;
    route_req.command_options = 0;
    route_req.route_request_id = get_next_route_request_id();
    route_req.dest_addr = dest_addr;
    route_req.path_cost = 0;

    // 广播路由请求
    nwk_broadcast(NWK_FRAME_COMMAND, &route_req, sizeof(route_req));
}

// 处理路由请求
void nwk_handle_route_req(nwk_route_req_cmd_t* req, nwk_addr_t src) {
    // 更新路由发现表
    update_route_discovery(req->route_request_id, src, req->path_cost);

    // 如果是目的节点或知道路由，发送路由应答
    if (req->dest_addr == nwk_pib.nwk_addr || has_route(req->dest_addr)) {
        nwk_send_route_reply(req, src);
    } else {
        // 转发路由请求，增加路径成本
        req->path_cost += link_cost_calc(src);
        nwk_broadcast(NWK_FRAME_COMMAND, req, sizeof(*req));
    }
}
```

---

## 应用层(APS/AF)

应用层包括应用支持子层(APS)、应用框架(AF)和Zigbee设备对象(ZDO)。

### APS层帧格式

```
┌─────────────────────────────────────────────────────────────────────┐
│                      APS层帧 (APDU)                                  │
├─────────┬─────────┬─────────┬─────────┬─────────┬───────────────────┤
│ 帧控制  │ 目的端点│ 簇ID   │ Profile │ 源端点  │ 计数器   │ APS载荷 │
│ 1字节   │ 0/1字节 │0/2字节 │0/2字节  │ 0/1字节 │0/1字节   │         │
├─────────┴─────────┴─────────┴─────────┴─────────┴──────────┴─────────┤
│                      安全头 (0/14字节，可选)                         │
├─────────────────────────────────────────────────────────────────────┤
│                      APS载荷 (上层数据)                              │
└─────────────────────────────────────────────────────────────────────┘

帧控制字段 (8位):
┌──────┬────────┬──────────┬────────┬──────────┐
│帧类型│投递模式│间接地址  │安全    │确认请求  │
│2bit  │2bit    │1bit      │1bit    │1bit      │
├──────┴────────┴──────────┴────────┴──────────┤
│           扩展头存在  │  保留                  │
│              1bit     │  1bit                  │
└───────────────────────┴────────────────────────┘

帧类型: 00=数据, 01=命令, 10=确认, 11=保留
投递模式: 00=正常(单播), 01=间接, 10=广播, 11=组播
```

### APS层服务

```c
// APS信息库
typedef struct {
    uint8_t aps_designated_coordinator;  // 是否指定为协调器
    uint8_t aps_channel_mask[8];         // 信道掩码
    uint8_t aps_extended_pan_id[8];      // 扩展PAN ID
    uint8_t aps_use_insecure_join;       // 是否允许不安全加入
    uint8_t aps_interframe_delay;        // 帧间延迟
    uint8_t aps_last_channel_energy;     // 上次信道能量
    uint8_t aps_last_channel_fail_rate;  // 上次信道失败率
    uint8_t aps_channel_timer;           // 信道定时器
    uint8_t aps_device_key_pair_count;   // 设备密钥对数量
} aib_t;

// APS数据服务原语
void apsde_data_request(apsde_data_req_t* req);
void apsde_data_confirm(aps_status_t status);
void apsde_data_indication(apsde_data_ind_t* ind);

// APS管理服务原语
void apsme_bind_request(apsme_bind_req_t* req);
void apsme_bind_confirm(aps_status_t status);
void apsme_unbind_request(apsme_unbind_req_t* req);
void apsme_unbind_confirm(aps_status_t status);

void apsme_get_request(aps_attribute_t attr);
void apsme_get_confirm(aps_status_t status, aps_attribute_t attr, void* value);
void apsme_set_request(aps_attribute_t attr, void* value);
void apsme_set_confirm(aps_status_t status, aps_attribute_t attr);
```

### 应用框架(AF)

```c
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
```

### ZDO（Zigbee设备对象）

```c
// ZDO服务原语

// 网络管理服务
void zdo_nwk_addr_req(uint64_t ieee_addr, uint8_t request_type, uint8_t start_index);
void zdo_nwk_addr_rsp(zdo_nwk_addr_rsp_t* rsp);
void zdo_ieee_addr_req(nwk_addr_t nwk_addr, uint8_t request_type, uint8_t start_index);
void zdo_ieee_addr_rsp(zdo_ieee_addr_rsp_t* rsp);

// 节点描述符服务
void zdo_node_desc_req(nwk_addr_t nwk_addr_of_interest);
void zdo_node_desc_rsp(zdo_node_desc_rsp_t* rsp);
void zdo_power_desc_req(nwk_addr_t nwk_addr_of_interest);
void zdo_power_desc_rsp(zdo_power_desc_rsp_t* rsp);

// 简单描述符服务
void zdo_simple_desc_req(nwk_addr_t nwk_addr_of_interest, uint8_t endpoint);
void zdo_simple_desc_rsp(zdo_simple_desc_rsp_t* rsp);
void zdo_active_ep_req(nwk_addr_t nwk_addr_of_interest);
void zdo_active_ep_rsp(zdo_active_ep_rsp_t* rsp);

// 绑定服务
void zdo_bind_req(zdo_bind_req_t* req);
void zdo_bind_rsp(zdo_bind_rsp_t* rsp);
void zdo_unbind_req(zdo_unbind_req_t* req);
void zdo_unbind_rsp(zdo_unbind_rsp_t* rsp);

// 设备与服务发现
void zdo_match_desc_req(zdo_match_desc_req_t* req);
void zdo_match_desc_rsp(zdo_match_desc_rsp_t* rsp);

// 网络管理
void zdo_mgmt_lqi_req(nwk_addr_t nwk_addr, uint8_t start_index);
void zdo_mgmt_lqi_rsp(zdo_mgmt_lqi_rsp_t* rsp);
void zdo_mgmt_rtg_req(nwk_addr_t nwk_addr, uint8_t start_index);
void zdo_mgmt_rtg_rsp(zdo_mgmt_rtg_rsp_t* rsp);
void zdo_mgmt_leave_req(zdo_mgmt_leave_req_t* req);
void zdo_mgmt_leave_rsp(zdo_mgmt_leave_rsp_t* rsp);
void zdo_mgmt_permit_join_req(uint8_t permit_duration, uint8_t tc_significance);
void zdo_mgmt_permit_join_rsp(zdo_mgmt_permit_join_rsp_t* rsp);
void zdo_mgmt_nwk_update_req(zdo_mgmt_nwk_update_req_t* req);
void zdo_mgmt_nwk_update_notify(zdo_mgmt_nwk_update_notify_t* notify);
```

---

## 安全机制

Zigbee提供多层安全保护，包括网络层加密、链路层加密和应用层安全。

### 安全架构

```
┌─────────────────────────────────────────────────────────────────────┐
│                         Zigbee安全架构                               │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    应用层安全 (APS)                          │   │
│  │  - 端到端加密    - 密钥建立    - 设备认证                     │   │
│  │  密钥: 链接密钥(Link Key)、主密钥(Master Key)               │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                      │
│  ┌───────────────────────────▼────────────────────────────────┐   │
│  │                    网络层安全 (NWK)                         │   │
│  │  - 网络加密    - 帧完整性检查    - 防重放攻击                │   │
│  │  密钥: 网络密钥(Network Key)                                 │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                              │                                      │
│  ┌───────────────────────────▼────────────────────────────────┐   │
│  │                    MAC层安全 (IEEE 802.15.4)                │   │
│  │  - 访问控制    - 帧加密    - 帧完整性检查                    │   │
│  │  密钥: 预共享密钥                                            │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
├─────────────────────────────────────────────────────────────────────┤
│                      安全服务原语                                   │
│  - APSME-ESTABLISH-KEY: 建立链接密钥                               │
│  - APSME-TRANSPORT-KEY: 传输密钥                                   │
│  - APSME-UPDATE-DEVICE: 更新设备状态                               │
│  - APSME-REMOVE-DEVICE: 移除设备                                   │
│  - APSME-REQUEST-KEY: 请求密钥                                     │
│  - APSME-SWITCH-KEY: 切换密钥                                      │
└─────────────────────────────────────────────────────────────────────┘
```

### 密钥类型

| 密钥类型 | 用途 | 分发方式 |
|----------|------|----------|
| **主密钥** | 密钥建立的基础 | 带外传输（如用户输入） |
| **链接密钥** | 端到端安全通信 | SKKE协议或预配置 |
| **网络密钥** | 网络层加密 | 集中式（信任中心）或分布式 |
| **密钥传输密钥** | 加密密钥传输 | 从主密钥派生 |
| **帧计数器主密钥** | 同步帧计数器 | 从网络密钥派生 |

### 安全处理流程

```c
// AES-CCM*加密/解密
// CCM*提供: 加密 + 认证 + 完整性检查

typedef struct {
    uint8_t key[16];           // 128位密钥
    uint8_t nonce[13];         // 13字节Nonce
    uint8_t a[256];            // 认证数据 (Adata)
    uint8_t m[128];            // 明文/密文数据
    uint8_t len_a;             // 认证数据长度
    uint8_t len_m;             // 数据长度
    uint8_t mic_len;           // MIC长度 (0, 4, 8, 16)
} ccm_params_t;

// 生成Nonce
void generate_nonce(uint8_t* src_addr, uint32_t frame_counter, uint8_t sec_level,
                    uint8_t* nonce) {
    memcpy(nonce, src_addr, 8);           // 源地址 (8字节)
    nonce[8] = frame_counter >> 24;       // 帧计数器 (4字节)
    nonce[9] = frame_counter >> 16;
    nonce[10] = frame_counter >> 8;
    nonce[11] = frame_counter;
    nonce[12] = sec_level;                // 安全级别
}

// CCM*加密
int aes_ccm_star_encrypt(ccm_params_t* params, uint8_t* ciphertext, uint8_t* mic) {
    // 1. CTR模式加密
    // 2. CBC-MAC计算认证标签
    // 3. 输出密文 + MIC
    // 具体实现使用AES硬件或软件库
    return 0;
}

// CCM*解密和验证
int aes_ccm_star_decrypt(ccm_params_t* params, uint8_t* ciphertext, uint8_t* mic,
                         uint8_t* plaintext) {
    // 1. 验证MIC
    // 2. CTR模式解密
    // 3. 返回明文或验证失败
    return 0;
}

// 安全级别
typedef enum {
    SEC_NONE = 0,           // 无安全
    SEC_MIC32 = 1,          // 仅MIC (32位)
    SEC_MIC64 = 2,          // 仅MIC (64位)
    SEC_MIC128 = 3,         // 仅MIC (128位)
    SEC_ENC = 4,            // 仅加密
    SEC_ENC_MIC32 = 5,      // 加密 + MIC (32位)
    SEC_ENC_MIC64 = 6,      // 加密 + MIC (64位)
    SEC_ENC_MIC128 = 7,     // 加密 + MIC (128位)
} security_level_t;
```

### 信任中心

```c
// 信任中心（协调器）安全功能

typedef struct {
    uint8_t active;                       // 信任中心使能
    uint8_t allow_rejoin;                 // 允许重新加入
    uint8_t allow_remote_tc_policy_change; // 允许远程TC策略更改
    uint8_t network_key[16];              // 当前网络密钥
    uint32_t network_key_seq_num;         // 网络密钥序列号
    uint8_t network_key_type;             // 密钥类型
    uint32_t frame_counter;               // 帧计数器
} trust_center_t;

// 设备加入流程（安全）
void tc_device_join_security(uint64_t ext_addr, nwk_addr_t nwk_addr) {
    // 1. 验证设备是否允许加入
    if (!is_device_allowed(ext_addr)) {
        return;
    }

    // 2. 获取或生成链接密钥
    uint8_t link_key[16];
    if (have_preconfigured_link_key(ext_addr)) {
        get_preconfigured_link_key(ext_addr, link_key);
    } else {
        // 使用默认全局链接密钥（不安全，仅用于测试）
        memcpy(link_key, default_global_link_key, 16);
    }

    // 3. 使用链接密钥加密传输网络密钥
    uint8_t encrypted_nwk_key[18];
    encrypt_transport_key(link_key, tc.network_key, encrypted_nwk_key);

    // 4. 发送APSME-TRANSPORT-KEY.indication
    send_transport_key(nwk_addr, KEY_TYPE_NWK, encrypted_nwk_key);

    // 5. 更新设备表
    add_device_to_network(ext_addr, nwk_addr);
}
```

---

## CC2530实现

CC2530是德州仪器推出的Zigbee/802.15.4片上系统芯片，集成了8051 MCU和2.4GHz射频收发器。

### CC2530硬件架构

```
┌─────────────────────────────────────────────────────────────────────┐
│                          CC2530芯片架构                              │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                     8051 MCU 内核                           │   │
│  │  - 32MHz时钟    - 代码/数据内存接口    - 调试接口            │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    内存子系统                                │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐         │   │
│  │  │  Flash      │  │  SRAM       │  │  SFR        │         │   │
│  │  │  32/64/128KB│  │  8KB        │  │  特殊功能   │         │   │
│  │  │  (代码)     │  │  (数据)     │  │  寄存器     │         │   │
│  │  └─────────────┘  └─────────────┘  └─────────────┘         │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    2.4GHz射频收发器                          │   │
│  │  - 符合IEEE 802.15.4    - 250kbps数据率    - DSSS调制       │   │
│  │  - 接收灵敏度 -97dBm    - 可编程发射功率(-24到4.5dBm)       │   │
│  │  - 硬件CSMA-CA支持    - 硬件AES-128加密/解密               │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    外设接口                                  │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐           │   │
│  │  │  USART0 │ │  USART1 │ │   SPI   │ │   I2C   │           │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘           │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────┐           │   │
│  │  │  Timer  │ │   ADC   │ │  GPIO   │ │  Watchdog│           │   │
│  │  │  (5个)  │ │ (12位)  │ │ (21个)  │ │          │           │   │
│  │  └─────────┘ └─────────┘ └─────────┘ └─────────┘           │   │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐                        │   │
│  │  │  DMA    │ │  AES    │ │  IR Gen │                        │   │
│  │  │ (8通道) │ │(硬件)   │ │         │                        │   │
│  │  └─────────┘ └─────────┘ └─────────┘                        │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### CC2530寄存器配置

```c
// CC2530寄存器定义

// 射频寄存器
#define FRMCTRL0    0x6180   // 帧控制0
#define FRMCTRL1    0x6181   // 帧控制1
#define RXENABLE    0x6182   // 接收使能
#define RXMASKSET   0x6183   // 接收掩码设置
#define FREQCTRL    0x6189   // 频率控制 (信道: 11 + (FREQCTRL - 11))
#define FSMSTAT1    0x6191   // FSM状态1
#define FIFOPCTRL   0x6194   // FIFOP阈值控制
#define RFIRQM0     0x61A3   // RF中断掩码0
#define RFIRQM1     0x61A4   // RF中断掩码1
#define RFERRM      0x61A5   // RF错误中断掩码
#define RFRND       0x61A7   // RF随机数据
#define MDMCTRL0    0x61A8   // 调制解调控制0
#define MDMCTRL1    0x61A9   // 调制解调控制1
#define RSSI        0x61AB   // RSSI状态
#define RXCTRL      0x61AC   // 接收控制
#define FSCTRL      0x61B0   // 频率合成控制
#define FSCAL1      0x61B2   // 频率合成校准1
#define AGCCTRL0    0x61B4   // AGC控制0
#define ADCTEST0    0x61C5   // ADC测试0
#define ADCTEST1    0x61C6   // ADC测试1
#define ADCTEST2    0x61C7   // ADC测试2

// 配置信道 (2.4GHz, 信道11-26)
void cc2530_set_channel(uint8_t channel) {
    if (channel < 11 || channel > 26) return;
    FREQCTRL = (channel - 11) + 11;
}

// 配置发射功率
typedef enum {
    POWER_MINUS_24_DBM = 0x03,
    POWER_MINUS_18_DBM = 0x2C,
    POWER_MINUS_12_DBM = 0x88,
    POWER_MINUS_6_DBM  = 0x81,
    POWER_0_DBM        = 0x32,
    POWER_4_5_DBM      = 0xF5,
} tx_power_t;

void cc2530_set_tx_power(tx_power_t power) {
    TXPOWER = power;
}

// 配置接收滤波器
void cc2530_set_address_filter(uint16_t pan_id, uint16_t short_addr,
                                uint64_t ext_addr) {
    // 设置PAN ID
    PAN_ID0 = pan_id & 0xFF;
    PAN_ID1 = (pan_id >> 8) & 0xFF;

    // 设置短地址
    SHORT_ADDR0 = short_addr & 0xFF;
    SHORT_ADDR1 = (short_addr >> 8) & 0xFF;

    // 设置扩展地址
    for (int i = 0; i < 8; i++) {
        EXT_ADDR[i] = (ext_addr >> (i * 8)) & 0xFF;
    }

    // 使能地址识别
    FRMCTRL0 |= 0x20;  // SET_RXENMASK_ON_RX
}
```

### CC2530射频操作

```c
// 射频状态
#define RX_ACTIVE   0x01
#define TX_ACTIVE   0x02
#define IDLE        0x00

// 初始化射频
void cc2530_radio_init(void) {
    // 软件复位射频
    SFR_RFST = RFST_SRXON;
    delay_us(200);

    // 配置调制解调器
    MDMCTRL0 = 0x0A;   // 自动确认，协调器模式
    MDMCTRL1 = 0x14;   // 相关器阈值

    // 配置频率合成器
    FSCTRL = 0x5A;
    FSCAL1 = 0x00;

    // 配置AGC
    AGCCTRL0 = 0x91;

    // 配置接收控制
    RXCTRL = 0x3F;

    // 设置FIFO阈值
    FIFOPCTRL = 64;    // FIFOP阈值64字节
}

// 发送数据包
void cc2530_tx(uint8_t* data, uint8_t len) {
    // 等待射频空闲
    while (FSMSTAT1 & (RX_ACTIVE | TX_ACTIVE));

    // 写入TX FIFO
    RFD = len;  // 长度字段
    for (int i = 0; i < len; i++) {
        RFD = data[i];
    }

    // 触发发送
    SFR_RFST = RFST_STXON;

    // 等待发送完成
    while (!(RFIRQF1 & IRQ_TXDONE));
    RFIRQF1 &= ~IRQ_TXDONE;  // 清除中断标志
}

// 接收中断处理
#pragma vector = RF_VECTOR
__interrupt void rf_isr(void) {
    if (RFIRQF0 & IRQ_FIFOP) {
        // 接收数据可用
        uint8_t len = RFD;  // 读取长度
        uint8_t rx_buf[128];

        for (int i = 0; i < len; i++) {
            rx_buf[i] = RFD;
        }

        // 处理接收数据
        process_rx_frame(rx_buf, len);

        RFIRQF0 &= ~IRQ_FIFOP;  // 清除中断
    }

    S1CON &= ~0x03;  // 清除CPU中断标志
}
```

### CC2530 AES加密

```c
// AES加密/解密操作
#define AES_SET_MODE    0x04
#define AES_ENCRYPT     0x00
#define AES_DECRYPT     0x02
#define AES_LOAD_KEY    0x00
#define AES_LOAD_IV     0x01
#define AES_START       0x02

// AES数据结构
typedef struct {
    uint8_t key[16];    // 128位密钥
    uint8_t data[16];   // 128位数据块
    uint8_t iv[16];     // 初始化向量（CBC模式）
} aes_block_t;

// 使用硬件AES加密
void cc2530_aes_encrypt(aes_block_t* block) {
    // 停止任何正在进行的AES操作
    ENCCS = 0x00;

    // 加载密钥
    ENCCS = AES_SET_MODE | AES_LOAD_KEY;
    for (int i = 0; i < 16; i++) {
        ENCDI = block->key[i];
    }

    // 设置加密模式
    ENCCS = AES_SET_MODE | AES_ENCRYPT;

    // 加载数据并启动加密
    for (int i = 0; i < 16; i++) {
        ENCDI = block->data[i];
    }

    // 等待完成
    while (ENCCS & 0x08);

    // 读取结果
    for (int i = 0; i < 16; i++) {
        block->data[i] = ENCDO;
    }
}

// CCM*模式（使用硬件AES引擎）
void cc2530_ccm_star(uint8_t* key, uint8_t* nonce, uint8_t* a, uint8_t len_a,
                     uint8_t* m, uint8_t len_m, uint8_t* ciphertext,
                     uint8_t* mic, uint8_t sec_level) {
    // CCM*实现，使用硬件AES加速
    // ...
}
```

---

## 802.15.4基础

IEEE 802.15.4是Zigbee的物理层和MAC层基础标准。

### 802.15.4协议特点

| 特性 | 参数 |
|------|------|
| **频段** | 868MHz (欧洲)、915MHz (北美)、2450MHz (全球) |
| **调制方式** | BPSK (868/915MHz)、O-QPSK (2450MHz) |
| **扩频** | DSSS (直接序列扩频) |
| **码片速率** | 300kchip/s (868MHz)、600kchip/s (915MHz)、2Mchip/s (2450MHz) |
| **数据率** | 20kbps (868MHz)、40kbps (915MHz)、250kbps (2450MHz) |
| **最大帧长** | 127字节 (PHY) / 116字节 (MAC载荷) |

### O-QPSK调制（2.4GHz）

```
数据比特 → 半正弦脉冲成型 → 正交调制 → 射频输出

符号到码片映射 (4位 → 32码片):
数据符号: d3d2d1d0

符号   码片序列 (c0-c31)
─────────────────────────────────────────────────
 0    1 1 0 1 1 0 0 1 1 1 0 0 0 0 1 1 0 1 0 1 0 0 1 0 0 0 1 0 1 1 1 0
 1    1 1 1 0 1 1 0 1 1 1 0 0 0 0 1 1 0 1 0 1 0 0 1 0 0 0 1 0 1 1 0 1
 2    0 0 1 0 1 1 1 0 1 1 0 0 0 0 1 1 0 1 0 1 0 0 1 0 0 0 1 0 1 1 1 0
 ...
 15   1 0 0 1 0 0 0 1 1 1 0 0 0 0 1 1 0 1 0 1 0 0 1 0 0 0 1 0 1 1 1 0

脉冲成型: 半正弦波形
s(t) = sin(πt/2Tc), 0 ≤ t ≤ 2Tc

调制公式:
I(t) = Σ c2k * s(t - kTc)
Q(t) = Σ c2k+1 * s(t - kTc - Tc)  [延迟Tc]
```

### 超帧结构（信标使能网络）

```
超帧结构:

时间 ──►
│
├───────────────────────────────────────────────────────────────────────┐
│                              信标间隔 (BI)                             │
│  ┌──┐                                                                 │
│  │信│  CAP (竞争访问期)      CFP (无竞争期)                            │
│  │标│  ──────────────────  ┌─────┬─────┬─────┐                        │
│  │  │  CSMA-CA访问          │ GTS0│ GTS1│ GTS2│  ...                  │
│  │  │  发送信标、数据帧、命令 │     │     │     │                        │
│  │  │  帧、关联请求等        │设备1 │设备2 │设备3│                        │
│  └──┘  ──────────────────  └─────┴─────┴─────┘                        │
│  ↑                                                                ↑   │
│  └──────────────────────── 超帧持续时间 (SD) ───────────────────────┘   │
└───────────────────────────────────────────────────────────────────────┘

超帧阶 (SO) 和信标阶 (BO):
SD = aBaseSuperframeDuration × 2^SO
BI = aBaseSuperframeDuration × 2^BO

其中: aBaseSuperframeDuration = 960 symbols ≈ 15.36 ms (2.4GHz)

例: SO=0, BO=3
    SD = 15.36 ms × 1 = 15.36 ms
    BI = 15.36 ms × 8 = 122.88 ms
    每个信标间隔包含8个超帧
```

### 网络建立流程

```
协调器建立网络:
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│ 启动    │────►│ 扫描    │────►│ 选择    │────►│ 配置    │
│ 请求    │     │ 信道    │     │ PAN ID  │     │ 网络    │
└─────────┘     └─────────┘     └─────────┘     └─────────┘
                                                    │
                                                    ▼
                                             ┌─────────┐
                                             │ 发送    │
                                             │ 信标    │
                                             └─────────┘

设备加入网络:
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│ 扫描    │────►│ 选择    │────►│ 关联    │────►│ 认证    │
│ 网络    │     │ 父节点  │     │ 请求    │     │ 密钥    │
└─────────┘     └─────────┘     └─────────┘     └─────────┘
                                                    │
                                                    ▼
                                             ┌─────────┐
                                             │ 加入    │
                                             │ 成功    │
                                             └─────────┘
```

---

## 总结

Zigbee协议栈是一个完整的低功耗无线通信解决方案，主要特点包括：

1. **分层架构**: 物理层、MAC层、网络层、应用层各司其职
2. **灵活组网**: 支持星型、树型、网状拓扑，最多65000+节点
3. **安全可靠**: 多层加密机制（AES-128）、设备认证、帧完整性检查
4. **低功耗**: 休眠电流μA级，电池寿命可达数年
5. **低成本**: 协议栈精简，硬件要求低

CC2530是Zigbee实现的典型平台，集成了射频、MCU和丰富的外设，配合Z-Stack等协议栈软件，可快速开发Zigbee应用。

---

> **状态**: ✅ 已完成


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
