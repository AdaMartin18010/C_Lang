# 链接器脚本编写指南 (Linker Scripts Guide)

> **层级定位**: 02 Formal Semantics and Physics / 08 Linking_Loading_Topology
> **主题**: GNU ld / LLVM lld 链接器脚本 (.ld) 的完整编写指南
> **难度级别**: L4 分析
> **预估学习时间**: 6-10 小时
> **对标课程**: Stanford CS107E / 嵌入式系统开发

## 概述

链接器脚本（Linker Script）是控制链接器行为的文本文件，定义了输出文件的内存布局、段（section）的放置地址、符号地址分配等。
在嵌入式系统、操作系统内核、裸机编程中，链接器脚本是必不可少的基础设施。
本指南系统阐述 GNU ld 链接器脚本的语法和实战技巧。

---

## 核心概念

### 链接器脚本的作用

```
输入: 多个 .o 目标文件 + 库文件 + 链接器脚本(.ld)
                 │
                 ▼ 链接器 (ld)
输出: 可执行文件 / 镜像文件
      • 定义各段（.text, .data, .bss）的内存地址
      • 分配全局符号地址
      • 处理重定位
      • 生成程序入口点
```

### 为什么需要链接器脚本？

| 场景 | 默认链接 vs 自定义链接器脚本 |
|:-----|:---------------------------|
| **Linux 用户程序** | 默认即可，glibc 已处理 |
| **裸机嵌入式** | **必须自定义**——没有操作系统管理内存 |
| **操作系统内核** | **必须自定义**——内核需要自己管理物理内存 |
| **Bootloader** | **必须自定义**——在内存布局确定前运行 |
| **ROM/Flash 程序** | **必须自定义**——代码在 Flash，数据在 RAM |

---

## 基础语法

### 最小链接器脚本

```ld
/* 最小链接器脚本 */
ENTRY(_start)          /* 程序入口点 */

SECTIONS {
    . = 0x80000000;    /* 当前位置计数器 = 起始地址 */

    .text : {
        *(.text*)      /* 所有输入文件的 .text 段 */
    }

    .data : {
        *(.data*)      /* 所有输入文件的 .data 段 */
    }

    .bss : {
        *(.bss*)       /* 所有输入文件的 .bss 段 */
    }
}
```

### 位置计数器 `.`

```ld
SECTIONS {
    . = 0x10000;        /* 设置当前地址为 0x10000 */
    .text : { ... }    /* .text 从 0x10000 开始 */
    . = 0x80000;        /* 跳转到 0x80000 */
    .data : { ... }    /* .data 从 0x80000 开始 */
}
```

---

## 完整嵌入式示例

### RISC-V 裸机启动链接器脚本

```ld
/* riscv64-virt.ld */
OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
    /* QEMU virt 机器的内存布局 */
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 128M
}

SECTIONS {
    /* 代码段: 从 RAM 起始地址开始 */
    . = ORIGIN(RAM);
    __text_start = .;

    .text : {
        *(.text.boot)      /* 启动代码在最前面 */
        *(.text*)
        *(.rodata*)
        . = ALIGN(8);
    } > RAM
    __text_end = .;

    /* 数据段 */
    __data_start = .;
    .data : {
        *(.data*)
        . = ALIGN(8);
    } > RAM
    __data_end = .;

    /* BSS 段 */
    __bss_start = .;
    .bss : {
        *(.bss*)
        *(COMMON)
        . = ALIGN(8);
    } > RAM
    __bss_end = .;

    /* 栈顶初始化 */
    __stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
```

### 对应的启动代码 (startup.s)

```asm
.section .text.boot
.global _start

_start:
    /* 初始化栈指针 */
    la sp, __stack_top

    /* 清零 BSS */
    la t0, __bss_start
    la t1, __bss_end
1:
    bge t0, t1, 2f
    sw zero, 0(t0)
    addi t0, t0, 8
    j 1b
2:
    /* 跳转到 C main */
    call main
    /* main 返回后停机 */
3:  j 3b
```

### 对应的 C 代码

```c
// 链接器脚本中定义的符号，在 C 中声明为外部
extern char __bss_start[], __bss_end[];
extern char __stack_top[];

void main(void) {
    volatile unsigned int *uart = (unsigned int *)0x10000000;
    const char *msg = "Hello, bare metal!\n";

    while (*msg) {
        *uart = *msg++;
    }

    while (1);
}
```

---

## 高级技巧

### 1. 段对齐与填充

```ld
SECTIONS {
    .text : {
        . = ALIGN(4096);        /* 页对齐 */
        *(.text*)
        . = ALIGN(8);           /* 8 字节对齐 */
    }
}
```

### 2. 将数据放入指定内存区域

```ld
MEMORY {
    FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 512K
    SRAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 128K
}

SECTIONS {
    /* 代码在 Flash */
    .text : { *(.text*) } > FLASH

    /* 初始化数据也在 Flash，但运行时复制到 SRAM */
    .data : {
        __data_load = LOADADDR(.data);
        __data_start = .;
        *(.data*)
        __data_end = .;
    } > SRAM AT > FLASH
}
```

### 3. 自定义符号暴露给 C

```ld
SECTIONS {
    . = 0x1000;
    __my_device_reg = .;
    . += 0x100;    /* 保留 256 字节给设备寄存器 */
}
```

```c
// C 代码中使用
extern char __my_device_reg[];
#define DEVICE_REG (*(volatile uint32_t *)__my_device_reg)
```

---

## 工具链命令

```bash
# 使用自定义链接器脚本编译
riscv64-unknown-elf-gcc \
    -nostdlib -nostartfiles \
    -T riscv64-virt.ld \
    -o firmware.elf \
    startup.s main.c

# 生成内存映射文件
riscv64-unknown-elf-ld -Map firmware.map -T riscv64-virt.ld *.o

# 转换为二进制烧录镜像
riscv64-unknown-elf-objcopy -O binary firmware.elf firmware.bin
```

---

## 常见错误与调试

| 错误信息 | 原因 | 解决 |
|:---------|:-----|:-----|
| `undefined reference to _start` | 入口点未定义或找不到 | 检查 `ENTRY()` 和 `_start` 符号 |
| `section .data overlaps` | 段地址冲突 | 检查位置计数器 `.` 的递增 |
| `region RAM overflowed` | 内存不足 | 增加 `LENGTH` 或优化代码 |
| `relocation truncated` | 跳转目标太远 | 使用长跳转指令或调整段布局 |

---

## 权威参考

- **GNU ld Manual**: [Linker Scripts](https://sourceware.org/binutils/docs/ld/Scripts.html)
- **Stanford CS107E**: Linker scripts for bare-metal RISC-V
- **RISC-V ELF psABI**: RISC-V 特定的 ELF 和链接约定
- **"Embedded Systems Architecture"** (Tammy Noergaard): 嵌入式链接与加载

---

*最后更新: 2026-05-10*
