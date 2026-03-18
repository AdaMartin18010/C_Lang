# MIT 6.S081 Lab 7: Network Driver

## 1. 实验概述

### 1.1 目标

- 理解网络设备驱动架构
- 掌握DMA和内存映射I/O
- 实现网卡驱动程序
- 理解网络协议栈底层

### 1.2 E1000网卡架构

```text
E1000网卡架构
├── PCI接口
│   └── 配置空间访问
├── 内存映射寄存器
│   └── 控制和状态寄存器(CSR)
├── DMA引擎
│   ├── 发送DMA (TX)
│   └── 接收DMA (RX)
├── 描述符环
│   ├── TX描述符环
│   └── RX描述符环
└── 数据包缓冲区
    ├── 发送缓冲区
    └── 接收缓冲区
```

---

## 2. PCI设备初始化

### 2.1 PCI配置空间

```c
// kernel/e1000.c

// E1000 PCI配置
#define E1000_VENDOR_ID 0x8086
#define E1000_DEVICE_ID 0x100E  // 82540EM

// PCI配置空间寄存器偏移
#define PCI_VENDOR_ID   0x00
#define PCI_DEVICE_ID   0x02
#define PCI_COMMAND     0x04
#define PCI_BAR0        0x10    // 内存映射基地址
#define PCI_BAR1        0x14    // I/O端口基地址

struct pci_dev {
    uint32_t reg_base[6];   // BAR寄存器基地址
    uint8_t irq;            // 中断线
};

// 检测E1000网卡
int e1000_probe(struct pci_dev *dev) {
    uint16_t vendor = pci_read_config_word(dev, PCI_VENDOR_ID);
    uint16_t device = pci_read_config_word(dev, PCI_DEVICE_ID);

    if (vendor != E1000_VENDOR_ID || device != E1000_DEVICE_ID)
        return -1;

    printf("e1000: found Intel 82540EM\n");

    // 启用PCI设备
    uint16_t cmd = pci_read_config_word(dev, PCI_COMMAND);
    cmd |= PCI_COMMAND_MEM | PCI_COMMAND_MASTER;  // 启用内存访问和总线主控
    pci_write_config_word(dev, PCI_COMMAND, cmd);

    // 获取内存映射基地址
    dev->reg_base[0] = pci_read_config_dword(dev, PCI_BAR0) & ~0xF;

    // 获取中断线
    dev->irq = pci_read_config_byte(dev, PCI_INTERRUPT_LINE);

    return 0;
}
```

### 2.2 内存映射寄存器访问

```c
// E1000寄存器偏移
#define E1000_CTRL      0x00000    // 设备控制
#define E1000_STATUS    0x00008    // 设备状态
#define E1000_EERD      0x00014    // EEPROM读取
#define E1000_ICR       0x000C0    // 中断原因读取
#define E1000_IMS       0x000D0    // 中断掩码设置
#define E1000_RCTL      0x00100    // 接收控制
#define E1000_TCTL      0x00400    // 发送控制
#define E1000_RDBAL     0x02800    // 接收描述符基地址(低)
#define E1000_RDBAH     0x02804    // 接收描述符基地址(高)
#define E1000_RDLEN     0x02808    // 接收描述符长度
#define E1000_RDH       0x02810    // 接收描述符头
#define E1000_RDT       0x02818    // 接收描述符尾
#define E1000_TDBAL     0x03800    // 发送描述符基地址(低)
#define E1000_TDBAH     0x03804    // 发送描述符基地址(高)
#define E1000_TDLEN     0x03808    // 发送描述符长度
#define E1000_TDH       0x03810    // 发送描述符头
#define E1000_TDT       0x03818    // 发送描述符尾

// 寄存器读写
static inline uint32_t e1000_read_reg(uint32_t reg) {
    return *(volatile uint32_t *)(e1000_regs + reg);
}

static inline void e1000_write_reg(uint32_t reg, uint32_t val) {
    *(volatile uint32_t *)(e1000_regs + reg) = val;
}
```

---

## 3. 描述符环机制

### 3.1 发送描述符

