# SPI 接口深度解析

> **全称**: Serial Peripheral Interface
> **发明者**: Motorola (1980年代)
> **类型**: 同步串行通信，全双工
> **拓扑**: 主从架构，一主多从
> **最后更新**: 2026-03-29

---

## 1. 协议形式化定义

### 1.1 信号定义与拓扑

```
┌─────────────────────────────────────────────────────────────────┐
│                    SPI 信号定义与拓扑                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  信号线 (4线标准):                                               │
│  ├── SCLK (Serial Clock): 时钟信号，主设备产生                  │
│  ├── MOSI (Master Out Slave In): 主设备→从设备数据              │
│  ├── MISO (Master In Slave Out): 从设备→主设备数据              │
│  └── SS/CS (Slave Select/Chip Select): 从设备选择，低有效       │
│                                                                 │
│  拓扑结构:                                                       │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    SPI Master                           │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐              │   │
│  │  │   SCLK   │  │   MOSI   │  │   MISO   │              │   │
│  │  └────┬─────┘  └────┬─────┘  └────┬─────┘              │   │
│  │       │             │             │                     │   │
│  │       └─────────────┼─────────────┘                     │   │
│  │                     │                                    │   │
│  │  ┌────────┬────────┼────────┬────────┐                 │   │
│  │  │        │        │        │        │                 │   │
│  │  ▼        ▼        ▼        ▼        ▼                 │   │
│  │ ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐                │   │
│  │ │SS1│   │SS2│   │SS3│   │SS4│   │SSn│  (独立片选)    │   │
│  │ └───┘   └───┘   └───┘   └───┘   └───┘                │   │
│  │   │       │       │       │       │                   │   │
│  │   ▼       ▼       ▼       ▼       ▼                   │   │
│  │ ┌───┐   ┌───┐   ┌───┐   ┌───┐   ┌───┐                │   │
│  │ │S1 │   │S2 │   │S3 │   │S4 │   │Sn │  (从设备)      │   │
│  │ └───┘   └───┘   └───┘   └───┘   └───┘                │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  特点:                                                           │
│  1. 一主多从: 每个从设备有独立片选线                            │
│  2. 全双工: 同时收发，每时钟周期传输2位                         │
│  3. 同步通信: 时钟由主设备控制，无波特率误差                    │
│  4. 灵活性: 时钟极性和相位可配置                                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 形式化时序定义

**定义 1.1 (SPI传输单元)**

```
SPITransfer ::= SCLK_Cycle × MOSI_Bit × MISO_Bit

传输帧 ::= SS_Active × (SPI_Word)+ × SS_Inactive

其中:
  SPI_Word = 8位 | 16位 | 32位 (可配置)

时钟极性 (CPOL):
  CPOL = 0: 空闲时SCLK为低电平
  CPOL = 1: 空闲时SCLK为高电平

时钟相位 (CPHA):
  CPHA = 0: 第一个边沿采样，第二个边沿改变
  CPHA = 1: 第一个边沿改变，第二个边沿采样
```

### 1.3 时钟模式矩阵

| 模式 | CPOL | CPHA | 空闲时钟 | 采样边沿 | 改变边沿 | 典型应用 |
|------|------|------|----------|----------|----------|----------|
| **Mode 0** | 0 | 0 | 低 | 上升沿 | 下降沿 | 最常见，通用 |
| **Mode 1** | 0 | 1 | 低 | 下降沿 | 上升沿 | 某些传感器 |
| **Mode 2** | 1 | 0 | 高 | 下降沿 | 上升沿 | 较少使用 |
| **Mode 3** | 1 | 1 | 高 | 上升沿 | 下降沿 | 存储器，传感器 |

**时序图对比**

```
Mode 0 (CPOL=0, CPHA=0):
SCLK  ─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌─┐ ┌────
       └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─┘ └─
       ↑   ↑   ↑   ↑   ↑   ↑   ↑   ↑
      D7  D6  D5  D4  D3  D2  D1  D0
      (采样点，上升沿)

MOSI ─────┐   ┌───┐   ┌───────┐   ┌────────
          └───┘   └───┘       └───┘
          D7      D6      D5      D4

