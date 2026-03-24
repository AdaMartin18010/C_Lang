---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# MIT 6.S081 Lab 7: Network Driver

## 1. 实验概述

### 1.1 目标

- 理解网络设备驱动程序的工作原理
- 实现E1000网卡的DMA管理
- 处理网卡中断
- 实现网络栈接口 (socket API)
- 完成可靠的数据包收发

### 1.2 E1000网卡概述

```
Intel E1000 (8254x系列) 以太网控制器:

特性:
- PCI接口
- DMA传输 (Direct Memory Access)
- 硬件校验和计算
- 多个发送/接收队列

关键组件:
┌─────────────────────────────────────┐
│           E1000 Controller          │
├─────────────────────────────────────┤
│  ┌─────────┐     ┌─────────┐        │
│  │ TX DMA  │     │ RX DMA  │        │
│  │ Engine  │     │ Engine  │        │
│  └────┬────┘     └────┬────┘        │
│       │               │             │
│  ┌────▼────┐     ┌────▼────┐        │
│  │ TX Ring │     │ RX Ring │        │
│  │ (描述符) │     │ (描述符) │        │
│  └────┬────┘     └────┬────┘        │
│       │               │             │
│  ┌────▼────┐     ┌────▼────┐        │
│  │ TX FIFO │     │ RX FIFO │        │
│  └─────────┘     └─────────┘        │
└─────────────────────────────────────┘
         │               │
         └───────┬───────┘
                 │
            以太网线
```

### 1.3 DMA环原理

```
发送描述符环 (Transmit Descriptor Ring):

软件                                 硬件
  │                                    │
  │  1. 填充描述符 (指向数据包)         │
  ▼                                    │
┌──────────┐                          │
│  Desc 0  │───► 数据包0               │
├──────────┤                          │
│  Desc 1  │───► 数据包1               │
├──────────┤                          │
│  Desc 2  │───► 数据包2               │
├──────────┤                          │
│    ...   │                          │
├──────────┤                          │
│  Desc N  │                          │
└──────────┘                          │
     │                                │
     │ 2. 更新TDT (Tail Desc Pointer) │
     │───────────────────────────────►│
     │                                │
     │ 3. 硬件读取描述符并发送          │
     │◄───────────────────────────────│
     │                                │
     │ 4. 更新TDH (Head Desc Pointer) │
     │◄───────────────────────────────│
     │                                │
     ▼                                ▼

重要规则:
- TDH == TDT 表示环空
- (TDT + 1) % N == TDH 表示环满
- 硬件移动TDH，软件移动TDT
```

---

## 2. E1000寄存器

### 2.1 关键寄存器

```c
// kernel/e1000.h

// E1000寄存器偏移 (BAR0 + offset)
#define E1000_CTL      0x00000   // 设备控制
#define E1000_STATUS   0x00008   // 设备状态
#define E1000_EERD     0x00014   // EEPROM读
#define E1000_ICR      0x000C0   // 中断原因读
#define E1000_IMS      0x000D0   // 中断掩码设置
#define E1000_RCTL     0x00100   // 接收控制
#define E1000_TCTL     0x00400   // 发送控制

// 接收描述符环
#define E1000_RDBAL    0x02800   // 接收描述符基地址 (低32位)
#define E1000_RDBAH    0x02804   // 接收描述符基地址 (高32位)
#define E1000_RDLEN    0x02808   // 接收描述符环长度
#define E1000_RDH      0x02810   // 接收描述符头
#define E1000_RDT      0x02818   // 接收描述符尾

// 发送描述符环
#define E1000_TDBAL    0x03800   // 发送描述符基地址 (低32位)
#define E1000_TDBAH    0x03804   // 发送描述符基地址 (高32位)
#define E1000_TDLEN    0x03808   // 发送描述符环长度
#define E1000_TDH      0x03810   // 发送描述符头
#define E1000_TDT      0x03818   // 发送描述符尾

// MAC地址
#define E1000_RA       0x05400   // 接收地址 (MAC)
#define E1000_MTA      0x05200   // 多播表数组

// 控制寄存器位
#define E1000_CTL_RST    0x00400000   // 复位
#define E1000_CTL_SLU    0x00000040   // Set Link Up
#define E1000_CTL_ASDE   0x00000020   // Auto-Speed Detection

// 接收控制位
#define E1000_RCTL_EN    0x00000002   // 接收使能
#define E1000_RCTL_BAM   0x00008000   // 广播使能
#define E1000_RCTL_SZ_2048 0x00000000 // 2048字节缓冲区
#define E1000_RCTL_SECRC 0x04000000   // 剥离CRC

// 发送控制位
#define E1000_TCTL_EN    0x00000002   // 发送使能
#define E1000_TCTL_PSP   0x00000008   // Pad Short Packets
```