```c
// 发送描述符结构
struct tx_desc {
    uint64_t addr;      // 数据包缓冲区地址
    uint16_t length;    // 数据包长度
    uint8_t  cso;       // 校验和偏移
    uint8_t  cmd;       // 命令字段
    uint8_t  status;    // 状态字段
    uint8_t  css;       // 校验和开始
    uint16_t special;   // 特殊字段
};

// 发送描述符环
#define TX_RING_SIZE 16
struct tx_desc tx_ring[TX_RING_SIZE] __attribute__((aligned(16)));
char tx_bufs[TX_RING_SIZE][2048];  // 发送缓冲区

// 初始化发送环
void e1000_init_tx(void) {
    // 清零描述符环
    memset(tx_ring, 0, sizeof(tx_ring));

    // 设置发送描述符基地址
    e1000_write_reg(E1000_TDBAL, (uint64_t)tx_ring);
    e1000_write_reg(E1000_TDBAH, (uint64_t)tx_ring >> 32);
    e1000_write_reg(E1000_TDLEN, sizeof(tx_ring));

    // 初始化和尾指针
    e1000_write_reg(E1000_TDH, 0);
    e1000_write_reg(E1000_TDT, 0);

    // 配置发送控制寄存器
    uint32_t tctl = E1000_TCTL_EN |      // 启用发送
                    E1000_TCTL_PSP |     // 填充短包
                    (0x10 << 4) |        // 冷阈值
                    (0x40 << 12);        // 热阈值
    e1000_write_reg(E1000_TCTL, tctl);

    // 设置IPG (帧间隙)
    e1000_write_reg(E1000_TIPG, 10 | (8 << 10) | (6 << 20));
}
```

### 3.2 接收描述符

```c
// 接收描述符结构
struct rx_desc {
    uint64_t addr;      // 数据包缓冲区地址
    uint16_t length;    // 接收长度
    uint16_t checksum;  // 校验和
    uint8_t  status;    // 状态字段
    uint8_t  errors;    // 错误字段
    uint16_t special;   // 特殊字段
};

// 接收描述符环
#define RX_RING_SIZE 16
struct rx_desc rx_ring[RX_RING_SIZE] __attribute__((aligned(16)));
char rx_bufs[RX_RING_SIZE][2048];  // 接收缓冲区

// 初始化接收环
void e1000_init_rx(void) {
    // 清零描述符环
    memset(rx_ring, 0, sizeof(rx_ring));

    // 分配接收缓冲区
    for (int i = 0; i < RX_RING_SIZE; i++) {
        rx_ring[i].addr = (uint64_t)rx_bufs[i];
    }

    // 设置接收描述符基地址
    e1000_write_reg(E1000_RDBAL, (uint64_t)rx_ring);
    e1000_write_reg(E1000_RDBAH, (uint64_t)rx_ring >> 32);
    e1000_write_reg(E1000_RDLEN, sizeof(rx_ring));

    // 初始化和尾指针
    e1000_write_reg(E1000_RDH, 0);
    e1000_write_reg(E1000_RDT, RX_RING_SIZE - 1);

    // 配置接收控制寄存器
    uint32_t rctl = E1000_RCTL_EN |      // 启用接收
                    E1000_RCTL_BAM |     // 接收广播
                    E1000_RCTL_SECRC |   // 剥离CRC
                    (3 << 16);           // 4KB缓冲区
    e1000_write_reg(E1000_RCTL, rctl);
}
```

---

## 4. 数据包发送与接收

### 4.1 发送数据包

```c
// 发送数据包
int e1000_tx(void *data, int len) {
    // 获取当前尾指针
    uint32_t tail = e1000_read_reg(E1000_TDT);
    uint32_t head = e1000_read_reg(E1000_TDH);

    // 检查是否有空闲描述符
    uint32_t next_tail = (tail + 1) % TX_RING_SIZE;
    if (next_tail == head) {
        printf("e1000_tx: ring full\n");
        return -1;
    }

    // 复制数据到发送缓冲区
    memcpy(tx_bufs[tail], data, len);

    // 设置描述符
    tx_ring[tail].addr = (uint64_t)tx_bufs[tail];
    tx_ring[tail].length = len;
    tx_ring[tail].cmd = E1000_TXD_CMD_EOP |  // 包结束
                        E1000_TXD_CMD_RS;    // 报告状态
    tx_ring[tail].status = 0;

    // 更新尾指针，通知网卡
    e1000_write_reg(E1000_TDT, next_tail);

    return 0;
}
```