Mode 3 (CPOL=1, CPHA=1):
SCLK  ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐ ┌──┐
      └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──┘ └──
         ↑    ↑    ↑    ↑    ↑    ↑    ↑    ↑
        D7   D6   D5   D4   D3   D2   D1   D0
        (采样点，上升沿)

MOSI    ┌────┐   ┌───┐   ┌───────┐   ┌────
      ──┘    └───┘   └───┘       └───┘
        D7      D6      D5      D4
```

---

## 2. 从设备选择策略

### 2.1 独立片选 vs 菊花链

```
┌─────────────────────────────────────────────────────────────────┐
│              SPI 从设备连接策略对比                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  策略1: 独立片选 (Individual Chip Select)                        │
│  ───────────────────────────────────────                         │
│                                                                 │
│  Master              Slaves                                     │
│  ┌─────┐            ┌─────┐ ┌─────┐ ┌─────┐                    │
│  │SCLK │────────────│SCLK │ │SCLK │ │SCLK │                    │
│  │MOSI │────────────│MOSI │ │MOSI │ │MOSI │                    │
│  │MISO │◄───────────│MISO │ │MISO │ │MISO │ (需要三态控制)    │
│  │SS1  │────────────│SS   │ │     │ │     │                    │
│  │SS2  │───────────────────│SS   │ │     │                    │
│  │SS3  │──────────────────────────│SS   │                    │
│  └─────┘            └─────┘ └─────┘ └─────┘                    │
│                                                                 │
│  优点:                                                          │
│  • 每个设备可独立访问                                           │
│  • 软件控制简单                                                 │
│  • 支持全双工                                                   │
│                                                                 │
│  缺点:                                                          │
│  • n个设备需要n个片选引脚                                       │
│  • 片选线多时布线复杂                                           │
│                                                                 │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  策略2: 菊花链 (Daisy Chain)                                     │
│  ───────────────────────────                                     │
│                                                                 │
│  Master              Slaves                                     │
│  ┌─────┐            ┌─────┐    ┌─────┐    ┌─────┐             │
│  │SCLK │────────────│SCLK │────│SCLK │────│SCLK │             │
│  │MOSI │────────────│MOSI │    │     │    │     │             │
│  │     │            │MISO │────│MOSI │    │     │             │
│  │     │            │     │    │MISO │────│MOSI │             │
│  │MISO │◄─────────────────────┘    │    │MISO │────┘        │
│  │SS   │────────────│SS   │────│SS   │────│SS   │             │
│  └─────┘            └─────┘    └─────┘    └─────┘             │
│                     (Slave 1)  (Slave 2)  (Slave 3)            │
│                                                                 │
│  工作原理:                                                       │
│  • 数据从Master→Slave1→Slave2→Slave3 (MOSI链)                 │
│  • 返回数据从Slave3→Slave2→Slave1→Master (MISO链)             │
│  • 需要发送n个字节才能与第n个设备通信                           │
│                                                                 │
│  优点:                                                          │
│  • 所有设备共享一个片选                                         │
│  • 节省引脚                                                     │
│                                                                 │
│  缺点:                                                          │
│  • 不能直接访问单个设备                                         │
│  • 延迟增加 (必须遍历整个链)                                   │
│  • 编程复杂                                                     │
│                                                                 │
│  典型应用:                                                       │
│  • LED驱动器链 (如WS2801)                                      │
│  • 多ADC级联                                                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 片选控制决策树

```
SPI片选控制策略
│
├── 硬件片选 (Hardware NSS)
│   ├── 自动片选控制
│   │   ├── 优点: 精确时序，无软件延迟
│   │   └── 限制: 灵活性差，通常只支持1个设备
│   │
│   └── 适用场景
│       └── 单一从设备，高速通信
│
└── 软件片选 (Software GPIO)
    ├── 手动片选控制
    │   ├── 优点: 灵活，支持无限多设备
    │   └── 缺点: 软件开销，可能有延迟
    │
    ├── 片选极性
    │   ├── 低有效 (常见)
    │   └── 高有效 (某些设备)
    │
    ├── 片选时序
    │   ├── 建立时间: SS下降沿到第一个时钟
    │   ├── 保持时间: 最后一个时钟到SS上升沿
    │   └── 间隔时间: 两次传输间的最小间隔
    │
    └── 适用场景
        └── 多设备，需要灵活控制
```

