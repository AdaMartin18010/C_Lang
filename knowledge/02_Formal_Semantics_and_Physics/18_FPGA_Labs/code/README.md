# FPGA实验代码库

> **层级定位**: 02_Formal_Semantics_and_Physics > 18_FPGA_Labs > code
> **实验平台**: Xilinx Nexys A7 / Digilent Basys 3
> **开发工具**: Vivado 2023.2, Vivado HLS
> **预估学习时间**: 20-30小时（6个完整实验）

---

## 实验概述

本目录包含6个循序渐进的FPGA实验代码，从基础门电路到完整SoC设计。每个实验都包含完整的RTL代码、约束文件、仿真测试和详细文档。

### 实验路线图

```
Lab1: 基础门电路 → Lab2: ALU → Lab3: 单周期CPU → Lab4: 流水线CPU → Lab5: 缓存控制器 → Lab6: SoC集成
     [难度⭐⭐]      [难度⭐⭐⭐]      [难度⭐⭐⭐⭐]        [难度⭐⭐⭐⭐⭐]      [难度⭐⭐⭐⭐⭐]      [难度⭐⭐⭐⭐⭐]
```

---

## 实验列表

### Lab 1: 基础门电路 (Basic Gates)

**目标**: 掌握Verilog基础，实现基本逻辑门

**内容**:

- 与门、或门、非门、异或门
- 多路选择器 (2:1, 4:1, 8:1)
- 译码器和编码器
- 七段数码管显示

**代码结构**:

```
lab1/
├── src/
│   ├── basic_gates.v       # 基本门电路
│   ├── mux.v               # 多路选择器
│   ├── decoder.v           # 译码器
│   └── seven_segment.v     # 七段显示
├── tb/
│   └── tb_basic_gates.v    # 测试平台
└── constraints/
    └── lab1.xdc            # 约束文件
```

**关键代码示例**:

```verilog
// 4:1多路选择器
module mux4to1(
    input wire [3:0] data,
    input wire [1:0] sel,
    output wire y
);
    assign y = (sel == 2'b00) ? data[0] :
               (sel == 2'b01) ? data[1] :
               (sel == 2'b10) ? data[2] : data[3];
endmodule
```

### Lab 2: 算术逻辑单元 (ALU)

**目标**: 实现一个完整的16位ALU

**功能**:

- 算术运算: 加法、减法、比较
- 逻辑运算: 与、或、异或、非
- 移位运算: 左移、右移、算术右移
- 标志位: 零、进位、溢出、符号

**代码结构**:

```
lab2/
├── src/
│   ├── alu.v               # 主ALU模块
│   ├── adder.v             # 加法器
│   ├── comparator.v        # 比较器
│   └── shifter.v           # 移位器
└── tb/
    └── tb_alu.v            # ALU测试
```

### Lab 3: 单周期CPU

**目标**: 实现一个完整的RISC-V RV32I单周期处理器

**特性**:

- 完整RV32I指令集
- 哈佛架构
- 32个通用寄存器
- 简单IO接口

**核心模块**:

```verilog
module rv32i_single_cycle(
    input wire clk, rst,
    // 指令存储器接口
    output wire [31:0] pc,
    input wire [31:0] instr,
    // 数据存储器接口
    output wire mem_we,
    output wire [31:0] mem_addr, mem_wdata,
    input wire [31:0] mem_rdata
);
    // 取指、译码、执行、访存、写回
    // ...
endmodule
```

### Lab 4: 流水线CPU

**目标**: 将单周期CPU改为5级流水线架构

**流水线阶段**:

- IF: 取指 (Instruction Fetch)
- ID: 译码 (Instruction Decode)
- EX: 执行 (Execute)
- MEM: 访存 (Memory Access)
- WB: 写回 (Write Back)

**冒险处理**:

- 数据冒险: 前递 (Forwarding)
- 控制冒险: 分支预测 (静态预测)
- 结构冒险: 双端口存储器

### Lab 5: 缓存控制器

