# 嵌入式 C 编程实战指南

> **难度**: L4-L5
> **目标平台**: ARM Cortex-M, 裸机/RTOS
> **核心目标**: 掌握寄存器级编程、启动流程、中断和实时约束

---

## 1. 内存映射 I/O

### 1.1 寄存器访问基础

```c
// 直接通过地址访问硬件寄存器
#define GPIOA_BASE      0x40020000
#define GPIOA_MODER     (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_ODR       (*(volatile uint32_t *)(GPIOA_BASE + 0x14))

// 设置 PA5 为输出
GPIOA_MODER &= ~(3U << (5 * 2));   // 清除模式位
GPIOA_MODER |=  (1U << (5 * 2));   // 设置为输出模式

// 点亮 LED（PA5）
GPIOA_ODR |= (1U << 5);

// 熄灭 LED
GPIOA_ODR &= ~(1U << 5);
```

### 1.2 为什么必须用 `volatile`

```c
// ❌ 错误：编译器可能优化掉重复读取
uint32_t status = STATUS_REG;
while (status & BUSY_BIT) {   // 编译器假设 status 不变 → 无限循环！
    // ...
}

// ✅ 正确：volatile 禁止优化
volatile uint32_t *status_reg = (volatile uint32_t *)0x40021000;
while (*status_reg & BUSY_BIT) {
    // 编译器每次循环都重新读取
}
```

### 1.3 结构体映射寄存器组

```c
// 将外设寄存器映射为结构体（更清晰）
typedef struct {
    volatile uint32_t MODER;    // 0x00: 模式寄存器
    volatile uint32_t OTYPER;   // 0x04: 输出类型
    volatile uint32_t OSPEEDR;  // 0x08: 输出速度
    volatile uint32_t PUPDR;    // 0x0C: 上拉/下拉
    volatile uint32_t IDR;      // 0x10: 输入数据
    volatile uint32_t ODR;      // 0x14: 输出数据
    volatile uint32_t BSRR;     // 0x18: 位设置/清除
    volatile uint32_t LCKR;     // 0x1C: 配置锁定
    volatile uint32_t AFR[2];   // 0x20: 复用功能
} GPIO_TypeDef;

#define GPIOA   ((GPIO_TypeDef *)0x40020000)
#define GPIOB   ((GPIO_TypeDef *)0x40020400)

// 使用
GPIOA->MODER |= (1U << 10);     // PA5 输出
GPIOA->BSRR = (1U << 5);        // 置位 PA5
GPIOA->BSRR = (1U << (5 + 16)); // 清除 PA5（BSRR 高16位为清除）
```

---

## 2. 启动代码 (Startup Code)

### 2.1 复位后的执行流程

```
上电/复位
    ↓
从复位向量表读取 SP 和 PC
    ↓
启动文件 (startup.s / startup.c)
    ├── 初始化 SP（栈顶）
    ├── 复制 .data 从 Flash 到 RAM
    ├── 清零 .bss
    ├── 调用 SystemInit()
    └── 调用 main()
```

### 2.2 最小启动文件（C 实现）

```c
// startup.c — 纯 C 启动代码

extern uint32_t _estack;        // 栈顶（链接器脚本定义）
extern uint32_t _sidata;        // .data 在 Flash 中的起始地址
extern uint32_t _sdata;         // .data 在 RAM 中的起始地址
extern uint32_t _edata;         // .data 在 RAM 中的结束地址
extern uint32_t _sbss;          // .bss 起始
extern uint32_t _ebss;          // .bss 结束

// 主函数（用户代码）
int main(void);

// 默认中断处理器
void Default_Handler(void) {
    while(1);
}

// 弱定义的异常处理器（可被用户覆盖）
void Reset_Handler(void);
void NMI_Handler(void)        __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void)  __attribute__((weak, alias("Default_Handler")));
// ... 其他异常

// 向量表
__attribute__((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {
    (void (*)(void))&_estack,    // 栈顶
    Reset_Handler,               // 复位
    NMI_Handler,
    HardFault_Handler,
    // ...
};

void Reset_Handler(void) {
    // 1. 复制初始化数据从 Flash 到 RAM
    uint32_t *src = &_sidata;
    uint32_t *dst = &_sdata;
    while (dst < &_edata) {
        *dst++ = *src++;
    }

    // 2. 清零 .bss
    dst = &_sbss;
    while (dst < &_ebss) {
        *dst++ = 0;
    }

    // 3. 调用系统初始化（时钟配置等）
    // SystemInit();

    // 4. 调用 main
    main();

    // 5. main 不应该返回，但如果返回了...
    while(1);
}
```

---

## 3. 链接器脚本 (Linker Script)

```ld
/* STM32F407 链接器脚本 */
MEMORY
{
    /* Flash: 1MB */
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 1024K

    /* RAM: 128KB SRAM1 + 64KB SRAM2 + 4KB CCM */
    RAM (rwx)   : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS
{
    /* .text 放在 Flash */
    .text :
    {
        KEEP(*(.isr_vector))    /* 向量表必须在最开始 */
        *(.text*)
        *(.rodata*)
    } > FLASH

    /* 初始化数据的 Flash 地址（用于启动时复制） */
    _sidata = LOADADDR(.data);

    /* .data 放在 RAM，初始化内容在 Flash */
    .data :
    {
        _sdata = .;
        *(.data*)
        _edata = .;
    } > RAM AT > FLASH

    /* .bss 放在 RAM，不占用 Flash */
    .bss :
    {
        _sbss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > RAM

    /* 栈顶 = RAM 末尾 */
    _estack = ORIGIN(RAM) + LENGTH(RAM);
}
```