### 2.2 描述符结构

```c
// kernel/e1000.h

// 发送描述符 (Legacy模式)
struct tx_desc {
    uint64 addr;       // 数据包缓冲区物理地址
    uint16 length;     // 数据包长度
    uint8  cso;        // 校验和偏移
    uint8  cmd;        // 命令
    uint8  status;     // 状态
    uint8  css;        // 校验和开始
    uint16 special;
};

// TX命令位
#define TX_CMD_EOP    0x01   // End Of Packet
#define TX_CMD_IFCS   0x02   // Insert FCS (CRC)
#define TX_CMD_RS     0x08   // Report Status

// 接收描述符 (Legacy模式)
struct rx_desc {
    uint64 addr;       // 缓冲区物理地址
    uint16 length;     // 接收长度
    uint16 checksum;   // 校验和
    uint8  status;     // 状态
    uint8  errors;     // 错误
    uint16 special;
};

// RX状态位
#define RX_STATUS_DD  0x01   // Descriptor Done
#define RX_STATUS_EOP 0x02   // End Of Packet

#define NTXDESC 32   // 发送描述符数量
#define NRXDESC 128  // 接收描述符数量
```

---

## 3. 驱动程序实现

### 3.1 全局数据结构

```c
// kernel/e1000.c

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"
#include "defs.h"
#include "e1000.h"

// MMIO基地址
volatile uint32 *e1000_regs;

// 发送描述符环
struct tx_desc tx_ring[NTXDESC] __attribute__((aligned(16)));
char tx_bufs[NTXDESC][2048];  // 发送缓冲区
int tx_mbuf[NTXDESC];         // 保存mbuf索引

// 接收描述符环
struct rx_desc rx_ring[NRXDESC] __attribute__((aligned(16)));
char rx_bufs[NRXDESC][2048];  // 接收缓冲区

// 锁
struct spinlock e1000_lock;

// MAC地址
uint8 mac_addr[6];
```

### 3.2 初始化函数

