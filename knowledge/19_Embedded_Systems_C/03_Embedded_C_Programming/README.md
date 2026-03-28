# 嵌入式C编程核心技术

> **目标**: 梳理嵌入式C语言特有的编程技术和最佳实践
> **最后更新**: 2026-03-29

---

## 核心特性概览

```
嵌入式C vs 标准C
├── 内存访问
│   ├── volatile关键字
│   ├── 内存屏障
│   └── 寄存器直接访问
├── 位操作
│   ├── 位域 vs 位移
│   ├── 原子操作
│   └── 位带（Bit-Banding）
├── 存储器
│   ├── Flash中的常量
│   ├── 栈/堆管理
│   └── 内存对齐
├── 中断
│   ├── 中断服务程序
│   ├── 可重入性
│   └── 临界区保护
└── 优化
    ├── 内联汇编
    ├── 编译器属性
    └── 链接器脚本
```

---

## volatile关键字深度解析

### 为什么需要volatile

```c
/* 问题代码 - 编译器优化导致的问题 */
void wait_for_button(void) {
    uint32_t *button_reg = (uint32_t*)0x40001000;
    while (*button_reg == 0) {  /* 编译器可能优化为只读一次 */
        /* 等待 */
    }
}

/* 正确代码 */
void wait_for_button(void) {
    volatile uint32_t *button_reg = (volatile uint32_t*)0x40001000;
    while (*button_reg == 0) {  /* 每次循环都从硬件读取 */
        /* 等待 */
    }
}
```

### volatile使用场景

| 场景 | 示例 | 说明 |
|------|------|------|
| **硬件寄存器** | `volatile uint32_t *reg` | 内存映射I/O |
| **多线程共享** | `volatile int flag` | 中断/RTOS通信 |
| **信号处理器** | `volatile sig_atomic_t` | Unix信号处理 |
| **延时循环** | `volatile int i` | 防止优化掉空循环 |

---

## 位操作技术

### 位域 vs 位移

```c
/* 方法1: 位域 - 可读性好，但可能不可移植 */
typedef struct {
    uint32_t enabled    : 1;
    uint32_t mode       : 3;
    uint32_t prescaler  : 4;
    uint32_t reserved   : 24;
} ControlReg;

/* 方法2: 位移 - 可移植，推荐用于硬件寄存器 */
#define CTRL_ENABLE     (1U << 0)
#define CTRL_MODE_POS   1
#define CTRL_MODE_MASK  (0x7U << CTRL_MODE_POS)
#define CTRL_PRE_POS    4
#define CTRL_PRE_MASK   (0xFU << CTRL_PRE_POS)

/* 操作 */
reg |= CTRL_ENABLE;                          /* 置位 */
reg &= ~CTRL_ENABLE;                         /* 清零 */
reg = (reg & ~CTRL_MODE_MASK) | (mode << CTRL_MODE_POS); /* 修改字段 */
```

### 原子位操作

```c
/* ARM Cortex-M bit-banding - 原子性 */
#define BITBAND_ADDR(addr, bit) (((addr) & 0xF0000000) + 0x2000000 +
                                 (((addr) & 0xFFFFF) << 5) + ((bit) << 2))
#define BITBAND_PTR(addr, bit) ((volatile uint32_t*)BITBAND_ADDR(addr, bit))

/* 原子设置/清除位 */
*BITBAND_PTR(&GPIOA->ODR, 5) = 1;  /* 原子设置PA5 */
*BITBAND_PTR(&GPIOA->ODR, 5) = 0;  /* 原子清除PA5 */

/* C11标准原子操作 */
#include <stdatomic.h>
_Atomic uint32_t shared_counter;
atomic_fetch_add(&shared_counter, 1);
```

---

## 内存屏障（Memory Barriers）

### 编译器屏障

```c
/* 防止编译器重排序 */
#define COMPILER_BARRIER() __asm__ volatile ("" ::: "memory")

void critical_function(void) {
    peripheral->control = 1;
    COMPILER_BARRIER();  /* 确保control写入后才读取status */
    while (peripheral->status == 0) {
        /* 等待 */
    }
}
```

### 硬件内存屏障

```c
/* ARM Cortex-M DMB/DSB/ISB */
__DSB();  /* 数据同步屏障 - 等待内存访问完成 */
__DMB();  /* 数据内存屏障 - 确保访问顺序 */
__ISB();  /* 指令同步屏障 - 清空流水线 */

/* 应用场景 */
void mmu_reconfigure(void) {
    /* 修改页表 */
    page_table[0] = new_entry;
    __DMB();           /* 确保页表写入完成 */
    MMU->TLB_INV = 1;  /* 使能TLB */
    __ISB();           /* 刷新流水线 */
}
```

---

## 文档列表

| 文档 | 内容 | 状态 |
|------|------|------|
| [01_Volatile_Keyword.md](./01_Volatile_Keyword.md) | volatile深度解析 | ⏳ |
| [02_Bit_Manipulation_Techniques.md](./02_Bit_Manipulation_Techniques.md) | 位操作技术 | ⏳ |
| [03_Memory_Barriers.md](./03_Memory_Barriers.md) | 内存屏障 | ⏳ |
| [04_Register_Access_Patterns.md](./04_Register_Access_Patterns.md) | 寄存器访问模式 | ⏳ |
| [05_Bitfields_vs_Shifts.md](./05_Bitfields_vs_Shifts.md) | 位域vs位移对比 | ⏳ |
| [06_Const_Data_in_Flash.md](./06_Const_Data_in_Flash.md) | Flash常量存储 | ⏳ |
| [07_Stack_Heap_in_Embedded.md](./07_Stack_Heap_in_Embedded.md) | 堆栈管理 | ⏳ |
| [08_Function_Pointers_for_ISRs.md](./08_Function_Pointers_for_ISRs.md) | 函数指针应用 | ⏳ |
