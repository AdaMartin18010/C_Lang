# CAN 总线协议知识体系

> **协议标准**: ISO 11898-1:2015 (CAN FD更新)
> **最后更新**: 2026-03-29
> **状态**: 🚧 知识梳理中

---

## 📋 协议概述

CAN（Controller Area Network，控制器局域网）是由Bosch公司于1986年开发的串行通信协议，最初用于汽车电子系统，现已成为工业自动化、医疗设备、航空航天等领域的标准通信协议。

### 核心优势

| 特性 | 说明 |
|------|------|
| **高可靠性** | 内置错误检测与处理机制 |
| **实时性** | 非破坏性仲裁，高优先级消息优先 |
| **灵活性** | 多主架构，无需主站调度 |
| **鲁棒性** | 差分信号，抗电磁干扰 |
| **经济性** | 两线制，布线成本低 |

### 应用领域

```
CAN总线应用领域
├── 汽车电子 (占比 ~70%)
│   ├── 动力系统 (发动机、变速箱)
│   ├── 底盘系统 (ABS、ESP、转向)
│   ├── 车身电子 (门窗、座椅、空调)
│   └── 信息娱乐
├── 工业控制
│   ├── 自动化生产线
│   ├── 机器人控制
│   └── 电梯控制
├── 医疗设备
│   ├── CT/MRI设备
│   └── 医疗床控制系统
└── 其他
    ├── 轨道交通
    ├── 船舶电子
    └── 航空航天
```

---

## 📁 本目录文档列表

| 文档 | 内容 | 状态 |
|------|------|------|
| [01_CAN_Overview.md](./01_CAN_Overview.md) | CAN协议整体架构 | ⏳ 待创建 |
| [02_CAN_2.0A_Standard_Frame.md](./02_CAN_2.0A_Standard_Frame.md) | 标准帧格式 | ⏳ 待创建 |
| [03_CAN_2.0B_Extended_Frame.md](./03_CAN_2.0B_Extended_Frame.md) | 扩展帧格式 | ⏳ 待创建 |
| [04_CAN_FD_Theory.md](./04_CAN_FD_Theory.md) | CAN FD详解 | ⏳ 待创建 |
| [05_Bit_Timing_Arbitration.md](./05_Bit_Timing_Arbitration.md) | 位定时与仲裁 | ⏳ 待创建 |
| [06_Error_Handling_Mechanisms.md](./06_Error_Handling_Mechanisms.md) | 错误处理机制 | ⏳ 待创建 |
| [07_Filter_Mask_Configuration.md](./07_Filter_Mask_Configuration.md) | 滤波器配置 | ⏳ 待创建 |
| [08_CAN_C_Implementation.md](./08_CAN_C_Implementation.md) | C语言实现思路 | ⏳ 待创建 |
| [09_CANopen_Introduction.md](./09_CANopen_Introduction.md) | CANopen协议介绍 | ⏳ 待创建 |

---

## 🆚 CAN 2.0 vs CAN FD 对比

| 特性 | CAN 2.0 | CAN FD |
|------|---------|--------|
| **发布时间** | 1991年 | 2012年 |
| **数据长度** | 0-8字节 | 0-64字节 |
| **数据段波特率** | ≤ 1 Mbps | ≤ 8 Mbps (理论15 Mbps) |
| **CRC长度** | 15位 | 17位(≤16字节) / 21位(>16字节) |
| **兼容性** | 基准 | 向下兼容CAN 2.0 |

---

## 🎯 C语言实现关键点

### 核心数据结构

```c
/* CAN帧结构 - 通用抽象 */
typedef struct {
    uint32_t id;          /* 11位标准ID 或 29位扩展ID */
    uint8_t  is_extended; /* 0=标准帧, 1=扩展帧 */
    uint8_t  is_remote;   /* 0=数据帧, 1=远程帧 */
    uint8_t  dlc;         /* 数据长度码 (CAN FD: 0-15, 实际0-64字节) */
    uint8_t  data[64];    /* 数据域 */
    uint8_t  is_fd;       /* 0=CAN 2.0, 1=CAN FD */
    uint8_t  brs;         /* Bit Rate Switch (CAN FD) */
} CAN_Frame;

/* CAN控制器配置 */
typedef struct {
    uint32_t clock_freq;     /* CAN时钟频率 */
    uint16_t bitrate;        /* 仲裁段波特率 */
    uint16_t data_bitrate;   /* 数据段波特率(CAN FD) */
    uint8_t  sample_point;   /* 采样点位置(%) */
} CAN_Config;
```

### 位定时计算

```c
/* 位时间 = 同步段(1) + 传播段 + 相位段1 + 相位段2 */
typedef struct {
    uint8_t sync_seg;     /* 固定1 Tq */
    uint8_t prop_seg;     /* 传播段 */
    uint8_t phase_seg1;   /* 相位段1 */
    uint8_t phase_seg2;   /* 相位段2 */
    uint8_t sjw;          /* 同步跳转宽度 */
    uint16_t prescaler;   /* 预分频器 */
} CAN_BitTiming;

/* 计算示例: 500kbps @ 48MHz时钟 */
/* 位时间 = 48MHz / (500k * prescaler) */
/* 取prescaler=6, 则位时间 = 16 Tq */
/* 配置: 1 + 4 + 8 + 3 = 16 Tq, 采样点 = (1+4+8)/16 = 81.25% */
```

---

## 📚 参考规范

- **ISO 11898-1**: 数据链路层和物理层信令
- **ISO 11898-2**: 高速物理层 (up to 1Mbps)
- **ISO 11898-3**: 低速物理层 (容错)
- **ISO 11898-4**: 时间触发CAN (TTCAN)
- **CiA 301**: CANopen应用层和通信规范
- **CiA 402**: CANopen运动控制设备规范
- **SAE J1939**: 重型车辆应用层标准

---

## 🔗 相关协议

| 协议 | 描述 | 应用 |
|------|------|------|
| **CANopen** | 应用层协议，对象字典 | 工业自动化 |
| **J1939** | 车辆网络标准 | 商用车、重型机械 |
| **DeviceNet** | 工业设备网络 | 工厂自动化 |
| **NMEA 2000** | 船舶电子设备 | 海事应用 |
| **CAN FD Light** | 轻量级CAN FD | 成本敏感应用 |
