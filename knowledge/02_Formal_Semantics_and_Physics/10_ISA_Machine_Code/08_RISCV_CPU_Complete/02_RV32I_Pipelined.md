---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# RV32I 流水线处理器设计与实现

## 目录

- [1. 流水线概述](#1-流水线概述)
  - [1.1 什么是流水线](#11-什么是流水线)
  - [1.2 RISC-V五级流水线](#12-risc-v五级流水线)
  - [1.3 流水线寄存器](#13-流水线寄存器)
- [2. 5级流水线设计](#2-5级流水线设计)
  - [2.1 完整数据通路图](#21-完整数据通路图)
  - [2.2 各级详细设计](#22-各级详细设计)
    - [2.2.1 IF阶段 (取指)](#221-if阶段-取指)
    - [2.2.2 ID阶段 (译码)](#222-id阶段-译码)
    - [2.2.3 EX阶段 (执行)](#223-ex阶段-执行)
    - [2.2.4 MEM阶段 (访存)](#224-mem阶段-访存)
    - [2.2.5 WB阶段 (写回)](#225-wb阶段-写回)
- [3. 流水线寄存器](#3-流水线寄存器)
  - [3.1 IF/ID流水线寄存器](#31-ifid流水线寄存器)
  - [3.2 ID/EX流水线寄存器](#32-idex流水线寄存器)
  - [3.3 EX/MEM和MEM/WB流水线寄存器](#33-exmem和memwb流水线寄存器)
- [4. 数据冒险检测与转发](#4-数据冒险检测与转发)
  - [4.1 数据冒险类型](#41-数据冒险类型)
  - [4.2 转发单元设计](#42-转发单元设计)
  - [4.3 冒险检测单元](#43-冒险检测单元)
- [5. 控制冒险处理](#5-控制冒险处理)
  - [5.1 分支预测策略](#51-分支预测策略)
  - [5.2 分支处理单元](#52-分支处理单元)
  - [5.3 延迟槽 (Delay Slot)](#53-延迟槽-delay-slot)
- [6. 结构冒险避免](#6-结构冒险避免)
  - [6.1 存储器端口冲突](#61-存储器端口冲突)
  - [6.2 寄存器文件端口冲突](#62-寄存器文件端口冲突)
- [7. 完整流水线CPU Verilog实现](#7-完整流水线cpu-verilog实现)
  - [7.1 顶层模块](#71-顶层模块)
  - [7.2 测试平台](#72-测试平台)
- [8. 性能分析](#8-性能分析)
  - [8.1 CPI计算](#81-cpi计算)
  - [8.2 流水线性能图](#82-流水线性能图)
  - [8.3 总结](#83-总结)
- [深入理解](#深入理解)
  - [核心原理](#核心原理)
  - [实践应用](#实践应用)
  - [最佳实践](#最佳实践)

---

## 1. 流水线概述

### 1.1 什么是流水线

流水线是一种将指令执行过程划分为多个阶段的技术，允许多条指令同时在不同阶段执行，从而提高指令吞吐量。

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    流水线vs单周期执行对比                                     │
└─────────────────────────────────────────────────────────────────────────────┘

单周期执行 (指令1耗时T, 指令2耗时T, 总时间2T):
┌─────────────────────┬─────────────────────┐
│      指令1          │      指令2          │
│   (取指译码执行...)  │   (取指译码执行...)  │
└─────────────────────┴─────────────────────┘
0                    T                    2T

流水线执行 (每级耗时T/5, 吞吐量提高5倍):
┌─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┬─────┐
│ IF  │ ID  │ EX  │ MEM │ WB  │     │     │     │     │ 指令1
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │ IF  │ ID  │ EX  │ MEM │ WB  │     │     │     │ 指令2
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │     │ IF  │ ID  │ EX  │ MEM │ WB  │     │     │ 指令3
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │     │     │ IF  │ ID  │ EX  │ MEM │ WB  │     │ 指令4
├─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┼─────┤
│     │     │     │     │ IF  │ ID  │ EX  │ MEM │ WB  │ 指令5
└─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┴─────┘
0    T/5  2T/5  3T/5  4T/5   T   6T/5  7T/5  8T/5  9T/5

理想CPI = 1 (每个时钟周期完成一条指令)
```

### 1.2 RISC-V五级流水线

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    RV32I五级流水线结构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐   ┌─────────┐       │
│  │   IF    │──►│   ID    │──►│   EX    │──►│   MEM   │──►│   WB    │       │
│  │ (取指)  │   │ (译码)  │   │ (执行)  │   │ (访存)  │   │ (写回)  │       │
│  └─────────┘   └─────────┘   └─────────┘   └─────────┘   └─────────┘       │
│       │            │            │            │            │                │
│       ▼            ▼            ▼            ▼            ▼                │
│   PC更新        寄存器读      ALU运算      内存访问      寄存器写           │
│   指令读取      立即数生成    地址计算     数据读写      结果写回           │
│                 控制信号      分支判断                                  │
│                 生成                                                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘

IF  - Instruction Fetch: 从指令存储器取指令
ID  - Instruction Decode: 译码指令, 读取寄存器
EX  - Execute: ALU运算, 计算地址
MEM - Memory Access: 访问数据存储器
WB  - Write Back: 写回结果到寄存器文件
```

### 1.3 流水线寄存器

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    流水线寄存器连接各阶段                                     │
└─────────────────────────────────────────────────────────────────────────────┘

     IF/ID寄存器           ID/EX寄存器          EX/MEM寄存器        MEM/WB寄存器
    ┌───────────┐        ┌───────────┐        ┌───────────┐      ┌───────────┐
    │ PC+4      │        │ PC+4      │        │ ALU结果   │      │ ALU结果   │
    │ 指令字    │        │ 立即数    │        │ 写数据    │      │ 读数据    │
    │           │───────►│ rs1数据   │───────►│ 目标寄存器│─────►│ 目标寄存器│
    │           │        │ rs2数据   │        │ 控制信号  │      │ 控制信号  │
    │           │        │ 控制信号  │        │           │      │           │
    └───────────┘        └───────────┘        └───────────┘      └───────────┘
         ▲                    ▲                  ▲                  ▲
         │                    │                  │                  │
    时钟上升沿锁存        时钟上升沿锁存      时钟上升沿锁存      时钟上升沿锁存
```

---

## 2. 5级流水线设计

### 2.1 完整数据通路图

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    RV32I 流水线处理器数据通路                                             │
└─────────────────────────────────────────────────────────────────────────────────────────────────────────┘

                                          Pipeline Registers
    ┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐   ┌───────────────────┐
    │      IF/ID        │   │      ID/EX        │   │      EX/MEM       │   │      MEM/WB       │
    ├───────────────────┤   ├───────────────────┤   ├───────────────────┤   ├───────────────────┤
    │ PC+4              │   │ PC+4              │   │ Branch Target     │   │ ALU Result        │
    │ Instruction       │   │ rs1_data          │   │ Zero Flag         │   │ Memory Data       │
    │ PC                │   │ rs2_data          │   │ ALU Result        │   │ Write Register    │
    │                   │   │ Immediate         │   │ Write Data        │   │ Control Signals   │
    │                   │   │ rs1_addr          │   │ Write Register    │   │                   │
    │                   │   │ rs2_addr          │   │ Control Signals   │   │                   │
    │                   │   │ rd_addr           │   │                   │   │                   │
    │                   │   │ funct3            │   │                   │   │                   │
    │                   │   │ funct7            │   │                   │   │                   │
    │                   │   │ Control Signals   │   │                   │   │                   │
    └─────────┬─────────┘   └─────────┬─────────┘   └─────────┬─────────┘   └─────────┬─────────┘
              │                       │                       │                       │
              │                       │                       │                       │
    ┌─────────▼─────────┐   ┌─────────▼─────────┐   ┌─────────▼─────────┐   ┌─────────▼─────────┐
    │                   │   │                   │   │                   │   │                   │
    │  ┌─────────────┐  │   │  ┌─────────────┐  │   │  ┌─────────────┐  │   │  ┌─────────────┐  │
    │  │  PC Reg     │  │   │  │ Register    │  │   │  │    ALU      │  │   │  │   Data      │  │
    │  │             │  │   │  │   File      │  │   │  │             │  │   │  │  Memory     │  │
    │  │  next_pc ───┼──┘   │  │             │  │   │  │   A  B      │  │   │  │             │  │
    │  │     │       │      │  │  read_reg1  │◄─┼───┘  │   │  │      │  │   │  │  Address    │  │
    │  │     ▼       │      │  │  read_reg2  │◄─┼──────┼───┘  │      │  │   │  │  WriteData  │  │
    │  │  ┌───────┐  │      │  │  write_reg  │◄─┼──────┼──────┼──────┼───┘   │  │  ReadData   │  │
    │  │  │  +4   │  │      │  │  write_data │◄─┼──────┼──────┼──────┼───────┼──┼─────────────┤  │
    │  │  └───┬───┘  │      │  │  reg_write  │◄─┼──────┼──────┼──────┼───────┼──┘             │  │
    │  │      │      │      │  │             │  │      │      │      │       │                  │  │
    │  │  ┌───▼───┐  │      │  │ read_data1 ─┼──┼──────┼──────┤      │       │                  │  │
    │  │  │  MUX  │  │      │  │ read_data2 ─┼──┼──────┼──────┼──────┤       │                  │  │
    │  │  └───┬───┘  │      │  └─────────────┘  │      │      │      │       │                  │  │
    │  │      │      │      │         ▲         │      │      │      │       │                  │  │
    │  │      │      │      │         │         │      │  ┌───▼───┐  │       │  ┌───────────┐   │  │
    │  └──────┼──────┘      │         │         │      │  │  MUX  │  │       │  │   MUX     │   │  │
    │         │             │         │         │      │  └───┬───┘  │       │  └───┬───────┘   │  │
    │         │             │  ┌──────┴──────┐  │      │      │      │       │      │           │  │
    │         │             │  │   Immediate │  │      │  ┌───┴───┐  │       │  ┌───┴───┐       │  │
    │         │             │  │  Generator  │  │      └──┤  +    │◄─┘       └──┤  MUX  │◄──────┘  │
    │         │             │  │             │  │         └───┬───┘             └───┬───┘          │
    │         │             │  └─────────────┘  │             │                     │              │
    │         │             │                   │             │                     │              │
    │         │             │  ┌─────────────┐  │             │                     │              │
    │         └─────────────┼──┤   Control   │  │             │                     │              │
    │                       │  │    Unit     │  │             │                     │              │
    │                       │  └─────────────┘  │             │                     │              │
    │                       │                   │             │                     │              │
    │  ┌──────────────────┐ │  ┌─────────────┐  │  ┌────────┐ │                     │              │
    │  │ Instruction      │ │  │  Hazard     │  │  │Branch  │ │                     │              │
    │  │  Memory          │ │  │  Detection  │  │  │ Unit   │ │                     │              │
    │  │                  │ │  │             │  │  │        │ │                     │              │
    │  │  Address ◄───────┘ │  │  ForwardA   ├──┼──┤        │ │                     │              │
    │  │  Instruction ──────┼──┤  ForwardB   ├──┼──┤        │ │                     │              │
    │  │                  │ │  │  Stall      ├──┘  │        │ │                     │              │
    │  │                  │ │  │  Flush      ├─────┤        │ │                     │              │
    │  └──────────────────┘ │  └─────────────┘     └────────┘ │                     │              │
    │                       │                                 │                     │              │
    └───────────────────────┴─────────────────────────────────┴─────────────────────┴──────────────┘
```

### 2.2 各级详细设计

#### 2.2.1 IF阶段 (取指)

```verilog
// IF阶段模块
module pipeline_stage_if(
    input         clk,
    input         reset,
    input         stall,           // 流水线暂停
    input         flush,           // 流水线刷新
    input  [31:0] branch_target,   // 分支目标地址
    input  [31:0] jump_target,     // 跳转目标地址
    input         pc_src,          // PC来源选择
    output [31:0] pc_out,
    output [31:0] pc_plus4_out,
    output [31:0] instruction_out
);
    reg [31:0] pc_reg;
    wire [31:0] pc_next;
    wire [31:0] pc_plus4;
    wire [31:0] instruction;

    // PC更新逻辑
    assign pc_plus4 = pc_reg + 32'd4;
    assign pc_next = pc_src ? (pc_src == 2'b01 ? branch_target : jump_target) : pc_plus4;

    // PC寄存器
    always @(posedge clk or posedge reset) begin
        if (reset)
            pc_reg <= 32'h00000000;
        else if (!stall)
            pc_reg <= pc_next;
    end

    // 指令存储器
    instruction_memory instr_mem(
        .address(pc_reg),
        .instruction(instruction)
    );

    assign pc_out = pc_reg;
    assign pc_plus4_out = pc_plus4;
    assign instruction_out = instruction;
endmodule
```

#### 2.2.2 ID阶段 (译码)

```verilog
// ID阶段模块
module pipeline_stage_id(
    input         clk,
    input         reset,
    input         stall,
    input         flush,
    input  [31:0] pc_in,
    input  [31:0] pc_plus4_in,
    input  [31:0] instruction_in,
    input         reg_write_wb,      // 来自WB阶段
    input  [4:0]  write_reg_wb,      // 来自WB阶段
    input  [31:0] write_data_wb,     // 来自WB阶段
    output [31:0] pc_out,
    output [31:0] pc_plus4_out,
    output [31:0] rs1_data_out,
    output [31:0] rs2_data_out,
    output [31:0] immediate_out,
    output [4:0]  rs1_addr_out,
    output [4:0]  rs2_addr_out,
    output [4:0]  rd_addr_out,
    output [2:0]  funct3_out,
    output [6:0]  funct7_out,
    output        reg_write_out,
    output        mem_read_out,
    output        mem_write_out,
    output        mem_to_reg_out,
    output        alu_src_out,
    output        branch_out,
    output        jump_out,
    output [1:0]  alu_op_out
);
    // 指令字段提取
    wire [6:0]  opcode = instruction_in[6:0];
    wire [4:0]  rd = instruction_in[11:7];
    wire [2:0]  funct3 = instruction_in[14:12];
    wire [4:0]  rs1 = instruction_in[19:15];
    wire [4:0]  rs2 = instruction_in[24:20];
    wire [6:0]  funct7 = instruction_in[31:25];

    // 寄存器文件
    wire [31:0] rs1_data, rs2_data;

    register_file reg_file(
        .clk(clk),
        .reset(reset),
        .read_reg1(rs1),
        .read_reg2(rs2),
        .write_reg(write_reg_wb),
        .write_data(write_data_wb),
        .reg_write(reg_write_wb),
        .read_data1(rs1_data),
        .read_data2(rs2_data)
    );

    // 立即数生成
    wire [31:0] immediate;

    immediate_generator imm_gen(
        .instruction(instruction_in),
        .immediate(immediate)
    );

    // 控制单元
    wire reg_write, mem_read, mem_write, mem_to_reg, alu_src, branch, jump;
    wire [1:0] alu_op;

    control_unit ctrl_unit(
        .opcode(opcode),
        .reg_write(reg_write),
        .mem_read(mem_read),
        .mem_write(mem_write),
        .mem_to_reg(mem_to_reg),
        .alu_src(alu_src),
        .branch(branch),
        .jump(jump),
        .alu_op(alu_op),
        .pc_src()
    );

    // 流水线寄存器输出
    reg [31:0] pc_reg, pc_plus4_reg, rs1_data_reg, rs2_data_reg, immediate_reg;
    reg [4:0]  rs1_addr_reg, rs2_addr_reg, rd_addr_reg;
    reg [2:0]  funct3_reg;
    reg [6:0]  funct7_reg;
    reg        reg_write_reg, mem_read_reg, mem_write_reg, mem_to_reg_reg;
    reg        alu_src_reg, branch_reg, jump_reg;
    reg [1:0]  alu_op_reg;

    always @(posedge clk or posedge reset) begin
        if (reset || flush) begin
            pc_reg <= 32'h00000000;
            pc_plus4_reg <= 32'h00000000;
            rs1_data_reg <= 32'h00000000;
            rs2_data_reg <= 32'h00000000;
            immediate_reg <= 32'h00000000;
            rs1_addr_reg <= 5'b00000;
            rs2_addr_reg <= 5'b00000;
            rd_addr_reg <= 5'b00000;
            funct3_reg <= 3'b000;
            funct7_reg <= 7'b0000000;
            reg_write_reg <= 1'b0;
            mem_read_reg <= 1'b0;
            mem_write_reg <= 1'b0;
            mem_to_reg_reg <= 1'b0;
            alu_src_reg <= 1'b0;
            branch_reg <= 1'b0;
            jump_reg <= 1'b0;
            alu_op_reg <= 2'b00;
        end else if (!stall) begin
            pc_reg <= pc_in;
            pc_plus4_reg <= pc_plus4_in;
            rs1_data_reg <= rs1_data;
            rs2_data_reg <= rs2_data;
            immediate_reg <= immediate;
            rs1_addr_reg <= rs1;
            rs2_addr_reg <= rs2;
            rd_addr_reg <= rd;
            funct3_reg <= funct3;
            funct7_reg <= funct7;
            reg_write_reg <= reg_write;
            mem_read_reg <= mem_read;
            mem_write_reg <= mem_write;
            mem_to_reg_reg <= mem_to_reg;
            alu_src_reg <= alu_src;
            branch_reg <= branch;
            jump_reg <= jump;
            alu_op_reg <= alu_op;
        end
    end

    // 输出赋值
    assign pc_out = pc_reg;
    assign pc_plus4_out = pc_plus4_reg;
    assign rs1_data_out = rs1_data_reg;
    assign rs2_data_out = rs2_data_reg;
    assign immediate_out = immediate_reg;
    assign rs1_addr_out = rs1_addr_reg;
    assign rs2_addr_out = rs2_addr_reg;
    assign rd_addr_out = rd_addr_reg;
    assign funct3_out = funct3_reg;
    assign funct7_out = funct7_reg;
    assign reg_write_out = reg_write_reg;
    assign mem_read_out = mem_read_reg;
    assign mem_write_out = mem_write_reg;
    assign mem_to_reg_out = mem_to_reg_reg;
    assign alu_src_out = alu_src_reg;
    assign branch_out = branch_reg;
    assign jump_out = jump_reg;
    assign alu_op_out = alu_op_reg;
endmodule
```

#### 2.2.3 EX阶段 (执行)

```verilog
// EX阶段模块
module pipeline_stage_ex(
    input         clk,
    input         reset,
    input  [31:0] pc_in,
    input  [31:0] pc_plus4_in,
    input  [31:0] rs1_data_in,
    input  [31:0] rs2_data_in,
    input  [31:0] immediate_in,
    input  [4:0]  rs1_addr_in,
    input  [4:0]  rs2_addr_in,
    input  [4:0]  rd_addr_in,
    input  [2:0]  funct3_in,
    input  [6:0]  funct7_in,
    input         reg_write_in,
    input         mem_read_in,
    input         mem_write_in,
    input         mem_to_reg_in,
    input         alu_src_in,
    input         branch_in,
    input         jump_in,
    input  [1:0]  alu_op_in,
    // 转发输入
    input  [1:0]  forward_a,
    input  [1:0]  forward_b,
    input  [31:0] ex_mem_alu_result,
    input  [31:0] mem_wb_write_data,
    // 输出
    output [31:0] branch_target_out,
    output        zero_out,
    output [31:0] alu_result_out,
    output [31:0] rs2_data_out,
    output [4:0]  rd_addr_out,
    output        reg_write_out,
    output        mem_read_out,
    output        mem_write_out,
    output        mem_to_reg_out,
    output        branch_out,
    output        jump_out
);
    // ALU输入选择（转发逻辑）
    wire [31:0] alu_input_a;
    wire [31:0] alu_input_b_raw;
    wire [31:0] alu_input_b;

    // ForwardA: 选择ALU的第一个操作数
    assign alu_input_a =
        (forward_a == 2'b00) ? rs1_data_in :
        (forward_a == 2'b01) ? ex_mem_alu_result :
        (forward_a == 2'b10) ? mem_wb_write_data : rs1_data_in;

    // ForwardB: 选择ALU的第二个操作数（用于比较和存储）
    assign alu_input_b_raw =
        (forward_b == 2'b00) ? rs2_data_in :
        (forward_b == 2'b01) ? ex_mem_alu_result :
        (forward_b == 2'b10) ? mem_wb_write_data : rs2_data_in;

    // ALU输入B选择（立即数或寄存器）
    assign alu_input_b = alu_src_in ? immediate_in : alu_input_b_raw;

    // ALU控制单元
    wire [3:0] alu_control;

    alu_control alu_ctrl(
        .alu_op(alu_op_in),
        .funct3(funct3_in),
        .funct7(funct7_in),
        .opcode_bit5(1'b0),
        .alu_control_out(alu_control)
    );

    // ALU
    wire [31:0] alu_result;
    wire        alu_zero;

    alu alu_unit(
        .a(alu_input_a),
        .b(alu_input_b),
        .alu_control(alu_control),
        .result(alu_result),
        .zero(alu_zero)
    );

    // 分支目标计算
    wire [31:0] branch_target = pc_in + immediate_in;

    // 流水线寄存器输出
    reg [31:0] branch_target_reg, alu_result_reg, rs2_data_reg;
    reg [4:0]  rd_addr_reg;
    reg        zero_reg;
    reg        reg_write_reg, mem_read_reg, mem_write_reg, mem_to_reg_reg;
    reg        branch_reg, jump_reg;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            branch_target_reg <= 32'h00000000;
            zero_reg <= 1'b0;
            alu_result_reg <= 32'h00000000;
            rs2_data_reg <= 32'h00000000;
            rd_addr_reg <= 5'b00000;
            reg_write_reg <= 1'b0;
            mem_read_reg <= 1'b0;
            mem_write_reg <= 1'b0;
            mem_to_reg_reg <= 1'b0;
            branch_reg <= 1'b0;
            jump_reg <= 1'b0;
        end else begin
            branch_target_reg <= branch_target;
            zero_reg <= alu_zero;
            alu_result_reg <= alu_result;
            rs2_data_reg <= alu_input_b_raw;  // 存储数据的转发值
            rd_addr_reg <= rd_addr_in;
            reg_write_reg <= reg_write_in;
            mem_read_reg <= mem_read_in;
            mem_write_reg <= mem_write_in;
            mem_to_reg_reg <= mem_to_reg_in;
            branch_reg <= branch_in;
            jump_reg <= jump_in;
        end
    end

    // 输出赋值
    assign branch_target_out = branch_target_reg;
    assign zero_out = zero_reg;
    assign alu_result_out = alu_result_reg;
    assign rs2_data_out = rs2_data_reg;
    assign rd_addr_out = rd_addr_reg;
    assign reg_write_out = reg_write_reg;
    assign mem_read_out = mem_read_reg;
    assign mem_write_out = mem_write_reg;
    assign mem_to_reg_out = mem_to_reg_reg;
    assign branch_out = branch_reg;
    assign jump_out = jump_reg;
endmodule
```

#### 2.2.4 MEM阶段 (访存)

```verilog
// MEM阶段模块
module pipeline_stage_mem(
    input         clk,
    input         reset,
    input  [31:0] alu_result_in,
    input  [31:0] rs2_data_in,
    input  [4:0]  rd_addr_in,
    input         reg_write_in,
    input         mem_read_in,
    input         mem_write_in,
    input         mem_to_reg_in,
    input  [2:0]  funct3_in,  // 用于确定访问类型
    output [31:0] alu_result_out,
    output [31:0] memory_data_out,
    output [4:0]  rd_addr_out,
    output        reg_write_out,
    output        mem_to_reg_out
);
    // 数据存储器
    wire [31:0] mem_read_data;

    data_memory data_mem(
        .clk(clk),
        .address(alu_result_in),
        .write_data(rs2_data_in),
        .mem_read(mem_read_in),
        .mem_write(mem_write_in),
        .funct3(funct3_in),
        .read_data(mem_read_data)
    );

    // 流水线寄存器输出
    reg [31:0] alu_result_reg, memory_data_reg;
    reg [4:0]  rd_addr_reg;
    reg        reg_write_reg, mem_to_reg_reg;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            alu_result_reg <= 32'h00000000;
            memory_data_reg <= 32'h00000000;
            rd_addr_reg <= 5'b00000;
            reg_write_reg <= 1'b0;
            mem_to_reg_reg <= 1'b0;
        end else begin
            alu_result_reg <= alu_result_in;
            memory_data_reg <= mem_read_data;
            rd_addr_reg <= rd_addr_in;
            reg_write_reg <= reg_write_in;
            mem_to_reg_reg <= mem_to_reg_in;
        end
    end

    // 输出赋值
    assign alu_result_out = alu_result_reg;
    assign memory_data_out = memory_data_reg;
    assign rd_addr_out = rd_addr_reg;
    assign reg_write_out = reg_write_reg;
    assign mem_to_reg_out = mem_to_reg_reg;
endmodule
```

#### 2.2.5 WB阶段 (写回)

```verilog
// WB阶段模块
module pipeline_stage_wb(
    input  [31:0] alu_result_in,
    input  [31:0] memory_data_in,
    input  [4:0]  rd_addr_in,
    input         reg_write_in,
    input         mem_to_reg_in,
    output [31:0] write_data_out,
    output [4:0]  rd_addr_out,
    output        reg_write_out
);
    // 写回数据源选择
    assign write_data_out = mem_to_reg_in ? memory_data_in : alu_result_in;

    // 直接输出控制信号
    assign rd_addr_out = rd_addr_in;
    assign reg_write_out = reg_write_in;
endmodule
```

---

## 3. 流水线寄存器

### 3.1 IF/ID流水线寄存器

```verilog
// IF/ID流水线寄存器
module if_id_register(
    input         clk,
    input         reset,
    input         stall,
    input         flush,
    input  [31:0] pc_in,
    input  [31:0] pc_plus4_in,
    input  [31:0] instruction_in,
    output [31:0] pc_out,
    output [31:0] pc_plus4_out,
    output [31:0] instruction_out
);
    reg [31:0] pc_reg, pc_plus4_reg, instruction_reg;

    always @(posedge clk or posedge reset) begin
        if (reset || flush) begin
            pc_reg <= 32'h00000000;
            pc_plus4_reg <= 32'h00000000;
            instruction_reg <= 32'h00000000;  // NOP
        end else if (!stall) begin
            pc_reg <= pc_in;
            pc_plus4_reg <= pc_plus4_in;
            instruction_reg <= instruction_in;
        end
    end

    assign pc_out = pc_reg;
    assign pc_plus4_out = pc_plus4_reg;
    assign instruction_out = instruction_reg;
endmodule
```

### 3.2 ID/EX流水线寄存器

```verilog
// ID/EX流水线寄存器
module id_ex_register(
    input         clk,
    input         reset,
    input         stall,
    input         flush,
    // 数据输入
    input  [31:0] pc_in,
    input  [31:0] pc_plus4_in,
    input  [31:0] rs1_data_in,
    input  [31:0] rs2_data_in,
    input  [31:0] immediate_in,
    input  [4:0]  rs1_addr_in,
    input  [4:0]  rs2_addr_in,
    input  [4:0]  rd_addr_in,
    input  [2:0]  funct3_in,
    input  [6:0]  funct7_in,
    // 控制信号输入
    input         reg_write_in,
    input         mem_read_in,
    input         mem_write_in,
    input         mem_to_reg_in,
    input         alu_src_in,
    input         branch_in,
    input         jump_in,
    input  [1:0]  alu_op_in,
    // 输出
    output [31:0] pc_out,
    output [31:0] pc_plus4_out,
    output [31:0] rs1_data_out,
    output [31:0] rs2_data_out,
    output [31:0] immediate_out,
    output [4:0]  rs1_addr_out,
    output [4:0]  rs2_addr_out,
    output [4:0]  rd_addr_out,
    output [2:0]  funct3_out,
    output [6:0]  funct7_out,
    output        reg_write_out,
    output        mem_read_out,
    output        mem_write_out,
    output        mem_to_reg_out,
    output        alu_src_out,
    output        branch_out,
    output        jump_out,
    output [1:0]  alu_op_out
);
    // 数据寄存器
    reg [31:0] pc_reg, pc_plus4_reg, rs1_data_reg, rs2_data_reg, immediate_reg;
    reg [4:0]  rs1_addr_reg, rs2_addr_reg, rd_addr_reg;
    reg [2:0]  funct3_reg;
    reg [6:0]  funct7_reg;
    // 控制信号寄存器
    reg        reg_write_reg, mem_read_reg, mem_write_reg, mem_to_reg_reg;
    reg        alu_src_reg, branch_reg, jump_reg;
    reg [1:0]  alu_op_reg;

    always @(posedge clk or posedge reset) begin
        if (reset || flush) begin
            pc_reg <= 32'h00000000;
            pc_plus4_reg <= 32'h00000000;
            rs1_data_reg <= 32'h00000000;
            rs2_data_reg <= 32'h00000000;
            immediate_reg <= 32'h00000000;
            rs1_addr_reg <= 5'b00000;
            rs2_addr_reg <= 5'b00000;
            rd_addr_reg <= 5'b00000;
            funct3_reg <= 3'b000;
            funct7_reg <= 7'b0000000;
            reg_write_reg <= 1'b0;
            mem_read_reg <= 1'b0;
            mem_write_reg <= 1'b0;
            mem_to_reg_reg <= 1'b0;
            alu_src_reg <= 1'b0;
            branch_reg <= 1'b0;
            jump_reg <= 1'b0;
            alu_op_reg <= 2'b00;
        end else if (!stall) begin
            pc_reg <= pc_in;
            pc_plus4_reg <= pc_plus4_in;
            rs1_data_reg <= rs1_data_in;
            rs2_data_reg <= rs2_data_in;
            immediate_reg <= immediate_in;
            rs1_addr_reg <= rs1_addr_in;
            rs2_addr_reg <= rs2_addr_in;
            rd_addr_reg <= rd_addr_in;
            funct3_reg <= funct3_in;
            funct7_reg <= funct7_in;
            reg_write_reg <= reg_write_in;
            mem_read_reg <= mem_read_in;
            mem_write_reg <= mem_write_in;
            mem_to_reg_reg <= mem_to_reg_in;
            alu_src_reg <= alu_src_in;
            branch_reg <= branch_in;
            jump_reg <= jump_in;
            alu_op_reg <= alu_op_in;
        end
    end

    // 输出赋值
    assign pc_out = pc_reg;
    assign pc_plus4_out = pc_plus4_reg;
    assign rs1_data_out = rs1_data_reg;
    assign rs2_data_out = rs2_data_reg;
    assign immediate_out = immediate_reg;
    assign rs1_addr_out = rs1_addr_reg;
    assign rs2_addr_out = rs2_addr_reg;
    assign rd_addr_out = rd_addr_reg;
    assign funct3_out = funct3_reg;
    assign funct7_out = funct7_reg;
    assign reg_write_out = reg_write_reg;
    assign mem_read_out = mem_read_reg;
    assign mem_write_out = mem_write_reg;
    assign mem_to_reg_out = mem_to_reg_reg;
    assign alu_src_out = alu_src_reg;
    assign branch_out = branch_reg;
    assign jump_out = jump_reg;
    assign alu_op_out = alu_op_reg;
endmodule
```

### 3.3 EX/MEM和MEM/WB流水线寄存器

```verilog
// EX/MEM流水线寄存器
module ex_mem_register(
    input         clk,
    input         reset,
    input  [31:0] branch_target_in,
    input         zero_in,
    input  [31:0] alu_result_in,
    input  [31:0] rs2_data_in,
    input  [4:0]  rd_addr_in,
    input         reg_write_in,
    input         mem_read_in,
    input         mem_write_in,
    input         mem_to_reg_in,
    input         branch_in,
    input         jump_in,
    output [31:0] branch_target_out,
    output        zero_out,
    output [31:0] alu_result_out,
    output [31:0] rs2_data_out,
    output [4:0]  rd_addr_out,
    output        reg_write_out,
    output        mem_read_out,
    output        mem_write_out,
    output        mem_to_reg_out,
    output        branch_out,
    output        jump_out
);
    reg [31:0] branch_target_reg, alu_result_reg, rs2_data_reg;
    reg [4:0]  rd_addr_reg;
    reg        zero_reg;
    reg        reg_write_reg, mem_read_reg, mem_write_reg, mem_to_reg_reg;
    reg        branch_reg, jump_reg;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            branch_target_reg <= 32'h00000000;
            zero_reg <= 1'b0;
            alu_result_reg <= 32'h00000000;
            rs2_data_reg <= 32'h00000000;
            rd_addr_reg <= 5'b00000;
            reg_write_reg <= 1'b0;
            mem_read_reg <= 1'b0;
            mem_write_reg <= 1'b0;
            mem_to_reg_reg <= 1'b0;
            branch_reg <= 1'b0;
            jump_reg <= 1'b0;
        end else begin
            branch_target_reg <= branch_target_in;
            zero_reg <= zero_in;
            alu_result_reg <= alu_result_in;
            rs2_data_reg <= rs2_data_in;
            rd_addr_reg <= rd_addr_in;
            reg_write_reg <= reg_write_in;
            mem_read_reg <= mem_read_in;
            mem_write_reg <= mem_write_in;
            mem_to_reg_reg <= mem_to_reg_in;
            branch_reg <= branch_in;
            jump_reg <= jump_in;
        end
    end

    assign branch_target_out = branch_target_reg;
    assign zero_out = zero_reg;
    assign alu_result_out = alu_result_reg;
    assign rs2_data_out = rs2_data_reg;
    assign rd_addr_out = rd_addr_reg;
    assign reg_write_out = reg_write_reg;
    assign mem_read_out = mem_read_reg;
    assign mem_write_out = mem_write_reg;
    assign mem_to_reg_out = mem_to_reg_reg;
    assign branch_out = branch_reg;
    assign jump_out = jump_reg;
endmodule

// MEM/WB流水线寄存器
module mem_wb_register(
    input         clk,
    input         reset,
    input  [31:0] alu_result_in,
    input  [31:0] memory_data_in,
    input  [4:0]  rd_addr_in,
    input         reg_write_in,
    input         mem_to_reg_in,
    output [31:0] alu_result_out,
    output [31:0] memory_data_out,
    output [4:0]  rd_addr_out,
    output        reg_write_out,
    output        mem_to_reg_out
);
    reg [31:0] alu_result_reg, memory_data_reg;
    reg [4:0]  rd_addr_reg;
    reg        reg_write_reg, mem_to_reg_reg;

    always @(posedge clk or posedge reset) begin
        if (reset) begin
            alu_result_reg <= 32'h00000000;
            memory_data_reg <= 32'h00000000;
            rd_addr_reg <= 5'b00000;
            reg_write_reg <= 1'b0;
            mem_to_reg_reg <= 1'b0;
        end else begin
            alu_result_reg <= alu_result_in;
            memory_data_reg <= memory_data_in;
            rd_addr_reg <= rd_addr_in;
            reg_write_reg <= reg_write_in;
            mem_to_reg_reg <= mem_to_reg_in;
        end
    end

    assign alu_result_out = alu_result_reg;
    assign memory_data_out = memory_data_reg;
    assign rd_addr_out = rd_addr_reg;
    assign reg_write_out = reg_write_reg;
    assign mem_to_reg_out = mem_to_reg_reg;
endmodule
```

---

## 4. 数据冒险检测与转发

### 4.1 数据冒险类型

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    数据冒险的三种类型                                         │
└─────────────────────────────────────────────────────────────────────────────┘

类型1: RAW (Read After Write) - 写后读 (真数据相关)
─────────────────────────────────────────────────────────────────────────────
指令1: add x1, x2, x3    # 写入x1
指令2: sub x4, x1, x5    # 读取x1 (需要指令1的结果)

┌─────┬─────┬─────┬─────┬─────┐
│ IF  │ ID  │ EX  │ MEM │ WB  │  指令1 add
└─────┴─────┴─────┴─────┴─────┘
      ┌─────┬─────┬─────┬─────┬─────┐
      │ IF  │ ID  │ EX  │ MEM │ WB  │  指令2 sub
      └─────┴─────┴─────┴─────┴─────┘
                  ↑
              冒险点: 指令2在EX阶段需要x1, 但指令1在MEM/WB阶段

解决方案: 转发(Forwarding)或暂停(Stall)


类型2: WAR (Write After Read) - 读后写 (反数据相关)
─────────────────────────────────────────────────────────────────────────────
在流水线中, 如果指令按顺序流出, WAR不会自然发生
因为读取总是在写入之前 (ID阶段读, WB阶段写)


类型3: WAW (Write After Write) - 写后写 (输出数据相关)
─────────────────────────────────────────────────────────────────────────────
在单发射流水线中, 指令按顺序完成, WAW不会发生
但在乱序执行处理器中可能发生
```

### 4.2 转发单元设计

```verilog
// 转发单元
module forwarding_unit(
    // ID/EX阶段寄存器地址
    input  [4:0]  id_ex_rs1,
    input  [4:0]  id_ex_rs2,
    // EX/MEM阶段寄存器写信息
    input         ex_mem_reg_write,
    input  [4:0]  ex_mem_rd,
    // MEM/WB阶段寄存器写信息
    input         mem_wb_reg_write,
    input  [4:0]  mem_wb_rd,
    // 转发控制输出
    output [1:0]  forward_a,
    output [1:0]  forward_b
);
    // ForwardA控制:
    // 00: 来自ID/EX寄存器 (rs1_data)
    // 01: 来自EX/MEM ALU结果
    // 10: 来自MEM/WB写回数据

    // ForwardA逻辑
    assign forward_a =
        // EX/MEM转发 (优先级高, 最新数据)
        (ex_mem_reg_write && (ex_mem_rd != 5'b00000) && (ex_mem_rd == id_ex_rs1)) ? 2'b01 :
        // MEM/WB转发
        (mem_wb_reg_write && (mem_wb_rd != 5'b00000) && (mem_wb_rd == id_ex_rs1)) ? 2'b10 :
        // 不转发
        2'b00;

    // ForwardB逻辑
    assign forward_b =
        // EX/MEM转发
        (ex_mem_reg_write && (ex_mem_rd != 5'b00000) && (ex_mem_rd == id_ex_rs2)) ? 2'b01 :
        // MEM/WB转发
        (mem_wb_reg_write && (mem_wb_rd != 5'b00000) && (mem_wb_rd == id_ex_rs2)) ? 2'b10 :
        // 不转发
        2'b00;
endmodule
```

### 4.3 冒险检测单元

```verilog
// 冒险检测单元 (用于Load-Use冒险)
module hazard_detection_unit(
    // ID/EX阶段信息
    input         id_ex_mem_read,
    input  [4:0]  id_ex_rd,
    // IF/ID阶段信息
    input  [4:0]  if_id_rs1,
    input  [4:0]  if_id_rs2,
    // 当前指令opcode (用于判断是否需要rs1/rs2)
    input  [6:0]  if_id_opcode,
    // 控制输出
    output        stall,
    output        pc_write,
    output        if_id_write
);
    // 操作码定义
    localparam OPCODE_R_TYPE = 7'b0110011;
    localparam OPCODE_I_TYPE = 7'b0010011;
    localparam OPCODE_LOAD   = 7'b0000011;
    localparam OPCODE_STORE  = 7'b0100011;
    localparam OPCODE_BRANCH = 7'b1100011;
    localparam OPCODE_JALR   = 7'b1100111;

    // 判断当前指令是否需要rs1和rs2
    wire needs_rs1 =
        (if_id_opcode == OPCODE_R_TYPE) ||
        (if_id_opcode == OPCODE_I_TYPE) ||
        (if_id_opcode == OPCODE_LOAD) ||
        (if_id_opcode == OPCODE_STORE) ||
        (if_id_opcode == OPCODE_BRANCH) ||
        (if_id_opcode == OPCODE_JALR);

    wire needs_rs2 =
        (if_id_opcode == OPCODE_R_TYPE) ||
        (if_id_opcode == OPCODE_STORE) ||
        (if_id_opcode == OPCODE_BRANCH);

    // Load-Use冒险检测
    // 如果ID/EX是一条Load指令, 且其目标寄存器与IF/ID的源寄存器冲突
    wire load_use_hazard_rs1 = id_ex_mem_read && (id_ex_rd != 5'b00000) &&
                               needs_rs1 && (id_ex_rd == if_id_rs1);
    wire load_use_hazard_rs2 = id_ex_mem_read && (id_ex_rd != 5'b00000) &&
                               needs_rs2 && (id_ex_rd == if_id_rs2);

    wire load_use_hazard = load_use_hazard_rs1 || load_use_hazard_rs2;

    // 输出信号
    assign stall = load_use_hazard;      // 需要暂停
    assign pc_write = ~load_use_hazard;  // 暂停时不更新PC
    assign if_id_write = ~load_use_hazard; // 暂停时不更新IF/ID寄存器
endmodule
```

---

## 5. 控制冒险处理

### 5.1 分支预测策略

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    分支预测策略                                              │
└─────────────────────────────────────────────────────────────────────────────┘

静态预测:
1. 总是不跳转 (Predict Not Taken)
   - 简单, 实现容易
   - 跳转时惩罚: 2个周期 (需要刷新IF和ID阶段)

2. 总是跳转 (Predict Taken)
   - 对循环有利
   - 不跳转时惩罚: 2个周期

3. 向后跳转预测为跳转, 向前跳转预测为不跳转
   - BTFN (Backward Taken, Forward Not taken)
   - 基于循环通常是向后跳转的观察

动态预测:
1. 1位分支历史表
   - 记录上次结果
   - 状态转换: Taken <-> Not Taken

2. 2位饱和计数器 (常用)
   - 四种状态: Strongly NT, Weakly NT, Weakly T, Strongly T
   - 需要两次预测错误才改变预测方向

   状态图:
   ┌──────────┐    Taken    ┌──────────┐
   │ Strongly │ ──────────► │ Weakly   │
   │    NT    │ ◄────────── │    NT    │
   └──────────┘   Not Taken └──────────┘
       ▲     Taken                │
       └──────────────────────────┘
       Not Taken

3. 分支目标缓冲区 (BTB)
   - 缓存分支指令地址和预测目标地址
   - IF阶段即可预测, 减少惩罚
```

### 5.2 分支处理单元

```verilog
// 分支处理单元
module branch_unit_pipeline(
    input  [2:0]  funct3,
    input         branch,          // 是否是分支指令
    input         jump,            // 是否是跳转指令
    input         zero,            // ALU zero标志
    input  [31:0] alu_result,      // ALU结果(用于比较)
    input  [31:0] branch_target,   // 分支目标地址
    input  [31:0] pc_plus4,        // PC+4
    output        branch_taken,    // 分支是否发生
    output [31:0] pc_next,         // 下一条PC值
    output        if_flush,        // 刷新IF阶段
    output        id_flush         // 刷新ID阶段
);
    // 分支条件判断
    wire beq_taken  = branch & (funct3 == 3'b000) & zero;
    wire bne_taken  = branch & (funct3 == 3'b001) & ~zero;
    wire blt_taken  = branch & (funct3 == 3'b100) & alu_result[0];
    wire bge_taken  = branch & (funct3 == 3'b101) & ~alu_result[0];
    wire bltu_taken = branch & (funct3 == 3'b110) & alu_result[0];
    wire bgeu_taken = branch & (funct3 == 3'b111) & ~alu_result[0];

    // 分支/跳转是否发生
    assign branch_taken = beq_taken | bne_taken | blt_taken |
                          bge_taken | bltu_taken | bgeu_taken | jump;

    // PC选择
    assign pc_next = branch_taken ? branch_target : pc_plus4;

    // 刷新信号 (分支预测错误时)
    assign if_flush = branch_taken;
    assign id_flush = branch_taken;
endmodule
```

### 5.3 延迟槽 (Delay Slot)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    延迟槽技术 (MIPS风格, RISC-V不使用)                        │
└─────────────────────────────────────────────────────────────────────────────┘

MIPS采用分支延迟槽: 分支指令后的指令总是执行

指令序列:
  beq  x1, x2, target   # 分支指令
  add  x3, x4, x5       # 延迟槽指令 (总是执行)
  sub  x6, x7, x8       # 如果分支不发生, 执行这条
target:
  and  x9, x10, x11     # 如果分支发生, 执行这条

RISC-V不使用延迟槽, 原因:
1. 简化编译器设计
2. 现代处理器使用分支预测, 延迟槽收益降低
3. 超标量处理器中延迟槽位置不确定

RISC-V分支处理:
- 采用硬件刷新流水线
- 支持静态/动态分支预测
- 分支惩罚: 1-2个周期
```

---

## 6. 结构冒险避免

### 6.1 存储器端口冲突

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    存储器结构冒险                                            │
└─────────────────────────────────────────────────────────────────────────────┘

问题: 单端口存储器同时被IF和MEM访问

解决方案1: 哈佛架构
- 指令存储器和数据存储器分离
- IF访问指令存储器
- MEM访问数据存储器
- 无冲突

解决方案2: 双端口存储器
- 一个读端口用于IF
- 一个读写端口用于MEM
- 硬件成本较高

解决方案3: 流水线暂停
- IF和MEM冲突时, 暂停IF阶段一个周期
- 简单但降低性能
```

### 6.2 寄存器文件端口冲突

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    寄存器文件端口设计                                        │
└─────────────────────────────────────────────────────────────────────────────┘

问题: 同一周期内WB写和ID读可能冲突

解决方案: 多端口寄存器文件 + 写优先读

端口需求:
- 2个读端口: ID阶段读取rs1和rs2
- 1个写端口: WB阶段写回数据

写优先读 (Write-before-Read):
- 如果读和写同一寄存器, 读操作得到写入的新值
- 通过旁路(bypass)实现
- 消除WB到ID的数据冒险

时序:
┌─────────────────────────────────────────────────────────────────────────────┐
│ 时钟上升沿:                                                                 │
│ 1. WB阶段数据写入寄存器文件                                                   │
│ 2. 同时, ID阶段读取寄存器 (使用新值)                                          │
│                                                                             │
│ 这种设计避免了额外转发路径, 简化了硬件                                         │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 7. 完整流水线CPU Verilog实现

### 7.1 顶层模块

```verilog
//////////////////////////////////////////////////////////////////////////////
// RV32I 流水线处理器顶层模块
// 文件名: rv32i_pipelined.v
// 描述: 完整的RV32I 5级流水线处理器实现
//////////////////////////////////////////////////////////////////////////////

module rv32i_pipelined(
    input         clk,
    input         reset,
    // 调试输出
    output [31:0] pc_current,
    output [31:0] instruction,
    output [31:0] reg_x1, reg_x2, reg_x3, reg_x4, reg_x5,
    output [4:0]  wb_rd,
    output        wb_reg_write
);
    //=========================================================================
    // 内部信号声明
    //=========================================================================

    // IF阶段信号
    wire [31:0] if_pc, if_pc_plus4, if_instruction;
    wire [31:0] if_next_pc;

    // IF/ID寄存器信号
    wire [31:0] if_id_pc, if_id_pc_plus4, if_id_instruction;

    // ID阶段信号
    wire [31:0] id_rs1_data, id_rs2_data, id_immediate;
    wire [4:0]  id_rs1_addr, id_rs2_addr, id_rd_addr;
    wire [2:0]  id_funct3;
    wire [6:0]  id_funct7;
    wire        id_reg_write, id_mem_read, id_mem_write;
    wire        id_mem_to_reg, id_alu_src, id_branch, id_jump;
    wire [1:0]  id_alu_op;

    // ID/EX寄存器信号
    wire [31:0] id_ex_pc, id_ex_pc_plus4, id_ex_rs1_data, id_ex_rs2_data, id_ex_immediate;
    wire [4:0]  id_ex_rs1, id_ex_rs2, id_ex_rd;
    wire [2:0]  id_ex_funct3;
    wire [6:0]  id_ex_funct7;
    wire        id_ex_reg_write, id_ex_mem_read, id_ex_mem_write;
    wire        id_ex_mem_to_reg, id_ex_alu_src, id_ex_branch, id_ex_jump;
    wire [1:0]  id_ex_alu_op;

    // EX阶段信号
    wire [31:0] ex_branch_target, ex_alu_result, ex_rs2_data_forwarded;
    wire        ex_zero, ex_branch_taken;

    // EX/MEM寄存器信号
    wire [31:0] ex_mem_branch_target, ex_mem_alu_result, ex_mem_rs2_data;
    wire [4:0]  ex_mem_rd;
    wire        ex_mem_zero, ex_mem_reg_write, ex_mem_mem_read;
    wire        ex_mem_mem_write, ex_mem_mem_to_reg, ex_mem_branch, ex_mem_jump;

    // MEM阶段信号
    wire [31:0] mem_read_data;

    // MEM/WB寄存器信号
    wire [31:0] mem_wb_alu_result, mem_wb_memory_data;
    wire [4:0]  mem_wb_rd;
    wire        mem_wb_reg_write, mem_wb_mem_to_reg;

    // WB阶段信号
    wire [31:0] wb_write_data;

    // 转发信号
    wire [1:0]  forward_a, forward_b;

    // 冒险检测信号
    wire        stall, pc_write, if_id_write;
    wire        if_flush, id_flush, ex_flush;

    // PC来源选择
    wire [1:0]  pc_src;
    wire [31:0] jump_target;

    //=========================================================================
    // IF阶段: 取指
    //=========================================================================

    // PC寄存器
    reg [31:0] pc_reg;
    wire [31:0] pc_plus4 = pc_reg + 32'd4;

    always @(posedge clk or posedge reset) begin
        if (reset)
            pc_reg <= 32'h00000000;
        else if (pc_write)
            pc_reg <= if_next_pc;
    end

    // 指令存储器
    instruction_memory instr_mem(
        .address(pc_reg),
        .instruction(if_instruction)
    );

    assign if_pc = pc_reg;
    assign if_pc_plus4 = pc_plus4;

    // IF/ID流水线寄存器
    if_id_register if_id_reg(
        .clk(clk),
        .reset(reset),
        .stall(stall),
        .flush(if_flush),
        .pc_in(if_pc),
        .pc_plus4_in(if_pc_plus4),
        .instruction_in(if_instruction),
        .pc_out(if_id_pc),
        .pc_plus4_out(if_id_pc_plus4),
        .instruction_out(if_id_instruction)
    );

    //=========================================================================
    // ID阶段: 译码
    //=========================================================================

    // 指令字段
    wire [6:0]  opcode = if_id_instruction[6:0];
    assign id_rd_addr = if_id_instruction[11:7];
    assign id_funct3 = if_id_instruction[14:12];
    assign id_rs1_addr = if_id_instruction[19:15];
    assign id_rs2_addr = if_id_instruction[24:20];
    assign id_funct7 = if_id_instruction[31:25];

    // 寄存器文件
    register_file reg_file(
        .clk(clk),
        .reset(reset),
        .read_reg1(id_rs1_addr),
        .read_reg2(id_rs2_addr),
        .write_reg(mem_wb_rd),
        .write_data(wb_write_data),
        .reg_write(mem_wb_reg_write),
        .read_data1(id_rs1_data),
        .read_data2(id_rs2_data)
    );

    // 立即数生成
    immediate_generator imm_gen(
        .instruction(if_id_instruction),
        .immediate(id_immediate)
    );

    // 控制单元
    control_unit ctrl_unit(
        .opcode(opcode),
        .reg_write(id_reg_write),
        .mem_read(id_mem_read),
        .mem_write(id_mem_write),
        .mem_to_reg(id_mem_to_reg),
        .alu_src(id_alu_src),
        .branch(id_branch),
        .jump(id_jump),
        .alu_op(id_alu_op),
        .pc_src()
    );

    // ID/EX流水线寄存器
    id_ex_register id_ex_reg(
        .clk(clk),
        .reset(reset),
        .stall(stall),
        .flush(id_flush),
        .pc_in(if_id_pc),
        .pc_plus4_in(if_id_pc_plus4),
        .rs1_data_in(id_rs1_data),
        .rs2_data_in(id_rs2_data),
        .immediate_in(id_immediate),
        .rs1_addr_in(id_rs1_addr),
        .rs2_addr_in(id_rs2_addr),
        .rd_addr_in(id_rd_addr),
        .funct3_in(id_funct3),
        .funct7_in(id_funct7),
        .reg_write_in(id_reg_write),
        .mem_read_in(id_mem_read),
        .mem_write_in(id_mem_write),
        .mem_to_reg_in(id_mem_to_reg),
        .alu_src_in(id_alu_src),
        .branch_in(id_branch),
        .jump_in(id_jump),
        .alu_op_in(id_alu_op),
        .pc_out(id_ex_pc),
        .pc_plus4_out(id_ex_pc_plus4),
        .rs1_data_out(id_ex_rs1_data),
        .rs2_data_out(id_ex_rs2_data),
        .immediate_out(id_ex_immediate),
        .rs1_addr_out(id_ex_rs1),
        .rs2_addr_out(id_ex_rs2),
        .rd_addr_out(id_ex_rd),
        .funct3_out(id_ex_funct3),
        .funct7_out(id_ex_funct7),
        .reg_write_out(id_ex_reg_write),
        .mem_read_out(id_ex_mem_read),
        .mem_write_out(id_ex_mem_write),
        .mem_to_reg_out(id_ex_mem_to_reg),
        .alu_src_out(id_ex_alu_src),
        .branch_out(id_ex_branch),
        .jump_out(id_ex_jump),
        .alu_op_out(id_ex_alu_op)
    );

    //=========================================================================
    // EX阶段: 执行
    //=========================================================================

    // ALU输入转发选择
    wire [31:0] alu_input_a =
        (forward_a == 2'b01) ? ex_mem_alu_result :
        (forward_a == 2'b10) ? wb_write_data : id_ex_rs1_data;

    wire [31:0] alu_input_b_raw =
        (forward_b == 2'b01) ? ex_mem_alu_result :
        (forward_b == 2'b10) ? wb_write_data : id_ex_rs2_data;

    wire [31:0] alu_input_b = id_ex_alu_src ? id_ex_immediate : alu_input_b_raw;

    // ALU控制
    wire [3:0] alu_control;
    alu_control alu_ctrl(
        .alu_op(id_ex_alu_op),
        .funct3(id_ex_funct3),
        .funct7(id_ex_funct7),
        .opcode_bit5(1'b0),
        .alu_control_out(alu_control)
    );

    // ALU
    alu alu_unit(
        .a(alu_input_a),
        .b(alu_input_b),
        .alu_control(alu_control),
        .result(ex_alu_result),
        .zero(ex_zero)
    );

    // 分支目标
    assign ex_branch_target = id_ex_pc + id_ex_immediate;

    // 跳转目标
    assign jump_target = id_ex_jump ?
                         ((id_ex_funct3 == 3'b000) ?
                          {alu_input_a[31:1] + id_ex_immediate[31:1], 1'b0} :
                          ex_branch_target) :
                         pc_plus4;

    // EX/MEM流水线寄存器
    ex_mem_register ex_mem_reg(
        .clk(clk),
        .reset(reset),
        .branch_target_in(ex_branch_target),
        .zero_in(ex_zero),
        .alu_result_in(ex_alu_result),
        .rs2_data_in(alu_input_b_raw),
        .rd_addr_in(id_ex_rd),
        .reg_write_in(id_ex_reg_write),
        .mem_read_in(id_ex_mem_read),
        .mem_write_in(id_ex_mem_write),
        .mem_to_reg_in(id_ex_mem_to_reg),
        .branch_in(id_ex_branch),
        .jump_in(id_ex_jump),
        .branch_target_out(ex_mem_branch_target),
        .zero_out(ex_mem_zero),
        .alu_result_out(ex_mem_alu_result),
        .rs2_data_out(ex_mem_rs2_data),
        .rd_addr_out(ex_mem_rd),
        .reg_write_out(ex_mem_reg_write),
        .mem_read_out(ex_mem_mem_read),
        .mem_write_out(ex_mem_mem_write),
        .mem_to_reg_out(ex_mem_mem_to_reg),
        .branch_out(ex_mem_branch),
        .jump_out(ex_mem_jump)
    );

    //=========================================================================
    // MEM阶段: 访存
    //=========================================================================

    // 数据存储器
    data_memory data_mem(
        .clk(clk),
        .address(ex_mem_alu_result),
        .write_data(ex_mem_rs2_data),
        .mem_read(ex_mem_mem_read),
        .mem_write(ex_mem_mem_write),
        .funct3(3'b010),  // 简化处理, 只支持字访问
        .read_data(mem_read_data)
    );

    // MEM/WB流水线寄存器
    mem_wb_register mem_wb_reg(
        .clk(clk),
        .reset(reset),
        .alu_result_in(ex_mem_alu_result),
        .memory_data_in(mem_read_data),
        .rd_addr_in(ex_mem_rd),
        .reg_write_in(ex_mem_reg_write),
        .mem_to_reg_in(ex_mem_mem_to_reg),
        .alu_result_out(mem_wb_alu_result),
        .memory_data_out(mem_wb_memory_data),
        .rd_addr_out(mem_wb_rd),
        .reg_write_out(mem_wb_reg_write),
        .mem_to_reg_out(mem_wb_mem_to_reg)
    );

    //=========================================================================
    // WB阶段: 写回
    //=========================================================================

    assign wb_write_data = mem_wb_mem_to_reg ? mem_wb_memory_data : mem_wb_alu_result;

    //=========================================================================
    // 冒险检测和转发单元
    //=========================================================================

    // 转发单元
    forwarding_unit fwd_unit(
        .id_ex_rs1(id_ex_rs1),
        .id_ex_rs2(id_ex_rs2),
        .ex_mem_reg_write(ex_mem_reg_write),
        .ex_mem_rd(ex_mem_rd),
        .mem_wb_reg_write(mem_wb_reg_write),
        .mem_wb_rd(mem_wb_rd),
        .forward_a(forward_a),
        .forward_b(forward_b)
    );

    // 冒险检测单元
    hazard_detection_unit hazard_unit(
        .id_ex_mem_read(id_ex_mem_read),
        .id_ex_rd(id_ex_rd),
        .if_id_rs1(id_rs1_addr),
        .if_id_rs2(id_rs2_addr),
        .if_id_opcode(opcode),
        .stall(stall),
        .pc_write(pc_write),
        .if_id_write(if_id_write)
    );

    //=========================================================================
    // 分支处理
    //=========================================================================

    // 分支判断
    wire beq_taken  = ex_mem_branch & (id_ex_funct3 == 3'b000) & ex_mem_zero;
    wire bne_taken  = ex_mem_branch & (id_ex_funct3 == 3'b001) & ~ex_mem_zero;
    wire blt_taken  = ex_mem_branch & (id_ex_funct3 == 3'b100) & ex_alu_result[0];
    wire bge_taken  = ex_mem_branch & (id_ex_funct3 == 3'b101) & ~ex_alu_result[0];

    assign ex_branch_taken = beq_taken | bne_taken | blt_taken | bge_taken | ex_mem_jump;

    // PC来源
    assign if_next_pc = ex_branch_taken ? ex_mem_branch_target : pc_plus4;

    // 刷新信号
    assign if_flush = ex_branch_taken;
    assign id_flush = ex_branch_taken;

    //=========================================================================
    // 调试输出
    //=========================================================================

    assign pc_current = if_pc;
    assign instruction = if_instruction;
    assign reg_x1 = reg_file.registers[1];
    assign reg_x2 = reg_file.registers[2];
    assign reg_x3 = reg_file.registers[3];
    assign reg_x4 = reg_file.registers[4];
    assign reg_x5 = reg_file.registers[5];
    assign wb_rd = mem_wb_rd;
    assign wb_reg_write = mem_wb_reg_write;

endmodule
```

### 7.2 测试平台

```verilog
//////////////////////////////////////////////////////////////////////////////
// RV32I 流水线处理器测试平台
// 文件名: tb_rv32i_pipelined.v
//////////////////////////////////////////////////////////////////////////////

`timescale 1ns/1ps

module tb_rv32i_pipelined;
    // 测试信号
    reg         clk;
    reg         reset;
    wire [31:0] pc_current;
    wire [31:0] instruction;
    wire [31:0] reg_x1, reg_x2, reg_x3, reg_x4, reg_x5;
    wire [4:0]  wb_rd;
    wire        wb_reg_write;

    // 实例化被测模块
    rv32i_pipelined uut(
        .clk(clk),
        .reset(reset),
        .pc_current(pc_current),
        .instruction(instruction),
        .reg_x1(reg_x1),
        .reg_x2(reg_x2),
        .reg_x3(reg_x3),
        .reg_x4(reg_x4),
        .reg_x5(reg_x5),
        .wb_rd(wb_rd),
        .wb_reg_write(wb_reg_write)
    );

    // 时钟生成
    initial begin
        clk = 0;
        forever #5 clk = ~clk;
    end

    // 测试程序初始化
    initial begin
        // 初始化指令存储器
        // 测试程序: 数据冒险和转发测试
        uut.instr_mem.memory[0]  = 32'h00100093;  // addi x1, x0, 1
        uut.instr_mem.memory[1]  = 32'h00200113;  // addi x2, x0, 2
        uut.instr_mem.memory[2]  = 32'h00300193;  // addi x3, x0, 3
        uut.instr_mem.memory[3]  = 32'h00208233;  // add  x4, x1, x2  (需要转发)
        uut.instr_mem.memory[4]  = 32'h003102b3;  // add  x5, x2, x3
        uut.instr_mem.memory[5]  = 32'h00418333;  // add  x6, x3, x4
        uut.instr_mem.memory[6]  = 32'h0000006f;  // jal  x0, 0
    end

    // 测试序列
    initial begin
        $display("============================================================");
        $display("       RV32I Pipelined Processor Testbench");
        $display("============================================================");

        // 复位
        reset = 1;
        #20;
        reset = 0;

        // 运行测试
        #100;

        // 显示结果
        $display("\n============================================================");
        $display("                Test Results");
        $display("============================================================");
        $display("Register x1: %d", reg_x1);
        $display("Register x2: %d", reg_x2);
        $display("Register x3: %d", reg_x3);
        $display("Register x4: %d (expected: 3)", reg_x4);
        $display("Register x5: %d (expected: 5)", reg_x5);
        $display("============================================================");

        $finish;
    end

    // 波形监控
    initial begin
        $dumpfile("rv32i_pipelined.vcd");
        $dumpvars(0, tb_rv32i_pipelined);
    end

    // 流水线追踪
    always @(posedge clk) begin
        if (!reset) begin
            #1;
            $display("Time=%0t PC=%h Inst=%h ForwardA=%b ForwardB=%b Stall=%b",
                     $time, pc_current, instruction,
                     uut.forward_a, uut.forward_b, uut.stall);
        end
    end

endmodule
```

---

## 8. 性能分析

### 8.1 CPI计算

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    流水线处理器CPI分析                                       │
└─────────────────────────────────────────────────────────────────────────────┘

理想CPI = 1
实际CPI = 1 + 停顿周期数 / 指令数

停顿原因:
1. Load-Use冒险: 1个周期停顿
2. 分支预测错误: 2个周期惩罚
3. 结构冒险: 1个周期停顿

性能公式:
CPI = 1 + (Load指令比例 × Load-Use频率 × 1)
    + (分支指令比例 × 预测错误率 × 2)

假设:
- Load指令占 25%
- Load-Use频率 50%
- 分支指令占 20%
- 预测错误率 10%

CPI = 1 + (0.25 × 0.5 × 1) + (0.20 × 0.10 × 2)
    = 1 + 0.125 + 0.04
    = 1.165

加速比 vs 单周期 = 5 / 1.165 ≈ 4.3x (理想5x)
```

### 8.2 流水线性能图

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    流水线执行时序图                                          │
│                    测试代码: 数据依赖序列                                     │
└─────────────────────────────────────────────────────────────────────────────┘

指令序列:
1. addi x1, x0, 1
2. addi x2, x0, 2
3. add  x3, x1, x2   (依赖指令1,2)
4. add  x4, x2, x3   (依赖指令2,3)
5. sw   x4, 0(x5)
6. lw   x6, 0(x5)
7. add  x7, x6, x1   (Load-Use冒险)

时序图:
Cycle  1    2    3    4    5    6    7    8    9    10   11   12   13
─────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────┬────
I1   │ IF │ ID │ EX │ MEM│ WB │    │    │    │    │    │    │    │
─────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────
I2   │    │ IF │ ID │ EX │ MEM│ WB │    │    │    │    │    │    │
─────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────
I3   │    │    │ IF │ ID │ EX │ MEM│ WB │    │    │    │    │    │
     │    │    │    │ FA │ FB │    │    │    │    │    │    │    │
─────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────
I4   │    │    │    │ IF │ ID │ EX │ MEM│ WB │    │    │    │    │
     │    │    │    │    │ FA │ FB │    │    │    │    │    │    │
─────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────
I5   │    │    │    │    │ IF │ ID │ EX │ MEM│ WB │    │    │    │
─────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────
I6   │    │    │    │    │    │ IF │ ID │ EX │ MEM│ WB │    │    │
─────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────┼────
I7   │    │    │    │    │    │    │ IF │ ID │ ID │ EX │ MEM│ WB │
     │    │    │    │    │    │    │    │    │ STALL│    │    │    │
─────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────┴────

图例: FA = Forward A, FB = Forward B, STALL = 暂停

说明:
- 周期4: I3的ID阶段使用转发获取I1的结果
- 周期5: I3的EX阶段使用转发获取I2的结果
- 周期11: I7因Load-Use冒险暂停一个周期
```

### 8.3 总结

流水线处理器的优势:

1. 提高指令吞吐量 (理想情况下每周期一条指令)
2. 提高时钟频率 (每级逻辑更简单)
3. 更好的资源利用率

设计挑战:

1. 数据冒险: 需要转发和暂停机制
2. 控制冒险: 需要分支预测和刷新机制
3. 结构冒险: 需要足够的硬件资源

---

**文档结束**


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