```c
// kernel/e1000.c

int
e1000_init(void)
{
    // 1. 查找PCI设备
    // 扫描PCI总线找到E1000
    // ... (PCI枚举代码)

    // 2. 映射BAR0寄存器
    // e1000_regs = (uint32 *)map_bar0();

    // 3. 复位设备
    uint32 ctrl = e1000_regs[E1000_CTL / 4];
    e1000_regs[E1000_CTL / 4] = ctrl | E1000_CTL_RST;

    // 等待复位完成
    while (e1000_regs[E1000_CTL / 4] & E1000_CTL_RST)
        ;

    // 4. 初始化MAC地址
    // 从EEPROM读取或使用硬编码
    e1000_read_mac();

    // 5. 初始化接收描述符环
    memset(rx_ring, 0, sizeof(rx_ring));
    for (int i = 0; i < NRXDESC; i++) {
        rx_ring[i].addr = (uint64)rx_bufs[i];
        rx_ring[i].status = 0;  // 硬件可写入
    }

    // 设置接收描述符环基地址
    e1000_regs[E1000_RDBAL / 4] = (uint64)rx_ring & 0xFFFFFFFF;
    e1000_regs[E1000_RDBAH / 4] = (uint64)rx_ring >> 32;
    e1000_regs[E1000_RDLEN / 4] = sizeof(rx_ring);
    e1000_regs[E1000_RDH / 4] = 0;
    e1000_regs[E1000_RDT / 4] = NRXDESC - 1;  // 初始时所有描述符可用

    // 6. 初始化发送描述符环
    memset(tx_ring, 0, sizeof(tx_ring));
    for (int i = 0; i < NTXDESC; i++) {
        tx_ring[i].status = TX_STATUS_DD;  // 初始时所有描述符空闲
    }

    // 设置发送描述符环基地址
    e1000_regs[E1000_TDBAL / 4] = (uint64)tx_ring & 0xFFFFFFFF;
    e1000_regs[E1000_TDBAH / 4] = (uint64)tx_ring >> 32;
    e1000_regs[E1000_TDLEN / 4] = sizeof(tx_ring);
    e1000_regs[E1000_TDH / 4] = 0;
    e1000_regs[E1000_TDT / 4] = 0;

    // 7. 配置接收控制寄存器
    e1000_regs[E1000_RCTL / 4] = E1000_RCTL_EN |      // 使能接收
                                  E1000_RCTL_BAM |     // 广播使能
                                  E1000_RCTL_SZ_2048 | // 2048字节缓冲区
                                  E1000_RCTL_SECRC;    // 剥离CRC

    // 8. 配置发送控制寄存器
    e1000_regs[E1000_TCTL / 4] = E1000_TCTL_EN |      // 使能发送
                                  E1000_TCTL_PSP;      // Pad短包

    // 9. 配置中断
    e1000_regs[E1000_IMS / 4] = 0x1;  // 使能接收中断

    printf("e1000: initialized, mac=%x:%x:%x:%x:%x:%x\n",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

    return 0;
}
```

### 3.3 发送数据包

```c
// kernel/e1000.c

int
e1000_transmit(struct mbuf *m)
{
    acquire(&e1000_lock);

    // 获取当前尾指针
    uint32 tail = e1000_regs[E1000_TDT / 4];
    uint32 next_tail = (tail + 1) % NTXDESC;

    // 检查是否有空闲描述符
    if (!(tx_ring[tail].status & TX_STATUS_DD)) {
        release(&e1000_lock);
        printf("e1000_transmit: no free descriptors\n");
        return -1;
    }

    // 如果有之前的数据包，释放它
    if (tx_mbuf[tail]) {
        mbuffree(tx_mbuf[tail]);
    }

    // 填充描述符
    tx_ring[tail].addr = (uint64)m->head;
    tx_ring[tail].length = m->len;
    tx_ring[tail].cmd = TX_CMD_EOP | TX_CMD_IFCS | TX_CMD_RS;
    tx_ring[tail].status = 0;

    // 保存mbuf指针以便稍后释放
    tx_mbuf[tail] = m;

    // 更新尾指针，通知硬件
    e1000_regs[E1000_TDT / 4] = next_tail;

    release(&e1000_lock);
    return 0;
}
```

### 3.4 接收数据包

```c
// kernel/e1000.c

static void
e1000_recv(void)
{
    // 获取当前头指针
    uint32 tail = (e1000_regs[E1000_RDT / 4] + 1) % NRXDESC;

    // 处理所有已完成的描述符
    while (rx_ring[tail].status & RX_STATUS_DD) {
        // 检查是否有错误
        if (rx_ring[tail].errors) {
            printf("e1000_recv: packet error %x\n", rx_ring[tail].errors);
        } else {
            // 将数据包传递给网络栈
            struct mbuf *m = mbufalloc(0);
            if (m) {
                uint16 len = rx_ring[tail].length;
                if (len > MBUF_SIZE)
                    len = MBUF_SIZE;

                memmove(m->head, rx_bufs[tail], len);
                m->len = len;

                // 传递给上层 (IP层)
                net_rx(m);
            }
        }

        // 重置描述符状态
        rx_ring[tail].status = 0;

        // 更新尾指针，释放描述符给硬件
        e1000_regs[E1000_RDT / 4] = tail;

        // 下一个描述符
        tail = (tail + 1) % NRXDESC;
    }
}
```

