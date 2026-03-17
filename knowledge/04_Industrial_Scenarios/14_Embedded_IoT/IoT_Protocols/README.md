# IoT 协议栈实现 (IoT Protocols Implementation)

> 轻量级物联网通信协议完整实现：MQTT、CoAP、Modbus、LoRaWAN、HTTP、WebSocket

---

## 📋 技术概述

物联网协议栈是实现设备互联、数据采集和远程控制的核心技术。本目录提供多种轻量级物联网通信协议的完整 C 语言实现，专为资源受限的嵌入式设备优化，同时也适用于高性能网关设备。

### IoT 协议分层模型

```
┌─────────────────────────────────────────────────────────────┐
│                    应用层 (Application)                      │
│  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌─────────────────┐   │
│  │  MQTT   │ │  CoAP   │ │  HTTP   │ │    WebSocket    │   │
│  │ 发布订阅 │ │请求/响应 │ │ RESTful │ │   全双工通信    │   │
│  └────┬────┘ └────┬────┘ └────┬────┘ └────────┬────────┘   │
├───────┼───────────┼───────────┼───────────────┼────────────┤
│       │           │           │               │            │
│  传输层 (Transport)                                            │
│  ┌────▼────┐ ┌────▼────┐ ┌────▼────┐ ┌───────▼────────┐    │
│  │   TCP   │ │   UDP   │ │   TCP   │ │      TCP       │    │
│  │ 可靠传输 │ │ 轻量传输 │ │ 可靠传输 │ │   可靠传输     │    │
│  └────┬────┘ └────┬────┘ └────┬────┘ └───────┬────────┘    │
├───────┼───────────┼───────────┼───────────────┼────────────┤
│       │           │           │               │            │
│  网络层 (Network)                                             │
│  ┌────▼───────────────────────────────────────▼────────┐    │
│  │                  IPv4 / IPv6                         │    │
│  │               (6LoWPAN for constrained)              │    │
│  └──────────────────────────────────────────────────────┘    │
├─────────────────────────────────────────────────────────────┤
│  链路层 (Link)                                               │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐   │
│  │ Ethernet │   WiFi   │   BLE    │   LoRa   │  ZigBee  │   │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘   │
└─────────────────────────────────────────────────────────────┘
```

### 协议对比

| 协议 | 传输层 | 模式 | 报文大小 | 功耗 | 复杂度 | 适用场景 |
|:-----|:-------|:-----|:---------|:-----|:-------|:---------|
| **MQTT** | TCP | 发布/订阅 | 2 字节起 | 中 | 低 | 实时消息、传感器数据 |
| **CoAP** | UDP | 请求/响应 | 4 字节起 | 低 | 低 | 受限设备、REST API |
| **HTTP** | TCP | 请求/响应 | 较大 | 高 | 中 | Web 服务、文件传输 |
| **Modbus** | TCP/RTU | 主/从 | 紧凑 | 低 | 低 | 工业控制、SCADA |
| **LoRaWAN** | LoRa | ALOHA | 可变 | 极低 | 高 | 远距离、低功耗 |
| **CAN** | 物理层 | 广播 | 8 字节 | 低 | 中 | 车载、工业现场 |
| **WebSocket** | TCP | 全双工 | 2 字节起 | 中 | 中 | 实时双向通信 |

---


---

## 📑 目录

