# 20_Hardware_Interfaces_C - 硬件接口（C语言驱动视角）

> **定位**: 从C语言驱动开发角度梳理硬件接口知识
> **目标**: 覆盖常见外设接口的理论、驱动架构、C语言实现模式
> **状态**: 🚧 持续构建中
> **最后更新**: 2026-03-29

---

## 📑 目录结构

```
20_Hardware_Interfaces_C/
├── README.md                           # 本文件
├── 00_Interface_Fundamentals/          # 接口基础
│   ├── 01_Synchronous_vs_Asynchronous.md
│   ├── 02_Parallel_vs_Serial.md
│   ├── 03_Master_Slave_Communication.md
│   ├── 04_Push_Pull_vs_Open_Drain.md
│   ├── 05_Electrical_Characteristics.md
│   └── 06_Signal_Integrity_Basics.md
├── 01_UART_Interfaces/                 # UART接口
│   ├── README.md
│   ├── 01_UART_Fundamentals.md
│   ├── 02_Baud_Rate_Generation.md
│   ├── 03_Framing_and_Parity.md
│   ├── 04_Flow_Control_RTS_CTS.md
│   ├── 05_UART_Driver_Architecture.md
│   ├── 06_Ring_Buffer_Implementation.md
│   ├── 07_RS485_Half_Duplex.md
│   └── 08_Modbus_over_UART.md
├── 02_SPI_Interfaces/                  # SPI接口
│   ├── README.md
│   ├── 01_SPI_Fundamentals.md
│   ├── 02_Clock_Polarity_Phase.md
│   ├── 03_Multi_Slave_Configurations.md
│   ├── 04_SPI_Driver_Design.md
│   ├── 05_DMA_with_SPI.md
│   ├── 06_QSPI_and_Quad_SPI.md
│   └── 07_Flash_Memory_Interface.md
├── 03_I2C_Interfaces/                  # I2C接口
│   ├── README.md
│   ├── 01_I2C_Fundamentals.md
│   ├── 02_Start_Stop_Conditions.md
│   ├── 03_Addressing_7bit_10bit.md
│   ├── 04_Clock_Stretching.md
│   ├── 05_Arbitration_Multi_Master.md
│   ├── 06_I2C_Driver_State_Machine.md
│   ├── 07_SMBus_Extensions.md
│   └── 08_I2C_Device_Discovery.md
├── 04_CAN_Interfaces/                  # CAN接口
│   ├── README.md
│   ├── 01_CAN_Physical_Layer.md
│   ├── 02_CAN_Controller_Registers.md
│   ├── 03_Mailbox_vs_FIFO.md
│   ├── 04_Acceptance_Filtering.md
│   └── 05_CAN_Error_Handling.md
├── 05_USB_Interfaces/                  # USB接口
│   ├── README.md
│   ├── 01_USB_Fundamentals.md
│   ├── 02_USB_Descriptors.md
│   ├── 03_CDC_Class.md
│   ├── 04_HID_Class.md
│   └── 05_USB_Driver_Framework.md
├── 06_Ethernet_Interfaces/             # 以太网接口
│   ├── README.md
│   ├── 01_Ethernet_PHY_Basics.md
│   ├── 02_MAC_Controller.md
│   ├── 03_MII_RMII_GMII.md
│   └── 04_LwIP_Integration.md
├── 07_Wireless_Interfaces/             # 无线接口
│   ├── README.md
│   ├── 01_WiFi_Espressif.md
│   ├── 02_Bluetooth_Low_Energy.md
│   ├── 03_Zigbee_802_15_4.md
│   └── 04_LoRa_LoRaWAN.md
├── 08_Memory_Interfaces/               # 存储器接口
│   ├── README.md
│   ├── 01_Flash_NOR_vs_NAND.md
│   ├── 02_EEPROM_Interface.md
│   ├── 03_SD_MMC_Card.md
│   ├── 04_External_RAM.md
│   └── 05_Memory_Mapped_IO.md
├── 09_Analog_Interfaces/               # 模拟接口
│   ├── README.md
│   ├── 01_ADC_Types_SAR_SigmaDelta.md
│   ├── 02_Sampling_Theory.md
│   ├── 03_DAC_Interfaces.md
│   └── 04_Comparators_and_Triggers.md
└── 10_Advanced_Interfaces/             # 高级接口
    ├── README.md
    ├── 01_MIPI_DSI_CSI.md
    ├── 02_LVDS_Differential.md
    ├── 03_PCIe_Basics.md
    └── 04_HDMI_DisplayPort.md
```