---

## 4. 中断处理 (ISR)

### 4.1 基本中断服务程序

```c
// 启用 NVIC 中断
void timer_irq_enable(void) {
    NVIC->ISER[0] = (1U << TIM2_IRQn);   // 使能 TIM2 中断
}

// ISR — 必须简短、快速
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {         // 检查更新中断标志
        TIM2->SR &= ~TIM_SR_UIF;         // 清除标志（必须软件清除）

        // 简短工作：设置标志、计数器递增
        g_tick_count++;
        g_schedule_flag = 1;
    }
}

// main 循环中处理
int main(void) {
    timer_init(1000);  // 1ms 定时器
    timer_irq_enable();
    __enable_irq();

    while (1) {
        if (g_schedule_flag) {
            g_schedule_flag = 0;
            // 执行调度工作（不在 ISR 中做重活）
        }
        __WFI();  // 等待中断（低功耗）
    }
}
```

### 4.2 临界区保护

```c
// 进入临界区：禁用中断
static inline uint32_t critical_enter(void) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

// 退出临界区：恢复中断状态
static inline void critical_exit(uint32_t primask) {
    __set_PRIMASK(primask);
}

// 使用
void update_shared_state(void) {
    uint32_t state = critical_enter();
    g_shared_counter++;
    critical_exit(state);
}
```

---

## 5. 位带操作 (Bit-Banding)

ARM Cortex-M3/M4 支持位带：将每个位映射到一个 32 位字，实现原子位操作。

```c
// SRAM 位带区域
#define BITBAND_SRAM_BASE   0x20000000
#define BITBAND_SRAM_REF    0x22000000

// 外设位带区域
#define BITBAND_PERI_BASE   0x40000000
#define BITBAND_PERI_REF    0x42000000

// 位带别名地址计算
#define BITBAND_SRAM_ADDR(addr, bitnum) \
    ((volatile uint32_t *)((BITBAND_SRAM_REF) + (((addr) - BITBAND_SRAM_BASE) * 32) + ((bitnum) * 4)))

#define BITBAND_PERI_ADDR(addr, bitnum) \
    ((volatile uint32_t *)((BITBAND_PERI_REF) + (((addr) - BITBAND_PERI_BASE) * 32) + ((bitnum) * 4)))

// 使用：原子设置/清除 GPIO 位
#define PA5_BIT    BITBAND_PERI_ADDR((&GPIOA->ODR), 5)

*PA5_BIT = 1;   // 原子置位（无需读-改-写）
*PA5_BIT = 0;   // 原子清除
```

---

## 6. 检查清单

- [ ] 所有外设寄存器访问使用 `volatile`
- [ ] ISR 简短快速，重活放到主循环
- [ ] 临界区保护共享数据
- [ ] 链接器脚本正确定义栈顶和向量表位置
- [ ] 启动代码初始化 .data 和 .bss
- [ ] 中断向量表放在 Flash 起始地址

---

## 7. 配套代码示例

本节配套可编译代码位于 `examples/embedded/` 目录：

| 示例文件 | 演示内容 | 编译命令 |
|---------|---------|---------|
| `memory_mapped_io.c` | GPIO 寄存器内存映射访问，`volatile` 指针操作 | `gcc -O2 -std=c11` |
| `bitfields_register.c` | `union` + `struct` 位域定义硬件寄存器 | `gcc -O2 -std=c11` |
| `interrupt_simulation.c` | 中断向量表注册、使能、触发与分发 | `gcc -O2 -std=c11` |

### 关键性能数据

| 操作 | 典型延迟 | 优化手段 |
|------|---------|---------|
| 寄存器读写 | 1-2 时钟周期 | 使用 `volatile` 防止优化 |
| 位带操作 | 1 时钟周期（原子） | ARM Cortex-M Bit-Banding |
| 中断延迟 | 12-16 时钟周期 | 保持 ISR 简短 |
| 上下文切换 | 50-200 时钟周期 | 减少寄存器保存范围 |

---

## 8. 常见错误模式

| 错误模式 | 后果 | 修复方案 |
|---------|------|---------|
| **缺少 `volatile`** | 编译器优化掉寄存器读取/写入 | 所有 MMIO 访问标记 `volatile` |
| **ISR 过长** | 高优先级中断被延迟，实时性丧失 | ISR 只设标志，主循环处理 |
| **竞态条件** | 中断与主循环同时访问共享数据 | 进入临界区：`__disable_irq()` |
| **位域布局假设** | 不同编译器生成不同布局 | 使用掩码+移位，或验证编译器文档 |
| **栈溢出** | 静默破坏全局变量/堆 | 静态分析栈深度，使用 MPU 保护 |
| **未初始化 .bss/.data** | 全局变量包含随机值 | 启动代码必须清零/复制初始化段 |

---

> **最后更新**: 2026-05-11
> **参考**: ARM Cortex-M4 TRM, STM32 Reference Manual, Joseph Yiu's Definitive Guide