### 4.2 接收数据包

```c
// 接收数据包
int e1000_rx(void *buf, int max_len) {
    // 获取当前头指针
    uint32_t head = e1000_read_reg(E1000_RDH);
    uint32_t tail = (e1000_read_reg(E1000_RDT) + 1) % RX_RING_SIZE;

    // 检查是否有新数据包
    if (head == tail)
        return 0;  // 没有新数据包

    // 检查描述符状态
    if (!(rx_ring[head].status & E1000_RXD_STAT_DD)) {
        return 0;  // 网卡还未写入
    }

    // 检查错误
    if (rx_ring[head].errors) {
        printf("e1000_rx: error %x\n", rx_ring[head].errors);
        rx_ring[head].status = 0;  // 清零状态
        return -1;
    }

    // 复制数据
    int len = rx_ring[head].length;
    if (len > max_len)
        len = max_len;
    memcpy(buf, rx_bufs[head], len);

    // 清零状态，归还描述符
    rx_ring[head].status = 0;

    // 更新尾指针
    e1000_write_reg(E1000_RDT, head);

    return len;
}
```

---

## 5. 中断处理

### 5.1 中断初始化

```c
// 初始化中断
void e1000_init_irq(void) {
    // 清零中断原因寄存器
    e1000_read_reg(E1000_ICR);

    // 启用接收中断和发送完成中断
    e1000_write_reg(E1000_IMS, E1000_IMS_RXT0 | E1000_IMS_TXDW);
}

// 中断处理函数
void e1000_intr(void) {
    uint32_t icr = e1000_read_reg(E1000_ICR);

    if (icr & E1000_ICR_RXT0) {
        // 接收中断
        // 通知网络栈处理数据包
        net_rx();
    }

    if (icr & E1000_ICR_TXDW) {
        // 发送完成中断
        // 清理已发送的描述符
        e1000_tx_cleanup();
    }
}
```

---

## 6. 与网络栈集成

### 6.1 数据包处理流程

```text
发送流程:
应用程序 → 网络栈(TCP/IP) → 网卡驱动 → DMA → 网卡 → 网络

接收流程:
网络 → 网卡 → DMA → 网卡驱动 → 网络栈 → 应用程序
```

### 6.2 驱动初始化流程

```c
void e1000_init(struct pci_dev *pci_dev) {
    // 1. PCI设备检测和配置
    if (e1000_probe(pci_dev) < 0)
        panic("e1000: probe failed");

    // 2. 映射寄存器空间
    e1000_regs = iomap(pci_dev->reg_base[0], 0x20000);

    // 3. 重置网卡
    e1000_reset();

    // 4. 读取MAC地址
    e1000_read_mac();

    // 5. 初始化多播表
    e1000_init_multicast();

    // 6. 初始化发送环
    e1000_init_tx();

    // 7. 初始化接收环
    e1000_init_rx();

    // 8. 初始化中断
    e1000_init_irq();
    pci_register_irq(pci_dev->irq, e1000_intr);

    printf("e1000: initialized\n");
}
```

---

## 7. 关键概念

### 7.1 DMA (直接内存访问)

```text
传统I/O:
CPU ←→ 数据 ←→ 设备
(占用CPU时间)

DMA:
CPU设置DMA控制器
    ↓
DMA控制器 ←→ 数据 ←→ 设备
(不占用CPU)
    ↓
DMA完成中断通知CPU
```

### 7.2 描述符环

```text
发送描述符环 (循环缓冲区):

    0     1     2     3     ...    N-1
  ┌─────┬─────┬─────┬─────┬─────┬─────┐
  │     │     │     │     │     │     │
  └─────┴─────┴─────┴─────┴─────┴─────┘
    ↑                       ↑
   HEAD (网卡读取)        TAIL (驱动写入)

当HEAD == TAIL时，环为空
当(TAIL+1) % N == HEAD时，环为满
```

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 8-10小时
