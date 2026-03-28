# MCU架构知识梳理（C语言视角）

> **目标**: 从C语言编程角度梳理主流MCU架构特点
> **最后更新**: 2026-03-29

---

## 架构分类概览

```
嵌入式MCU架构
├── CISC架构
│   ├── x86 (工业控制、边缘计算)
│   └── 8051 (传统嵌入式，仍在使用)
├── RISC架构
│   ├── ARM Cortex-M系列 (主流)
│   │   ├── Cortex-M0/M0+ (超低功耗)
│   │   ├── Cortex-M3 (均衡性能)
│   │   ├── Cortex-M4/M7 (DSP+FPU)
│   │   └── Cortex-M33/M55 (安全+AI)
│   ├── RISC-V (新兴开放架构)
│   │   ├── RV32IMAC (基础)
│   │   ├── RV32GC (通用)
│   │   └── RV64 (64位高性能)
│   ├── MIPS (逐步淡出)
│   └── AVR (Atmel，Microchip收购)
└── 专有架构
    ├── Xtensa (ESP8266/32)
    ├── PIC (Microchip)
    └── MSP430 (TI，超低功耗)
```

---

## 架构对比矩阵

| 架构 | 指令集 | 功耗 | 性能 | 生态 | C语言注意点 |
|------|--------|------|------|------|-------------|
| **ARM Cortex-M** | Thumb-2 | 低-中 | 中-高 | ⭐⭐⭐⭐⭐ | 对齐要求、bit-banding |
| **RISC-V** | RV32/64 | 低-中 | 中-高 | ⭐⭐⭐⭐ | 编译器优化、标准扩展 |
| **ESP32 (Xtensa)** | 专有 | 中 | 高 | ⭐⭐⭐⭐ | 特定ABI、窗口寄存器 |
| **8051** | CISC | 极低 | 低 | ⭐⭐⭐ | bank切换、xdata |
| **AVR** | RISC | 低 | 低-中 | ⭐⭐⭐⭐ | Harvard架构、PROGMEM |

---

## C语言编程差异

### 内存模型

```c
/* ARM Cortex-M - 统一内存映射 */
#define GPIOA_BASE  0x40020000
#define SRAM_BASE   0x20000000
#define FLASH_BASE  0x08000000

/* 8051 - 分离内存空间 */
__data uint8_t var1;     /* 内部RAM低128字节 */
__idata uint8_t var2;    /* 内部RAM高128字节 */
__xdata uint8_t var3;    /* 外部RAM */
__code uint8_t const1;   /* 代码空间 */

/* AVR - Harvard架构，程序内存访问 */
const uint8_t data[] PROGMEM = {1, 2, 3};
uint8_t val = pgm_read_byte(&data[i]);
```

### 中断处理

```c
/* ARM Cortex-M - 统一向量表 */
void TIM2_IRQHandler(void) __attribute__((interrupt));
void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF;  /* 清除标志 */
    /* 处理 */
}

/* 8051 - 固定向量地址 */
void Timer0_ISR(void) __interrupt(1) __using(1) {
    /* 自动切换寄存器bank */
}

/* RISC-V - 向量/非向量模式 */
__attribute__((interrupt)) void timer_isr(void) {
    /* 保存上下文由硬件/软件决定 */
}
```

---

## 子目录规划

| 文档 | 内容 |
|------|------|
| [01_ARM_Cortex_M_Series.md](./01_ARM_Cortex_M_Series.md) | ARM Cortex-M全系列详解 |
| [02_RISC_V_Embedded.md](./02_RISC_V_Embedded.md) | RISC-V嵌入式应用 |
| [03_8051_Legacy.md](./03_8051_Legacy.md) | 8051架构及现代变体 |
| [04_AVR_Architecture.md](./04_AVR_Architecture.md) | AVR架构特点 |
| [05_Espressif_SoC_Architecture.md](./05_Espressif_SoC_Architecture.md) | ESP32架构详解 |
