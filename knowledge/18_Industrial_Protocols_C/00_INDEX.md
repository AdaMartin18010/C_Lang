# 工业通信协议知识库索引

> **模块**: 18_Industrial_Protocols_C
> **定位**: 从C语言视角梳理工业通信协议知识体系
> **最后更新**: 2026-03-29

---

## 快速导航

### 协议基础

- [协议基础理论](./00_Protocol_Fundamentals/)

### 核心协议

| 协议 | 难度 | 应用场景 | 入口 |
|------|------|----------|------|
| **Modbus** | ⭐⭐ | 通用工业通信 | [01_Modbus_Protocol](./01_Modbus_Protocol/) |
| **CAN Bus** | ⭐⭐⭐ | 汽车、工业控制 | [02_CAN_Bus_Protocol](./02_CAN_Bus_Protocol/) |
| **OPC UA** | ⭐⭐⭐⭐⭐ | 工业4.0、数字化 | [03_OPC_UA_Protocol](./03_OPC_UA_Protocol/) |
| **EtherCAT** | ⭐⭐⭐⭐⭐ | 运动控制 | [04_EtherCAT_Protocol](./04_EtherCAT_Protocol/) |

### 扩展内容

- [工业以太网](./05_Industrial_Ethernet/)
- [无线工业协议](./06_Wireless_Industrial/)
- [协议对比分析](./07_Protocol_Comparison/)
- [实际案例](./08_Case_Studies/)

---

## 学习路径建议

### 路径1: 工业自动化工程师

```
Modbus RTU → Modbus TCP → CAN Bus → CANopen → 工业以太网
```

### 路径2: 汽车电子工程师

```
CAN 2.0 → CAN FD → J1939 → LIN → Automotive Ethernet
```

### 路径3: 工业软件开发者

```
Modbus → OPC UA → EtherCAT → TSN → 工业云网关
```

---

## 知识关联图

```
                    ┌─────────────────────────────────────┐
                    │         工业通信协议知识体系          │
                    └─────────────────────────────────────┘
                                      │
          ┌───────────────────────────┼───────────────────────────┐
          │                           │                           │
          ▼                           ▼                           ▼
┌─────────────────────┐  ┌─────────────────────┐  ┌─────────────────────┐
│   现场总线层         │  │   工业以太网层       │  │   无线层            │
├─────────────────────┤  ├─────────────────────┤  ├─────────────────────┤
│ • Modbus RTU        │  │ • Modbus TCP        │  │ • WirelessHART      │
│ • CAN Bus           │  │ • Profinet          │  │ • ISA100            │
│ • CANopen           │  │ • EtherNet/IP       │  │ • LoRa              │
│ • DeviceNet         │  │ • EtherCAT          │  │ • Zigbee            │
│ • Profibus          │  │ • TSN               │  │ • WiFi Industrial   │
└─────────────────────┘  └─────────────────────┘  └─────────────────────┘
          │                           │                           │
          └───────────────────────────┼───────────────────────────┘
                                      │
                                      ▼
                    ┌─────────────────────────────────────┐
                    │      应用层协议 (OPC UA/MQTT)        │
                    └─────────────────────────────────────┘
```

---

## C语言实现要点速查

| 协议 | 核心数据结构 | 关键算法 | 硬件接口 |
|------|-------------|----------|----------|
| Modbus RTU | 帧缓冲区、状态机 | CRC16 | UART/RS-485 |
| Modbus TCP | Socket、事务处理 | MBAP头解析 | Ethernet |
| CAN | 消息对象、滤波器 | 位定时计算 | CAN控制器 |
| CAN FD | 扩展帧缓冲区 | 双波特率 | CAN FD控制器 |
| OPC UA | 节点地址空间 | 加密、序列化 | TCP/IP |

---

## 相关模块

- [19_Embedded_Systems_C](../19_Embedded_Systems_C/) - 嵌入式系统实现
- [20_Hardware_Interfaces_C](../20_Hardware_Interfaces_C/) - 底层硬件接口
- [04_Industrial_Scenarios](../04_Industrial_Scenarios/) - 工业场景应用