### 3.5 中断处理

```c
// kernel/e1000.c

void
e1000_intr(void)
{
    // 读取中断原因寄存器 (自动清除中断)
    uint32 icr = e1000_regs[E1000_ICR / 4];

    if (icr & 0x80) {
        // 接收中断
        e1000_recv();
    }

    if (icr & 0x01) {
        // 发送中断
        // 可以在这里处理发送完成通知
    }
}
```

---

## 4. 网络栈接口

### 4.1 mbuf数据结构

```c
// kernel/mbuf.h

#ifndef MBUF_SIZE
#define MBUF_SIZE 2048
#endif

// mbuf头部大小
#define MBUF_HEADROOM 128

struct mbuf {
    struct mbuf *next;      // 链表指针
    char *head;             // 数据起始
    uint len;               // 数据长度
    char buf[MBUF_SIZE];    // 数据缓冲区
};

// 分配mbuf
struct mbuf *mbufalloc(uint headroom);

// 释放mbuf
void mbuffree(struct mbuf *m);

// 在头部预留空间
char *mbufpush(struct mbuf *m, uint len);

// 在尾部添加空间
char *mbufput(struct mbuf *m, uint len);

// 从头部移除数据
char *mbufpull(struct mbuf *m, uint len);

// 从尾部移除数据
void mbuftrim(struct mbuf *m, uint len);
```

### 4.2 mbuf实现

```c
// kernel/mbuf.c

struct mbuf *
mbufalloc(uint headroom)
{
    struct mbuf *m;

    if (headroom > MBUF_SIZE)
        return 0;

    m = kalloc();
    if (!m)
        return 0;

    m->next = 0;
    m->head = m->buf + headroom;
    m->len = 0;

    return m;
}

void
mbuffree(struct mbuf *m)
{
    kfree((void *)m);
}

char *
mbufpush(struct mbuf *m, uint len)
{
    m->head -= len;
    m->len += len;
    return m->head;
}

char *
mbufput(struct mbuf *m, uint len)
{
    char *tail = m->head + m->len;
    m->len += len;
    return tail;
}

char *
mbufpull(struct mbuf *m, uint len)
{
    char *head = m->head;
    m->head += len;
    m->len -= len;
    return head;
}
```

---

## 5. Socket API实现

### 5.1 Socket系统调用

```c
// kernel/sysnet.c

// 创建socket
int
sys_socket(void)
{
    int domain, type, protocol;

    if (argint(0, &domain) < 0 ||
        argint(1, &type) < 0 ||
        argint(2, &protocol) < 0)
        return -1;

    // 分配socket结构
    struct sock *so = sockalloc(domain, type, protocol);
    if (!so)
        return -1;

    // 分配文件描述符
    int fd = fdalloc(so);
    if (fd < 0) {
        sockclose(so);
        return -1;
    }

    return fd;
}

// 发送数据
int
sys_send(void)
{
    int fd, n;
    uint64 addr;
    struct file *f;
    struct mbuf *m;

    if (argint(0, &fd) < 0 ||
        argint(2, &n) < 0 ||
        argaddr(1, &addr) < 0)
        return -1;

    f = myproc()->ofile[fd];
    if (!f || f->type != FD_SOCK)
        return -1;

    // 分配mbuf并复制数据
    m = mbufalloc(0);
    if (!m)
        return -1;

    if (copyin(myproc()->pagetable, mbufput(m, n), addr, n) < 0) {
        mbuffree(m);
        return -1;
    }

    return socksend(f->sock, m);
}

// 接收数据
int
sys_recv(void)
{
    int fd, n;
    uint64 addr;
    struct file *f;
    struct mbuf *m;

    if (argint(0, &fd) < 0 ||
        argint(2, &n) < 0 ||
        argaddr(1, &addr) < 0)
        return -1;

    f = myproc()->ofile[fd];
    if (!f || f->type != FD_SOCK)
        return -1;

    m = sockrecv(f->sock);
    if (!m)
        return -1;

    // 复制到用户空间
    int len = m->len;
    if (len > n)
        len = n;

    if (copyout(myproc()->pagetable, addr, m->head, len) < 0) {
        mbuffree(m);
        return -1;
    }

    mbuffree(m);
    return len;
}
```