---

## 3. C语言驱动架构

### 3.1 分层架构设计

```c
/* SPI分层架构 */

/* 层1: 硬件抽象层 (HAL) */
typedef struct {
    volatile uint32_t *cr1;     /* 控制寄存器1 */
    volatile uint32_t *cr2;     /* 控制寄存器2 */
    volatile uint32_t *sr;      /* 状态寄存器 */
    volatile uint32_t *dr;      /* 数据寄存器 */
} SPI_Registers;

/* 层2: 驱动层 */
typedef struct {
    SPI_Registers *regs;
    uint32_t clock_freq;
    uint8_t mode;               /* CPOL:1 | CPHA:1 = Mode 0-3 */
    uint8_t bit_order;          /* MSB/LSB first */
    uint8_t data_size;          /* 8/16 bit */
} SPI_Handle;

/* 层3: 设备抽象层 */
typedef struct {
    SPI_Handle *spi;
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;
    uint32_t cs_setup_time;     /* ns */
    uint32_t cs_hold_time;      /* ns */
} SPI_Device;

/* 层4: 应用层 */
int SPI_Device_TransmitReceive(SPI_Device *dev,
                                uint8_t *tx_buf,
                                uint8_t *rx_buf,
                                uint16_t len);
```

### 3.2 时序控制形式化

**定义 3.1 (片选时序约束)**

```
设:
  t_setup = SS下降沿到SCLK第一个边沿的时间
  t_hold = SCLK最后一个边沿到SS上升沿的时间
  t_idle = 两次传输间SS高电平的最短时间

约束:
  t_setup ≥ t_setup_min (设备依赖，通常10-50ns)
  t_hold ≥ t_hold_min (设备依赖，通常10-50ns)
  t_idle ≥ t_idle_min (设备依赖，通常100ns-1μs)

C语言实现:
  void spi_select(SPI_Device *dev) {
      GPIO_Clear(dev->cs_port, dev->cs_pin);
      delay_ns(dev->cs_setup_time);  /* 满足t_setup */
  }

  void spi_deselect(SPI_Device *dev) {
      delay_ns(dev->cs_hold_time);   /* 满足t_hold */
      GPIO_Set(dev->cs_port, dev->cs_pin);
      delay_ns(dev->cs_idle_time);   /* 满足t_idle */
  }
```

---

## 4. DMA传输优化

### 4.1 DMA传输决策树

```
SPI传输方式选择
│
├── 数据量 < 8字节?
│   ├── YES → 轮询方式
│   │           ├── 优点: 简单，无中断开销
│   │           └── 代码: 直接读写DR寄存器
│   │
│   └── NO → 继续判断
│
├── 需要异步传输?
│   ├── YES → DMA方式
│   │           ├── 优点: CPU零开销，适合大数据量
│   │           ├── 复杂度: 高 (需配置DMA流)
│   │           └── 代码: 设置DMA，等待完成中断
│   │
│   └── NO → 中断方式
│               ├── 优点: 响应快，适合中等数据量
│               ├── 缺点: 频繁中断开销
│               └── 代码: 每字节中断
│
└── 双缓冲需求?
    └── YES → DMA + 双缓冲
                └── 乒乓操作，零拷贝
```

### 4.2 DMA配置代码框架

```c
/* SPI DMA传输 */
int SPI_TransmitReceive_DMA(SPI_Handle *spi,
                             uint8_t *tx_buf,
                             uint8_t *rx_buf,
                             uint16_t len) {
    /* 1. 配置TX DMA */
    DMA_Config tx_cfg = {
        .source = tx_buf,
        .dest = &spi->regs->dr,
        .size = len,
        .dir = DMA_DIR_MEM_TO_PERIPH,
        .mode = DMA_MODE_NORMAL
    };
    DMA_Init(spi->tx_dma, &tx_cfg);

    /* 2. 配置RX DMA */
    DMA_Config rx_cfg = {
        .source = &spi->regs->dr,
        .dest = rx_buf,
        .size = len,
        .dir = DMA_DIR_PERIPH_TO_MEM,
        .mode = DMA_MODE_NORMAL
    };
    DMA_Init(spi->rx_dma, &rx_cfg);

    /* 3. 使能SPI DMA请求 */
    *spi->regs->cr2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;

    /* 4. 启动DMA */
    DMA_Start(spi->tx_dma);
    DMA_Start(spi->rx_dma);

    /* 5. 等待完成 (中断或轮询) */
    return wait_for_completion(spi, len);
}

/* 关键: 全双工要求TX和RX DMA同时工作 */
/* 如果只发送，仍需读取DR以清空RXFIFO */
```