- [IoT 协议栈实现 (IoT Protocols Implementation)](#iot-协议栈实现-iot-protocols-implementation)
  - [📋 技术概述](#-技术概述)
    - [IoT 协议分层模型](#iot-协议分层模型)
    - [协议对比](#协议对比)
  - [📑 目录](#-目录)
  - [⭐ 核心协议特性](#-核心协议特性)
    - [1. MQTT (Message Queuing Telemetry Transport)](#1-mqtt-message-queuing-telemetry-transport)
    - [2. CoAP (Constrained Application Protocol)](#2-coap-constrained-application-protocol)
    - [3. Modbus (RTU/TCP)](#3-modbus-rtutcp)
    - [4. LoRaWAN](#4-lorawan)
    - [5. HTTP/REST](#5-httprest)
    - [6. WebSocket](#6-websocket)
  - [🎯 应用场景](#-应用场景)
    - [1. 多协议工业网关](#1-多协议工业网关)
    - [2. 智能家居中控](#2-智能家居中控)
    - [3. 农业物联网](#3-农业物联网)
    - [4. 智慧城市](#4-智慧城市)
    - [5. 能源管理](#5-能源管理)
  - [🛠️ 开发环境要求](#️-开发环境要求)
    - [软件依赖](#软件依赖)
    - [嵌入式平台支持](#嵌入式平台支持)
    - [推荐的测试工具](#推荐的测试工具)
  - [📚 与主文档的关系](#-与主文档的关系)
    - [文档架构](#文档架构)
    - [平台集成指南](#平台集成指南)
  - [🚀 快速开始指引](#-快速开始指引)
    - [步骤 1: MQTT 客户端](#步骤-1-mqtt-客户端)
    - [步骤 2: CoAP 客户端](#步骤-2-coap-客户端)
    - [步骤 3: Modbus RTU 主站](#步骤-3-modbus-rtu-主站)
  - [❓ 常见问题](#-常见问题)
    - [MQTT 问题](#mqtt-问题)
    - [CoAP 问题](#coap-问题)
    - [Modbus 问题](#modbus-问题)
    - [协议选择决策](#协议选择决策)
  - [📖 推荐学习路径](#-推荐学习路径)
    - [初级路径](#初级路径)
    - [中级路径](#中级路径)
    - [高级路径](#高级路径)
  - [🔗 返回上级](#-返回上级)
  - [📞 支持与资源](#-支持与资源)


---

## ⭐ 核心协议特性

### 1. MQTT (Message Queuing Telemetry Transport)

**协议特点：**

- 基于 TCP 的发布/订阅模式
- 三种 QoS 等级（0-最多一次，1-至少一次，2-恰好一次）
- 遗嘱消息和保留消息支持
- 会话保持机制
- 心跳保活

**报文结构：**

```
固定报头 (Fixed Header)
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
│ 报文类型 │  标志位 │                        剩余长度                                │
│ (4 bits)│(4 bits) │                  (1-4 字节可变长编码)                            │
└─────────┴─────────┴───────────────────────────────────────────────────────────────┘

可变报头 (Variable Header) - 根据报文类型变化
有效载荷 (Payload) - 根据报文类型变化
```

**典型应用：**

- 智能家居控制中心
- 工业传感器数据采集
- 实时消息推送
- 设备状态监控

### 2. CoAP (Constrained Application Protocol)

**协议特点：**

- 基于 UDP 的类 HTTP 协议
- 支持 RESTful 操作（GET/POST/PUT/DELETE）
- 支持观察者模式（Observe）
- 支持块传输（Block-wise）
- 低头部开销（4 字节固定头部）

**报文结构：**

```
 0                   1                   2                   3
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|Ver| T |  TKL  |      Code     |          Message ID           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Token (if any, TKL bytes) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|   Options (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|1 1 1 1 1 1 1 1|    Payload (if any) ...
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
```

**典型应用：**

- 6LoWPAN 网络设备
- 电池供电传感器
- 智能照明控制
- 楼宇自动化

### 3. Modbus (RTU/TCP)

**协议特点：**

- 工业领域的事实标准
- 简单的主/从架构
- 多种数据模型（线圈、离散输入、保持寄存器、输入寄存器）
- RTU（串口）和 TCP（以太网）两种模式
- 高可靠性，广泛支持

**数据模型：**

| 数据类型 | 功能码 | 访问方式 | 典型用途 |
|:---------|:-------|:---------|:---------|
| 线圈 (Coils) | 01/05/15 | 读写 | 开关控制 |
| 离散输入 | 02 | 只读 | 状态监测 |
| 保持寄存器 | 03/06/16 | 读写 | 模拟量设定 |
| 输入寄存器 | 04 | 只读 | 传感器数据 |

**典型应用：**

- PLC 通信
- 工业传感器网络
- HVAC 系统
- 能源管理系统

### 4. LoRaWAN

**协议特点：**

- 远距离、低功耗无线通信
- 星型网络拓扑
- 自适应数据速率 (ADR)
- 三种设备类别（A/B/C）
- 端到端加密

**设备类别：**

| 类别 | 接收窗口 | 功耗 | 适用场景 |
|:-----|:---------|:-----|:---------|
| Class A | 2 个（发送后） | 最低 | 电池供电传感器 |
| Class B | 2 个 + Beacon | 中 | 中延迟控制 |
| Class C | 持续开启 | 高 | 供电型执行器 |

**典型应用：**

- 智能电表
- 资产追踪
- 环境监测
- 智慧农业

### 5. HTTP/REST

**协议特点：**

- 互联网标准协议
- 丰富的生态系统
- 支持 JSON/XML 数据格式
- 广泛的开发工具和库

**适用场景：**

- 与云服务集成
- Web 界面交互
- 文件上传下载
- 配置管理

### 6. WebSocket

**协议特点：**

- 全双工通信通道
- 基于 TCP，初始 HTTP 握手
- 低延迟、低头部开销
- 支持二进制和文本帧

**适用场景：**

- 实时数据流
- 远程控制面板
- 在线监控
- 即时消息

---

## 🎯 应用场景

### 1. 多协议工业网关

- 汇聚 Modbus/OPC-UA/Profinet 设备数据
- 转换为 MQTT/HTTP 上送云端
- 本地数据预处理和缓存
- 设备远程配置管理

### 2. 智能家居中控

- WiFi/BLE/Zigbee 多协议支持
- MQTT 连接云平台
- CoAP 控制 Zigbee 设备
- WebSocket 支持 APP 实时控制

### 3. 农业物联网

- LoRaWAN 传感器网络
- 低功耗土壤墒情监测
- 气象站数据采集
- 精准灌溉控制

### 4. 智慧城市

- 智能路灯控制
- 垃圾桶满溢监测
- 停车位状态检测
- 空气质量监测网络

### 5. 能源管理

- 智能电表数据采集
- 光伏逆变器监控
- 储能系统管理
- 负荷预测分析

---

## 🛠️ 开发环境要求

### 软件依赖

| 组件 | 版本要求 | 说明 |
|:-----|:---------|:-----|
| C 编译器 | C99+ | GCC、Clang、MSVC |
| 网络栈 | lwIP / BSD Socket | TCP/IP 支持 |
| TLS 库 | mbedTLS / OpenSSL | 安全连接（可选） |
| JSON 库 | cJSON / Jansson | 数据序列化（可选） |

### 嵌入式平台支持

| 平台 | MQTT | CoAP | Modbus | LoRaWAN | HTTP | WebSocket |
|:-----|:----:|:----:|:------:|:-------:|:----:|:---------:|
| ESP32 | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |
| RPi Pico | ✓ | ✓ | ✓ | ✓ | ✓ | ○ |
| STM32 | ✓ | ✓ | ✓ | ✓ | ○ | ○ |
| Linux | ✓ | ✓ | ✓ | ✓ | ✓ | ✓ |

*✓ 完全支持，○ 有限支持（资源受限）*

### 推荐的测试工具

| 工具 | 用途 | 平台 |
|:-----|:-----|:-----|
| Mosquitto | MQTT 代理 | 全平台 |
| CoAPthon | CoAP 测试 | Python |
| Modbus Poll/Slave | Modbus 测试 | Windows |
| ChirpStack | LoRaWAN NS | Linux |
| Postman/curl | HTTP 测试 | 全平台 |
| websocat | WebSocket 测试 | 全平台 |

---

## 📚 与主文档的关系

本目录是 [14_Embedded_IoT](../README.md) 的通信协议核心，为各硬件平台提供协议实现参考。

### 文档架构

```
IoT_Protocols/
├── README.md                              # 本文件 - 协议概览
├── 01_IoT_Protocols_Implementation.md     # 完整实现指南（800+ 行代码）
├── include/                               # 头文件目录
│   ├── mqtt_client.h
│   ├── coap_client.h
│   ├── modbus.h
│   ├── lorawan_stack.h
│   ├── http_client.h
│   └── websocket.h
└── src/                                   # 源文件目录
    ├── mqtt_client.c
    ├── coap_client.c
    └── ...
```

### 平台集成指南

| 本目录协议 | 目标平台 | 集成要点 |
|:-----------|:---------|:---------|
| MQTT | [../ESP32](../ESP32/) | 配合 WiFi 客户端使用 |
| CoAP | [../RPi_Pico](../RPi_Pico/) | 轻量级 UDP 实现 |
| Modbus | [../RaspberryPi](../RaspberryPi/) | 工业网关应用 |
| 协议优化 | [../Memory_Constrained](../Memory_Constrained/) | 缓冲区管理 |

---

## 🚀 快速开始指引

### 步骤 1: MQTT 客户端

```c
#include "mqtt_client.h"
#include <string.h>

// 网络接口实现
int network_read(void *ctx, uint8_t *buf, size_t len, uint32_t timeout) {
    // 实现网络读取
    return recv(*(int*)ctx, buf, len, 0);
}

int network_write(void *ctx, const uint8_t *buf, size_t len, uint32_t timeout) {
    // 实现网络写入
    return send(*(int*)ctx, buf, len, 0);
}

int main() {
    mqtt_client_t client;
    mqtt_network_t network = {
        .context = &sock_fd,
        .read = network_read,
        .write = network_write,
        .close = NULL
    };

    // 初始化客户端
    mqtt_client_init(&client, &network);

    // 配置连接选项
    mqtt_connect_options_t options = {
        .client_id = "pico_client",
        .keep_alive_sec = 60,
        .clean_session = true
    };

    // 连接服务器
    if (mqtt_client_connect(&client, &options) == 0) {
        printf("MQTT Connected!\n");

        // 订阅主题
        mqtt_client_subscribe(&client, "sensors/+/temp", MQTT_QOS_1);

        // 发布消息
        uint8_t payload[] = "25.5";
        mqtt_client_publish(&client, "sensors/room1/temp",
                           payload, sizeof(payload)-1, MQTT_QOS_1, false);

        // 处理消息
        while (1) {
            mqtt_client_process(&client, 1000);
        }
    }

    return 0;
}
```

### 步骤 2: CoAP 客户端

```c
#include "coap_client.h"

int main() {
    coap_client_t client;

    // 初始化客户端
    coap_client_init(&client, "coap.me", 5683);

    // 发送 GET 请求
    coap_request_t request = {
        .code = COAP_GET,
        .path = "hello"
    };

    coap_response_t response;
    if (coap_client_request(&client, &request, &response) == 0) {
        printf("Response: %.*s\n", response.payload_len, response.payload);
    }

    return 0;
}
```

### 步骤 3: Modbus RTU 主站

```c
#include "modbus.h"

int main() {
    modbus_t ctx;

    // 初始化 Modbus RTU
    modbus_init_rtu(&ctx, "/dev/ttyUSB0", 9600, 'N', 8, 1);
    modbus_set_slave(&ctx, 1);

    // 连接设备
    if (modbus_connect(&ctx) == 0) {
        uint16_t regs[10];

        // 读取保持寄存器
        int rc = modbus_read_registers(&ctx, 0, 10, regs);
        if (rc > 0) {
            for (int i = 0; i < rc; i++) {
                printf("Register %d: %d\n", i, regs[i]);
            }
        }
    }

    modbus_close(&ctx);
    return 0;
}
```

---

## ❓ 常见问题

### MQTT 问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| 频繁断开 | 保活时间过短 | 增加 keep_alive 间隔 |
| 消息丢失 | QoS 0 使用 | 升级至 QoS 1/2 |
| 连接拒绝 | 认证失败 | 检查用户名密码 |
| 订阅失败 | ACL 限制 | 检查 Broker 配置 |

### CoAP 问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| 请求超时 | UDP 丢包 | 实现重传机制 |
| 块传输失败 | 分块错误 | 正确设置 Block 选项 |
| 观察者断开 | Max-Age 过期 | 刷新观察注册 |
| 4.04 响应 | 资源不存在 | 检查 URI 路径 |

### Modbus 问题

| 问题 | 原因 | 解决方案 |
|:-----|:-----|:---------|
| 超时无响应 | 设备离线 | 检查物理连接 |
| CRC 错误 | 线路干扰 | 检查接线，使用屏蔽线 |
| 异常码 02 | 非法数据地址 | 检查寄存器地址 |
| 异常码 03 | 非法数据值 | 检查写入值范围 |

### 协议选择决策

```
是否需要实时双向通信?
├─ 是 → WebSocket
└─ 否
    是否需要极低功耗?
    ├─ 是 → LoRaWAN (远距离) / CoAP (短距离)
    └─ 否
        是否需要工业级可靠?
        ├─ 是 → Modbus (工业现场)
        └─ 否
            是否需要云集成?
            ├─ 是 → MQTT (推荐) / HTTP
            └─ 否 → CoAP
```

---

## 📖 推荐学习路径

### 初级路径

1. 阅读 [01_IoT_Protocols_Implementation.md](./01_IoT_Protocols_Implementation.md) 概述
2. 学习 MQTT 协议基础，实现简单客户端
3. 掌握 CoAP 请求/响应模式
4. 理解 Modbus 数据模型

### 中级路径

1. 实现多协议网关
2. 学习 TLS/DTLS 安全传输
3. 掌握 QoS 和可靠性机制
4. 理解协议状态机实现

### 高级路径

1. 设计多协议统一抽象层
2. 实现自定义协议扩展
3. 优化协议栈内存使用
4. 开发协议 fuzz 测试

---

## 🔗 返回上级

[← 返回 14_Embedded_IoT 目录](../README.md)

---

## 📞 支持与资源

- **MQTT 规范**: <https://docs.oasis-open.org/mqtt/>
- **CoAP RFC**: <https://tools.ietf.org/rfc/rfc7252.txt>
- **Modbus 协议**: <https://modbus.org/specs.php>
- **LoRaWAN 规范**: <https://lora-alliance.org/resource-hub/>
- **WebSocket RFC**: <https://tools.ietf.org/rfc/rfc6455.txt>

---

> **最后更新**: 2026-03-17
> **维护者**: C_Lang Knowledge Base Team
> **支持版本**: MQTT 3.1.1, CoAP RFC7252, Modbus RTU/TCP