---

## 6. 调试技巧

### 6.1 常见问题

```
问题1: 无法发送数据包
原因: 描述符未正确设置或尾指针未更新
解决: 检查描述符状态位和TDT寄存器

问题2: 无法接收数据包
原因: 接收描述符未正确初始化或RDT设置错误
解决: 确保所有接收描述符可用，RDT = N-1

问题3: 中断不触发
原因: 中断掩码未设置或PCI中断配置错误
解决: 检查IMS寄存器和PCI配置

问题4: 数据包损坏
原因: 缓冲区未对齐或DMA地址错误
解决: 确保描述符环16字节对齐
```

### 6.2 调试命令

```c
// 打印寄存器状态
void e1000_debug(void) {
    printf("E1000 Status:\n");
    printf("  STATUS: %08x\n", e1000_regs[E1000_STATUS / 4]);
    printf("  RCTL:   %08x\n", e1000_regs[E1000_RCTL / 4]);
    printf("  TCTL:   %08x\n", e1000_regs[E1000_TCTL / 4]);
    printf("  RDH:    %d, RDT: %d\n",
           e1000_regs[E1000_RDH / 4],
           e1000_regs[E1000_RDT / 4]);
    printf("  TDH:    %d, TDT: %d\n",
           e1000_regs[E1000_TDH / 4],
           e1000_regs[E1000_TDT / 4]);
}
```

---

## 7. 测试程序

### 7.1 简单ping测试

```c
// user/ping.c
#include "kernel/types.h"
#include "user/user.h"

// ICMP Echo Request
struct icmp_packet {
    uint8 type;
    uint8 code;
    uint16 checksum;
    uint16 id;
    uint16 seq;
    char data[56];
};

uint16 checksum(void *data, int len) {
    uint32 sum = 0;
    uint16 *p = data;

    while (len > 1) {
        sum += *p++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(uint8 *)p;
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return ~sum;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: ping <ip>\n");
        exit(1);
    }

    // 创建原始socket
    int sock = socket(2, 3, 1);  // AF_INET, SOCK_RAW, ICMP
    if (sock < 0) {
        printf("socket failed\n");
        exit(1);
    }

    struct icmp_packet pkt;
    pkt.type = 8;  // Echo Request
    pkt.code = 0;
    pkt.id = getpid();
    pkt.seq = 1;
    memset(pkt.data, 0xAB, sizeof(pkt.data));
    pkt.checksum = 0;
    pkt.checksum = checksum(&pkt, sizeof(pkt));

    // 发送
    send(sock, &pkt, sizeof(pkt), 0);

    // 接收响应
    char buf[128];
    int n = recv(sock, buf, sizeof(buf), 0);
    if (n > 0) {
        struct icmp_packet *resp = (void *)(buf + 20);  // 跳过IP头
        if (resp->type == 0) {  // Echo Reply
            printf("Reply from %s: bytes=%d\n", argv[1], n);
        }
    }

    close(sock);
    exit(0);
}
```

---

## 8. 总结

### 8.1 核心概念

| 概念 | 说明 |
|------|------|
| **DMA** | 直接内存访问，无需CPU参与数据传输 |
| **描述符环** | 硬件和软件共享的环形缓冲区 |
| **MMIO** | 内存映射I/O，通过内存访问设备寄存器 |
| **mbuf** | 网络数据包缓冲区管理 |
| **中断** | 设备通知CPU事件发生的机制 |

### 8.2 关键要点

1. **描述符管理**: 正确维护头尾指针
2. **内存对齐**: 描述符和缓冲区需要适当对齐
3. **并发控制**: 使用锁保护共享资源
4. **错误处理**: 检查状态位和错误标志

---

**难度**: ⭐⭐⭐⭐
**预计时间**: 12-15小时
**依赖**: Lab 4 (中断处理)


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
