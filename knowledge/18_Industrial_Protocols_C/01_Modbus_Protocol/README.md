# Modbus 协议知识体系

> **协议版本**: Modbus Application Protocol V1.1b3
> **最后更新**: 2026-03-29
> **状态**: 🚧 知识梳理中

---

## 📋 协议概述

Modbus是一种串行通信协议，由Modicon（现施耐德电气）于1979年开发，用于可编程逻辑控制器（PLC）通信。如今已成为工业领域事实标准，是工业电子设备之间最常用的连接方式。

### 协议特点

| 特性 | 说明 |
|------|------|
| **开放性** | 公开发布，无版权费用 |
| **简单性** | 易于部署和维护 |
| **通用性** | 支持多种物理层（RS-232/RS-485/Ethernet） |
| **广泛性** | 数百万设备支持 |

### 协议变体

```
Modbus协议族
├── Modbus RTU
│   ├── 物理层: RS-485/RS-232
│   ├── 编码: 二进制
│   └── 错误检测: CRC16
├── Modbus ASCII
│   ├── 物理层: RS-485/RS-232
│   ├── 编码: ASCII字符
│   └── 错误检测: LRC
└── Modbus TCP
    ├── 物理层: Ethernet
    ├── 编码: 二进制
    └── 错误检测: TCP校验
```

---

## 📁 本目录文档列表

| 文档 | 内容 | 状态 |
|------|------|------|
| [01_Modbus_Overview.md](./01_Modbus_Overview.md) | 协议整体架构 | ⏳ 待创建 |
| [02_Modbus_RTU_Theory.md](./02_Modbus_RTU_Theory.md) | RTU模式理论 | ⏳ 待创建 |
| [03_Modbus_ASCII_Theory.md](./03_Modbus_ASCII_Theory.md) | ASCII模式理论 | ⏳ 待创建 |
| [04_Modbus_TCP_Theory.md](./04_Modbus_TCP_Theory.md) | TCP模式理论 | ⏳ 待创建 |
| [05_Function_Codes_Detail.md](./05_Function_Codes_Detail.md) | 功能码详解 | ⏳ 待创建 |
| [06_Data_Model_Coils_Registers.md](./06_Data_Model_Coils_Registers.md) | 数据模型 | ⏳ 待创建 |
| [07_CRC_LRC_Calculation.md](./07_CRC_LRC_Calculation.md) | 校验算法 | ⏳ 待创建 |
| [08_Timing_and_Synchronization.md](./08_Timing_and_Synchronization.md) | 时序与同步 | ⏳ 待创建 |
| [09_Exception_Responses.md](./09_Exception_Responses.md) | 异常响应 | ⏳ 待创建 |
| [10_Master_Slave_Architecture.md](./10_Master_Slave_Architecture.md) | 主从架构 | ⏳ 待创建 |
| [11_Modbus_C_Implementation.md](./11_Modbus_C_Implementation.md) | C语言实现思路 | ⏳ 待创建 |
| [12_State_Machine_Design.md](./12_State_Machine_Design.md) | 状态机设计 | ⏳ 待创建 |
| [13_Buffer_Management.md](./13_Buffer_Management.md) | 缓冲区管理 | ⏳ 待创建 |
| [14_Modbus_Security_Considerations.md](./14_Modbus_Security_Considerations.md) | 安全考量 | ⏳ 待创建 |

---

## 🎯 C语言实现关键点

### 核心数据结构

```c
/* Modbus帧结构 - 通用抽象 */
typedef struct {
    uint8_t  slave_id;        /* 从机地址 */
    uint8_t  function_code;   /* 功能码 */
    uint8_t  data[252];       /* 数据域 */
    uint16_t data_length;     /* 数据长度 */
    uint16_t crc;             /* CRC校验 */
} ModbusFrame;

/* Modbus数据模型 */
typedef struct {
    uint8_t  coils[256];          /* 线圈状态 */
    uint8_t  discrete_inputs[256];/* 离散输入 */
    uint16_t holding_regs[256];   /* 保持寄存器 */
    uint16_t input_regs[256];     /* 输入寄存器 */
} ModbusDataMap;
```

### 关键算法

| 算法 | 用途 | 复杂度 |
|------|------|--------|
| CRC16-IBM | RTU帧校验 | O(n) |
| LRC | ASCII帧校验 | O(n) |
| 字节序转换 | 多字节数据处理 | O(1) |
| 超时计算 | 帧间间隔判定 | O(1) |

---

## 📚 参考规范

- [Modbus Application Protocol V1.1b3](https://modbus.org/docs/Modbus_Application_Protocol_V1_1b3.pdf)
- [Modbus over Serial Line V1.02](https://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf)
- [Modbus Messaging on TCP/IP V1.0b](https://modbus.org/docs/Modbus_Messaging_Implementation_Guide_V1_0b.pdf)
- GB/T 19582-2008 基于Modbus协议的工业自动化网络规范