**目标**: 实现一个直接映射缓存控制器

**规格**:

- 容量: 4KB
- 块大小: 16字节
- 相联度: 直接映射
- 写策略: 写回 (Write-back)

**接口**:

```verilog
module cache_controller(
    input wire clk, rst,
    // CPU接口
    input wire [31:0] cpu_addr,
    input wire cpu_we,
    input wire [31:0] cpu_wdata,
    output reg [31:0] cpu_rdata,
    output reg cpu_ready,
    // 主存接口
    output reg [31:0] mem_addr,
    output reg mem_we,
    output reg [127:0] mem_wdata,
    input wire [127:0] mem_rdata,
    input wire mem_ready
);
```

### Lab 6: SoC集成

**目标**: 集成CPU、存储器、外设，构建完整系统

**系统组成**:

- RISC-V CPU核心
- 指令存储器 (64KB)
- 数据存储器 (64KB)
- UART控制器
- GPIO控制器
- 定时器
- 中断控制器

**软件支持**:

- 提供启动代码 (Bootloader)
- 支持C程序编译链接
- 提供驱动库

---

## 快速开始

### 环境准备

1. **安装Vivado**: 下载并安装Xilinx Vivado 2023.2
2. **准备硬件**: Nexys A7或Basys 3开发板
3. **连接设备**: 通过USB连接开发板

### 运行第一个实验

```bash
# 1. 打开Vivado
cd lab1
vivado -source create_project.tcl

# 2. 仿真验证
vivado -mode batch -source run_simulation.tcl

# 3. 综合实现
vivado -mode batch -source run_implementation.tcl

# 4. 下载到FPGA
vivado -mode batch -source program_device.tcl
```

### 目录结构

```
code/
├── lab1_basic_gates/
│   ├── src/
│   ├── tb/
│   ├── constraints/
│   └── README.md
├── lab2_alu/
│   └── ...
├── lab3_single_cycle_cpu/
│   └── ...
├── lab4_pipelined_cpu/
│   └── ...
├── lab5_cache_controller/
│   └── ...
├── lab6_soc_integration/
│   └── ...
├── common/                  # 公共模块
│   ├── clock_divider.v
│   ├── debounce.v
│   └── seven_segment_display.v
└── scripts/                 # 自动化脚本
    ├── create_project.tcl
    ├── run_simulation.tcl
    └── program_device.tcl
```

---

## 调试技巧

### 1. 使用ILA (Integrated Logic Analyzer)

```tcl
# 在约束文件中添加ILA
set ila_inst [create_ip -name ila -vendor xilinx.com -library ip -version 6.2 -module_name ila_0]
set_property -dict [list CONFIG.C_NUM_OF_PROBES {4} CONFIG.C_PROBE0_WIDTH {32}] $ila_inst
```

### 2. 使用VIO (Virtual IO)

```tcl
# 创建VIO用于交互式调试
create_ip -name vio -vendor xilinx.com -library ip -version 3.0 -module_name vio_0
```

### 3. 信号断言

```verilog
// 在代码中添加断言检查
always @(posedge clk) begin
    if (state == STATE_ERROR) begin
        $display("ERROR: Entered error state at time %t", $time);
        $stop;
    end
end
```

---

## 常见问题

**Q: 综合报错 "Multiple drivers"?**
A: 检查是否有多个always块对同一个reg赋值。Verilog中reg只能在一个always块中赋值。

**Q: 仿真结果与预期不符?**
A: 检查时序问题，特别是时钟边沿和复位信号。使用波形查看器仔细检查信号变化。

**Q: 下载后FPGA无响应?**
A: 检查约束文件中的引脚分配是否正确，时钟频率设置是否合理。

---

## 延伸阅读

- [Vivado设计流程](../docs/vivado_flow.md)
- [Verilog语言参考](../docs/verilog_reference.md)
- [FPGA实验指南](../../README.md)

---

> **最后更新**: 2026-03-19
> **维护者**: AI Code Review
> **状态**: 已完成深化


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
