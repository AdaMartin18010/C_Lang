# 实验三：单周期CPU实验

## 目录

- [实验三：单周期CPU实验](#实验三单周期cpu实验)
  - [目录](#目录)
  - [1. 实验概述](#1-实验概述)
    - [1.1 CPU设计层级](#11-cpu设计层级)
    - [1.2 实验难度](#12-实验难度)
  - [2. 实验目标](#2-实验目标)
    - [2.1 知识目标](#21-知识目标)
    - [2.2 技能目标](#22-技能目标)
    - [2.3 实验成果](#23-实验成果)
  - [3. RISC-V架构基础](#3-risc-v架构基础)
    - [3.1 RV32I指令格式](#31-rv32i指令格式)
    - [3.2 支持的指令列表](#32-支持的指令列表)
    - [3.3 寄存器文件](#33-寄存器文件)
  - [4. 单周期CPU数据通路](#4-单周期cpu数据通路)
    - [4.1 数据通路框图](#41-数据通路框图)
    - [4.2 控制信号真值表](#42-控制信号真值表)
    - [4.3 ALU控制信号](#43-alu控制信号)
  - [5. 实验环境](#5-实验环境)
    - [5.1 硬件资源分配](#51-硬件资源分配)
    - [5.2 存储器配置](#52-存储器配置)
  - [6. 模块设计详解](#6-模块设计详解)
    - [6.1 程序计数器（PC）模块](#61-程序计数器pc模块)
    - [6.2 寄存器文件模块](#62-寄存器文件模块)
    - [6.3 立即数扩展模块](#63-立即数扩展模块)
    - [6.4 ALU控制单元](#64-alu控制单元)
  - [7. 完整Verilog代码](#7-完整verilog代码)
    - [7.1 ALU模块](#71-alu模块)
    - [7.2 控制单元模块](#72-控制单元模块)
    - [7.3 指令存储器模块](#73-指令存储器模块)
    - [7.4 数据存储器模块](#74-数据存储器模块)
    - [7.5 CPU顶层模块](#75-cpu顶层模块)
  - [8. 存储器初始化](#8-存储器初始化)
    - [8.1 指令存储器初始化文件](#81-指令存储器初始化文件)
    - [8.2 汇编程序示例](#82-汇编程序示例)
  - [9. XDC约束文件](#9-xdc约束文件)
  - [10. 仿真与调试](#10-仿真与调试)
    - [10.1 CPU Testbench](#101-cpu-testbench)
    - [10.2 调试技巧](#102-调试技巧)
  - [11. 上板验证](#11-上板验证)
    - [11.1 验证步骤](#111-验证步骤)
    - [11.2 预期结果](#112-预期结果)
  - [12. 故障排除FAQ](#12-故障排除faq)
    - [Q1: 指令不执行或PC不增加](#q1-指令不执行或pc不增加)
    - [Q2: 运算结果错误](#q2-运算结果错误)
    - [Q3: 分支跳转异常](#q3-分支跳转异常)
  - [13. 实验报告模板](#13-实验报告模板)
  - [14. 扩展练习](#14-扩展练习)
    - [14.1 多周期CPU](#141-多周期cpu)
    - [14.2 流水线CPU](#142-流水线cpu)
    - [14.3 中断和异常](#143-中断和异常)
  - [15. 参考资料](#15-参考资料)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. 实验概述

本实验是FPGA系列实验的综合性设计，目标是实现一个基于RISC-V指令集架构（RV32I）的单周期处理器。学习者将从零开始构建一个能够执行真实程序的完整CPU，包括数据通路、控制单元、指令存储器和数据存储器。

### 1.1 CPU设计层级

```
┌─────────────────────────────────────────────────────────────┐
│                    计算机系统层次                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  第5层: 应用软件      C/C++, Python, Java...                │
│       ↓                                                     │
│  第4层: 操作系统      Linux, RTOS...                        │
│       ↓                                                     │
│  第3层: 指令集架构    RISC-V, ARM, x86...  ←── 本实验关注点   │
│       ↓                                                     │
│  第2层: 微架构        单周期/多周期/流水线                    │
│       ↓                                                     │
│  第1层: 数字逻辑      ALU, 寄存器, 存储器                     │
│       ↓                                                     │
│  第0层: 物理器件      FPGA, 标准单元                          │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 实验难度

本实验是三个实验中最复杂的一个，涉及：

- 理解RISC-V指令集架构
- 设计完整的数据通路
- 实现复杂的控制单元
- 处理存储器访问时序
- 编写和调试机器码程序

---

## 2. 实验目标

### 2.1 知识目标

1. **RISC-V架构理解**：掌握RV32I整数指令集的编码格式
2. **数据通路设计**：理解指令执行的数据流动路径
3. **控制单元设计**：掌握ALU控制、PC控制、存储器控制信号生成
4. **处理器设计流程**：从指令集到RTL实现的完整流程

### 2.2 技能目标

1. **复杂系统设计**：能够分解CPU为可管理的模块
2. **时序分析**：理解单周期处理器的时序约束
3. **调试技能**：使用仿真和硬件调试解决复杂问题
4. **汇编编程**：编写简单的RISC-V汇编程序

### 2.3 实验成果

实现一个可运行的RV32I单周期处理器，能够执行：

- 所有RV32I整数运算指令（ADD, SUB, AND, OR, XOR等）
- 加载存储指令（LW, SW）
- 分支跳转指令（BEQ, BNE, JAL, JALR）
- 立即数指令（ADDI, ANDI等）
- 斐波那契数列计算程序

---

## 3. RISC-V架构基础

### 3.1 RV32I指令格式

```
RISC-V RV32I 有6种基本指令格式：

┌─────────────────────────────────────────────────────────────┐
│ R-Type: 寄存器-寄存器操作                                     │
├─────────────────────────────────────────────────────────────┤
│  31:25  │ 24:20 │ 19:15 │ 14:12 │ 11:7  │ 6:0             │
│  funct7 │  rs2  │  rs1  │ funct3│  rd   │ opcode          │
│   7位   │  5位  │  5位  │  3位  │  5位  │  7位            │
│         │       │       │       │       │                 │
│ 示例: ADD x1, x2, x3  // x1 = x2 + x3                       │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ I-Type: 立即数操作                                           │
├─────────────────────────────────────────────────────────────┤
│  31:20          │ 19:15 │ 14:12 │ 11:7  │ 6:0             │
│  imm[11:0]      │  rs1  │ funct3│  rd   │ opcode          │
│     12位        │  5位  │  3位  │  5位  │  7位            │
│                 │       │       │       │                 │
│ 示例: ADDI x1, x2, 10  // x1 = x2 + 10                      │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ S-Type: 存储指令                                             │
├─────────────────────────────────────────────────────────────┤
│  31:25    │ 24:20 │ 19:15 │ 14:12 │ 11:7    │ 6:0         │
│ imm[11:5] │  rs2  │  rs1  │ funct3│imm[4:0] │ opcode      │
│   7位     │  5位  │  5位  │  3位  │  5位    │  7位        │
│           │       │       │       │         │             │
│ 示例: SW x2, 4(x3)  // Mem[x3+4] = x2                       │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ B-Type: 分支指令                                             │
├─────────────────────────────────────────────────────────────┤
│ 31│30:25 │ 24:20 │ 19:15 │ 14:12 │ 11:8  │7│ 6:0         │
│imm│imm   │  rs2  │  rs1  │ funct3│imm    │ │ opcode      │
│[12]      │       │       │       │       │ │             │
│   │[10:5]│       │       │       │[4:1]  │ │             │
│   │      │       │       │       │[11]   │ │             │
│   7位    │  5位  │  5位  │  3位  │  5位  │ │  7位        │
│           │       │       │       │         │             │
│ 示例: BEQ x1, x2, label  // if(x1==x2) PC+=offset           │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ U-Type: 高位立即数                                           │
├─────────────────────────────────────────────────────────────┤
│  31:12              │ 11:7  │ 6:0                           │
│  imm[31:12]         │  rd   │ opcode                        │
│     20位            │  5位  │  7位                          │
│                     │       │                               │
│ 示例: LUI x1, 0x12345  // x1 = 0x12345000                   │
└─────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────┐
│ J-Type: 跳转指令                                             │
├─────────────────────────────────────────────────────────────┤
│ 31│30:21   │ 20   │ 19:12   │ 11:7  │ 6:0                   │
│imm│imm     │imm   │imm      │  rd   │ opcode                │
│[20]│[10:1] │[11]  │[19:12]  │       │                       │
│   10位     │ 1位  │  8位    │  5位  │  7位                  │
│                     │       │       │                       │
│ 示例: JAL x1, label  // x1=PC+4, PC+=offset                 │
└─────────────────────────────────────────────────────────────┘
```

### 3.2 支持的指令列表

| 指令 | 类型 | 操作 | 说明 |
|------|------|------|------|
| ADD | R | rd = rs1 + rs2 | 加法 |
| SUB | R | rd = rs1 - rs2 | 减法 |
| AND | R | rd = rs1 & rs2 | 按位与 |
| OR | R | rd = rs1 \| rs2 | 按位或 |
| XOR | R | rd = rs1 ^ rs2 | 按位异或 |
| SLL | R | rd = rs1 << rs2 | 逻辑左移 |
| SRL | R | rd = rs1 >> rs2 | 逻辑右移 |
| SRA | R | rd = rs1 >>> rs2 | 算术右移 |
| ADDI | I | rd = rs1 + imm | 加立即数 |
| ANDI | I | rd = rs1 & imm | 与立即数 |
| ORI | I | rd = rs1 \| imm | 或立即数 |
| XORI | I | rd = rs1 ^ imm | 异或立即数 |
| LW | I | rd = Mem[rs1+imm] | 加载字 |
| SW | S | Mem[rs1+imm] = rs2 | 存储字 |
| BEQ | B | if(rs1==rs2) PC+=imm | 相等分支 |
| BNE | B | if(rs1!=rs2) PC+=imm | 不等分支 |
| JAL | J | rd=PC+4; PC+=imm | 跳转并链接 |
| JALR | I | rd=PC+4; PC=rs1+imm | 寄存器跳转 |
| LUI | U | rd = imm << 12 | 加载高位立即数 |

### 3.3 寄存器文件

```
RISC-V有32个32位通用寄存器（x0-x31）：

x0  (zero):  恒为0
x1  (ra):    返回地址
x2  (sp):    栈指针
x3  (gp):    全局指针
x4  (tp):    线程指针
x5-x7 (t0-t2): 临时寄存器
x8  (s0/fp): 保存寄存器/帧指针
x9  (s1):    保存寄存器
x10-x17 (a0-a7): 函数参数/返回值
x18-x27 (s2-s11): 保存寄存器
x28-x31 (t3-t6): 临时寄存器
```

---

## 4. 单周期CPU数据通路

### 4.1 数据通路框图

```
┌───────────────────────────────────────────────────────────────────────────┐
│                         单周期CPU数据通路                                  │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                           │
│     ┌─────────────────────────────────────────────────────────────┐       │
│     │                      指令存储器 (IMEM)                       │       │
│     │                    地址    数据                             │       │
│     │                     ↑        ↓                              │       │
│     └─────────────────────┼────────┼──────────────────────────────┘       │
│                           │        │                                      │
│                           │    ┌───┴───┐                                  │
│     ┌──────────┐          │    │ 指令   │  [31:0]                        │
│     │    PC    │          │    │寄存器  │                                │
│     │          │          │    └───┬───┘                                │
│     │   +4     │          │        │                                     │
│     │   ↑      │          │    ┌───┴───┐                                 │
│     │  PC      │          │    │控制单元│                                │
│     └────┬─────┘          │    │  CU   │                                │
│          │                │    └───┬───┘                                │
│          │                │        │                                     │
│          │     ┌──────────┴────────┴────────┐                          │
│          │     │         控制信号            │                          │
│          │     │  ALUOp, Branch, MemRead...  │                          │
│          │     └─────────────────────────────┘                          │
│          │                                                              │
│          │         ┌──────────────────────────────────────┐             │
│          │         │           寄存器文件                  │             │
│          │         │  ReadReg1 ──→ [    ] ──→ ReadData1   │             │
│          │         │  ReadReg2 ──→ [ 32x  ] ──→ ReadData2  │             │
│          │         │  WriteReg ←── [ 32位 ] ←── WriteData   │             │
│          │         │  RegWrite ──→ [    ]                  │             │
│          │         └──────────────────────────────────────┘             │
│          │                      ↑           │                           │
│          │                      │           │                           │
│          │                      │       ┌───┴───┐                       │
│          │                      │       │ ALU   │                       │
│          │                      │       │       │                       │
│          │                      │    ┌──┤ 运算  ├──┐                    │
│          │                      │    │  │ 单元  │  │                    │
│          │                      │    │  └───┬───┘  │                    │
│          │                      │    │      │      │                    │
│          │                      │    │   结果    Zero                   │
│          │                      │    │      │      │                    │
│          │                      │    │      ↓      │                    │
│          │                      │    │   ┌─────┐   │                    │
│          │                      │    └──→│ MUX │←──┘                    │
│          │                      │        └──┬──┘                        │
│          │                      │           │                           │
│          │     ┌────────────────┴───────────┴───────────┐               │
│          │     │              数据存储器 (DMEM)          │               │
│          │     │  地址 ←────── 写数据 ←────── MemWrite   │               │
│          │     │           ───→ 读数据 ───→ MemRead      │               │
│          │     └──────────────────────────────────────────┘               │
│          │                                                              │
│          └───────────────────┐    ┌─────────────────────────────────────┘
│                              │    │
│     ┌────────────────────────┴────┴────────────────┐
│     │          下一条PC选择逻辑                      │
│     │  PC+4 ──→ [ MUX ] ←── Branch & Zero          │
│     │           [     ] ←── JumpTarget             │
│     │              ↓                                 │
│     │            新PC                                │
│     └────────────────────────────────────────────────┘
│
└───────────────────────────────────────────────────────────────────────────┘
```

### 4.2 控制信号真值表

| 指令 | RegWrite | ALUSrc | MemRead | MemWrite | MemtoReg | Branch | ALUOp |
|------|----------|--------|---------|----------|----------|--------|-------|
| ADD | 1 | 0 | 0 | 0 | 0 | 0 | 10 |
| ADDI | 1 | 1 | 0 | 0 | 0 | 0 | 10 |
| LW | 1 | 1 | 1 | 0 | 1 | 0 | 00 |
| SW | 0 | 1 | 0 | 1 | X | 0 | 00 |
| BEQ | 0 | 0 | 0 | 0 | X | 1 | 01 |
| JAL | 1 | X | 0 | 0 | 2 | 0 | XX |

### 4.3 ALU控制信号

| ALUOp | funct3 | funct7 | ALU操作 | ALUControl |
|-------|--------|--------|---------|------------|
| 00 | X | X | 加法（地址计算） | 0000 |
| 01 | X | X | 减法（分支比较） | 0001 |
| 10 | 000 | 0000000 | 加法 | 0000 |
| 10 | 000 | 0100000 | 减法 | 0001 |
| 10 | 111 | 0000000 | 与 | 0010 |
| 10 | 110 | 0000000 | 或 | 0011 |
| 10 | 100 | 0000000 | 异或 | 0100 |

---

## 5. 实验环境

### 5.1 硬件资源分配

```
Basys3开发板资源使用：

输入：
- SW0-SW4:  选择要查看的寄存器编号
- SW15:     运行/暂停控制
- BTNC:     单步执行（按下执行一条指令）
- BTNU:     复位

输出：
- LED0-7:   显示当前PC低8位
- LED8-15:  显示选中寄存器的低8位
- 七段数码管: 显示选中寄存器的16进制值

调试接口：
- UART:     串口输出调试信息（可选）
```

### 5.2 存储器配置

```
存储器映射（简化版）：

┌─────────────────┐ 0x0000_0000
│   指令存储器     │  8KB (2K x 32bit)
│    (IMEM)       │
├─────────────────┤ 0x0000_2000
│   数据存储器     │  8KB (2K x 32bit)
│    (DMEM)       │
├─────────────────┤ 0x0000_4000
│   I/O设备区域    │  4KB
│  (LED, 数码管等) │
├─────────────────┤ 0x0000_5000
│     保留        │
└─────────────────┘ 0xFFFF_FFFF
```

---

## 6. 模块设计详解

### 6.1 程序计数器（PC）模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: pc_module
// 功能描述: 程序计数器，支持下一条PC选择和分支跳转
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module pc_module (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        pc_write,      // PC写使能
    input  wire [31:0] pc_next,       // 下一条PC值
    output reg  [31:0] pc_current     // 当前PC值
);

    always @(posedge clk or negedge rst_n) begin
        if (!rst_n)
            pc_current <= 32'h0000_0000;  // 复位到起始地址
        else if (pc_write)
            pc_current <= pc_next;
    end

endmodule
```

### 6.2 寄存器文件模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: register_file
// 功能描述: 32个32位通用寄存器，双读端口，单写端口
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module register_file (
    input  wire        clk,
    input  wire        rst_n,
    input  wire        reg_write,     // 写使能
    input  wire [4:0]  read_reg1,     // 读寄存器1地址
    input  wire [4:0]  read_reg2,     // 读寄存器2地址
    input  wire [4:0]  write_reg,     // 写寄存器地址
    input  wire [31:0] write_data,    // 写数据
    output wire [31:0] read_data1,    // 读数据1
    output wire [31:0] read_data2,    // 读数据2

    // 调试接口
    input  wire [4:0]  debug_reg_addr,
    output wire [31:0] debug_reg_data
);

    // 32个32位寄存器
    reg [31:0] registers [0:31];
    integer i;

    // 读操作（组合逻辑）
    // x0恒为0
    assign read_data1 = (read_reg1 == 5'd0) ? 32'd0 : registers[read_reg1];
    assign read_data2 = (read_reg2 == 5'd0) ? 32'd0 : registers[read_reg2];

    // 调试读
    assign debug_reg_data = (debug_reg_addr == 5'd0) ? 32'd0 : registers[debug_reg_addr];

    // 写操作（时序逻辑，上升沿）
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // 复位时清零所有寄存器
            for (i = 0; i < 32; i = i + 1)
                registers[i] <= 32'd0;
        end
        else if (reg_write && (write_reg != 5'd0)) begin
            // x0恒为0，不能写入
            registers[write_reg] <= write_data;
        end
    end

endmodule
```

### 6.3 立即数扩展模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: immediate_gen
// 功能描述: 根据指令类型生成32位立即数
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module immediate_gen (
    input  wire [31:0] instruction,
    output reg  [31:0] immediate
);

    // 提取指令字段
    wire [6:0]  opcode = instruction[6:0];
    wire [2:0]  funct3 = instruction[14:12];

    // 操作码定义
    localparam OPCODE_I_TYPE = 7'b0010011;  // I-type (ADDI, ANDI, etc.)
    localparam OPCODE_LOAD   = 7'b0000011;  // Load (LW)
    localparam OPCODE_STORE  = 7'b0100011;  // Store (SW)
    localparam OPCODE_BRANCH = 7'b1100011;  // Branch (BEQ, BNE)
    localparam OPCODE_JAL    = 7'b1101111;  // JAL
    localparam OPCODE_JALR   = 7'b1100111;  // JALR
    localparam OPCODE_LUI    = 7'b0110111;  // LUI
    localparam OPCODE_AUIPC  = 7'b0010111;  // AUIPC

    always @(*) begin
        case (opcode)
            // I-type (ADDI, ANDI, ORI, XORI, etc.) 和 Load (LW), JALR
            OPCODE_I_TYPE, OPCODE_LOAD, OPCODE_JALR: begin
                // 符号扩展12位立即数
                immediate = {{20{instruction[31]}}, instruction[31:20]};
            end

            // S-type (Store)
            OPCODE_STORE: begin
                // 组合两个字段，符号扩展
                immediate = {{20{instruction[31]}}, instruction[31:25], instruction[11:7]};
            end

            // B-type (Branch)
            OPCODE_BRANCH: begin
                // 组合字段，左移1位（半字对齐），符号扩展
                immediate = {{19{instruction[31]}}, instruction[31], instruction[7],
                             instruction[30:25], instruction[11:8], 1'b0};
            end

            // U-type (LUI, AUIPC)
            OPCODE_LUI, OPCODE_AUIPC: begin
                // 20位立即数左移12位
                immediate = {instruction[31:12], 12'b0};
            end

            // J-type (JAL)
            OPCODE_JAL: begin
                // 组合字段，左移1位，符号扩展
                immediate = {{11{instruction[31]}}, instruction[31], instruction[19:12],
                             instruction[20], instruction[30:21], 1'b0};
            end

            default: immediate = 32'd0;
        endcase
    end

endmodule
```

### 6.4 ALU控制单元

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: alu_control
// 功能描述: 根据ALUOp和指令funct字段生成ALU控制信号
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module alu_control (
    input  wire [1:0] alu_op,        // 来自主控制单元
    input  wire [2:0] funct3,        // 指令funct3字段
    input  wire       funct7_bit,    // funct7的最高位（bit30）
    output reg  [3:0] alu_control    // ALU控制信号
);

    // ALU操作编码
    localparam ALU_ADD  = 4'b0000;
    localparam ALU_SUB  = 4'b0001;
    localparam ALU_AND  = 4'b0010;
    localparam ALU_OR   = 4'b0011;
    localparam ALU_XOR  = 4'b0100;
    localparam ALU_SLT  = 4'b0101;  // 小于则置位
    localparam ALU_SLTU = 4'b0110;  // 无符号小于则置位
    localparam ALU_SLL  = 4'b0111;  // 逻辑左移
    localparam ALU_SRL  = 4'b1000;  // 逻辑右移
    localparam ALU_SRA  = 4'b1001;  // 算术右移

    always @(*) begin
        case (alu_op)
            // ALUOp = 00: 加载/存储指令，使用加法
            2'b00: alu_control = ALU_ADD;

            // ALUOp = 01: 分支指令，使用减法比较
            2'b01: alu_control = ALU_SUB;

            // ALUOp = 10: R-type和I-type算术逻辑指令
            2'b10: begin
                case (funct3)
                    3'b000: begin
                        // ADD/SUB区分：看funct7_bit
                        if (funct7_bit)
                            alu_control = ALU_SUB;
                        else
                            alu_control = ALU_ADD;
                    end
                    3'b001: alu_control = ALU_SLL;   // SLLI/SLL
                    3'b010: alu_control = ALU_SLT;   // SLTI/SLT
                    3'b011: alu_control = ALU_SLTU;  // SLTIU/SLTU
                    3'b100: alu_control = ALU_XOR;   // XORI/XOR
                    3'b101: begin
                        // SRL/SRA区分
                        if (funct7_bit)
                            alu_control = ALU_SRA;
                        else
                            alu_control = ALU_SRL;
                    end
                    3'b110: alu_control = ALU_OR;    // ORI/OR
                    3'b111: alu_control = ALU_AND;   // ANDI/AND
                    default: alu_control = ALU_ADD;
                endcase
            end

            default: alu_control = ALU_ADD;
        endcase
    end

endmodule
```

---

## 7. 完整Verilog代码

### 7.1 ALU模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: alu
// 功能描述: 32位ALU，支持RV32I所有算术逻辑运算
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module alu (
    input  wire [31:0] a,            // 操作数A
    input  wire [31:0] b,            // 操作数B
    input  wire [3:0]  alu_control,  // ALU控制信号
    output reg  [31:0] result,       // 运算结果
    output wire        zero          // 零标志
);

    // ALU操作编码
    localparam ALU_ADD  = 4'b0000;
    localparam ALU_SUB  = 4'b0001;
    localparam ALU_AND  = 4'b0010;
    localparam ALU_OR   = 4'b0011;
    localparam ALU_XOR  = 4'b0100;
    localparam ALU_SLT  = 4'b0101;
    localparam ALU_SLTU = 4'b0110;
    localparam ALU_SLL  = 4'b0111;
    localparam ALU_SRL  = 4'b1000;
    localparam ALU_SRA  = 4'b1001;

    // 零标志
    assign zero = (result == 32'd0);

    // ALU运算
    always @(*) begin
        case (alu_control)
            ALU_ADD:  result = a + b;
            ALU_SUB:  result = a - b;
            ALU_AND:  result = a & b;
            ALU_OR:   result = a | b;
            ALU_XOR:  result = a ^ b;
            ALU_SLT:  result = ($signed(a) < $signed(b)) ? 32'd1 : 32'd0;
            ALU_SLTU: result = (a < b) ? 32'd1 : 32'd0;
            ALU_SLL:  result = a << b[4:0];  // 只使用低5位作为移位量
            ALU_SRL:  result = a >> b[4:0];
            ALU_SRA:  result = $signed(a) >>> b[4:0];
            default:  result = 32'd0;
        endcase
    end

endmodule
```

### 7.2 控制单元模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: control_unit
// 功能描述: 主控制单元，根据操作码生成所有控制信号
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module control_unit (
    input  wire [6:0]  opcode,       // 指令操作码
    output reg         branch,       // 分支使能
    output reg         mem_read,     // 存储器读使能
    output reg         mem_to_reg,   // 写回数据源选择
    output reg [1:0]   alu_op,       // ALU操作类型
    output reg         mem_write,    // 存储器写使能
    output reg         alu_src,      // ALU第二操作数选择
    output reg         reg_write,    // 寄存器写使能
    output reg         jump,         // 跳转指令
    output reg         jalr          // JALR指令
);

    // 操作码定义
    localparam OPCODE_R_TYPE = 7'b0110011;
    localparam OPCODE_I_TYPE = 7'b0010011;
    localparam OPCODE_LOAD   = 7'b0000011;
    localparam OPCODE_STORE  = 7'b0100011;
    localparam OPCODE_BRANCH = 7'b1100011;
    localparam OPCODE_JAL    = 7'b1101111;
    localparam OPCODE_JALR   = 7'b1100111;
    localparam OPCODE_LUI    = 7'b0110111;
    localparam OPCODE_AUIPC  = 7'b0010111;

    always @(*) begin
        // 默认值
        branch     = 1'b0;
        mem_read   = 1'b0;
        mem_to_reg = 1'b0;
        alu_op     = 2'b00;
        mem_write  = 1'b0;
        alu_src    = 1'b0;
        reg_write  = 1'b0;
        jump       = 1'b0;
        jalr       = 1'b0;

        case (opcode)
            // R-type 寄存器-寄存器操作
            OPCODE_R_TYPE: begin
                reg_write = 1'b1;
                alu_op = 2'b10;
            end

            // I-type 立即数操作
            OPCODE_I_TYPE: begin
                reg_write = 1'b1;
                alu_src = 1'b1;
                alu_op = 2'b10;
            end

            // Load 加载指令
            OPCODE_LOAD: begin
                mem_read = 1'b1;
                reg_write = 1'b1;
                mem_to_reg = 1'b1;
                alu_src = 1'b1;
            end

            // Store 存储指令
            OPCODE_STORE: begin
                mem_write = 1'b1;
                alu_src = 1'b1;
            end

            // Branch 分支指令
            OPCODE_BRANCH: begin
                branch = 1'b1;
                alu_op = 2'b01;
            end

            // JAL 跳转并链接
            OPCODE_JAL: begin
                jump = 1'b1;
                reg_write = 1'b1;
            end

            // JALR 寄存器跳转并链接
            OPCODE_JALR: begin
                jalr = 1'b1;
                reg_write = 1'b1;
                alu_src = 1'b1;
            end

            // LUI 加载高位立即数
            OPCODE_LUI: begin
                reg_write = 1'b1;
                alu_src = 1'b1;
            end

            // AUIPC PC加立即数
            OPCODE_AUIPC: begin
                reg_write = 1'b1;
                alu_src = 1'b1;
            end

            default: begin
                // 默认所有控制信号为0（空操作）
            end
        endcase
    end

endmodule
```

### 7.3 指令存储器模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: instruction_memory
// 功能描述: 指令存储器，使用分布式RAM实现
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module instruction_memory (
    input  wire [31:0] address,      // 字节地址
    output wire [31:0] instruction   // 指令输出
);

    // 8KB指令存储器 = 2048 x 32bit
    // 使用字节地址，所以取地址的[12:2]作为字索引
    reg [31:0] imem [0:2047];

    // 初始化存储器（从文件加载）
    initial begin
        $readmemh("instructions.mem", imem);
    end

    // 读操作（组合逻辑）
    // 地址按字对齐，忽略低2位
    assign instruction = imem[address[12:2]];

endmodule
```

### 7.4 数据存储器模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: data_memory
// 功能描述: 数据存储器，支持字节寻址的字访问
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module data_memory (
    input  wire        clk,
    input  wire        rst_n,
    input  wire [31:0] address,      // 字节地址
    input  wire [31:0] write_data,   // 写数据
    input  wire        mem_read,     // 读使能
    input  wire        mem_write,    // 写使能
    output wire [31:0] read_data,    // 读数据

    // I/O映射接口
    output reg  [7:0]  io_led,       // LED输出
    output reg  [15:0] io_seg_data,  // 数码管数据
    output reg  [3:0]  io_seg_an,    // 数码管位选
    input  wire [15:0] io_switch     // 开关输入
);

    // 8KB数据存储器
    reg [31:0] dmem [0:2047];
    integer i;

    // I/O地址定义（简化）
    localparam ADDR_LED = 32'h0000_4000;
    localparam ADDR_SEG_DATA = 32'h0000_4004;
    localparam ADDR_SEG_AN = 32'h0000_4008;
    localparam ADDR_SWITCH = 32'h0000_400C;

    // 初始化
    initial begin
        for (i = 0; i < 2048; i = i + 1)
            dmem[i] = 32'd0;
    end

    // 写操作
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            io_led <= 8'd0;
            io_seg_data <= 16'd0;
            io_seg_an <= 4'b1111;
        end
        else if (mem_write) begin
            case (address)
                ADDR_LED: io_led <= write_data[7:0];
                ADDR_SEG_DATA: io_seg_data <= write_data[15:0];
                ADDR_SEG_AN: io_seg_an <= write_data[3:0];
                default: begin
                    // 正常存储器写入
                    if (address < 32'h0000_2000)
                        dmem[address[12:2]] <= write_data;
                end
            endcase
        end
    end

    // 读操作
    assign read_data = (address == ADDR_SWITCH) ? {16'd0, io_switch} :
                       (mem_read && address < 32'h0000_2000) ? dmem[address[12:2]] : 32'd0;

endmodule
```

### 7.5 CPU顶层模块

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 模块名称: cpu_top
// 功能描述: 单周期CPU顶层模块，集成所有子模块
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module cpu_top (
    input  wire        clk,
    input  wire        rst_n,

    // 调试接口
    input  wire        run_mode,     // 运行/暂停
    input  wire        step,         // 单步执行
    input  wire [4:0]  debug_reg_addr,
    output wire [31:0] debug_pc,
    output wire [31:0] debug_reg_data,
    output wire [7:0]  io_led,
    output wire [15:0] io_seg_data,
    output wire [3:0]  io_seg_an,
    input  wire [15:0] io_switch
);

    ///////////////////////////////////////////////////////////////////////////////
    // 内部信号定义
    ///////////////////////////////////////////////////////////////////////////////

    // PC相关
    wire [31:0] pc_current;
    wire [31:0] pc_plus4;
    wire [31:0] pc_next;
    wire [31:0] pc_branch;
    wire [31:0] pc_jump;

    // 指令
    wire [31:0] instruction;
    wire [6:0]  opcode;
    wire [4:0]  rs1, rs2, rd;
    wire [2:0]  funct3;
    wire        funct7_bit;

    // 控制信号
    wire        branch;
    wire        mem_read;
    wire        mem_to_reg;
    wire [1:0]  alu_op;
    wire        mem_write;
    wire        alu_src;
    wire        reg_write;
    wire        jump;
    wire        jalr;

    // 立即数
    wire [31:0] immediate;

    // 寄存器文件
    wire [31:0] read_data1;
    wire [31:0] read_data2;
    wire [31:0] write_data;

    // ALU
    wire [31:0] alu_operand_b;
    wire [3:0]  alu_control;
    wire [31:0] alu_result;
    wire        alu_zero;

    // 数据存储器
    wire [31:0] mem_read_data;

    // 分支跳转逻辑
    wire        pc_src;
    wire        branch_taken;

    ///////////////////////////////////////////////////////////////////////////////
    // 指令字段提取
    ///////////////////////////////////////////////////////////////////////////////
    assign opcode = instruction[6:0];
    assign rd = instruction[11:7];
    assign funct3 = instruction[14:12];
    assign rs1 = instruction[19:15];
    assign rs2 = instruction[24:20];
    assign funct7_bit = instruction[30];

    ///////////////////////////////////////////////////////////////////////////////
    // PC逻辑
    ///////////////////////////////////////////////////////////////////////////////
    assign pc_plus4 = pc_current + 32'd4;
    assign pc_branch = pc_current + immediate;
    assign pc_jump = jalr ? (read_data1 + immediate) : (pc_current + immediate);

    // 分支条件判断
    assign branch_taken = branch & (
        (funct3 == 3'b000) ? alu_zero :      // BEQ
        (funct3 == 3'b001) ? ~alu_zero :     // BNE
        (funct3 == 3'b100) ? alu_result[0] : // BLT
        (funct3 == 3'b101) ? ~alu_result[0] :// BGE
        (funct3 == 3'b110) ? alu_result[0] : // BLTU
        (funct3 == 3'b111) ? ~alu_result[0] :// BGEU
        1'b0
    );

    // PC选择
    assign pc_src = branch_taken | jump;
    assign pc_next = pc_src ? pc_jump : pc_plus4;

    // PC模块
    pc_module u_pc (
        .clk        (clk),
        .rst_n      (rst_n),
        .pc_write   (run_mode | step),
        .pc_next    (pc_next),
        .pc_current (pc_current)
    );

    // 调试输出
    assign debug_pc = pc_current;

    ///////////////////////////////////////////////////////////////////////////////
    // 指令存储器
    ///////////////////////////////////////////////////////////////////////////////
    instruction_memory u_imem (
        .address     (pc_current),
        .instruction (instruction)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 控制单元
    ///////////////////////////////////////////////////////////////////////////////
    control_unit u_control (
        .opcode     (opcode),
        .branch     (branch),
        .mem_read   (mem_read),
        .mem_to_reg (mem_to_reg),
        .alu_op     (alu_op),
        .mem_write  (mem_write),
        .alu_src    (alu_src),
        .reg_write  (reg_write),
        .jump       (jump),
        .jalr       (jalr)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 寄存器文件
    ///////////////////////////////////////////////////////////////////////////////
    register_file u_regfile (
        .clk           (clk),
        .rst_n         (rst_n),
        .reg_write     (reg_write),
        .read_reg1     (rs1),
        .read_reg2     (rs2),
        .write_reg     (rd),
        .write_data    (write_data),
        .read_data1    (read_data1),
        .read_data2    (read_data2),
        .debug_reg_addr(debug_reg_addr),
        .debug_reg_data(debug_reg_data)
    );

    // 写回数据选择
    assign write_data = jump ? pc_plus4 :      // JAL/JALR
                        mem_to_reg ? mem_read_data : // Load
                        alu_result;              // ALU结果

    ///////////////////////////////////////////////////////////////////////////////
    // 立即数扩展
    ///////////////////////////////////////////////////////////////////////////////
    immediate_gen u_imm_gen (
        .instruction (instruction),
        .immediate   (immediate)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // ALU控制
    ///////////////////////////////////////////////////////////////////////////////
    alu_control u_alu_ctrl (
        .alu_op      (alu_op),
        .funct3      (funct3),
        .funct7_bit  (funct7_bit),
        .alu_control (alu_control)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // ALU
    ///////////////////////////////////////////////////////////////////////////////
    assign alu_operand_b = alu_src ? immediate : read_data2;

    alu u_alu (
        .a            (read_data1),
        .b            (alu_operand_b),
        .alu_control  (alu_control),
        .result       (alu_result),
        .zero         (alu_zero)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 数据存储器
    ///////////////////////////////////////////////////////////////////////////////
    data_memory u_dmem (
        .clk        (clk),
        .rst_n      (rst_n),
        .address    (alu_result),
        .write_data (read_data2),
        .mem_read   (mem_read),
        .mem_write  (mem_write),
        .read_data  (mem_read_data),
        .io_led     (io_led),
        .io_seg_data(io_seg_data),
        .io_seg_an  (io_seg_an),
        .io_switch  (io_switch)
    );

endmodule
```

---

## 8. 存储器初始化

### 8.1 指令存储器初始化文件

```
; instructions.mem - RISC-V机器码（十六进制）
; 斐波那契数列计算程序

; 初始化x1=0, x2=1, 循环计算x3=x1+x2

00000093    ; 0000: addi x1, x0, 0      # x1 = 0 (fib(0))
00100113    ; 0004: addi x2, x0, 1      # x2 = 1 (fib(1))
00a00293    ; 0008: addi x5, x0, 10     # x5 = 10 (循环次数)
00100213    ; 000c: addi x4, x0, 1      # x4 = 1 (计数器)

; 循环开始
002081b3    ; 0010: add x3, x1, x2      # x3 = x1 + x2
00010213    ; 0014: addi x4, x2, 0      # x1 = x2
00018193    ; 0018: addi x3, x3, 0      # x2 = x3
00120213    ; 001c: addi x4, x4, 1      # 计数器++
fe521ce3    ; 0020: bne x4, x5, -8      # if(x4!=x5) goto loop

; 程序结束（无限循环）
0000006f    ; 0024: jal x0, 0           # 原地跳转

; 其他测试程序可以添加在这里
```

### 8.2 汇编程序示例

```asm
# fibonacci.s - 斐波那契数列计算
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# 计算前N个斐波那契数

        .text
        .globl _start

_start:
        # 初始化
        li x1, 0            # fib(0) = 0
        li x2, 1            # fib(1) = 1
        li x5, 10           # 计算10个数
        li x4, 0            # 计数器

loop:
        # 计算下一个斐波那契数
        add x3, x1, x2      # x3 = x1 + x2

        # 将当前结果存储到数据存储器
        sw x3, 0(x0)        # 存储到地址0

        # 更新数值
        mv x1, x2           # x1 = x2
        mv x2, x3           # x2 = x3

        # 计数和分支
        addi x4, x4, 1      # 计数器++
        blt x4, x5, loop    # if(x4 < 10) continue

end:
        j end               # 无限循环
```

---

## 9. XDC约束文件

```xdc
################################################################################
# 单周期CPU实验约束文件
# 文件名: cpu_lab.xdc
# 适用开发板: Digilent Basys3
################################################################################

##------------------------------------------------------------------------------
## 时钟约束
##------------------------------------------------------------------------------
set_property PACKAGE_PIN W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]

create_clock -period 20.000 -name sys_clk -waveform {0.000 10.000} [get_ports clk]

##------------------------------------------------------------------------------
## 复位
##------------------------------------------------------------------------------
set_property PACKAGE_PIN U18 [get_ports rst_n]
set_property IOSTANDARD LVCMOS33 [get_ports rst_n]

##------------------------------------------------------------------------------
## 调试接口 - 拨码开关
##------------------------------------------------------------------------------
# SW0-SW4: 选择查看的寄存器
set_property PACKAGE_PIN V17 [get_ports {debug_reg_addr[0]}]
set_property PACKAGE_PIN V16 [get_ports {debug_reg_addr[1]}]
set_property PACKAGE_PIN W16 [get_ports {debug_reg_addr[2]}]
set_property PACKAGE_PIN W17 [get_ports {debug_reg_addr[3]}]
set_property PACKAGE_PIN W15 [get_ports {debug_reg_addr[4]}]

set_property IOSTANDARD LVCMOS33 [get_ports {debug_reg_addr[*]}]

# SW15: 运行/暂停
set_property PACKAGE_PIN R2 [get_ports run_mode]
set_property IOSTANDARD LVCMOS33 [get_ports run_mode]

##------------------------------------------------------------------------------
## 调试接口 - 按钮
##------------------------------------------------------------------------------
# BTNC: 单步执行
set_property PACKAGE_PIN U18 [get_ports step]
set_property IOSTANDARD LVCMOS33 [get_ports step]

##------------------------------------------------------------------------------
## 调试输出 - LED
##------------------------------------------------------------------------------
# LED0-7: PC低8位
set_property PACKAGE_PIN U16 [get_ports {debug_pc[0]}]
set_property PACKAGE_PIN E19 [get_ports {debug_pc[1]}]
set_property PACKAGE_PIN U19 [get_ports {debug_pc[2]}]
set_property PACKAGE_PIN V19 [get_ports {debug_pc[3]}]
set_property PACKAGE_PIN W18 [get_ports {debug_pc[4]}]
set_property PACKAGE_PIN U15 [get_ports {debug_pc[5]}]
set_property PACKAGE_PIN U14 [get_ports {debug_pc[6]}]
set_property PACKAGE_PIN V14 [get_ports {debug_pc[7]}]

set_property IOSTANDARD LVCMOS33 [get_ports {debug_pc[*]}]

# LED8-15: 寄存器值低8位
set_property PACKAGE_PIN V13 [get_ports {debug_reg_data[0]}]
set_property PACKAGE_PIN V3  [get_ports {debug_reg_data[1]}]
set_property PACKAGE_PIN W3  [get_ports {debug_reg_data[2]}]
set_property PACKAGE_PIN U3  [get_ports {debug_reg_data[3]}]
set_property PACKAGE_PIN P3  [get_ports {debug_reg_data[4]}]
set_property PACKAGE_PIN N3  [get_ports {debug_reg_data[5]}]
set_property PACKAGE_PIN P1  [get_ports {debug_reg_data[6]}]
set_property PACKAGE_PIN L1  [get_ports {debug_reg_data[7]}]

set_property IOSTANDARD LVCMOS33 [get_ports {debug_reg_data[*]}]

##------------------------------------------------------------------------------
## 七段数码管
##------------------------------------------------------------------------------
set_property PACKAGE_PIN W7 [get_ports {io_seg[0]}]
set_property PACKAGE_PIN W6 [get_ports {io_seg[1]}]
set_property PACKAGE_PIN U8 [get_ports {io_seg[2]}]
set_property PACKAGE_PIN V8 [get_ports {io_seg[3]}]
set_property PACKAGE_PIN U5 [get_ports {io_seg[4]}]
set_property PACKAGE_PIN V5 [get_ports {io_seg[5]}]
set_property PACKAGE_PIN U7 [get_ports {io_seg[6]}]

set_property PACKAGE_PIN U2 [get_ports {io_seg_an[0]}]
set_property PACKAGE_PIN U4 [get_ports {io_seg_an[1]}]
set_property PACKAGE_PIN V4 [get_ports {io_seg_an[2]}]
set_property PACKAGE_PIN W4 [get_ports {io_seg_an[3]}]

set_property IOSTANDARD LVCMOS33 [get_ports {io_seg[*]}]
set_property IOSTANDARD LVCMOS33 [get_ports {io_seg_an[*]}]

##------------------------------------------------------------------------------
## 配置设置
##------------------------------------------------------------------------------
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
```

---

## 10. 仿真与调试

### 10.1 CPU Testbench

```verilog
//////////////////////////////////////////////////////////////////////////////////
// 测试文件: tb_cpu.v
// 功能描述: CPU完整测试平台
//////////////////////////////////////////////////////////////////////////////////

`timescale 1ns / 1ps

module tb_cpu;

    ///////////////////////////////////////////////////////////////////////////////
    // 信号定义
    ///////////////////////////////////////////////////////////////////////////////
    reg         clk;
    reg         rst_n;
    reg         run_mode;
    reg         step;
    reg  [4:0]  debug_reg_addr;
    wire [31:0] debug_pc;
    wire [31:0] debug_reg_data;
    wire [7:0]  io_led;
    wire [15:0] io_seg_data;
    wire [3:0]  io_seg_an;
    reg  [15:0] io_switch;

    // 时钟周期
    parameter CLK_PERIOD = 20;  // 50MHz

    ///////////////////////////////////////////////////////////////////////////////
    // 实例化待测CPU
    ///////////////////////////////////////////////////////////////////////////////
    cpu_top uut (
        .clk            (clk),
        .rst_n          (rst_n),
        .run_mode       (run_mode),
        .step           (step),
        .debug_reg_addr (debug_reg_addr),
        .debug_pc       (debug_pc),
        .debug_reg_data (debug_reg_data),
        .io_led         (io_led),
        .io_seg_data    (io_seg_data),
        .io_seg_an      (io_seg_an),
        .io_switch      (io_switch)
    );

    ///////////////////////////////////////////////////////////////////////////////
    // 时钟生成
    ///////////////////////////////////////////////////////////////////////////////
    initial begin
        clk = 0;
        forever #(CLK_PERIOD/2) clk = ~clk;
    end

    ///////////////////////////////////////////////////////////////////////////////
    // 测试序列
    ///////////////////////////////////////////////////////////////////////////////
    initial begin
        $display("========================================");
        $display("      CPU Testbench Started            ");
        $display("========================================");

        // 初始化
        rst_n = 0;
        run_mode = 0;
        step = 0;
        debug_reg_addr = 5'd0;
        io_switch = 16'd0;

        // 复位
        #(CLK_PERIOD * 5);
        rst_n = 1;
        #(CLK_PERIOD * 2);

        // 启动CPU运行
        run_mode = 1;

        // 运行一段时间，观察PC变化
        repeat(50) begin
            @(posedge clk);
            $display("Time=%0t: PC=%h, Instr=%h",
                     $time, debug_pc, uut.instruction);
        end

        // 暂停，检查寄存器
        run_mode = 0;
        #(CLK_PERIOD * 5);

        // 读取各个寄存器
        $display("\n--- Register File Contents ---");
        for (integer i = 0; i < 8; i = i + 1) begin
            debug_reg_addr = i[4:0];
            #(CLK_PERIOD);
            $display("x%0d = %h", i, debug_reg_data);
        end

        // 继续运行
        run_mode = 1;
        repeat(30) @(posedge clk);

        $display("\n========================================");
        $display("      CPU Testbench Finished           ");
        $display("========================================");

        $finish;
    end

    ///////////////////////////////////////////////////////////////////////////////
    // 波形生成
    ///////////////////////////////////////////////////////////////////////////////
    initial begin
        $dumpfile("cpu.vcd");
        $dumpvars(0, tb_cpu);
    end

endmodule
```

### 10.2 调试技巧

```
1. 使用$display输出关键信号
2. 观察PC变化判断程序执行流程
3. 检查寄存器值确认运算正确性
4. 使用波形查看器分析时序问题
5. 分段测试各个子模块
```

---

## 11. 上板验证

### 11.1 验证步骤

```
1. 确认指令存储器初始化文件正确
2. 生成比特流并下载
3. 观察PC LED，确认程序运行
4. 选择寄存器地址，观察寄存器值
5. 验证斐波那契数列计算结果
```

### 11.2 预期结果

```
斐波那契数列：0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89...

验证点：
- PC应该递增（除分支/跳转外）
- 寄存器值应该按斐波那契规律变化
- 数据存储器中应该保存计算结果
```

---

## 12. 故障排除FAQ

### Q1: 指令不执行或PC不增加

**检查点：**

- PC写使能信号是否正常
- 时钟信号是否正确连接到所有时序逻辑
- 复位信号是否释放

### Q2: 运算结果错误

**检查点：**

- ALU控制信号是否正确
- 立即数扩展是否正确（特别注意符号扩展）
- 寄存器写使能和写回数据选择

### Q3: 分支跳转异常

**检查点：**

- 立即数是否正确计算（注意左移1位）
- 分支条件判断逻辑
- PC更新逻辑中的选择信号

---

## 13. 实验报告模板

```markdown
    # FPGA实验三报告：单周期CPU实现

    ## 一、实验目标
    （描述本实验的主要目标）

    ## 二、CPU设计方案

    ### 2.1 整体架构
    （描述CPU的整体架构设计）

    ### 2.2 数据通路图
    （绘制数据通路框图）

    ### 2.3 控制信号表
    （列出所有控制信号及生成逻辑）

    ## 三、核心模块实现

    ### 3.1 PC模块
    ```verilog
    // 代码
    ```

    ### 3.2 寄存器文件

    ```verilog
    // 代码
    ```

    ### 3.3 ALU

    ```verilog
    // 代码
    ```

    ### 3.4 控制单元

    ```verilog
    // 代码
    ```

    ## 四、测试程序

    ### 4.1 斐波那契程序

    ```
    机器码
    ```

    ### 4.2 仿真结果

    （粘贴仿真波形或日志）

    ## 五、硬件验证

    （描述上板测试过程和结果）

    ## 六、问题与解决

    ## 七、扩展设计（选做）

    - 多周期CPU
    - 流水线CPU
    - 中断支持

    ## 八、心得体会

    ## 附录

    - 完整代码
    - XDC文件

```

---

## 14. 扩展练习

### 14.1 多周期CPU

将单周期CPU改为多周期，共享功能部件，降低成本。

### 14.2 流水线CPU

实现5级流水线CPU，提高指令吞吐率。

### 14.3 中断和异常

添加中断和异常处理机制。

---

## 15. 参考资料

1. 《计算机组成与设计：RISC-V版》- Patterson & Hennessy
2. RISC-V指令集手册
3. 《数字设计和计算机体系结构》- Harris & Harris
4. Xilinx Vivado用户指南

---

**文档信息**

- 版本: v1.0
- 创建日期: 2024
- 适用课程: 计算机组成原理


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
