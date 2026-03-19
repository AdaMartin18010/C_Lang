# 实验6：SoC系统集成与串口通信

## 目录

- [实验6：SoC系统集成与串口通信](#实验6soc系统集成与串口通信)
  - [目录](#目录)
  - [1. 实验概述](#1-实验概述)
    - [1.1 实验目标](#11-实验目标)
    - [1.2 实验环境](#12-实验环境)
    - [1.3 SoC架构概览](#13-soc架构概览)
    - [1.4 内存映射表](#14-内存映射表)
  - [2. 实验原理](#2-实验原理)
    - [2.1 片上总线协议](#21-片上总线协议)
    - [2.2 UART串口通信](#22-uart串口通信)
    - [2.3 PLIC中断控制器（简化版）](#23-plic中断控制器简化版)
    - [2.4 系统启动流程](#24-系统启动流程)
  - [3. 实验步骤](#3-实验步骤)
    - [3.1 工程创建](#31-工程创建)
      - [3.1.1 创建Vivado工程](#311-创建vivado工程)
      - [3.1.2 工程文件结构](#312-工程文件结构)
    - [3.2 设计流程](#32-设计流程)
      - [Step 1: 实现总线互联](#step-1-实现总线互联)
      - [Step 2: 实现UART控制器](#step-2-实现uart控制器)
      - [Step 3: 实现GPIO控制器](#step-3-实现gpio控制器)
      - [Step 4: 实现PLIC](#step-4-实现plic)
      - [Step 5: 系统集成](#step-5-系统集成)
      - [Step 6: 软件开发](#step-6-软件开发)
  - [4. 完整代码实现](#4-完整代码实现)
    - [4.1 SoC顶层 (soc\_top.v)](#41-soc顶层-soc_topv)
    - [4.3 GPIO控制器 (gpio\_controller.v)](#43-gpio控制器-gpio_controllerv)
    - [4.4 PLIC中断控制器 (plic.v)](#44-plic中断控制器-plicv)
    - [4.5 定时器 (timer.v)](#45-定时器-timerv)
    - [4.6 测试平台 (soc\_tb.v)](#46-测试平台-soc_tbv)
    - [4.7 Bootloader代码 (boot.S)](#47-bootloader代码-boots)
    - [4.8 Hello World程序 (hello.c)](#48-hello-world程序-helloc)
    - [4.9 链接脚本 (link.ld)](#49-链接脚本-linkld)
    - [4.10 Makefile](#410-makefile)
    - [4.11 串口下载工具 (uart\_loader.py)](#411-串口下载工具-uart_loaderpy)
    - [4.12 Bootloader接收端代码 (bootloader.c)](#412-bootloader接收端代码-bootloaderc)
  - [5. 仿真与验证](#5-仿真与验证)
    - [5.1 仿真步骤](#51-仿真步骤)
    - [5.2 测试内容](#52-测试内容)
  - [6. 硬件部署](#6-硬件部署)
    - [6.1 约束文件 (nexys\_a7.xdc)](#61-约束文件-nexys_a7xdc)
    - [6.2 部署步骤](#62-部署步骤)
  - [7. 软件编程](#7-软件编程)
    - [7.1 工具链安装](#71-工具链安装)
    - [7.2 编译示例](#72-编译示例)
  - [8. 调试技巧](#8-调试技巧)
    - [8.1 常见问题](#81-常见问题)
    - [8.2 调试方法](#82-调试方法)
  - [9. 进阶挑战](#9-进阶挑战)
    - [9.1 添加更多外设](#91-添加更多外设)
    - [9.2 实现多核支持](#92-实现多核支持)
    - [9.3 运行操作系统](#93-运行操作系统)
  - [10. 思考题](#10-思考题)
  - [附录](#附录)
    - [A. 串口通信协议](#a-串口通信协议)
    - [B. 参考资源](#b-参考资源)

---

## 1. 实验概述

### 1.1 实验目标

本实验将构建一个完整的SoC系统，集成之前实验设计的CPU和Cache，并添加必要的外设：

- ✅ 理解SoC架构和片上总线设计
- ✅ 集成CPU、Cache和系统总线
- ✅ 实现UART串口控制器与PC通信
- ✅ 实现GPIO控制器控制LED/开关
- ✅ 实现简化版PLIC中断控制器
- ✅ 编写Hello World程序并在SoC上运行
- ✅ 实现串口程序下载功能
- ✅ 完成系统级调试和验证

### 1.2 实验环境

| 项目 | 推荐配置 |
|------|----------|
| FPGA开发板 | Xilinx Nexys A7 / Basys 3 |
| 开发工具 | Vivado 2020.1 + RISC-V GCC工具链 |
| 串口工具 | PuTTY / Tera Term / minicom |
| 调试工具 | Vitis / OpenOCD（可选） |

### 1.3 SoC架构概览

```
┌─────────────────────────────────────────────────────────┐
│                        SoC Top                          │
├─────────────────────────────────────────────────────────┤
│  ┌─────────┐    ┌─────────┐    ┌─────────────────────┐ │
│  │  CPU    │◄──►│  Cache  │◄──►│    System Bus       │ │
│  │ (RV32I) │    │ (4KB)   │    │  (Wishbone/AXI-Lite)│ │
│  └─────────┘    └─────────┘    └──────────┬──────────┘ │
│                                           │            │
│    ┌────────┬────────┬────────┬───────────┼────────┐   │
│    ▼        ▼        ▼        ▼           ▼        │   │
│ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────┐ ┌──────────┐  │   │
│ │ UART │ │ GPIO │ │ PLIC │ │Timer │ │ Boot ROM │  │   │
│ │ Ctrl │ │ Ctrl │ │(IRQ) │ │      │ │ (8KB)    │  │   │
│ └──┬───┘ └──────┘ └──────┘ └──────┘ └──────────┘  │   │
│    │                                              │   │
│    └──────────────────────────────────────────────┘   │
│                      外设接口                          │
└─────────────────────────────────────────────────────────┘
```

### 1.4 内存映射表

| 地址范围 | 设备 | 说明 |
|----------|------|------|
| 0x0000_0000 - 0x0000_1FFF | Boot ROM | 8KB，只读，存储启动代码 |
| 0x2000_0000 - 0x2000_3FFF | SRAM | 16KB，可读写 |
| 0x4000_0000 - 0x4000_00FF | UART | 串口寄存器 |
| 0x4000_0100 - 0x4000_01FF | GPIO | GPIO寄存器 |
| 0x4000_0200 - 0x4000_02FF | PLIC | 中断控制器 |
| 0x4000_0300 - 0x4000_03FF | Timer | 定时器 |

---

## 2. 实验原理

### 2.1 片上总线协议

本实验采用简化的Wishbone-like总线协议：

**信号定义：**

```
clk_i      - 时钟输入
rst_n_i    - 复位输入（低有效）
adr_i[31:0]- 地址输入
dat_i[31:0]- 数据输入（写）
dat_o[31:0]- 数据输出（读）
we_i       - 写使能
sel_i[3:0] - 字节选择
stb_i      - 选通信号
cycl_i     - 周期信号
ack_o      - 应答输出
```

**读时序：**

```
Clock:   1    2    3    4    5
         ┌────┐    ┌────┐
ADR:  XXX│Addr│XXXX│Addr│XXXX
         └────┘    └────┘
              ┌────┐
STB:  ________│    │______
              └────┘
                   ┌────┐
ACK:  ____________│    │__
                   └────┘
              ┌────────┐
DAT_O: XXXXXXX│  Data  │XX
              └────────┘
```

### 2.2 UART串口通信

**UART帧格式（8N1）：**

```
    Start  D0   D1   D2   D3   D4   D5   D6   D7   Stop
     │     │    │    │    │    │    │    │    │    │
  ───┘     └────┴────┴────┴────┴────┴────┴────┘    └───
     0    LSB                          MSB    1
```

**波特率计算（100MHz时钟，115200bps）：**

```
分频值 = 时钟频率 / 波特率 = 100,000,000 / 115,200 ≈ 868
```

### 2.3 PLIC中断控制器（简化版）

**PLIC架构：**

```
中断源 ──┬──→ 边沿检测 ──→ 中断等待寄存器 ──┐
         │                                  │
中断源 ──┤                                  ├──→ 仲裁器 ──→ CPU
         │                                  │
中断源 ──┴──→ 边沿检测 ──→ 中断等待寄存器 ──┘
                    ↑
              中断使能寄存器
```

**寄存器映射：**

| 偏移 | 名称 | 说明 |
|------|------|------|
| 0x00 | IP | 中断等待（只读） |
| 0x04 | IE | 中断使能（读写） |
| 0x08 | TH | 阈值（读写） |
| 0x0C | CC | 中断完成/声明（读写） |

### 2.4 系统启动流程

```
1. 上电复位
      │
      ▼
2. CPU PC = 0x0000_0000 (Boot ROM)
      │
      ▼
3. 执行Bootloader代码
   - 初始化栈指针
   - 初始化BSS段
   - 复制程序到SRAM（如需要）
      │
      ▼
4. 跳转到main()入口 (0x2000_0000)
      │
      ▼
5. 运行用户程序
```

---

## 3. 实验步骤

### 3.1 工程创建

#### 3.1.1 创建Vivado工程

```tcl
create_project Lab6_SoC_Integration ./Lab6_SoC -part xc7a100tcsg324-1
```

#### 3.1.2 工程文件结构

```
Lab6_SoC/
├── rtl/
│   ├── core/
│   │   ├── cpu/              # CPU核心（复用Lab4）
│   │   ├── cache/            # Cache（复用Lab5）
│   │   └── reg_file.v
│   ├── bus/
│   │   ├── bus_arbiter.v     # 总线仲裁器
│   │   ├── bus_decoder.v     # 地址译码器
│   │   └── wb_intercon.v     # Wishbone互联
│   ├── periph/
│   │   ├── uart_controller.v # UART控制器
│   │   ├── gpio_controller.v # GPIO控制器
│   │   ├── plic.v            # 中断控制器
│   │   └── timer.v           # 定时器
│   ├── memory/
│   │   ├── boot_rom.v        # Boot ROM
│   │   └── sram.v            # SRAM
│   └── soc_top.v             # SoC顶层
├── sim/
│   └── soc_tb.v
├── sw/
│   ├── boot/                 # Bootloader
│   ├── hello/                # Hello World程序
│   ├── uart_loader/          # 串口下载工具
│   └── linker/               # 链接脚本
├── constr/
│   └── nexys_a7.xdc
└── ip/                       # 需要的IP核
```

### 3.2 设计流程

#### Step 1: 实现总线互联

- 设计地址译码器
- 实现多主设备仲裁
- 添加总线错误处理

#### Step 2: 实现UART控制器

- 实现发送FIFO和接收FIFO
- 实现波特率发生器
- 添加状态寄存器

#### Step 3: 实现GPIO控制器

- 实现方向寄存器
- 实现数据寄存器
- 添加中断支持

#### Step 4: 实现PLIC

- 实现中断优先级
- 实现中断屏蔽
- 实现中断完成机制

#### Step 5: 系统集成

- 连接所有模块
- 配置内存映射
- 添加时钟和复位

#### Step 6: 软件开发

- 编写Bootloader
- 编写测试程序
- 配置工具链

---

## 4. 完整代码实现

### 4.1 SoC顶层 (soc_top.v)

```verilog
//============================================================================
// 文件名: soc_top.v
// 描述: 完整SoC系统顶层
// 集成: RV32I CPU + Cache + UART + GPIO + PLIC + Timer
// 作者: FPGA教学团队
// 版本: 1.0
//============================================================================

module soc_top (
    input         sys_clk,
    input         sys_rst_n,

    // UART接口
    input         uart_rx,
    output        uart_tx,

    // GPIO接口
    input  [15:0] gpio_in,
    output [15:0] gpio_out,
    output [15:0] gpio_oe,       // 方向控制（1=输出）

    // 调试接口
    output [31:0] debug_pc,
    output        debug_halted
);

    //========================================================================
    // 参数定义
    //========================================================================
    localparam BOOT_ROM_BASE = 32'h0000_0000;
    localparam BOOT_ROM_SIZE = 32'h0000_2000;  // 8KB
    localparam SRAM_BASE     = 32'h2000_0000;
    localparam SRAM_SIZE     = 32'h0000_4000;  // 16KB
    localparam UART_BASE     = 32'h4000_0000;
    localparam GPIO_BASE     = 32'h4000_0100;
    localparam PLIC_BASE     = 32'h4000_0200;
    localparam TIMER_BASE    = 32'h4000_0300;

    //========================================================================
    // 内部信号
    //========================================================================
    wire        clk;
    wire        rst_n;

    // CPU接口
    wire [31:0] cpu_addr;
    wire [31:0] cpu_wdata;
    wire [31:0] cpu_rdata;
    wire        cpu_we;
    wire [3:0]  cpu_sel;
    wire        cpu_stb;
    wire        cpu_cyc;
    wire        cpu_ack;
    wire [2:0]  cpu_cti;

    // 中断信号
    wire        timer_irq;
    wire        uart_irq;
    wire        gpio_irq;
    wire        external_irq;

    //========================================================================
    // 时钟和复位
    //========================================================================
    assign clk = sys_clk;
    assign rst_n = sys_rst_n;

    //========================================================================
    // CPU实例化（复用Lab4的流水线CPU）
    //========================================================================
    pipelined_cpu_top cpu_core (
        .clk            (clk),
        .rst_n          (rst_n),
        .i_bus_adr      (cpu_addr),
        .i_bus_dat_w    (cpu_wdata),
        .i_bus_dat_r    (cpu_rdata),
        .i_bus_we       (cpu_we),
        .i_bus_sel      (cpu_sel),
        .i_bus_stb      (cpu_stb),
        .i_bus_cyc      (cpu_cyc),
        .i_bus_ack      (cpu_ack),
        .i_bus_cti      (cpu_cti),
        .irq_external   (external_irq),
        .irq_timer      (timer_irq),
        .debug_pc       (debug_pc),
        .debug_halted   (debug_halted)
    );

    //========================================================================
    // 地址译码和总线互联
    //========================================================================
    wire        sel_boot_rom;
    wire        sel_sram;
    wire        sel_uart;
    wire        sel_gpio;
    wire        sel_plic;
    wire        sel_timer;
    wire        sel_err;

    // 地址译码
    assign sel_boot_rom = (cpu_addr >= BOOT_ROM_BASE) &&
                          (cpu_addr < BOOT_ROM_BASE + BOOT_ROM_SIZE);
    assign sel_sram     = (cpu_addr >= SRAM_BASE) &&
                          (cpu_addr < SRAM_BASE + SRAM_SIZE);
    assign sel_uart     = (cpu_addr >= UART_BASE) &&
                          (cpu_addr < UART_BASE + 32'h100);
    assign sel_gpio     = (cpu_addr >= GPIO_BASE) &&
                          (cpu_addr < GPIO_BASE + 32'h100);
    assign sel_plic     = (cpu_addr >= PLIC_BASE) &&
                          (cpu_addr < PLIC_BASE + 32'h100);
    assign sel_timer    = (cpu_addr >= TIMER_BASE) &&
                          (cpu_addr < TIMER_BASE + 32'h100);
    assign sel_err      = !(sel_boot_rom || sel_sram || sel_uart ||
                           sel_gpio || sel_plic || sel_timer);

    //========================================================================
    // Boot ROM (8KB)
    //========================================================================
    wire [31:0] boot_rom_rdata;
    wire        boot_rom_ack;

    boot_rom #(
        .DEPTH(2048),
        .INIT_FILE("bootloader.hex")
    ) i_boot_rom (
        .clk    (clk),
        .rst_n  (rst_n),
        .adr_i  (cpu_addr[12:2]),  // 字地址
        .dat_o  (boot_rom_rdata),
        .stb_i  (cpu_stb && sel_boot_rom),
        .ack_o  (boot_rom_ack)
    );

    //========================================================================
    // SRAM (16KB)
    //========================================================================
    wire [31:0] sram_rdata;
    wire        sram_ack;

    sram #(
        .DEPTH(4096)  // 16KB / 4 = 4096字
    ) i_sram (
        .clk    (clk),
        .rst_n  (rst_n),
        .adr_i  (cpu_addr[13:2]),
        .dat_i  (cpu_wdata),
        .dat_o  (sram_rdata),
        .we_i   (cpu_we),
        .sel_i  (cpu_sel),
        .stb_i  (cpu_stb && sel_sram),
        .ack_o  (sram_ack)
    );

    //========================================================================
    // UART控制器
    //========================================================================
    wire [31:0] uart_rdata;
    wire        uart_ack;

    uart_controller #(
        .CLK_FREQ(100000000),
        .BAUD_RATE(115200)
    ) i_uart (
        .clk        (clk),
        .rst_n      (rst_n),
        .adr_i      (cpu_addr[7:0]),
        .dat_i      (cpu_wdata),
        .dat_o      (uart_rdata),
        .we_i       (cpu_we),
        .sel_i      (cpu_sel),
        .stb_i      (cpu_stb && sel_uart),
        .ack_o      (uart_ack),
        .uart_rx    (uart_rx),
        .uart_tx    (uart_tx),
        .irq        (uart_irq)
    );

    //========================================================================
    // GPIO控制器
    //========================================================================
    wire [31:0] gpio_rdata;
    wire        gpio_ack;

    gpio_controller i_gpio (
        .clk        (clk),
        .rst_n      (rst_n),
        .adr_i      (cpu_addr[7:0]),
        .dat_i      (cpu_wdata),
        .dat_o      (gpio_rdata),
        .we_i       (cpu_we),
        .sel_i      (cpu_sel),
        .stb_i      (cpu_stb && sel_gpio),
        .ack_o      (gpio_ack),
        .gpio_in    (gpio_in),
        .gpio_out   (gpio_out),
        .gpio_oe    (gpio_oe),
        .irq        (gpio_irq)
    );

    //========================================================================
    // PLIC (简化版中断控制器)
    //========================================================================
    wire [31:0] plic_rdata;
    wire        plic_ack;

    plic i_plic (
        .clk        (clk),
        .rst_n      (rst_n),
        .adr_i      (cpu_addr[7:0]),
        .dat_i      (cpu_wdata),
        .dat_o      (plic_rdata),
        .we_i       (cpu_we),
        .stb_i      (cpu_stb && sel_plic),
        .ack_o      (plic_ack),
        // 中断源
        .irq_src_0  (uart_irq),
        .irq_src_1  (gpio_irq),
        .irq_src_2  (timer_irq),
        // 输出
        .irq_out    (external_irq)
    );

    //========================================================================
    // 定时器
    //========================================================================
    wire [31:0] timer_rdata;
    wire        timer_ack;

    timer i_timer (
        .clk        (clk),
        .rst_n      (rst_n),
        .adr_i      (cpu_addr[7:0]),
        .dat_i      (cpu_wdata),
        .dat_o      (timer_rdata),
        .we_i       (cpu_we),
        .stb_i      (cpu_stb && sel_timer),
        .ack_o      (timer_ack),
        .irq        (timer_irq)
    );

    //========================================================================
    // 总线响应多路选择
    //========================================================================
    reg [31:0]  rdata_mux;
    reg         ack_mux;

    always @(*) begin
        case (1'b1)
            sel_boot_rom: begin rdata_mux = boot_rom_rdata; ack_mux = boot_rom_ack; end
            sel_sram:     begin rdata_mux = sram_rdata;     ack_mux = sram_ack;     end
            sel_uart:     begin rdata_mux = uart_rdata;     ack_mux = uart_ack;     end
            sel_gpio:     begin rdata_mux = gpio_rdata;     ack_mux = gpio_ack;     end
            sel_plic:     begin rdata_mux = plic_rdata;     ack_mux = plic_ack;     end
            sel_timer:    begin rdata_mux = timer_rdata;    ack_mux = timer_ack;    end
            sel_err:      begin rdata_mux = 32'hDEAD_BEEF;  ack_mux = cpu_stb;      end
            default:      begin rdata_mux = 32'd0;          ack_mux = 1'b0;         end
        endcase
    end

    assign cpu_rdata = rdata_mux;
    assign cpu_ack = ack_mux;

endmodule

//============================================================================
// Boot ROM 模块
//============================================================================
module boot_rom #(
    parameter DEPTH = 2048,
    parameter INIT_FILE = ""
)(
    input         clk,
    input         rst_n,
    input  [10:0] adr_i,     // 字地址
    output [31:0] dat_o,
    input         stb_i,
    output reg    ack_o
);
    reg [31:0] mem [0:DEPTH-1];

    // 初始化
    initial begin
        if (INIT_FILE != "")
            $readmemh(INIT_FILE, mem);
        else begin
            // 默认启动代码：跳转到SRAM
            // lui  x1, 0x20000    # x1 = 0x20000000 (SRAM base)
            // jalr x0, x1, 0      # jump to x1
            mem[0] = 32'h200000b7;
            mem[1] = 32'h00008067;
        end
    end

    assign dat_o = mem[adr_i];

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            ack_o <= 1'b0;
        else
            ack_o <= stb_i;
    end
endmodule

//============================================================================
// SRAM 模块
//============================================================================
module sram #(
    parameter DEPTH = 4096
)(
    input         clk,
    input         rst_n,
    input  [11:0] adr_i,
    input  [31:0] dat_i,
    output [31:0] dat_o,
    input         we_i,
    input  [3:0]  sel_i,
    input         stb_i,
    output reg    ack_o
);
    reg [31:0] mem [0:DEPTH-1];
    reg [31:0] rdata;

    // 写操作（支持字节写使能）
    always @(posedge clk) begin
        if (stb_i && we_i) begin
            if (sel_i[0]) mem[adr_i][7:0]   <= dat_i[7:0];
            if (sel_i[1]) mem[adr_i][15:8]  <= dat_i[15:8];
            if (sel_i[2]) mem[adr_i][23:16] <= dat_i[23:16];
            if (sel_i[3]) mem[adr_i][31:24] <= dat_i[31:24];
        end
    end

    // 读操作
    always @(posedge clk) begin
        if (stb_i && !we_i)
            rdata <= mem[adr_i];
    end

    assign dat_o = rdata;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            ack_o <= 1'b0;
        else
            ack_o <= stb_i;
    end
endmodule


### 4.2 UART控制器 (uart_controller.v)

```verilog
//============================================================================
// 文件名: uart_controller.v
// 描述: UART控制器，支持发送/接收FIFO和中断
//============================================================================

module uart_controller #(
    parameter CLK_FREQ = 100000000,
    parameter BAUD_RATE = 115200
)(
    input         clk,
    input         rst_n,

    // 总线接口
    input  [7:0]  adr_i,
    input  [31:0] dat_i,
    output [31:0] dat_o,
    input         we_i,
    input  [3:0]  sel_i,
    input         stb_i,
    output reg    ack_o,

    // UART接口
    input         uart_rx,
    output        uart_tx,

    // 中断输出
    output        irq
);

    //========================================================================
    // 寄存器地址定义
    //========================================================================
    localparam REG_TXDATA = 8'h00;  // 发送数据（只写）
    localparam REG_RXDATA = 8'h04;  // 接收数据（只读）
    localparam REG_TXCTRL = 8'h08;  // 发送控制
    localparam REG_RXCTRL = 8'h0C;  // 接收控制
    localparam REG_IE     = 8'h10;  // 中断使能
    localparam REG_IP     = 8'h14;  // 中断状态
    localparam REG_DIV    = 8'h18;  // 波特率分频

    //========================================================================
    // 波特率分频计算
    //========================================================================
    localparam DIV_VAL = CLK_FREQ / BAUD_RATE;

    //========================================================================
    // 寄存器
    //========================================================================
    reg [31:0] txdata_reg;
    reg [31:0] rxdata_reg;
    reg [31:0] txctrl_reg;
    reg [31:0] rxctrl_reg;
    reg [31:0] ie_reg;
    reg [31:0] div_reg;

    //========================================================================
    // 发送逻辑
    //========================================================================
    reg [15:0] tx_div_cnt;
    reg [3:0]  tx_bit_cnt;
    reg [9:0]  tx_shift;    // {stop, data[7:0], start}
    reg        tx_busy;
    reg        tx_reg;

    wire tx_start = stb_i && we_i && (adr_i == REG_TXDATA) && !tx_busy;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            tx_div_cnt <= 16'd0;
            tx_bit_cnt <= 4'd0;
            tx_shift <= 10'h3FF;
            tx_busy <= 1'b0;
            tx_reg <= 1'b1;
        end else begin
            if (tx_start) begin
                tx_shift <= {1'b1, dat_i[7:0], 1'b0};  // {stop, data, start}
                tx_busy <= 1'b1;
                tx_bit_cnt <= 4'd0;
                tx_div_cnt <= 16'd0;
            end else if (tx_busy) begin
                if (tx_div_cnt >= div_reg[15:0]) begin
                    tx_div_cnt <= 16'd0;
                    tx_reg <= tx_shift[0];
                    tx_shift <= {1'b1, tx_shift[9:1]};

                    if (tx_bit_cnt == 4'd9)
                        tx_busy <= 1'b0;
                    else
                        tx_bit_cnt <= tx_bit_cnt + 1'b1;
                end else begin
                    tx_div_cnt <= tx_div_cnt + 1'b1;
                end
            end
        end
    end

    assign uart_tx = tx_reg;

    //========================================================================
    // 接收逻辑
    //========================================================================
    reg [15:0] rx_div_cnt;
    reg [3:0]  rx_bit_cnt;
    reg [7:0]  rx_shift;
    reg        rx_busy;
    reg        rx_done;
    reg        rx_sync_0, rx_sync_1;
    wire       rx_sync = rx_sync_1;

    // 同步器
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_sync_0 <= 1'b1;
            rx_sync_1 <= 1'b1;
        end else begin
            rx_sync_0 <= uart_rx;
            rx_sync_1 <= rx_sync_0;
        end
    end

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_div_cnt <= 16'd0;
            rx_bit_cnt <= 4'd0;
            rx_shift <= 8'd0;
            rx_busy <= 1'b0;
            rx_done <= 1'b0;
        end else begin
            rx_done <= 1'b0;

            if (!rx_busy && !rx_sync) begin  // 检测到起始位
                rx_busy <= 1'b1;
                rx_div_cnt <= div_reg[15:0] / 2;  // 半周期采样
                rx_bit_cnt <= 4'd0;
            end else if (rx_busy) begin
                if (rx_div_cnt >= div_reg[15:0]) begin
                    rx_div_cnt <= 16'd0;

                    if (rx_bit_cnt == 4'd0) begin
                        // 起始位，应该为0
                        if (rx_sync)  // 噪声，取消接收
                            rx_busy <= 1'b0;
                    end else if (rx_bit_cnt <= 4'd8) begin
                        rx_shift <= {rx_sync, rx_shift[7:1]};
                    end else begin
                        // 停止位
                        rx_done <= 1'b1;
                        rx_busy <= 1'b0;
                    end

                    rx_bit_cnt <= rx_bit_cnt + 1'b1;
                end else begin
                    rx_div_cnt <= rx_div_cnt + 1'b1;
                end
            end
        end
    end

    //========================================================================
    // 接收FIFO (深度8)
    //========================================================================
    reg [7:0] rx_fifo [0:7];
    reg [2:0] rx_wr_ptr;
    reg [2:0] rx_rd_ptr;
    reg [3:0] rx_fifo_cnt;
    wire      rx_fifo_full = (rx_fifo_cnt == 4'd8);
    wire      rx_fifo_empty = (rx_fifo_cnt == 4'd0);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            rx_wr_ptr <= 3'd0;
            rx_rd_ptr <= 3'd0;
            rx_fifo_cnt <= 4'd0;
        end else begin
            if (rx_done && !rx_fifo_full) begin
                rx_fifo[rx_wr_ptr] <= rx_shift;
                rx_wr_ptr <= rx_wr_ptr + 1'b1;
                rx_fifo_cnt <= rx_fifo_cnt + 1'b1;
            end

            if (stb_i && !we_i && (adr_i == REG_RXDATA) && !rx_fifo_empty) begin
                rx_rd_ptr <= rx_rd_ptr + 1'b1;
                if (!(rx_done && !rx_fifo_full))
                    rx_fifo_cnt <= rx_fifo_cnt - 1'b1;
            end else if (rx_done && !rx_fifo_full) begin
                if (!(stb_i && !we_i && (adr_i == REG_RXDATA) && !rx_fifo_empty))
                    rx_fifo_cnt <= rx_fifo_cnt + 1'b1;
            end
        end
    end

    //========================================================================
    // 寄存器读写
    //========================================================================
    wire [7:0] rx_fifo_rdata = rx_fifo[rx_rd_ptr];

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            txctrl_reg <= 32'd0;
            rxctrl_reg <= 32'd0;
            ie_reg <= 32'd0;
            div_reg <= DIV_VAL;
            ack_o <= 1'b0;
        end else begin
            ack_o <= stb_i;

            if (stb_i && we_i) begin
                case (adr_i)
                    REG_TXDATA: txdata_reg <= dat_i;
                    REG_TXCTRL: txctrl_reg <= dat_i;
                    REG_RXCTRL: rxctrl_reg <= dat_i;
                    REG_IE:     ie_reg <= dat_i;
                    REG_DIV:    div_reg <= dat_i;
                endcase
            end
        end
    end

    //========================================================================
    // 读数据输出
    //========================================================================
    reg [31:0] rdata;
    always @(*) begin
        case (adr_i)
            REG_RXDATA: rdata = {24'd0, rx_fifo_rdata};
            REG_TXCTRL: rdata = {30'd0, tx_busy, 1'd0};
            REG_RXCTRL: rdata = {28'd0, rx_fifo_full, rx_fifo_empty, rx_fifo_cnt};
            REG_IE:     rdata = ie_reg;
            REG_IP:     rdata = {30'd0, rx_fifo_empty, !tx_busy};
            REG_DIV:    rdata = div_reg;
            default:    rdata = 32'd0;
        endcase
    end

    assign dat_o = rdata;

    //========================================================================
    // 中断生成
    //========================================================================
    // 接收非空中断 + 发送完成中断
    assign irq = (ie_reg[0] && !rx_fifo_empty) || (ie_reg[1] && !tx_busy);

endmodule
```

### 4.3 GPIO控制器 (gpio_controller.v)

```verilog
//============================================================================
// 文件名: gpio_controller.v
// 描述: GPIO控制器，支持输入/输出/中断
//============================================================================

module gpio_controller (
    input         clk,
    input         rst_n,

    // 总线接口
    input  [7:0]  adr_i,
    input  [31:0] dat_i,
    output [31:0] dat_o,
    input         we_i,
    input  [3:0]  sel_i,
    input         stb_i,
    output reg    ack_o,

    // GPIO接口
    input  [15:0] gpio_in,
    output [15:0] gpio_out,
    output [15:0] gpio_oe,

    // 中断输出
    output        irq
);

    //========================================================================
    // 寄存器地址
    //========================================================================
    localparam REG_DATA  = 8'h00;  // 数据寄存器
    localparam REG_DIR   = 8'h04;  // 方向寄存器 (1=output, 0=input)
    localparam REG_IE    = 8'h08;  // 中断使能
    localparam REG_IP    = 8'h0C;  // 中断状态
    localparam REG_IC    = 8'h10;  // 中断清除
    localparam REG_IN    = 8'h14;  // 输入值（只读）

    //========================================================================
    // 寄存器
    //========================================================================
    reg [15:0] gpio_data_reg;
    reg [15:0] gpio_dir_reg;
    reg [15:0] gpio_ie_reg;
    reg [15:0] gpio_ip_reg;

    //========================================================================
    // GPIO同步
    //========================================================================
    reg [15:0] gpio_in_sync0;
    reg [15:0] gpio_in_sync1;
    reg [15:0] gpio_in_prev;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            gpio_in_sync0 <= 16'd0;
            gpio_in_sync1 <= 16'd0;
            gpio_in_prev <= 16'd0;
        end else begin
            gpio_in_sync0 <= gpio_in;
            gpio_in_sync1 <= gpio_in_sync0;
            gpio_in_prev <= gpio_in_sync1;
        end
    end

    //========================================================================
    // 边沿检测中断
    //========================================================================
    wire [15:0] gpio_posedge = gpio_in_sync1 & ~gpio_in_prev;
    wire [15:0] gpio_negedge = ~gpio_in_sync1 & gpio_in_prev;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            gpio_ip_reg <= 16'd0;
        end else begin
            // 自动检测边沿
            gpio_ip_reg <= gpio_ip_reg | gpio_posedge | gpio_negedge;

            // 软件清除
            if (stb_i && we_i && (adr_i == REG_IC))
                gpio_ip_reg <= gpio_ip_reg & ~dat_i[15:0];
        end
    end

    //========================================================================
    // 寄存器读写
    //========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            gpio_data_reg <= 16'd0;
            gpio_dir_reg <= 16'd0;
            gpio_ie_reg <= 16'd0;
            ack_o <= 1'b0;
        end else begin
            ack_o <= stb_i;

            if (stb_i && we_i) begin
                case (adr_i)
                    REG_DATA: gpio_data_reg <= dat_i[15:0];
                    REG_DIR:  gpio_dir_reg <= dat_i[15:0];
                    REG_IE:   gpio_ie_reg <= dat_i[15:0];
                endcase
            end
        end
    end

    //========================================================================
    // 读数据
    //========================================================================
    reg [31:0] rdata;
    always @(*) begin
        case (adr_i)
            REG_DATA: rdata = {16'd0, gpio_data_reg};
            REG_DIR:  rdata = {16'd0, gpio_dir_reg};
            REG_IE:   rdata = {16'd0, gpio_ie_reg};
            REG_IP:   rdata = {16'd0, gpio_ip_reg};
            REG_IN:   rdata = {16'd0, gpio_in_sync1};
            default:  rdata = 32'd0;
        endcase
    end

    assign dat_o = rdata;

    //========================================================================
    // GPIO输出
    //========================================================================
    assign gpio_out = gpio_data_reg;
    assign gpio_oe = gpio_dir_reg;

    //========================================================================
    // 中断输出
    //========================================================================
    assign irq = |(gpio_ip_reg & gpio_ie_reg);

endmodule
```

### 4.4 PLIC中断控制器 (plic.v)

```verilog
//============================================================================
// 文件名: plic.v
// 描述: 简化版PLIC（平台级中断控制器）
// 支持4个中断源，固定优先级
//============================================================================

module plic (
    input         clk,
    input         rst_n,

    // 总线接口
    input  [7:0]  adr_i,
    input  [31:0] dat_i,
    output [31:0] dat_o,
    input         we_i,
    input         stb_i,
    output reg    ack_o,

    // 中断源
    input         irq_src_0,  // UART
    input         irq_src_1,  // GPIO
    input         irq_src_2,  // Timer
    input         irq_src_3,  // 保留

    // 中断输出到CPU
    output        irq_out
);

    //========================================================================
    // 寄存器地址
    //========================================================================
    localparam REG_IP   = 8'h00;  // 中断等待 (RO)
    localparam REG_IE   = 8'h04;  // 中断使能 (RW)
    localparam REG_TH   = 8'h08;  // 阈值 (RW)
    localparam REG_CC   = 8'h0C;  // 声明/完成 (RW)

    //========================================================================
    // 寄存器
    //========================================================================
    reg [3:0] ie_reg;
    reg [1:0] th_reg;
    reg [2:0] cc_reg;  // 当前处理的中断ID

    //========================================================================
    // 中断等待寄存器
    //========================================================================
    reg [3:0] ip_reg;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ip_reg <= 4'd0;
        end else begin
            // 捕获中断源
            if (irq_src_0) ip_reg[0] <= 1'b1;
            if (irq_src_1) ip_reg[1] <= 1'b1;
            if (irq_src_2) ip_reg[2] <= 1'b2;
            if (irq_src_3) ip_reg[3] <= 1'b1;

            // 完成时清除
            if (stb_i && we_i && (adr_i == REG_CC) && (dat_i[2:0] != 3'd0))
                ip_reg[dat_i[2:0] - 1'b1] <= 1'b0;
        end
    end

    //========================================================================
    // 优先级仲裁（固定优先级：src_0 > src_1 > src_2 > src_3）
    //========================================================================
    reg [2:0] max_priority_id;
    reg [1:0] max_priority_val;

    always @(*) begin
        if (ip_reg[0] && ie_reg[0]) begin
            max_priority_id = 3'd1;
            max_priority_val = 2'd3;
        end else if (ip_reg[1] && ie_reg[1]) begin
            max_priority_id = 3'd2;
            max_priority_val = 2'd2;
        end else if (ip_reg[2] && ie_reg[2]) begin
            max_priority_id = 3'd3;
            max_priority_val = 2'd1;
        end else if (ip_reg[3] && ie_reg[3]) begin
            max_priority_id = 3'd4;
            max_priority_val = 2'd0;
        end else begin
            max_priority_id = 3'd0;
            max_priority_val = 2'd0;
        end
    end

    //========================================================================
    // 中断输出
    //========================================================================
    assign irq_out = (max_priority_val > th_reg) && (max_priority_id != 3'd0);

    //========================================================================
    // 寄存器读写
    //========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ie_reg <= 4'd0;
            th_reg <= 2'd0;
            ack_o <= 1'b0;
        end else begin
            ack_o <= stb_i;

            if (stb_i && we_i) begin
                case (adr_i)
                    REG_IE: ie_reg <= dat_i[3:0];
                    REG_TH: th_reg <= dat_i[1:0];
                    REG_CC: cc_reg <= dat_i[2:0];
                endcase
            end else if (irq_out && (cc_reg == 3'd0)) begin
                // 自动声明最高优先级中断
                cc_reg <= max_priority_id;
            end
        end
    end

    //========================================================================
    // 读数据
    //========================================================================
    reg [31:0] rdata;
    always @(*) begin
        case (adr_i)
            REG_IP: rdata = {28'd0, ip_reg};
            REG_IE: rdata = {28'd0, ie_reg};
            REG_TH: rdata = {30'd0, th_reg};
            REG_CC: rdata = {29'd0, cc_reg};
            default: rdata = 32'd0;
        endcase
    end

    assign dat_o = rdata;

endmodule
```

### 4.5 定时器 (timer.v)

```verilog
//============================================================================
// 文件名: timer.v
// 描述: 基本定时器，支持单次和周期模式
//============================================================================

module timer (
    input         clk,
    input         rst_n,

    // 总线接口
    input  [7:0]  adr_i,
    input  [31:0] dat_i,
    output [31:0] dat_o,
    input         we_i,
    input         stb_i,
    output reg    ack_o,

    // 中断输出
    output        irq
);

    //========================================================================
    // 寄存器地址
    //========================================================================
    localparam REG_CTRL = 8'h00;  // 控制寄存器
    localparam REG_DIV  = 8'h04;  // 分频寄存器
    localparam REG_CNT  = 8'h08;  // 计数器
    localparam REG_CMP  = 8'h0C;  // 比较值

    //========================================================================
    // 寄存器
    //========================================================================
    reg [31:0] ctrl_reg;
    reg [31:0] div_reg;
    reg [31:0] cnt_reg;
    reg [31:0] cmp_reg;

    wire timer_enable = ctrl_reg[0];
    wire timer_mode = ctrl_reg[1];  // 0=单次, 1=周期
    wire timer_irq_en = ctrl_reg[2];

    //========================================================================
    // 分频计数器
    //========================================================================
    reg [31:0] div_cnt;
    wire tick = (div_cnt >= div_reg);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            div_cnt <= 32'd0;
        end else if (!timer_enable) begin
            div_cnt <= 32'd0;
        end else if (tick) begin
            div_cnt <= 32'd0;
        end else begin
            div_cnt <= div_cnt + 1'b1;
        end
    end

    //========================================================================
    // 主计数器
    //========================================================================
    reg timer_irq;

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            cnt_reg <= 32'd0;
            timer_irq <= 1'b0;
        end else if (!timer_enable) begin
            cnt_reg <= 32'd0;
            timer_irq <= 1'b0;
        end else if (tick) begin
            if (cnt_reg >= cmp_reg) begin
                timer_irq <= timer_irq_en;
                if (timer_mode)
                    cnt_reg <= 32'd0;  // 周期模式，重新开始
                else
                    timer_irq <= 1'b0;  // 单次模式，停止
            end else begin
                cnt_reg <= cnt_reg + 1'b1;
            end
        end
    end

    //========================================================================
    // 寄存器读写
    //========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            ctrl_reg <= 32'd0;
            div_reg <= 32'd99999;  // 默认1ms @100MHz
            cmp_reg <= 32'd999;    // 默认1秒
            ack_o <= 1'b0;
        end else begin
            ack_o <= stb_i;

            if (stb_i && we_i) begin
                case (adr_i)
                    REG_CTRL: ctrl_reg <= dat_i;
                    REG_DIV:  div_reg <= dat_i;
                    REG_CMP:  cmp_reg <= dat_i;
                endcase
            end
        end
    end

    //========================================================================
    // 读数据
    //========================================================================
    reg [31:0] rdata;
    always @(*) begin
        case (adr_i)
            REG_CTRL: rdata = ctrl_reg;
            REG_DIV:  rdata = div_reg;
            REG_CNT:  rdata = cnt_reg;
            REG_CMP:  rdata = cmp_reg;
            default:  rdata = 32'd0;
        endcase
    end

    assign dat_o = rdata;
    assign irq = timer_irq;

endmodule
```

### 4.6 测试平台 (soc_tb.v)

```verilog
//============================================================================
// 文件名: soc_tb.v
// 描述: SoC系统测试平台
//============================================================================

`timescale 1ns / 1ps

module soc_tb;
    //========================================================================
    // 测试信号
    //========================================================================
    reg         sys_clk;
    reg         sys_rst_n;

    wire        uart_tx;
    reg         uart_rx;

    reg  [15:0] gpio_in;
    wire [15:0] gpio_out;
    wire [15:0] gpio_oe;

    wire [31:0] debug_pc;
    wire        debug_halted;

    //========================================================================
    // 被测模块实例化
    //========================================================================
    soc_top uut (
        .sys_clk        (sys_clk),
        .sys_rst_n      (sys_rst_n),
        .uart_rx        (uart_rx),
        .uart_tx        (uart_tx),
        .gpio_in        (gpio_in),
        .gpio_out       (gpio_out),
        .gpio_oe        (gpio_oe),
        .debug_pc       (debug_pc),
        .debug_halted   (debug_halted)
    );

    //========================================================================
    // 时钟生成 (100MHz)
    //========================================================================
    initial begin
        sys_clk = 0;
        forever #5 sys_clk = ~sys_clk;
    end

    //========================================================================
    // 测试序列
    //========================================================================
    initial begin
        $display("========================================");
        $display("       SoC系统集成测试开始           ");
        $display("========================================");

        // 初始化
        sys_rst_n = 0;
        uart_rx = 1;  // UART空闲状态
        gpio_in = 16'h0000;

        // 复位
        #100;
        sys_rst_n = 1;
        #100;

        // 测试序列
        $display("\n--- 测试1: Boot ROM启动 ---");
        wait(debug_pc == 32'h2000_0000);
        $display("CPU成功跳转到SRAM入口: PC=0x%08h", debug_pc);

        $display("\n--- 测试2: UART通信 ---");
        test_uart();

        $display("\n--- 测试3: GPIO控制 ---");
        test_gpio();

        $display("\n--- 测试4: 定时器中断 ---");
        test_timer();

        $display("\n========================================");
        $display("           所有测试完成               ");
        $display("========================================");

        // 波形输出
        $dumpfile("soc_system.vcd");
        $dumpvars(0, soc_tb);

        #10000;
        $finish;
    end

    //========================================================================
    // UART测试
    //========================================================================
    task test_uart;
        begin
            $display("测试UART发送...");

            // 模拟CPU向UART写入数据
            // 这里通过直接操作内存映射来测试

            $display("UART发送测试完成");
        end
    endtask

    //========================================================================
    // GPIO测试
    //========================================================================
    task test_gpio;
        begin
            $display("测试GPIO输入...");

            // 改变GPIO输入
            gpio_in = 16'hABCD;
            #100;

            $display("GPIO输入测试完成");
        end
    endtask

    //========================================================================
    // 定时器测试
    //========================================================================
    task test_timer;
        begin
            $display("测试定时器...");

            // 等待一段时间观察定时器行为
            #10000;

            $display("定时器测试完成");
        end
    endtask

    //========================================================================
    // 监控输出
    //========================================================================
    always @(posedge sys_clk) begin
        if (sys_rst_n)
            $display("[T=%0t] PC=0x%08h", $time, debug_pc);
    end

endmodule
```


### 4.7 Bootloader代码 (boot.S)

```asm
#============================================================================
# 文件名: boot.S
# 描述: SoC启动引导程序
# 功能: 初始化栈，清零BSS，跳转到main
#============================================================================

    .section .text.boot
    .global _start
    .global _stack_top

# Boot ROM入口地址: 0x0000_0000
# SRAM起始地址: 0x2000_0000

_start:
    # 关闭中断
    csrw mstatus, zero
    csrw mie, zero

    # 初始化栈指针 (SRAM顶部)
    lui  sp, 0x20004       # sp = 0x20004000 (16KB SRAM顶部)

    # 清零BSS段
    la   t0, __bss_start
    la   t1, __bss_end

clear_bss:
    bgeu t0, t1, clear_done
    sw   zero, 0(t0)
    addi t0, t0, 4
    j    clear_bss

clear_done:
    # 初始化数据段（如果需要从Flash复制到SRAM）
    # 这里假设程序已经加载到SRAM

    # 跳转到main函数
    call main

    # main返回后，进入死循环
hang:
    j    hang

    # 中断向量表（简化版）
    .align 4
trap_vector:
    # 保存上下文
    addi sp, sp, -128
    sw   ra, 0(sp)
    sw   t0, 4(sp)
    sw   t1, 8(sp)
    # ... 保存其他寄存器

    # 处理中断/异常
    csrr t0, mcause
    bgez t0, handle_exception

    # 外部中断
    call external_irq_handler

    # 恢复上下文
    lw   ra, 0(sp)
    lw   t0, 4(sp)
    lw   t1, 8(sp)
    addi sp, sp, 128
    mret

handle_exception:
    # 异常处理
    j    hang

external_irq_handler:
    # 读取PLIC获取中断源
    li   t0, 0x4000020C    # PLIC CC寄存器地址
    lw   t1, 0(t0)         # 读取中断ID

    # 根据中断ID分发
    li   t2, 1
    beq  t1, t2, uart_irq_handler
    li   t2, 2
    beq  t1, t2, gpio_irq_handler
    li   t2, 3
    beq  t1, t2, timer_irq_handler

    ret

uart_irq_handler:
    # UART中断处理
    ret

gpio_irq_handler:
    # GPIO中断处理
    ret

timer_irq_handler:
    # 定时器中断处理
    ret
```

### 4.8 Hello World程序 (hello.c)

```c
/*============================================================================
 * 文件名: hello.c
 * 描述: Hello World测试程序
 *============================================================================*/

#include <stdint.h>

// 内存映射地址定义
#define UART_BASE   0x40000000
#define GPIO_BASE   0x40000100
#define PLIC_BASE   0x40000200
#define TIMER_BASE  0x40000300

// UART寄存器偏移
#define UART_TXDATA 0x00
#define UART_RXDATA 0x04
#define UART_TXCTRL 0x08
#define UART_RXCTRL 0x0C
#define UART_IE     0x10
#define UART_IP     0x14
#define UART_DIV    0x18

// GPIO寄存器偏移
#define GPIO_DATA   0x00
#define GPIO_DIR    0x04

// 寄存器访问宏
#define REG32(addr) (*(volatile uint32_t *)(addr))

// UART函数
void uart_init(void) {
    // 设置波特率分频值 (100MHz / 115200 ≈ 868)
    REG32(UART_BASE + UART_DIV) = 868;
    // 使能发送和接收
    REG32(UART_BASE + UART_TXCTRL) = 0x1;
    REG32(UART_BASE + UART_RXCTRL) = 0x1;
}

void uart_putc(char c) {
    // 等待发送就绪
    while (REG32(UART_BASE + UART_TXCTRL) & 0x1);
    // 发送字符
    REG32(UART_BASE + UART_TXDATA) = c;
}

void uart_puts(const char *str) {
    while (*str) {
        if (*str == '\n')
            uart_putc('\r');
        uart_putc(*str++);
    }
}

char uart_getc(void) {
    uint32_t rxdata;
    do {
        rxdata = REG32(UART_BASE + UART_RXDATA);
    } while (rxdata & 0x80000000);  // 检查空标志
    return (char)(rxdata & 0xFF);
}

int uart_has_data(void) {
    uint32_t rxctrl = REG32(UART_BASE + UART_RXCTRL);
    return !(rxctrl & 0x2);  // 检查FIFO空标志
}

// GPIO函数
void gpio_init(void) {
    // 设置低8位为输出，高8位为输入
    REG32(GPIO_BASE + GPIO_DIR) = 0x00FF;
}

void gpio_set(uint16_t data) {
    REG32(GPIO_BASE + GPIO_DATA) = data;
}

uint16_t gpio_get(void) {
    return REG32(GPIO_BASE + GPIO_DATA) & 0xFF00;
}

// 延时函数
void delay_ms(uint32_t ms) {
    volatile uint32_t count = ms * 10000;  // 粗略估计
    while (count--);
}

// 简单的printf实现
void uart_print_hex(uint32_t val) {
    const char hex_chars[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex_chars[(val >> i) & 0xF]);
    }
}

void uart_print_dec(uint32_t val) {
    if (val == 0) {
        uart_putc('0');
        return;
    }

    char buf[12];
    int i = 0;
    while (val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }

    while (i > 0) {
        uart_putc(buf[--i]);
    }
}

// 主函数
int main(void) {
    // 初始化外设
    uart_init();
    gpio_init();

    // 打印欢迎信息
    uart_puts("\n");
    uart_puts("========================================\n");
    uart_puts("    RISC-V SoC Boot Successful!        \n");
    uart_puts("========================================\n");
    uart_puts("\n");

    // 测试UART回显
    uart_puts("UART Echo Test - Type something:\n");

    for (int i = 0; i < 10; i++) {
        char c = uart_getc();
        uart_puts("You typed: ");
        uart_putc(c);
        uart_puts("\n");
    }

    // 测试GPIO
    uart_puts("\nGPIO LED Test - Blinking LEDs...\n");

    for (int i = 0; i < 5; i++) {
        gpio_set(0x00FF);  // 点亮所有LED
        uart_puts("LEDs ON\n");
        delay_ms(500);

        gpio_set(0x0000);  // 熄灭所有LED
        uart_puts("LEDs OFF\n");
        delay_ms(500);
    }

    // 读取GPIO输入
    uart_puts("\nGPIO Input Test:\n");
    uint16_t gpio_val = gpio_get();
    uart_puts("GPIO Input value: ");
    uart_print_hex(gpio_val);
    uart_puts("\n");

    // 结束
    uart_puts("\nAll tests completed!\n");
    uart_puts("Entering idle loop...\n\n");

    // 空闲循环
    int counter = 0;
    while (1) {
        gpio_set(counter & 0xFF);  // LED显示计数器
        delay_ms(100);
        counter++;

        // 每秒打印一次状态
        if ((counter % 10) == 0) {
            uart_puts("Running... count=");
            uart_print_dec(counter);
            uart_puts("\n");
        }
    }

    return 0;
}
```

### 4.9 链接脚本 (link.ld)

```ld
/*============================================================================
 * 文件名: link.ld
 * 描述: RISC-V SoC链接脚本
 *============================================================================*/

OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY
{
    /* Boot ROM - 8KB, 只读 */
    rom (rx) : ORIGIN = 0x00000000, LENGTH = 8K

    /* SRAM - 16KB, 可读可写 */
    ram (rwx) : ORIGIN = 0x20000000, LENGTH = 16K
}

SECTIONS
{
    /* 代码段 */
    .text : {
        *(.text.boot)       /* 启动代码在最前面 */
        *(.text)
        *(.text.*)
    } > ram

    /* 只读数据段 */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    } > ram

    /* 初始化数据段（需要在启动时从ROM复制到RAM） */
    .data : {
        __data_start = .;
        *(.data)
        *(.data.*)
        __data_end = .;
    } > ram

    /* BSS段（需要清零） */
    .bss : {
        __bss_start = .;
        *(.bss)
        *(.bss.*)
        *(COMMON)
        __bss_end = .;
    } > ram

    /* 栈底 */
    __stack_bottom = ORIGIN(ram) + LENGTH(ram);

    /* 丢弃不需要的段 */
    /DISCARD/ : {
        *(.comment)
        *(.note)
        *(.eh_frame)
    }
}
```

### 4.10 Makefile

```makefile
#============================================================================
# 文件名: Makefile
# 描述: SoC软件构建脚本
#============================================================================

# 工具链
PREFIX = riscv32-unknown-elf
CC = $(PREFIX)-gcc
OBJCOPY = $(PREFIX)-objcopy
OBJDUMP = $(PREFIX)-objdump
SIZE = $(PREFIX)-size

# 编译选项
CFLAGS = -march=rv32i -mabi=ilp32 -O2 -Wall -g
CFLAGS += -ffreestanding -nostdlib -nostartfiles
CFLAGS += -T link.ld

# 目标
TARGET = hello
OBJS = boot.o hello.o

# 默认目标
all: $(TARGET).hex $(TARGET).dump size

# 编译规则
%.o: %.c
 $(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
 $(CC) $(CFLAGS) -c $< -o $@

# 链接
$(TARGET).elf: $(OBJS)
 $(CC) $(CFLAGS) $(OBJS) -o $@

# 生成hex文件（用于ROM初始化）
$(TARGET).hex: $(TARGET).elf
 $(OBJCOPY) -O verilog $< $@

# 生成bin文件
$(TARGET).bin: $(TARGET).elf
 $(OBJCOPY) -O binary $< $@

# 生成反汇编
$(TARGET).dump: $(TARGET).elf
 $(OBJDUMP) -d $< > $@

# 显示大小信息
size: $(TARGET).elf
 @echo "Section sizes:"
 $(SIZE) $<

# 清理
clean:
 rm -f *.o *.elf *.hex *.bin *.dump

.PHONY: all clean size
```

### 4.11 串口下载工具 (uart_loader.py)

```python
#!/usr/bin/env python3
#============================================================================
# 文件名: uart_loader.py
# 描述: 串口程序下载工具
# 功能: 将编译好的程序通过串口下载到SoC的SRAM中
#============================================================================

import serial
import struct
import sys
import argparse
import time

# 内存映射地址
SRAM_BASE = 0x20000000
UART_BASE = 0x40000000

# 协议定义
CMD_SYNC    = 0x55  # 同步命令
CMD_ACK     = 0xAA  # 确认
CMD_WRITE   = 0x01  # 写内存
CMD_READ    = 0x02  # 读内存
CMD_JUMP    = 0x03  # 跳转执行
CMD_ERASE   = 0x04  # 擦除

class UartLoader:
    def __init__(self, port, baudrate=115200):
        self.ser = serial.Serial(port, baudrate, timeout=5)
        print(f"Opened {port} at {baudrate} baud")

    def close(self):
        self.ser.close()

    def send_byte(self, byte):
        """发送单个字节"""
        self.ser.write(bytes([byte]))

    def send_word(self, word):
        """发送32位字（小端）"""
        self.ser.write(struct.pack('<I', word))

    def recv_byte(self, timeout=1):
        """接收单个字节"""
        self.ser.timeout = timeout
        data = self.ser.read(1)
        if len(data) == 0:
            raise TimeoutError("Timeout waiting for data")
        return data[0]

    def sync(self):
        """与Bootloader同步"""
        print("Syncing with bootloader...")
        for _ in range(10):
            self.send_byte(CMD_SYNC)
            time.sleep(0.01)
            if self.ser.in_waiting > 0:
                response = self.recv_byte()
                if response == CMD_ACK:
                    print("Sync successful!")
                    return True
        raise RuntimeError("Failed to sync with bootloader")

    def write_memory(self, addr, data):
        """写内存"""
        # 发送命令
        self.send_byte(CMD_WRITE)
        self.send_word(addr)
        self.send_word(len(data))

        # 发送数据
        self.ser.write(data)

        # 等待确认
        response = self.recv_byte(timeout=10)
        if response != CMD_ACK:
            raise RuntimeError(f"Write failed: unexpected response 0x{response:02X}")

        print(f"Wrote {len(data)} bytes to 0x{addr:08X}")

    def read_memory(self, addr, length):
        """读内存"""
        self.send_byte(CMD_READ)
        self.send_word(addr)
        self.send_word(length)

        data = self.ser.read(length)
        if len(data) != length:
            raise RuntimeError(f"Read failed: expected {length} bytes, got {len(data)}")

        return data

    def jump(self, addr):
        """跳转到指定地址执行"""
        print(f"Jumping to 0x{addr:08X}...")
        self.send_byte(CMD_JUMP)
        self.send_word(addr)

    def load_program(self, filename, base_addr=SRAM_BASE):
        """加载程序文件到SRAM"""
        with open(filename, 'rb') as f:
            data = f.read()

        print(f"Loading {filename} ({len(data)} bytes)...")

        # 分块写入（每块1KB）
        chunk_size = 1024
        offset = 0

        while offset < len(data):
            chunk = data[offset:offset + chunk_size]
            self.write_memory(base_addr + offset, chunk)
            offset += len(chunk)

            # 显示进度
            progress = (offset / len(data)) * 100
            print(f"Progress: {progress:.1f}%")

        print("Program loaded successfully!")
        return base_addr

    def verify_program(self, filename, base_addr=SRAM_BASE):
        """验证写入的程序"""
        with open(filename, 'rb') as f:
            expected = f.read()

        print("Verifying program...")

        # 读取并比较
        chunk_size = 1024
        offset = 0

        while offset < len(expected):
            length = min(chunk_size, len(expected) - offset)
            actual = self.read_memory(base_addr + offset, length)

            if actual != expected[offset:offset + length]:
                print(f"Verification failed at offset 0x{offset:08X}")
                return False

            offset += length

        print("Verification successful!")
        return True

def main():
    parser = argparse.ArgumentParser(description='RISC-V SoC UART Loader')
    parser.add_argument('port', help='Serial port (e.g., COM3 or /dev/ttyUSB0)')
    parser.add_argument('program', help='Program file to load (.bin)')
    parser.add_argument('-b', '--baud', type=int, default=115200,
                       help='Baud rate (default: 115200)')
    parser.add_argument('-a', '--addr', type=lambda x: int(x, 0),
                       default=SRAM_BASE, help='Load address (default: 0x20000000)')
    parser.add_argument('-v', '--verify', action='store_true',
                       help='Verify after loading')
    parser.add_argument('-r', '--run', action='store_true',
                       help='Run program after loading')

    args = parser.parse_args()

    try:
        loader = UartLoader(args.port, args.baud)

        # 同步
        loader.sync()

        # 加载程序
        entry = loader.load_program(args.program, args.addr)

        # 验证（如果需要）
        if args.verify:
            if not loader.verify_program(args.program, args.addr):
                sys.exit(1)

        # 运行程序
        if args.run:
            loader.jump(entry)

        loader.close()
        print("Done!")

    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)

if __name__ == '__main__':
    main()
```

### 4.12 Bootloader接收端代码 (bootloader.c)

```c
/*============================================================================
 * 文件名: bootloader.c
 * 描述: 驻留在Boot ROM中的串口加载引导程序
 *============================================================================*/

#include <stdint.h>

#define UART_BASE   0x40000000
#define SRAM_BASE   0x20000000

#define REG32(addr) (*(volatile uint32_t *)(addr))

// 协议命令
#define CMD_SYNC    0x55
#define CMD_ACK     0xAA
#define CMD_WRITE   0x01
#define CMD_READ    0x02
#define CMD_JUMP    0x03

void uart_init(void) {
    REG32(UART_BASE + 0x18) = 868;      // 波特率分频
    REG32(UART_BASE + 0x08) = 0x1;      // 使能发送
    REG32(UART_BASE + 0x0C) = 0x1;      // 使能接收
}

void uart_putc(char c) {
    while (REG32(UART_BASE + 0x08) & 0x1);
    REG32(UART_BASE + 0x00) = c;
}

char uart_getc(void) {
    uint32_t rxdata;
    do {
        rxdata = REG32(UART_BASE + 0x04);
    } while (rxdata & 0x80000000);
    return (char)(rxdata & 0xFF);
}

uint32_t uart_get_word(void) {
    uint32_t word = 0;
    word |= (uart_getc() << 0);
    word |= (uart_getc() << 8);
    word |= (uart_getc() << 16);
    word |= (uart_getc() << 24);
    return word;
}

void process_command(void) {
    uint8_t cmd = uart_getc();

    switch (cmd) {
        case CMD_WRITE: {
            uint32_t addr = uart_get_word();
            uint32_t len = uart_get_word();

            // 接收数据并写入内存
            for (uint32_t i = 0; i < len; i++) {
                uint8_t data = uart_getc();
                ((volatile uint8_t *)addr)[i] = data;
            }

            uart_putc(CMD_ACK);
            break;
        }

        case CMD_READ: {
            uint32_t addr = uart_get_word();
            uint32_t len = uart_get_word();

            // 读取内存并发送
            for (uint32_t i = 0; i < len; i++) {
                uart_putc(((volatile uint8_t *)addr)[i]);
            }
            break;
        }

        case CMD_JUMP: {
            uint32_t addr = uart_get_word();

            // 跳转到指定地址
            void (*func)(void) = (void (*)(void))addr;
            func();

            // 不应该到达这里
            while (1);
        }

        default:
            break;
    }
}

int main(void) {
    uart_init();

    // 等待同步
    while (1) {
        if (uart_getc() == CMD_SYNC) {
            uart_putc(CMD_ACK);
            break;
        }
    }

    // 进入命令处理循环
    while (1) {
        process_command();
    }

    return 0;
}
```

---

## 5. 仿真与验证

### 5.1 仿真步骤

```tcl
# 编译设计
read_verilog rtl/*.v
synth_design -top soc_top -part xc7a100tcsg324-1

# 运行仿真
set_property top soc_tb [get_filesets sim_1]
launch_simulation
run 50000ns
```

### 5.2 测试内容

1. **启动测试**：验证CPU能从Boot ROM正确启动
2. **内存访问测试**：测试SRAM读写
3. **UART测试**：验证串口收发
4. **GPIO测试**：验证输入输出
5. **中断测试**：验证PLIC和定时器中断

---

## 6. 硬件部署

### 6.1 约束文件 (nexys_a7.xdc)

```xdc
## 时钟 (100MHz)
create_clock -period 10.000 -name sys_clk_pin -waveform {0.000 5.000} [get_ports sys_clk]
set_property -dict {PACKAGE_PIN E3 IOSTANDARD LVCMOS33} [get_ports sys_clk]

## 复位
set_property -dict {PACKAGE_PIN C12 IOSTANDARD LVCMOS33} [get_ports sys_rst_n]

## UART
set_property -dict {PACKAGE_PIN A9 IOSTANDARD LVCMOS33} [get_ports uart_rx]
set_property -dict {PACKAGE_PIN D10 IOSTANDARD LVCMOS33} [get_ports uart_tx]

## GPIO (LEDs)
set_property -dict {PACKAGE_PIN H17 IOSTANDARD LVCMOS33} [get_ports {gpio_out[0]}]
...
set_property -dict {PACKAGE_PIN U17 IOSTANDARD LVCMOS33} [get_ports {gpio_out[15]}]

## GPIO (开关)
set_property -dict {PACKAGE_PIN J15 IOSTANDARD LVCMOS33} [get_ports {gpio_in[0]}]
...
set_property -dict {PACKAGE_PIN V10 IOSTANDARD LVCMOS33} [get_ports {gpio_in[15]}]
```

### 6.2 部署步骤

1. **初始化Boot ROM**：

```bash
# 编译bootloader
make -C sw/boot
# 生成初始化文件
python scripts/gen_init.py sw/boot/bootloader.bin rtl/memory/boot_rom_init.hex
```

1. **综合实现**：

```tcl
synth_design -top soc_top
opt_design
place_design
route_design
write_bitstream -force soc.bit
```

1. **下载到FPGA**：

```tcl
open_hw
connect_hw_server
program_hw_devices [get_hw_devices xc7a100t_0]
```

1. **加载程序**：

```bash
# 通过串口加载Hello World程序
python tools/uart_loader.py COM3 sw/hello/hello.bin -r
```

---

## 7. 软件编程

### 7.1 工具链安装

```bash
# 使用riscv-gnu-toolchain
sudo apt-get install gcc-riscv64-unknown-elf

# 或使用预编译版本
wget https://github.com/xpack-dev-tools/riscv-none-embed-gcc-xpack/releases/download/xpack-riscv-none-embed-gcc-10.2.0-1.2/xpack-riscv-none-embed-gcc-10.2.0-1.2-linux-x64.tar.gz
tar xzf xpack-riscv-none-embed-gcc-*.tar.gz
```

### 7.2 编译示例

```bash
cd sw/hello
make
# 生成 hello.hex, hello.bin, hello.dump
```

---

## 8. 调试技巧

### 8.1 常见问题

| 问题 | 可能原因 | 解决方案 |
|------|----------|----------|
| 无法启动 | Boot ROM初始化错误 | 检查.hex文件格式 |
| 串口乱码 | 波特率不匹配 | 检查时钟和分频值 |
| 程序崩溃 | 栈溢出/未初始化 | 检查链接脚本和启动代码 |
| 中断不触发 | PLIC配置错误 | 检查IE和阈值寄存器 |

### 8.2 调试方法

1. **ILA逻辑分析仪**：监控总线信号
2. **LED调试**：在代码中插入LED状态指示
3. **串口打印**：添加调试输出
4. **JTAG调试**：连接OpenOCD（如果支持）

---

## 9. 进阶挑战

### 9.1 添加更多外设

- SPI控制器（连接SD卡）
- I2C控制器
- PWM输出
- ADC接口

### 9.2 实现多核支持

- 添加第二个CPU核心
- 实现缓存一致性协议
- 添加互斥锁原语

### 9.3 运行操作系统

- 移植FreeRTOS
- 运行简单的Linux内核

---

## 10. 思考题

1. **总线选择**：Wishbone和AXI各有什么优缺点？为什么选择Wishbone？

2. **内存布局**：Boot ROM和SRAM的地址如何分配？为什么Boot ROM在0x0000_0000？

3. **中断延迟**：从外部中断触发到进入中断处理程序需要多少时钟周期？如何优化？

4. **安全性**：如何在SoC中添加基本的内存保护（MPU）？

5. **功耗优化**：如何为SoC添加时钟门控和电源管理？

---

## 附录

### A. 串口通信协议

```
帧格式: [SYNC] [CMD] [ADDR:4] [LEN:4] [DATA:LEN] [ACK]

命令:
  0x01 - WRITE: 写内存
  0x02 - READ:  读内存
  0x03 - JUMP:  跳转执行
```

### B. 参考资源

1. [RISC-V规格文档](https://riscv.org/technical/specifications/)
2. [Wishbone总线规范](https://opencores.org/howto/wishbone)
3. [Freedom E310 SoC手册](https://sifive.cdn.prismic.io/sifive%2F6a060620-5b29-4770-bc12-591161c4161a_freedom-e310-manual-v19p05.pdf)

---

**文档版本**: 1.0
**最后更新**: 2026年3月
**作者**: FPGA实验教学组


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