---

## 5. 常见设备协议

### 5.1 Flash存储器协议

```
SPI Flash操作命令集
│
├── 读操作
│   ├── 0x03: Read Data (标准读取，最多50MHz)
│   ├── 0x0B: Fast Read (快速读取， Dummy周期)
│   └── 0xEB: Quad I/O Fast Read (4线I/O)
│
├── 编程操作 (写)
│   ├── 0x06: Write Enable (写使能)
│   ├── 0x02: Page Program (页编程，256字节)
│   └── 0x04: Write Disable (写禁止)
│
├── 擦除操作
│   ├── 0x20: Sector Erase (4KB擦除)
│   ├── 0x52: Block Erase (32KB擦除)
│   ├── 0xD8: Block Erase (64KB擦除)
│   └── 0xC7: Chip Erase (整片擦除)
│
└── 状态检查
    └── 0x05: Read Status Register
        ├── Bit0: WIP (Write In Progress)
        └── Bit1: WEL (Write Enable Latch)
```

### 5.2 传感器接口协议

```c
/* SPI传感器读取示例 (LIS3DH加速度计) */
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} AccelData;

/* 读取多字节寄存器 */
int LIS3DH_ReadAccel(SPI_Device *dev, AccelData *data) {
    uint8_t tx_buf[7] = {0xE8, 0, 0, 0, 0, 0, 0};  /* 0x28 | 0xC0 (多字节读取) */
    uint8_t rx_buf[7];

    /* 发送读取命令，同时接收数据 */
    spi_transfer(dev, tx_buf, rx_buf, 7);

    /* 解析数据 (小端序) */
    data->x = (int16_t)(rx_buf[1] | (rx_buf[2] << 8));
    data->y = (int16_t)(rx_buf[3] | (rx_buf[4] << 8));
    data->z = (int16_t)(rx_buf[5] | (rx_buf[6] << 8));

    return 0;
}

/* 关键: SPI是环形缓冲区，发送和接收同时发生 */
/* 要读取n字节，必须发送n字节 (通常是0x00) */
```

---

## 6. 故障排查决策树

```
SPI通信故障诊断
│
├── 无数据返回
│   ├── 检查物理连接
│   │   ├── SCLK/MOSI/MISO/SS是否连通?
│   │   ├── 地线是否连接?
│   │   └── 电平是否匹配? (3.3V vs 5V)
│   │
│   ├── 检查片选
│   │   ├── 片选引脚配置正确?
│   │   ├── 片选极性正确? (低有效vs高有效)
│   │   └── 片选时序满足? (setup/hold)
│   │
│   └── 检查设备
│       └── 设备是否上电?
│
├── 数据错误
│   ├── 时钟模式匹配?
│   │   ├── 检查CPOL/CPHA配置
│   │   └── 与设备数据手册对比
│   │
│   ├── 时钟速率过高?
│   │   └── 降低时钟频率测试
│   │
│   ├── 信号完整性问题?
│   │   ├── 线缆过长? (建议<30cm)
│   │   └── 无终端电阻? (高速时需考虑)
│   │
│   └── 数据位序错误?
│       └── 检查MSB/LSB first配置
│
└── 间歇性错误
    ├── 片选信号毛刺?
    │   └── 增加片选消抖
    ├── 电源不稳?
    │   └── 检查电源纹波
    └── 中断干扰?
        └── 关中断或使用DMA
```

---

## 7. 参考规范

- **Motorola SPI Specification**: 原始SPI规范
- **SPI Block Guide V04.01**: 详细时序定义
- **JEDEC JESD251**: xSPI (扩展SPI) 标准
- **各设备数据手册**: 具体时序和命令集