---

## 🔌 接口协议对比矩阵

| 接口 | 信号线 | 速度 | 距离 | C语言关注点 | 典型应用 |
|------|--------|------|------|-------------|----------|
| UART | 2 (TX/RX) | < 1Mbps | 短 | 波特率配置、环形缓冲区 | 调试、GPS、蓝牙 |
| SPI | 4+ (SCK/MOSI/MISO/CS) | < 50MHz | 短 | 时钟极性、DMA传输 | 传感器、Flash、显示屏 |
| I2C | 2 (SDA/SCL) | < 3.4MHz | 中 | 地址冲突、时钟拉伸 | EEPROM、传感器、RTC |
| CAN | 2 (CAN_H/CAN_L) | < 1Mbps (CAN FD: 8Mbps) | 长 | 滤波器、错误处理 | 汽车、工业控制 |
| USB | 2 (D+/D-) | < 480Mbps (HS) | 中 | 描述符、端点管理 | 设备连接、调试 |
| Ethernet | 4 (TX+/TX-/RX+/RX-) | 1Gbps | 长 | MAC配置、DMA描述符 | 网络通信 |

---

## 🛠️ C语言驱动设计模式

### 分层架构模型

```
┌─────────────────────────────────────────────┐
│           Application Layer                 │  ← 业务逻辑
├─────────────────────────────────────────────┤
│        Protocol Stack Layer                 │  ← Modbus/CANopen等
├─────────────────────────────────────────────┤
│         Driver Abstraction Layer            │  ← 统一接口
├─────────────────────────────────────────────┤
│        Hardware Interface Layer             │  ← 寄存器操作
├─────────────────────────────────────────────┤
│            Hardware                         │  ← 物理外设
└─────────────────────────────────────────────┘
```

### 关键设计模式

| 模式 | 用途 | C语言实现 |
|------|------|-----------|
| **环形缓冲区** | 中断安全的收发 | 读写指针管理 |
| **状态机** | 协议解析 | switch-case/函数指针表 |
| **回调函数** | 事件通知 | 函数指针注册 |
| **设备抽象** | 多平台兼容 | 结构体+函数指针 |
| **DMA链式** | 大数据传输 | 描述符链表 |

---

## 📊 接口选型决策树

```
需要通信？
├── 片内通信（芯片内部）
│   ├── 高速 → AXI/AHB总线
│   └── 低速 → APB总线
└── 片外通信
    ├── 点对点
    │   ├── 高速 → SPI/QSPI
    │   ├── 中速 → I2C
    │   └── 长距离 → RS485/CAN
    ├── 网络拓扑
    │   ├── 多主 → CAN/I2C
    │   └── 主从 → SPI/Modbus
    └── 无线
        ├── 高速 → WiFi
        ├── 低功耗 → BLE/Zigbee
        └── 远距离 → LoRa
```

---

## 🔗 关联模块

| 关联模块 | 关联内容 |
|----------|----------|
| `18_Industrial_Protocols_C` | 基于硬件接口的上层协议 |
| `19_Embedded_Systems_C` | 驱动在嵌入式系统中的应用 |
| `04_Industrial_Scenarios` | 实际硬件设计案例 |

---

## 📝 文档编写重点

1. **时序图**: 使用ASCII时序图展示信号交互
2. **寄存器描述**: 位域定义、读写属性、复位值
3. **状态机**: 驱动内部的状态转换图
4. **代码框架**: 展示API设计，非完整实现
5. **故障排查**: 常见硬件问题及软件检测方法
