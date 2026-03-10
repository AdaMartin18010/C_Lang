# Zig 嵌入式系统开发：裸机编程与硬件控制

> **定位**: Zig 应用 / 嵌入式系统 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 裸机、MCU、RTOS

---

## 📋 目录

- [Zig 嵌入式系统开发：裸机编程与硬件控制](#zig-嵌入式系统开发裸机编程与硬件控制)
  - [📋 目录](#-目录)
  - [一、为什么选择 Zig 做嵌入式](#一为什么选择-zig-做嵌入式)
  - [二、裸机启动代码](#二裸机启动代码)
    - [2.1 ARM Cortex-M 启动](#21-arm-cortex-m-启动)
    - [2.2 RISC-V 启动](#22-risc-v-启动)
  - [三、内存布局与链接脚本](#三内存布局与链接脚本)
    - [3.1 链接脚本 (linker.ld)](#31-链接脚本-linkerld)
    - [3.2 Zig 构建配置](#32-zig-构建配置)
  - [四、外设驱动开发](#四外设驱动开发)
    - [4.1 MMIO 寄存器映射](#41-mmio-寄存器映射)
    - [4.2 GPIO 驱动](#42-gpio-驱动)
    - [4.3 UART 驱动](#43-uart-驱动)
  - [五、中断与异常处理](#五中断与异常处理)
    - [5.1 NVIC 配置 (Cortex-M)](#51-nvic-配置-cortex-m)
    - [5.2 中断处理函数](#52-中断处理函数)
  - [六、与 C 混合开发](#六与-c-混合开发)
    - [6.1 调用 C 库](#61-调用-c-库)
    - [6.2 导出 Zig 函数给 C](#62-导出-zig-函数给-c)
  - [七、实战：STM32 点灯](#七实战stm32-点灯)
    - [7.1 完整项目](#71-完整项目)
  - [八、实战：Raspberry Pi Pico 驱动](#八实战raspberry-pi-pico-驱动)
  - [九、RTOS 集成](#九rtos-集成)
    - [9.1 FreeRTOS 包装](#91-freertos-包装)
  - [十、调试技巧](#十调试技巧)
    - [10.1 ITM/SWO 输出](#101-itmswo-输出)
    - [10.2 断言与检查](#102-断言与检查)

---

## 一、为什么选择 Zig 做嵌入式

```
┌─────────────────────────────────────────────────────────────────────┐
│                 Zig vs C/C++ 嵌入式开发对比                          │
├─────────────────────────────────────────────────────────────────────┤
│                                                                      │
│  特性                    C              C++           Zig            │
│  ─────────────────────────────────────────────────────────────      │
│  编译时计算              宏(危险)       模板(复杂)    comptime(安全)   │
│  错误处理                返回码         异常          错误联合         │
│  内存安全                手动           RAII          显式+检查        │
│  标准库                  大             巨大          可选+精简        │
│  交叉编译                困难           困难          内置支持         │
│  代码体积                小             大            小/可定制        │
│  运行时                  无             部分          无/可选          │
│                                                                      │
│  Zig 优势:                                                           │
│  1. 可选标准库 - 可以只编译需要的东西                                  │
│  2. 编译时优化 - 在编译期计算常量、展开循环                             │
│  3. 更好的 C 互操作 - 直接导入 C 头文件                                │
│  4. 显式控制 - 没有隐藏的构造函数/析构函数                              │
│  5. 现代工具链 - 内置交叉编译，无需配置工具链                           │
│                                                                      │
└─────────────────────────────────────────────────────────────────────┘
```

---

## 二、裸机启动代码

### 2.1 ARM Cortex-M 启动

```zig
// startup.zig - ARM Cortex-M 启动代码
const std = @import("std");

// 链接脚本导出的符号
extern const _stack_top: u32;
extern const _text_start: u32;
extern const _text_end: u32;
extern const _data_load: u32;
extern const _data_start: u32;
extern const _data_end: u32;
extern const _bss_start: u32;
extern const _bss_end: u32;

// 主函数入口 (用户实现)
extern fn main() noreturn;

// 默认中断处理
fn default_handler() callconv(.C) void {
    while (true) {
        asm volatile ("wfi");
    }
}

// 系统调用处理
fn svcall_handler() callconv(.C) void {
    // 处理 SVC
}

// 中断向量表
export const vector_table linksection(".vectors") = [_]fn () callconv(.C) void{
    @ptrCast(&_stack_top),  // 栈顶指针
    reset_handler,           // 复位
    default_handler,         // NMI
    default_handler,         // HardFault
    default_handler,         // MemManage
    default_handler,         // BusFault
    default_handler,         // UsageFault
    default_handler,         // 保留
    default_handler,         // 保留
    default_handler,         // 保留
    default_handler,         // 保留
    svcall_handler,          // SVCall
    default_handler,         // DebugMonitor
    default_handler,         // 保留
    default_handler,         // PendSV
    default_handler,         // SysTick
    // 外设中断...
};

// 复位处理
export fn reset_handler() callconv(.C) noreturn {
    // 1. 复制 .data 从 Flash 到 RAM
    const data_src = @as([*]const u32, @ptrCast(&_data_load));
    const data_dst = @as([*]u32, @ptrCast(&_data_start));
    const data_size = @intFromPtr(&_data_end) - @intFromPtr(&_data_start);

    for (0..data_size / 4) |i| {
        data_dst[i] = data_src[i];
    }

    // 2. 清零 .bss
    const bss_dst = @as([*]u32, @ptrCast(&_bss_start));
    const bss_size = @intFromPtr(&_bss_end) - @intFromPtr(&_bss_start);

    for (0..bss_size / 4) |i| {
        bss_dst[i] = 0;
    }

    // 3. 设置时钟 (可选)
    system_init();

    // 4. 跳转到主程序
    main();
}

fn system_init() void {
    // 初始化时钟系统
    // 配置 PLL
    // 设置 Flash 等待状态
}
```

### 2.2 RISC-V 启动

```zig
// riscv_startup.zig

export fn _start() linksection(".text.boot") callconv(.Naked) noreturn {
    asm volatile (
        \\la sp, _stack_top
        \\la t0, _bss_start
        \\la t1, _bss_end
        \\1:
        \\bgeu t0, t1, 2f
        \\sw zero, 0(t0)
        \\addi t0, t0, 4
        \\j 1b
        \\2:
        \\call main
        \\3:
        \\wfi
        \\j 3b
    );
    unreachable;
}
```

---

## 三、内存布局与链接脚本

### 3.1 链接脚本 (linker.ld)

```ld
/* memory.ld */

MEMORY {
    RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 128K
    FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 512K
}

SECTIONS {
    .text : {
        KEEP(*(.vectors))
        *(.text*)
        *(.rodata*)
        . = ALIGN(4);
    } > FLASH

    _text_start = ADDR(.text);
    _text_end = ADDR(.text) + SIZEOF(.text);

    .data : AT(_text_end) {
        _data_start = .;
        *(.data*)
        . = ALIGN(4);
        _data_end = .;
    } > RAM

    _data_load = LOADADDR(.data);

    .bss : {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        _bss_end = .;
    } > RAM

    _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}
```

### 3.2 Zig 构建配置

```zig
// build.zig
const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.resolveTargetQuery(.{
        .cpu_arch = .thumb,
        .cpu_model = .{ .explicit = &std.Target.arm.cpu.cortex_m4 },
        .os_tag = .freestanding,
        .abi = .none,
    });

    const optimize = b.standardOptimizeOption(.{});

    const elf = b.addExecutable(.{
        .name = "firmware.elf",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    // 链接脚本
    elf.setLinkerScript(.{ .path = "memory.ld" });

    // 无默认入口
    elf.entry = .{ .symbol_name = "_start" };

    // 生成二进制文件
    const bin = b.addObjCopy(elf.getEmittedBin(), .{
        .format = .bin,
    });
    bin.step.dependOn(&elf.step);

    b.getInstallStep().dependOn(&bin.step);

    // 安装二进制文件
    b.installArtifact(elf);

    // 生成 hex 文件
    const hex = b.addObjCopy(elf.getEmittedBin(), .{
        .format = .hex,
    });
    hex.step.dependOn(&elf.step);

    // 调试目标
    const debug_cmd = b.addSystemCommand(&.{
        "gdb-multiarch",
        "-ex", "target remote localhost:3333",
        "-ex", "monitor reset halt",
        "-ex", "load",
        "-ex", "continue",
    });
    debug_cmd.addFileArg(elf.getEmittedBin());

    const debug_step = b.step("debug", "Start GDB debug session");
    debug_step.dependOn(&debug_cmd.step);
}
```

---

## 四、外设驱动开发

### 4.1 MMIO 寄存器映射

```zig
// mmio.zig - 内存映射 I/O 工具

const std = @import("std");

// 寄存器读写
pub fn read_volatile(comptime T: type, address: usize) T {
    return @as(*volatile T, @ptrFromInt(address)).*;
}

pub fn write_volatile(comptime T: type, address: usize, value: T) void {
    @as(*volatile T, @ptrFromInt(address)).* = value;
}

// 修改寄存器字段
pub fn modify_volatile(comptime T: type, address: usize, mask: T, value: T) void {
    const current = read_volatile(T, address);
    write_volatile(T, address, (current & ~mask) | (value & mask));
}

// 位操作宏
pub fn bit(comptime n: u5) u32 {
    return @as(u32, 1) << n;
}

pub fn bits(comptime msb: u5, comptime lsb: u5) u32 {
    return ((@as(u32, 1) << (msb - lsb + 1)) - 1) << lsb;
}
```

### 4.2 GPIO 驱动

```zig
// gpio.zig - STM32 GPIO 驱动

const mmio = @import("mmio.zig");

// GPIO 寄存器基址
const GPIOA_BASE = 0x40020000;
const GPIOB_BASE = 0x40020400;
const GPIOC_BASE = 0x40020800;

// 寄存器偏移
const MODER_OFFSET = 0x00;
const OTYPER_OFFSET = 0x04;
const OSPEEDR_OFFSET = 0x08;
const PUPDR_OFFSET = 0x0C;
const IDR_OFFSET = 0x10;
const ODR_OFFSET = 0x14;
const BSRR_OFFSET = 0x18;

pub const Port = enum {
    A, B, C, D, E, F, G, H, I,

    fn base(self: Port) usize {
        return switch (self) {
            .A => GPIOA_BASE,
            .B => GPIOB_BASE,
            .C => GPIOC_BASE,
            else => @panic("TODO"),
        };
    }
};

pub const Mode = enum(u2) {
    input = 0b00,
    output = 0b01,
    af = 0b10,
    analog = 0b11,
};

pub const Speed = enum(u2) {
    low = 0b00,
    medium = 0b01,
    high = 0b10,
    very_high = 0b11,
};

pub const Pull = enum(u2) {
    none = 0b00,
    up = 0b01,
    down = 0b10,
};

pub const Pin = struct {
    port: Port,
    number: u4,

    pub fn init(port: Port, number: u4) Pin {
        return .{ .port = port, .number = number };
    }

    pub fn set_mode(self: Pin, mode: Mode) void {
        const base = self.port.base();
        const shift = @as(u5, self.number) * 2;
        mmio.modify_volatile(u32, base + MODER_OFFSET, @as(u32, 0b11) << shift, @intFromEnum(mode) << shift);
    }

    pub fn set_speed(self: Pin, speed: Speed) void {
        const base = self.port.base();
        const shift = @as(u5, self.number) * 2;
        mmio.modify_volatile(u32, base + OSPEEDR_OFFSET, @as(u32, 0b11) << shift, @intFromEnum(speed) << shift);
    }

    pub fn set_pull(self: Pin, pull: Pull) void {
        const base = self.port.base();
        const shift = @as(u5, self.number) * 2;
        mmio.modify_volatile(u32, base + PUPDR_OFFSET, @as(u32, 0b11) << shift, @intFromEnum(pull) << shift);
    }

    pub fn write(self: Pin, value: bool) void {
        const base = self.port.base();
        const bit_num = @as(u5, self.number);
        if (value) {
            mmio.write_volatile(u32, base + BSRR_OFFSET, @as(u32, 1) << bit_num);
        } else {
            mmio.write_volatile(u32, base + BSRR_OFFSET, @as(u32, 1) << (bit_num + 16));
        }
    }

    pub fn read(self: Pin) bool {
        const base = self.port.base();
        const idr = mmio.read_volatile(u32, base + IDR_OFFSET);
        return (idr >> @as(u5, self.number)) & 1 == 1;
    }

    pub fn toggle(self: Pin) void {
        self.write(!self.read());
    }
};

// 便捷函数
pub fn configure_output(pin: Pin) void {
    pin.set_mode(.output);
    pin.set_speed(.high);
    pin.set_pull(.none);
}

pub fn configure_input(pin: Pin, pull: Pull) void {
    pin.set_mode(.input);
    pin.set_pull(pull);
}
```

### 4.3 UART 驱动

```zig
// uart.zig - 串口驱动

const mmio = @import("mmio.zig");

const USART1_BASE = 0x40011000;

const SR_OFFSET = 0x00;
const DR_OFFSET = 0x04;
const BRR_OFFSET = 0x08;
const CR1_OFFSET = 0x0C;

pub const UART = struct {
    base: usize,

    pub fn init(base: usize) UART {
        return .{ .base = base };
    }

    pub fn configure(self: UART, baudrate: u32, clock_freq: u32) void {
        // 计算波特率
        const div = (clock_freq + baudrate / 2) / baudrate;
        mmio.write_volatile(u32, self.base + BRR_OFFSET, div);

        // 使能 TX, RX, UART
        const cr1 = 0x0C | 0x2000;  // TE + RE + UE
        mmio.write_volatile(u32, self.base + CR1_OFFSET, cr1);
    }

    pub fn write_byte(self: UART, byte: u8) void {
        // 等待发送寄存器空
        while (mmio.read_volatile(u32, self.base + SR_OFFSET) & 0x80 == 0) {}

        mmio.write_volatile(u32, self.base + DR_OFFSET, byte);
    }

    pub fn read_byte(self: UART) ?u8 {
        // 检查接收数据
        if (mmio.read_volatile(u32, self.base + SR_OFFSET) & 0x20 != 0) {
            return @truncate(mmio.read_volatile(u32, self.base + DR_OFFSET));
        }
        return null;
    }

    pub fn write_string(self: UART, s: []const u8) void {
        for (s) |c| {
            self.write_byte(c);
        }
    }

    pub fn format(self: UART, comptime fmt: []const u8, args: anytype) void {
        var buf: [256]u8 = undefined;
        const printed = std.fmt.bufPrint(&buf, fmt, args) catch return;
        self.write_string(printed);
    }
};

// 全局 UART 实例
pub const uart1 = UART.init(USART1_BASE);
```

---

## 五、中断与异常处理

### 5.1 NVIC 配置 (Cortex-M)

```zig
// nvic.zig - 嵌套向量中断控制器

const NVIC_BASE = 0xE000E100;

const ISER_OFFSET = 0x000;  // 中断使能
const ICER_OFFSET = 0x080;  // 中断禁用
const ISPR_OFFSET = 0x100;  // 中断挂起
const ICPR_OFFSET = 0x180;  // 中断清除挂起
const IABR_OFFSET = 0x200;  // 激活位
const IPR_OFFSET = 0x300;   // 优先级

pub fn enable_irq(irqn: u8) void {
    const reg = NVIC_BASE + ISER_OFFSET + (irqn / 32) * 4;
    mmio.write_volatile(u32, reg, @as(u32, 1) << @truncate(irqn % 32));
}

pub fn disable_irq(irqn: u8) void {
    const reg = NVIC_BASE + ICER_OFFSET + (irqn / 32) * 4;
    mmio.write_volatile(u32, reg, @as(u32, 1) << @truncate(irqn % 32));
}

pub fn set_priority(irqn: u8, priority: u8) void {
    const reg = NVIC_BASE + IPR_OFFSET + irqn;
    mmio.write_volatile(u8, reg, priority << 4);
}
```

### 5.2 中断处理函数

```zig
// interrupts.zig

// 外部中断 0 处理
export fn EXTI0_IRQHandler() void {
    // 清除中断标志
    mmio.write_volatile(u32, 0x40013C14, 0x01);

    // 用户代码
    gpio.Pin.init(.A, 0).toggle();
}

// 定时器中断
export fn TIM2_IRQHandler() void {
    const TIM2_SR = 0x40000010;

    // 检查更新中断
    if (mmio.read_volatile(u32, TIM2_SR) & 0x01 != 0) {
        // 清除标志
        mmio.write_volatile(u32, TIM2_SR, ~@as(u32, 0x01));

        // 时基递增
        system_tick += 1;
    }
}

var system_tick: u32 = 0;

pub fn get_tick() u32 {
    return @atomicLoad(u32, &system_tick, .Monotonic);
}

pub fn delay_ms(ms: u32) void {
    const start = get_tick();
    while (get_tick() - start < ms) {
        asm volatile ("wfi");  // 等待中断
    }
}
```

---

## 六、与 C 混合开发

### 6.1 调用 C 库

```zig
// 导入 CMSIS
const cmsis = @cImport({
    @cInclude("stm32f4xx.h");
});

// 使用 C 定义
pub fn use_cmsis() void {
    // 直接访问 C 结构体
    cmsis.RCC->AHB1ENR |= cmsis.RCC_AHB1ENR_GPIOAEN;

    // 配置 GPIO
    cmsis.GPIOA->MODER |= cmsis.GPIO_MODER_MODER5_0;
}

// 链接 C 库
// build.zig:
// lib.addCSourceFile("stm32f4xx_gpio.c", &.{});
// lib.addIncludePath("CMSIS/Include");
```

### 6.2 导出 Zig 函数给 C

```zig
// 导出初始化函数
export fn SystemInit() void {
    // 设置时钟
    // 配置 PLL
}

// 导出驱动函数
export fn LED_Init() void {
    const led = gpio.Pin.init(.A, 5);
    gpio.configure_output(led);
}

export fn LED_On() void {
    gpio.Pin.init(.A, 5).write(true);
}

export fn LED_Off() void {
    gpio.Pin.init(.A, 5).write(false);
}

export fn LED_Toggle() void {
    gpio.Pin.init(.A, 5).toggle();
}

// C 头文件 (generated.h):
// void SystemInit(void);
// void LED_Init(void);
// void LED_On(void);
// void LED_Off(void);
// void LED_Toggle(void);
```

---

## 七、实战：STM32 点灯

### 7.1 完整项目

```zig
// main.zig - STM32F4 LED 闪烁

const std = @import("std");
const gpio = @import("gpio.zig");
const uart = @import("uart.zig");
const nvic = @import("nvic.zig");

// 系统时钟配置
fn system_clock_config() void {
    // 启用 HSE
    const RCC_CR = 0x40023800;
    mmio.write_volatile(u32, RCC_CR, mmio.read_volatile(u32, RCC_CR) | 0x00010000);

    // 等待 HSE 就绪
    while (mmio.read_volatile(u32, RCC_CR) & 0x00020000 == 0) {}

    // 配置 PLL
    const RCC_PLLCFGR = 0x40023804;
    mmio.write_volatile(u32, RCC_PLLCFGR, 0x27405408);

    // 启用 PLL
    mmio.write_volatile(u32, RCC_CR, mmio.read_volatile(u32, RCC_CR) | 0x01000000);

    // 等待 PLL 就绪
    while (mmio.read_volatile(u32, RCC_CR) & 0x02000000 == 0) {}

    // 切换系统时钟到 PLL
    const RCC_CFGR = 0x40023808;
    mmio.modify_volatile(u32, RCC_CFGR, 0b11, 0b10);

    // 等待切换完成
    while ((mmio.read_volatile(u32, RCC_CFGR) >> 2) & 0b11 != 0b10) {}

    // 更新 SystemCoreClock 变量
    SystemCoreClock = 168000000;
}

var SystemCoreClock: u32 = 16000000;

pub export fn main() noreturn {
    // 配置系统时钟
    system_clock_config();

    // 启用 GPIOA 时钟
    const RCC_AHB1ENR = 0x40023830;
    mmio.write_volatile(u32, RCC_AHB1ENR, mmio.read_volatile(u32, RCC_AHB1ENR) | 0x01);

    // 配置 PA5 (LED) 为输出
    const led = gpio.Pin.init(.A, 5);
    gpio.configure_output(led);

    // 配置串口用于调试
    uart.uart1.configure(115200, SystemCoreClock);
    uart.uart1.write_string("STM32 Zig Boot!\r\n");

    // 主循环
    var counter: u32 = 0;
    while (true) {
        led.toggle();

        uart.uart1.format("Tick: {}\r\n", .{counter});
        counter += 1;

        delay_ms(500);
    }
}

fn delay_ms(ms: u32) void {
    // 简单忙等待
    const cycles_per_ms = SystemCoreClock / 1000;
    for (0..ms) |_| {
        for (0..cycles_per_ms / 3) |_| {
            asm volatile ("nop");
        }
    }
}
```

---

## 八、实战：Raspberry Pi Pico 驱动

```zig
// pico_main.zig - RP2040 驱动

const mmio = @import("mmio.zig");

// RP2040 寄存器基址
const SIO_BASE = 0xD0000000;
const GPIO_OE = SIO_BASE + 0x020;
const GPIO_OUT = SIO_BASE + 0x014;
const GPIO_OUT_XOR = SIO_BASE + 0x01C;

const RESETS_BASE = 0x4000C000;

fn reset_peripheral(peripheral: u32) void {
    const RESET = RESETS_BASE + 0x00;
    const RESET_DONE = RESETS_BASE + 0x08;

    // 复位
    mmio.modify_volatile(u32, RESET, peripheral, peripheral);

    // 等待复位完成
    while (mmio.read_volatile(u32, RESET_DONE) & peripheral == 0) {}
}

pub export fn main() noreturn {
    // 复位 IO _BANK0
    reset_peripheral(1 << 5);

    // 配置 GPIO25 (板载 LED)
    const GPIO25_CTRL = 0x40014000 + 0x0CC;
    mmio.write_volatile(u32, GPIO25_CTRL, 5);  // SIO 功能

    // 设置为输出
    mmio.write_volatile(u32, GPIO_OE, mmio.read_volatile(u32, GPIO_OE) | (1 << 25));

    // 闪烁 LED
    while (true) {
        // 翻转输出
        mmio.write_volatile(u32, GPIO_OUT_XOR, 1 << 25);

        // 延时
        var delay: u32 = 0;
        while (delay < 500000) : (delay += 1) {
            asm volatile ("nop");
        }
    }
}
```

---

## 九、RTOS 集成

### 9.1 FreeRTOS 包装

```zig
// freertos.zig

const freertos = @cImport({
    @cInclude("FreeRTOS.h");
    @cInclude("task.h");
    @cInclude("queue.h");
    @cInclude("semphr.h");
});

pub const Task = struct {
    handle: freertos.TaskHandle_t,

    pub fn create(name: []const u8, stack_size: u16, priority: u32, func: fn () void) !Task {
        var handle: freertos.TaskHandle_t = undefined;

        const result = freertos.xTaskCreate(
            @ptrCast(func),
            name.ptr,
            stack_size,
            null,
            priority,
            &handle
        );

        if (result != freertos.pdPASS) {
            return error.TaskCreateFailed;
        }

        return .{ .handle = handle };
    }

    pub fn delay(ms: u32) void {
        freertos.vTaskDelay(ms / portTICK_PERIOD_MS);
    }

    pub fn start_scheduler() noreturn {
        freertos.vTaskStartScheduler();
        unreachable;
    }
};

pub const Queue = struct {
    handle: freertos.QueueHandle_t,

    pub fn create(length: u32, item_size: u32) !Queue {
        const handle = freertos.xQueueCreate(length, item_size);
        if (handle == null) return error.QueueCreateFailed;
        return .{ .handle = handle };
    }

    pub fn send(self: Queue, item: *const anyopaque, timeout_ms: u32) bool {
        return freertos.xQueueSend(self.handle, item, timeout_ms) == freertos.pdTRUE;
    }

    pub fn receive(self: Queue, buffer: *anyopaque, timeout_ms: u32) bool {
        return freertos.xQueueReceive(self.handle, buffer, timeout_ms) == freertos.pdTRUE;
    }
};

// 使用示例
fn led_task() void {
    while (true) {
        led.toggle();
        Task.delay(500);
    }
}

fn sensor_task() void {
    while (true) {
        const value = adc.read();
        _ = queue.send(&value, 100);
        Task.delay(100);
    }
}
```

---

## 十、调试技巧

### 10.1 ITM/SWO 输出

```zig
// itm.zig - Instrumentation Trace Macrocell

const ITM_BASE = 0xE0000000;
const ITM_PORT = ITM_BASE + 0x000;

pub fn init() void {
    // 启用 ITM
    const DEMCR = 0xE000EDFC;
    mmio.write_volatile(u32, DEMCR, mmio.read_volatile(u32, DEMCR) | 0x01000000);

    // 解锁 ITM
    const ITM_LAR = ITM_BASE + 0xFB0;
    mmio.write_volatile(u32, ITM_LAR, 0xC5ACCE55);

    // 启用端口 0
    const ITM_TER0 = ITM_BASE + 0xE00;
    mmio.write_volatile(u32, ITM_TER0, 0x01);

    const ITM_TCR = ITM_BASE + 0xE80;
    mmio.write_volatile(u32, ITM_TCR, 0x00010001);
}

pub fn write_char(c: u8) void {
    const port = ITM_PORT;
    while (mmio.read_volatile(u32, port) == 0) {}
    mmio.write_volatile(u8, port, c);
}

pub fn print(s: []const u8) void {
    for (s) |c| {
        write_char(c);
    }
}
```

### 10.2 断言与检查

```zig
pub fn assert(condition: bool, msg: []const u8) void {
    if (!condition) {
        uart.uart1.write_string("ASSERTION FAILED: ");
        uart.uart1.write_string(msg);
        uart.uart1.write_string("\r\n");

        // 触发断点
        asm volatile ("bkpt #0");

        // 或者重启
        const AIRCR = 0xE000ED0C;
        mmio.write_volatile(u32, AIRCR, 0x05FA0004);

        while (true) {}
    }
}

pub fn check_stack_overflow() void {
    // 检查栈底魔法值
    const stack_bottom: *volatile u32 = @ptrFromInt(0x20000000);
    if (stack_bottom.* != 0xDEADBEEF) {
        @panic("Stack overflow detected!");
    }
}
```

---

> **文档信息**
>
> - 创建时间: 2026-03-10
> - 目标平台: ARM Cortex-M, RISC-V, x86 bare metal
> - 示例硬件: STM32F4, RP2040
