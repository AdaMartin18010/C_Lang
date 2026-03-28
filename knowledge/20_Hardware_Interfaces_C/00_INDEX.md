# 硬件接口知识库索引

> **模块**: 20_Hardware_Interfaces_C
> **定位**: 从C语言驱动开发角度梳理硬件接口知识
> **最后更新**: 2026-03-29

---

## 快速导航

### 接口基础

- [接口基础理论](./00_Interface_Fundamentals/)

### 有线通信接口

| 接口 | 速度 | 距离 | 复杂度 | 入口 |
|------|------|------|--------|------|
| **UART** | < 1Mbps | 短 | ⭐⭐ | [01_UART_Interfaces](./01_UART_Interfaces/) |
| **SPI** | < 50MHz | 短 | ⭐⭐ | [02_SPI_Interfaces](./02_SPI_Interfaces/) |
| **I2C** | < 3.4Mbps | 中 | ⭐⭐⭐ | [03_I2C_Interfaces](./03_I2C_Interfaces/) |
| **CAN** | < 8Mbps | 长 | ⭐⭐⭐⭐ | [04_CAN_Interfaces](./04_CAN_Interfaces/) |
| **USB** | < 480Mbps | 中 | ⭐⭐⭐⭐⭐ | [05_USB_Interfaces](./05_USB_Interfaces/) |
| **Ethernet** | < 1Gbps | 长 | ⭐⭐⭐⭐ | [06_Ethernet_Interfaces](./06_Ethernet_Interfaces/) |

### 无线接口

- [无线接口](./07_Wireless_Interfaces/)

### 存储与模拟接口

- [存储器接口](./08_Memory_Interfaces/)
- [模拟接口](./09_Analog_Interfaces/)

### 高级接口

- [高级接口](./10_Advanced_Interfaces/)

---

## 接口选型决策矩阵

| 应用场景 | 推荐接口 | 理由 |
|----------|----------|------|
| 板内芯片间通信 | SPI/I2C | 简单、引脚少 |
| 传感器网络 | I2C | 多设备共享总线 |
| 高速数据 | SPI/Ethernet | 带宽高 |
| 工业现场 | CAN/RS-485 | 抗干扰、距离远 |
| 外设连接 | USB | 热插拔、通用 |
| 无线通信 | BLE/WiFi | 灵活、无布线 |

---

## C语言驱动设计模式

```
驱动分层架构
├── 应用层
│   └── 业务逻辑
├── 协议层 (可选)
│   └── Modbus/CANopen等
├── 驱动抽象层
│   ├── 设备无关API
│   └── 统一接口
├── HAL层
│   ├── 寄存器访问
│   └── 中断处理
└── 硬件层
    └── 物理外设
```

---

## 相关模块

- [18_Industrial_Protocols_C](../18_Industrial_Protocols_C/) - 基于硬件接口的协议实现
- [19_Embedded_Systems_C](../19_Embedded_Systems_C/) - 嵌入式系统集成
- [04_Industrial_Scenarios](../04_Industrial_Scenarios/) - 实际应用案例
