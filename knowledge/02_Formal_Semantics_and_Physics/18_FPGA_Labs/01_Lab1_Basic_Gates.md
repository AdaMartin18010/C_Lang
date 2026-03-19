# 实验1：FPGA基本逻辑门实现

## 目录

- [实验1：FPGA基本逻辑门实现](#实验1fpga基本逻辑门实现)
  - [目录](#目录)
  - [实验概述](#实验概述)
  - [实验目标](#实验目标)
  - [硬件平台介绍](#硬件平台介绍)
    - [Basys3 开发板 (Artix-7)](#basys3-开发板-artix-7)
      - [主要规格](#主要规格)
      - [板载外设](#板载外设)
      - [时钟系统](#时钟系统)
      - [引脚分配 (XDC约束文件)](#引脚分配-xdc约束文件)
    - [Nexys4 DDR 开发板](#nexys4-ddr-开发板)
  - [开发环境搭建](#开发环境搭建)
    - [第一步：下载和安装Vivado](#第一步下载和安装vivado)
      - [系统要求](#系统要求)
      - [下载步骤](#下载步骤)
      - [安装过程](#安装过程)
      - [许可证配置](#许可证配置)
    - [第二步：安装板卡支持文件](#第二步安装板卡支持文件)
    - [第三步：验证安装](#第三步验证安装)
  - [实验1-1：LED闪烁项目](#实验1-1led闪烁项目)
    - [设计原理](#设计原理)
    - [完整Verilog代码](#完整verilog代码)
    - [创建Vivado工程](#创建vivado工程)
      - [步骤1：创建新工程](#步骤1创建新工程)
      - [步骤2：选择目标器件](#步骤2选择目标器件)
      - [步骤3：添加源文件](#步骤3添加源文件)
      - [步骤4：编写约束文件 (XDC)](#步骤4编写约束文件-xdc)
    - [综合与实现](#综合与实现)
    - [下载到FPGA](#下载到fpga)
      - [方法1：通过Vivado下载](#方法1通过vivado下载)
      - [方法2：通过命令行下载](#方法2通过命令行下载)
  - [实验1-2：拨码开关控制LED](#实验1-2拨码开关控制led)
    - [设计原理](#设计原理-1)
    - [完整Verilog代码](#完整verilog代码-1)
    - [约束文件](#约束文件)
  - [实验1-3：基本门电路实现](#实验1-3基本门电路实现)
    - [真值表回顾](#真值表回顾)
    - [完整Verilog代码](#完整verilog代码-2)
    - [扩展：使用2输入产生4输入门](#扩展使用2输入产生4输入门)
  - [实验1-4：七段数码管显示](#实验1-4七段数码管显示)
    - [七段数码管原理](#七段数码管原理)
    - [完整Verilog代码](#完整verilog代码-3)
    - [顶层模块集成](#顶层模块集成)
  - [综合实验：简易计算器](#综合实验简易计算器)
    - [设计规格](#设计规格)
    - [完整代码](#完整代码)
  - [故障排除指南](#故障排除指南)
    - [常见问题及解决方案](#常见问题及解决方案)
      - [问题1：Vivado安装失败](#问题1vivado安装失败)
      - [问题2：无法识别开发板](#问题2无法识别开发板)
      - [问题3：综合/实现报错](#问题3综合实现报错)
      - [问题4：下载后无反应](#问题4下载后无反应)
      - [问题5：LED不亮或乱闪](#问题5led不亮或乱闪)
  - [实验报告模板](#实验报告模板)
    - [仿真结果](#仿真结果)
    - [上板验证](#上板验证)
    - [3.2 基本门电路实验](#32-基本门电路实验)
      - [真值表验证](#真值表验证)
    - [3.3 数码管显示实验](#33-数码管显示实验)
      - [扫描频率计算](#扫描频率计算)
      - [显示效果](#显示效果)
  - [四、遇到的问题及解决方案](#四遇到的问题及解决方案)
  - [五、思考题](#五思考题)
    - [5.1 如何将LED闪烁频率改为2Hz？](#51-如何将led闪烁频率改为2hz)
    - [5.2 为什么需要数码管动态扫描？](#52-为什么需要数码管动态扫描)
    - [5.3 解释同步复位和异步复位的区别](#53-解释同步复位和异步复位的区别)
  - [六、实验总结](#六实验总结)
  - [附录：代码清单](#附录代码清单)
    - [练习2：流水灯](#练习2流水灯)
    - [练习3：按键消抖](#练习3按键消抖)
    - [练习4：BCD码转换](#练习4bcd码转换)
  - [参考资料](#参考资料)
  - [版本历史](#版本历史)

---

## 实验概述

本实验是FPGA系列的入门实验，旨在帮助学习者掌握FPGA开发的基本流程。通过本实验，你将学会：

- 安装和配置Vivado开发环境
- 创建和管理FPGA工程项目
- 编写基本的Verilog HDL代码
- 进行功能仿真和时序约束
- 生成比特流文件并下载到FPGA
- 使用板载资源（LED、开关、数码管）

## 实验目标

完成本实验后，学习者应能够：

1. **环境搭建**：独立完成Vivado开发环境的安装和配置
2. **项目创建**：创建完整的Vivado工程，包括源文件、约束文件和仿真文件
3. **基础编程**：使用Verilog HDL描述组合逻辑和时序逻辑电路
4. **硬件调试**：在Basys3/Nexys4开发板上验证设计功能
5. **问题排查**：识别和解决常见的FPGA开发问题

---

## 硬件平台介绍

### Basys3 开发板 (Artix-7)

Basys3是Digilent公司推出的基于Xilinx Artix-7 FPGA的入门级开发板，非常适合教学和初学者使用。

#### 主要规格

| 组件 | 规格 |
|------|------|
| FPGA芯片 | Xilinx Artix-7 XC7A35T-1CPG236C |
| 逻辑单元 | 33,280个 |
| 触发器 | 41,600个 |
| Block RAM | 1,800 Kbits |
| 时钟资源 | 5个时钟管理模块 |
| I/O引脚 | 106个用户可用 |

#### 板载外设

```
┌─────────────────────────────────────────────────────────────┐
│                    Basys3 开发板布局                          │
├─────────────────────────────────────────────────────────────┤
│  [USB电源/编程]        [FPGA芯片]        [USB-UART]           │
│       │                   │                   │              │
│       ▼                   ▼                   ▼              │
│  ┌─────────┐        ┌─────────┐        ┌─────────┐          │
│  │ USB接口  │        │ Artix-7 │        │ UART接口 │          │
│  └─────────┘        └─────────┘        └─────────┘          │
│                                                             │
│  [16个拨码开关SW0-SW15]                                      │
│  ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐                          │
│  │0│1│2│3│4│5│6│7│8│9│10│11│12│13│14│15│                          │
│  └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘                          │
│                                                             │
│  [16个LED灯LD0-LD15]                                         │
│  ┌─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┬─┐                          │
│  │○│○│○│○│○│○│○│○│○│○│○ │○ │○ │○ │○ │○ │                          │
│  └─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┴─┘                          │
│                                                             │
│  [RGB LED] [5个按钮]                                         │
│   ┌───┐   ┌─┬─┬─┬─┬─┐                                       │
│   │RGB│   │U│D│L│R│C│                                       │
│   └───┘   └─┴─┴─┴─┴─┘                                       │
│                                                             │
│  [4位七段数码管]          [PMOD接口]                          │
│  ┌─────┐                ┌─────┐ ┌─────┐                     │
│  │8888 │                │PMODA│ │PMODB│                     │
│  └─────┘                └─────┘ └─────┘                     │
│                                                             │
│  [XADC] [配置存储器]          [VGA接口]                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

#### 时钟系统

Basys3板载100MHz晶振，通过FPGA内部PLL可以生成各种频率的时钟信号。

```
时钟架构：
100MHz 晶振 ──→ MMCM/PLL ──→ 系统时钟
                  │
                  ├──→ 分频器 ──→ 低频时钟 (用于LED闪烁等)
                  │
                  └──→ 高频时钟 ──→ 高速接口
```

#### 引脚分配 (XDC约束文件)

Basys3的约束文件定义了所有外设与FPGA引脚的连接关系。主要引脚如下：

**时钟引脚**

- 系统时钟：W5 (100MHz)

**开关引脚**

- SW0: V17
- SW1: V16
- SW2: W16
- ... (共16个)

**LED引脚**

- LD0: U16
- LD1: E19
- LD2: U19
- ... (共16个)

**按钮引脚**

- BTNC: U18 (中心)
- BTNU: T18 (上)
- BTND: U17 (下)
- BTNL: W19 (左)
- BTNR: T17 (右)

**数码管引脚**

- 段选：W7, W6, U8, V8, U5, V5, U7 (a-g段)
- 位选：U2, U4, V4, W4 (4个数码管)

### Nexys4 DDR 开发板

Nexys4 DDR是更高级的开发板，拥有更多资源：

- FPGA: Artix-7 XC7A100T-1CSG324C
- 更多逻辑单元 (101,440个)
- 更大存储容量
- 额外外设：温度传感器、加速度计、数字麦克风等

---

## 开发环境搭建

### 第一步：下载和安装Vivado

#### 系统要求

| 项目 | 最低要求 | 推荐配置 |
|------|----------|----------|
| 操作系统 | Windows 10/11 64位 | Windows 11 64位 |
| 处理器 | 4核心 | 8核心以上 |
| 内存 | 8GB | 16GB以上 |
| 硬盘空间 | 50GB空闲 | 100GB SSD |
| 显示器 | 1024x768 | 1920x1080 |

#### 下载步骤

1. 访问Xilinx官网下载页面

   ```
   URL: https://www.xilinx.com/support/download.html
   ```

2. 选择 **Vivado ML Edition**

3. 下载 **Vivado ML Standard** (免费版本包含Device Support)

4. 选择操作系统对应的安装包

#### 安装过程

```bash
# 1. 解压安装包
tar -xvf Xilinx_Unified_2023.2_1013_2256_Lin64.bin.tar

# 2. 运行安装程序 (Windows双击xsetup.exe)
# Linux系统运行：
./xsetup

# 3. 选择安装选项
# - Vivado (而非Vitis)
# - 选择"Vivado ML Standard"
# - 勾选 Artix-7 器件支持
```

**安装选项详细说明**：

```
安装向导步骤：
┌─────────────────────────────────────────────────────────┐
│ 1. 欢迎界面                                              │
│    └── 点击 Next                                         │
│                                                         │
│ 2. 登录Xilinx账号                                        │
│    └── 输入账号密码（免费注册）                            │
│                                                         │
│ 3. 选择产品                                              │
│    ├── ○ Vitis (包含Vivado + 软件开发)                    │
│    ├── ● Vivado (仅硬件设计)  ← 选择此项                  │
│    └── ○ 仅安装设备支持包                                  │
│                                                         │
│ 4. 选择版本                                              │
│    ├── ○ Vivado ML Enterprise (需许可证)                  │
│    └── ● Vivado ML Standard (免费)  ← 选择此项            │
│                                                         │
│ 5. 选择器件                                              │
│    └── ☑ Artix-7  ← 必须勾选                              │
│        ☑ Zynq-7000 (可选)                                 │
│        ☑ Kintex-7 (可选)                                  │
│                                                         │
│ 6. 选择安装路径                                          │
│    └── 建议: C:\Xilinx\Vivado\2023.2                      │
│                                                         │
│ 7. 开始安装 (约30-60分钟)                                 │
└─────────────────────────────────────────────────────────┘
```

#### 许可证配置

Vivado ML Standard使用免费许可证，无需额外配置。首次启动时会自动生成WebPACK许可证。

```
许可证类型：
- WebPACK License: 免费，支持Artix-7、Spartan-7等
- Device Locked License: 绑定特定器件
- Floating License: 企业网络许可证
```

### 第二步：安装板卡支持文件

为了让Vivado识别Basys3/Nexys4板卡，需要安装板卡定义文件。

```bash
# 1. 下载Digilent板卡文件
git clone https://github.com/Digilent/vivado-boards.git

# 2. 复制到Vivado安装目录
cd vivado-boards
xcopy /E new\board_files C:\Xilinx\Vivado\2023.2\data\boards\board_files\

# 3. 重启Vivado
```

### 第三步：验证安装

1. **启动Vivado**

   ```
   开始菜单 → Xilinx Design Tools → Vivado 2023.2 → Vivado 2023.2
   ```

2. **检查版本信息**

   ```
   Help → About
   确认版本号: 2023.2
   ```

3. **测试创建工程**

   ```
   File → New Project → Next
   如果能正常创建工程，说明安装成功
   ```

---

## 实验1-1：LED闪烁项目

这是FPGA开发的"Hello World"项目，通过让LED灯周期性闪烁，验证开发环境是否正常工作。

### 设计原理

```
时钟分频原理：
┌─────────┐     ┌─────────────┐     ┌─────────┐
│ 100MHz  │────→│ 分频计数器   │────→│  1Hz    │
│ 晶振    │     │ (计数到50M)  │     │ LED信号 │
└─────────┘     └─────────────┘     └─────────┘

时序图：
时钟:  _|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_|‾|_ (100MHz)
计数:  0→1→2→...→49_999_999→0→1→...
LED:   ‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾‾|_______________|‾‾‾‾‾ (1Hz, 50%占空比)
```

### 完整Verilog代码

创建文件 `code/lab1/blink_led.v`：

```verilog
//============================================================================
// 模块名称: blink_led
// 功能描述: 基础LED闪烁模块
// 作者: FPGA Lab
// 版本: 1.0
// 日期: 2024
//============================================================================

`timescale 1ns / 1ps

module blink_led (
    input  wire clk,        // 系统时钟 (100MHz)
    input  wire rst_n,      // 复位信号 (低电平有效)
    output reg  led         // LED输出
);

    //========================================================================
    // 参数定义
    //========================================================================
    // 时钟频率: 100MHz = 100,000,000 Hz
    // 目标闪烁频率: 1Hz (每秒闪烁一次)
    // 计数器最大值 = 100,000,000 / 2 - 1 = 49,999,999
    // 除以2是因为需要50M个周期亮，50M个周期灭

    parameter CLK_FREQ = 100_000_000;   // 系统时钟频率
    parameter BLINK_FREQ = 1;            // LED闪烁频率
    parameter MAX_COUNT = CLK_FREQ / (2 * BLINK_FREQ) - 1;

    //========================================================================
    // 寄存器定义
    //========================================================================
    reg [31:0] counter;     // 32位计数器，足够容纳50M

    //========================================================================
    // 计数器逻辑 (时序逻辑)
    //========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // 异步复位：计数器清零
            counter <= 32'd0;
        end
        else if (counter >= MAX_COUNT) begin
            // 计数到达最大值，归零
            counter <= 32'd0;
        end
        else begin
            // 正常计数
            counter <= counter + 32'd1;
        end
    end

    //========================================================================
    // LED输出逻辑 (时序逻辑)
    //========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            // 异步复位：LED熄灭
            led <= 1'b0;
        end
        else if (counter >= MAX_COUNT) begin
            // 计数到达最大值，LED状态翻转
            led <= ~led;
        end
        // 其他时刻保持当前状态
    end

endmodule
```

### 创建Vivado工程

#### 步骤1：创建新工程

```
1. 启动Vivado
2. File → New Project → Next
3. 设置工程名称: lab1_blink_led
4. 设置工程路径: E:/FPGA_Projects/lab1
5. 勾选 "Create project subdirectory"
6. 点击 Next

工程类型选择：
┌─────────────────────────────────────────┐
│  RTL Project  ← 选择此项                 │
│  ☑ Do not specify sources at this time  │
└─────────────────────────────────────────┘
```

#### 步骤2：选择目标器件

```
Default Part 选择：
- 如果安装了板卡文件:
  Boards → Digilent → Basys3 → xc7a35tcpg236-1

- 如果手动选择器件:
  Parts → 搜索 "xc7a35tcpg236-1"
  Family: Artix-7
  Package: cpg236
  Speed: -1
```

#### 步骤3：添加源文件

```
1. 在Sources窗口中右键点击 "Design Sources"
2. Add Sources → Add or create design sources
3. Create File → 命名: blink_led.v
4. 点击 Finish
5. 在弹出的Define Module对话框中定义端口:
   - clk: input
   - rst_n: input
   - led: output
```

#### 步骤4：编写约束文件 (XDC)

创建文件 `xdc/blink_led.xdc`：

```xdc
#============================================================================
# 约束文件: blink_led.xdc
# 目标板卡: Basys3 (Artix-7)
# 功能: LED闪烁项目约束
#============================================================================

#----------------------------------------------------------------------------
# 时钟约束
#----------------------------------------------------------------------------
set_property PACKAGE_PIN W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]

# 创建时钟约束，周期10ns (100MHz)
create_clock -period 10.000 -name sys_clk_pin -waveform {0.000 5.000} [get_ports clk]

#----------------------------------------------------------------------------
# 复位按钮约束
#----------------------------------------------------------------------------
set_property PACKAGE_PIN U18 [get_ports rst_n]
set_property IOSTANDARD LVCMOS33 [get_ports rst_n]

# 按钮需要下拉电阻，确保未按下时为低电平
set_property PULLDOWN true [get_ports rst_n]

#----------------------------------------------------------------------------
# LED约束
#----------------------------------------------------------------------------
# LD0 - 使用第一个LED
set_property PACKAGE_PIN U16 [get_ports led]
set_property IOSTANDARD LVCMOS33 [get_ports led]

# 所有16个LED的引脚定义 (备用)
# set_property PACKAGE_PIN E19 [get_ports {led[1]}]
# set_property PACKAGE_PIN U19 [get_ports {led[2]}]
# set_property PACKAGE_PIN V19 [get_ports {led[3]}]
# set_property PACKAGE_PIN W18 [get_ports {led[4]}]
# set_property PACKAGE_PIN U15 [get_ports {led[5]}]
# set_property PACKAGE_PIN U14 [get_ports {led[6]}]
# set_property PACKAGE_PIN V14 [get_ports {led[7]}]
# set_property PACKAGE_PIN V13 [get_ports {led[8]}]
# set_property PACKAGE_PIN V3  [get_ports {led[9]}]
# set_property PACKAGE_PIN W3  [get_ports {led[10]}]
# set_property PACKAGE_PIN U3  [get_ports {led[11]}]
# set_property PACKAGE_PIN P3  [get_ports {led[12]}]
# set_property PACKAGE_PIN N3  [get_ports {led[13]}]
# set_property PACKAGE_PIN P1  [get_ports {led[14]}]
# set_property PACKAGE_PIN L1  [get_ports {led[15]}]

#----------------------------------------------------------------------------
# 配置设置
#----------------------------------------------------------------------------
# 配置电压
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]

# 配置模式：SPIx4
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 33 [current_design]
set_property BITSTREAM.CONFIG.SPI_BUSWIDTH 4 [current_design]
```

添加约束文件到工程：

```
Sources窗口 → 右键 "Constraints" → Add Sources
→ Add or create constraints → Add Files
→ 选择 blink_led.xdc
```

### 综合与实现

```
1. 点击左侧 "Run Synthesis" 按钮
   - 等待综合完成 (约1-3分钟)
   - 查看综合报告，确认无关键警告

2. 点击 "Run Implementation"
   - 等待实现完成 (约2-5分钟)
   - 查看实现报告，检查时序是否收敛

3. 点击 "Generate Bitstream"
   - 等待比特流生成 (约1-2分钟)
   - 生成的.bit文件在: lab1_blink_led.runs/impl_1/blink_led.bit
```

### 下载到FPGA

#### 方法1：通过Vivado下载

```
1. 连接Basys3开发板到电脑USB端口
2. 开发板上电 (JP1跳线设置为USB供电)
3. 点击 "Open Hardware Manager"
4. 点击 "Open Target" → "Auto Connect"
5. 右键设备 → "Program Device"
6. 选择生成的.bit文件
7. 点击 "Program"

预期结果：
- LD0 LED开始以1Hz频率闪烁 (每秒亮灭一次)
- 按下BTNC按钮 (rst_n)，LED复位熄灭
```

#### 方法2：通过命令行下载

```tcl
# 打开Vivado Tcl Shell
open_hw_manager
connect_hw_server
open_hw_target
set_property PROGRAM.FILE {E:/FPGA_Projects/lab1/lab1_blink_led.runs/impl_1/blink_led.bit} [get_hw_devices xc7a35t_0]
program_hw_devices [get_hw_devices xc7a35t_0]
```

---

## 实验1-2：拨码开关控制LED

本实验实现拨码开关与LED的直接连接，验证输入输出接口。

### 设计原理

```
连接关系：
SW0 ────→ LD0
SW1 ────→ LD1
SW2 ────→ LD2
 ...       ...
SW15 ───→ LD15

真值表：
SW  | LED
----|-----
 0  |  0 (熄灭)
 1  |  1 (点亮)
```

### 完整Verilog代码

创建文件 `code/lab1/switch_led.v`：

```verilog
//============================================================================
// 模块名称: switch_led
// 功能描述: 16位拨码开关控制16位LED
//============================================================================

`timescale 1ns / 1ps

module switch_led (
    input  wire [15:0] sw,      // 16个拨码开关输入
    output wire [15:0] led      // 16个LED输出
);

    // 直接连接开关和LED
    // 当SW为1时，对应LED点亮
    assign led = sw;

endmodule
```

### 约束文件

创建文件 `xdc/switch_led.xdc`：

```xdc
#============================================================================
# 约束文件: switch_led.xdc
#============================================================================

#----------------------------------------------------------------------------
# 时钟约束 (即使不使用，也建议添加)
#----------------------------------------------------------------------------
set_property PACKAGE_PIN W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -period 10.000 -name sys_clk_pin [get_ports clk]

#----------------------------------------------------------------------------
# 16个拨码开关约束
#----------------------------------------------------------------------------
set_property PACKAGE_PIN V17 [get_ports {sw[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[0]}]

set_property PACKAGE_PIN V16 [get_ports {sw[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[1]}]

set_property PACKAGE_PIN W16 [get_ports {sw[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[2]}]

set_property PACKAGE_PIN W17 [get_ports {sw[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[3]}]

set_property PACKAGE_PIN W15 [get_ports {sw[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[4]}]

set_property PACKAGE_PIN V15 [get_ports {sw[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[5]}]

set_property PACKAGE_PIN W14 [get_ports {sw[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[6]}]

set_property PACKAGE_PIN W13 [get_ports {sw[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[7]}]

set_property PACKAGE_PIN V2  [get_ports {sw[8]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[8]}]

set_property PACKAGE_PIN T3  [get_ports {sw[9]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[9]}]

set_property PACKAGE_PIN T2  [get_ports {sw[10]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[10]}]

set_property PACKAGE_PIN R3  [get_ports {sw[11]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[11]}]

set_property PACKAGE_PIN W2  [get_ports {sw[12]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[12]}]

set_property PACKAGE_PIN U1  [get_ports {sw[13]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[13]}]

set_property PACKAGE_PIN T1  [get_ports {sw[14]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[14]}]

set_property PACKAGE_PIN R2  [get_ports {sw[15]}]
set_property IOSTANDARD LVCMOS33 [get_ports {sw[15]}]

#----------------------------------------------------------------------------
# 16个LED约束
#----------------------------------------------------------------------------
set_property PACKAGE_PIN U16 [get_ports {led[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[0]}]

set_property PACKAGE_PIN E19 [get_ports {led[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[1]}]

set_property PACKAGE_PIN U19 [get_ports {led[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[2]}]

set_property PACKAGE_PIN V19 [get_ports {led[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[3]}]

set_property PACKAGE_PIN W18 [get_ports {led[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[4]}]

set_property PACKAGE_PIN U15 [get_ports {led[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[5]}]

set_property PACKAGE_PIN U14 [get_ports {led[6]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[6]}]

set_property PACKAGE_PIN V14 [get_ports {led[7]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[7]}]

set_property PACKAGE_PIN V13 [get_ports {led[8]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[8]}]

set_property PACKAGE_PIN V3  [get_ports {led[9]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[9]}]

set_property PACKAGE_PIN W3  [get_ports {led[10]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[10]}]

set_property PACKAGE_PIN U3  [get_ports {led[11]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[11]}]

set_property PACKAGE_PIN P3  [get_ports {led[12]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[12]}]

set_property PACKAGE_PIN N3  [get_ports {led[13]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[13]}]

set_property PACKAGE_PIN P1  [get_ports {led[14]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[14]}]

set_property PACKAGE_PIN L1  [get_ports {led[15]}]
set_property IOSTANDARD LVCMOS33 [get_ports {led[15]}]

#----------------------------------------------------------------------------
# 配置设置
#----------------------------------------------------------------------------
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
```

---

## 实验1-3：基本门电路实现

本实验实现所有基本逻辑门：与门、或门、非门、异或门、同或门、与非门、或非门。

### 真值表回顾

```
与门 (AND):          或门 (OR):           非门 (NOT):
A B | Y              A B | Y              A | Y
----|---             ----|---             --|---
0 0 | 0              0 0 | 0              0 | 1
0 1 | 0              0 1 | 1              1 | 0
1 0 | 0              1 0 | 1
1 1 | 1              1 1 | 1

异或门 (XOR):        与非门 (NAND):       或非门 (NOR):
A B | Y              A B | Y              A B | Y
----|---             ----|---             ----|---
0 0 | 0              0 0 | 1              0 0 | 1
0 1 | 1              0 1 | 1              0 1 | 0
1 0 | 1              1 0 | 1              1 0 | 0
1 1 | 0              1 1 | 0              1 1 | 0
```

### 完整Verilog代码

创建文件 `code/lab1/basic_gates.v`：

```verilog
//============================================================================
// 模块名称: basic_gates
// 功能描述: 实现7种基本逻辑门
//============================================================================

`timescale 1ns / 1ps

module basic_gates (
    input  wire [1:0] sw,       // 2位开关输入 (A=sw[1], B=sw[0])
    output wire [6:0] led       // 7个LED显示各门电路输出
);

    // 输入分配
    wire a = sw[1];     // 第一个输入
    wire b = sw[0];     // 第二个输入

    // 输出分配
    // led[0]: 与门 (AND)
    // led[1]: 或门 (OR)
    // led[2]: 非门 (NOT, 对A取反)
    // led[3]: 异或门 (XOR)
    // led[4]: 同或门 (XNOR)
    // led[5]: 与非门 (NAND)
    // led[6]: 或非门 (NOR)

    // 与门: Y = A & B
    assign led[0] = a & b;

    // 或门: Y = A | B
    assign led[1] = a | b;

    // 非门: Y = ~A (对A取反)
    assign led[2] = ~a;

    // 异或门: Y = A ^ B
    assign led[3] = a ^ b;

    // 同或门: Y = ~(A ^ B)
    assign led[4] = ~(a ^ b);

    // 与非门: Y = ~(A & B)
    assign led[5] = ~(a & b);

    // 或非门: Y = ~(A | B)
    assign led[6] = ~(a | b);

endmodule
```

### 扩展：使用2输入产生4输入门

创建文件 `code/lab1/multi_input_gates.v`：

```verilog
//============================================================================
// 模块名称: multi_input_gates
// 功能描述: 4输入基本逻辑门实现
//============================================================================

`timescale 1ns / 1ps

module multi_input_gates (
    input  wire [3:0] sw,       // 4位开关输入
    output wire [5:0] led       // 6个LED输出
);

    // 输入
    wire a = sw[3];
    wire b = sw[2];
    wire c = sw[1];
    wire d = sw[0];

    // led[0]: 4输入与门
    assign led[0] = a & b & c & d;

    // led[1]: 4输入或门
    assign led[1] = a | b | c | d;

    // led[2]: 4输入异或门
    assign led[2] = a ^ b ^ c ^ d;

    // led[3]: 4输入与非门
    assign led[3] = ~(a & b & c & d);

    // led[4]: 4输入或非门
    assign led[4] = ~(a | b | c | d);

    // led[5]: 多数表决器 (3个或以上为1则输出1)
    assign led[5] = (a & b & c) | (a & b & d) | (a & c & d) | (b & c & d);

endmodule
```

---

## 实验1-4：七段数码管显示

本实验学习如何使用七段数码管显示数字0-9。

### 七段数码管原理

```
    ┌───┐ a
    │   │
   f│   │b
    ├───┤ g
    │   │
   e│   │c
    ├───┘ d

段码真值表 (共阳极，低电平点亮):
数字 | a b c d e f g | 十六进制
-----|-------------|----------
  0  | 0 0 0 0 0 0 1 | 0x40
  1  | 1 0 0 1 1 1 1 | 0x79
  2  | 0 0 1 0 0 1 0 | 0x24
  3  | 0 0 0 0 1 1 0 | 0x30
  4  | 1 0 0 1 1 0 0 | 0x19
  5  | 0 1 0 0 1 0 0 | 0x12
  6  | 0 1 0 0 0 0 0 | 0x02
  7  | 0 0 0 1 1 1 1 | 0x78
  8  | 0 0 0 0 0 0 0 | 0x00
  9  | 0 0 0 0 1 0 0 | 0x10
```

### 完整Verilog代码

创建文件 `code/lab1/seven_segment.v`：

```verilog
//============================================================================
// 模块名称: seven_segment
// 功能描述: 4位七段数码管驱动
//============================================================================

`timescale 1ns / 1ps

module seven_segment (
    input  wire        clk,         // 100MHz系统时钟
    input  wire        rst_n,       // 复位
    input  wire [15:0] display_data,// 要显示的16位数据
    output reg  [6:0]  seg,         // 段选输出 (a-g)
    output reg  [3:0]  an           // 位选输出 (4个数码管)
);

    //========================================================================
    // 参数定义
    //========================================================================
    parameter CLK_DIV = 16'd50000;   // 分频系数，1kHz扫描频率

    // 段码表 (共阳极，低电平点亮)
    // 对应: {g, f, e, d, c, b, a}
    localparam [6:0] SEG_0 = 7'b1000000;  // 0
    localparam [6:0] SEG_1 = 7'b1111001;  // 1
    localparam [6:0] SEG_2 = 7'b0100100;  // 2
    localparam [6:0] SEG_3 = 7'b0110000;  // 3
    localparam [6:0] SEG_4 = 7'b0011001;  // 4
    localparam [6:0] SEG_5 = 7'b0010010;  // 5
    localparam [6:0] SEG_6 = 7'b0000010;  // 6
    localparam [6:0] SEG_7 = 7'b1111000;  // 7
    localparam [6:0] SEG_8 = 7'b0000000;  // 8
    localparam [6:0] SEG_9 = 7'b0010000;  // 9
    localparam [6:0] SEG_A = 7'b0001000;  // A
    localparam [6:0] SEG_B = 7'b0000011;  // b
    localparam [6:0] SEG_C = 7'b1000110;  // C
    localparam [6:0] SEG_D = 7'b0100001;  // d
    localparam [6:0] SEG_E = 7'b0000110;  // E
    localparam [6:0] SEG_F = 7'b0001110;  // F
    localparam [6:0] SEG_OFF = 7'b1111111; // 全灭

    //========================================================================
    // 寄存器定义
    //========================================================================
    reg [15:0] clk_div;      // 时钟分频计数器
    reg [1:0]  scan_cnt;     // 扫描计数器 (0-3)
    reg [3:0]  digit;        // 当前位要显示的数字

    //========================================================================
    // 时钟分频 (生成1kHz扫描时钟)
    //========================================================================
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            clk_div <= 16'd0;
            scan_cnt <= 2'd0;
        end
        else if (clk_div >= CLK_DIV) begin
            clk_div <= 16'd0;
            scan_cnt <= scan_cnt + 2'd1;
        end
        else begin
            clk_div <= clk_div + 16'd1;
        end
    end

    //========================================================================
    // 位选控制
    //========================================================================
    always @(*) begin
        case (scan_cnt)
            2'd0: an = 4'b1110;  // 选中最右边数码管 (AN0)
            2'd1: an = 4'b1101;  // 选中AN1
            2'd2: an = 4'b1011;  // 选中AN2
            2'd3: an = 4'b0111;  // 选中最左边数码管 (AN3)
            default: an = 4'b1111; // 全部关闭
        endcase
    end

    //========================================================================
    // 取出当前位要显示的数字
    //========================================================================
    always @(*) begin
        case (scan_cnt)
            2'd0: digit = display_data[3:0];    // 最低4位
            2'd1: digit = display_data[7:4];
            2'd2: digit = display_data[11:8];
            2'd3: digit = display_data[15:12];  // 最高4位
            default: digit = 4'd0;
        endcase
    end

    //========================================================================
    // 段码译码
    //========================================================================
    always @(*) begin
        case (digit)
            4'd0:  seg = SEG_0;
            4'd1:  seg = SEG_1;
            4'd2:  seg = SEG_2;
            4'd3:  seg = SEG_3;
            4'd4:  seg = SEG_4;
            4'd5:  seg = SEG_5;
            4'd6:  seg = SEG_6;
            4'd7:  seg = SEG_7;
            4'd8:  seg = SEG_8;
            4'd9:  seg = SEG_9;
            4'd10: seg = SEG_A;
            4'd11: seg = SEG_B;
            4'd12: seg = SEG_C;
            4'd13: seg = SEG_D;
            4'd14: seg = SEG_E;
            4'd15: seg = SEG_F;
            default: seg = SEG_OFF;
        endcase
    end

endmodule
```

### 顶层模块集成

创建文件 `code/lab1/lab1_top.v`：

```verilog
//============================================================================
// 模块名称: lab1_top
// 功能描述: Lab1综合顶层模块
//============================================================================

`timescale 1ns / 1ps

module lab1_top (
    input  wire        clk,
    input  wire        rst_n,
    input  wire [15:0] sw,
    input  wire [4:0]  btn,         // 5个按钮
    output wire [15:0] led,
    output wire [6:0]  seg,
    output wire [3:0]  an
);

    // 模式选择：使用最高2位开关选择显示模式
    wire [1:0] mode = sw[15:14];
    wire [13:0] data = sw[13:0];

    // 寄存器显示数据
    reg [15:0] display_reg;
    reg [15:0] counter;

    // 计数器 (用于自动计数显示)
    always @(posedge clk or negedge rst_n) begin
        if (!rst_n) begin
            counter <= 16'd0;
        end
        else begin
            counter <= counter + 16'd1;
        end
    end

    // 根据模式选择显示内容
    always @(*) begin
        case (mode)
            2'b00: display_reg = {2'b00, data};        // 显示开关值
            2'b01: display_reg = counter;              // 显示计数器
            2'b10: display_reg = {12'd0, sw[3:0]};     // 显示低4位开关
            2'b11: display_reg = 16'hABCD;             // 显示测试图案
            default: display_reg = 16'd0;
        endcase
    end

    // 实例化数码管驱动
    seven_segment u_seg (
        .clk(clk),
        .rst_n(rst_n),
        .display_data(display_reg),
        .seg(seg),
        .an(an)
    );

    // LED显示开关状态
    assign led = sw;

endmodule
```

---

## 综合实验：简易计算器

结合前面所学，实现一个支持加减法的4位计算器。

### 设计规格

```
功能：4位加法器和减法器
输入：
  - SW[7:4]: 操作数A (4位)
  - SW[3:0]: 操作数B (4位)
  - SW[15]:  运算选择 (0=加法, 1=减法)
输出：
  - 数码管: 显示A (左2位) 和 B (右2位) 或结果
  - LED[8]: 进位/借位标志
  - LED[0]: 零标志
```

### 完整代码

创建文件 `code/lab1/simple_calculator.v`：

```verilog
//============================================================================
// 模块名称: simple_calculator
// 功能描述: 4位加减法计算器
//============================================================================

`timescale 1ns / 1ps

module simple_calculator (
    input  wire        clk,
    input  wire        rst_n,
    input  wire [15:0] sw,
    output wire [15:0] led,
    output wire [6:0]  seg,
    output wire [3:0]  an
);

    //========================================================================
    // 输入定义
    //========================================================================
    wire [3:0] operand_a = sw[7:4];   // 操作数A
    wire [3:0] operand_b = sw[3:0];   // 操作数B
    wire       op_sub = sw[15];       // 运算选择
    wire       show_result = sw[14];  // 显示结果/操作数选择

    //========================================================================
    // 运算逻辑
    //========================================================================
    wire [4:0] add_result = {1'b0, operand_a} + {1'b0, operand_b};
    wire [4:0] sub_result = {1'b0, operand_a} - {1'b0, operand_b};
    wire [4:0] result = op_sub ? sub_result : add_result;

    wire       carry = result[4];     // 进位/借位
    wire       zero = (result[3:0] == 4'd0);  // 零标志

    //========================================================================
    // LED输出
    //========================================================================
    assign led[3:0] = operand_a;      // 显示操作数A
    assign led[7:4] = operand_b;      // 显示操作数B
    assign led[8]   = carry;          // 进位标志
    assign led[9]   = zero;           // 零标志
    assign led[10]  = op_sub;         // 运算模式
    assign led[15:11] = 5'd0;

    //========================================================================
    // 显示数据准备
    //========================================================================
    reg [15:0] display_data;

    always @(*) begin
        if (show_result) begin
            // 显示结果
            display_data = {11'd0, result};
        end
        else begin
            // 显示操作数A和B
            display_data = {operand_a, 4'd0, operand_b};
        end
    end

    //========================================================================
    // 实例化数码管驱动
    //========================================================================
    seven_segment u_seg (
        .clk(clk),
        .rst_n(rst_n),
        .display_data(display_data),
        .seg(seg),
        .an(an)
    );

endmodule
```

---

## 故障排除指南

### 常见问题及解决方案

#### 问题1：Vivado安装失败

**症状**：安装过程中报错，无法完成安装

**可能原因**：

- 磁盘空间不足
- 杀毒软件拦截
- 权限不足

**解决方案**：

```
1. 确保C盘至少有50GB空闲空间
2. 临时关闭杀毒软件
3. 以管理员身份运行安装程序
4. 检查安装日志：%TEMP%/xilinx_install.log
```

#### 问题2：无法识别开发板

**症状**：Hardware Manager中看不到设备

**排查步骤**：

```
1. 检查USB连接线 (必须是数据线，非充电线)
2. 检查开发板电源指示灯
3. 检查设备管理器中的驱动程序
4. 尝试更换USB端口
5. 重新安装Digilent Adept驱动
```

#### 问题3：综合/实现报错

**常见错误1**：未定义端口

```
[Common 17-55] 'get_ports' expects at least one object
```

**解决**：检查约束文件中的引脚名称是否与代码一致

**常见错误2**：时序不满足

```
Timing constraints are not met
```

**解决**：检查时钟约束是否正确，减小设计频率

**常见错误3**：资源不足

```
Place Design failed
```

**解决**：优化代码，或使用更大容量的FPGA

#### 问题4：下载后无反应

**排查流程**：

```
1. 检查比特流文件是否生成成功
2. 确认选择了正确的.bit文件
3. 检查开发板供电是否正常
4. 观察DONE指示灯是否变绿
5. 重新生成比特流并下载
```

#### 问题5：LED不亮或乱闪

**可能原因**：

- 时钟约束错误
- 复位信号未正确连接
- 代码逻辑错误

**调试方法**：

```
1. 在Vivado中打开Schematic视图检查连接
2. 使用ILA (Integrated Logic Analyzer) 抓取信号
3. 检查XDC约束文件中的引脚分配
```

---

## 实验报告模板

创建文件 `reports/lab1_report_template.md`：

```markdown
# FPGA实验报告 - 实验1：基本逻辑门

## 基本信息
- **姓名**: ________________
- **学号**: ________________
- **日期**: ________________
- **实验时长**: ______ 小时

---

## 一、实验目的

1. 掌握Vivado开发环境的使用
2. 学会创建完整的FPGA工程
3. 理解基本逻辑门的Verilog实现
4. 掌握FPGA下载和调试方法

---

## 二、实验设备和环境

| 项目 | 说明 |
|------|------|
| 开发板 | Basys3 / Nexys4 |
| FPGA型号 | XC7A35T-1CPG236C |
| 开发工具 | Vivado 2023.2 |
| 操作系统 | Windows 10/11 |

---

## 三、实验内容

### 3.1 LED闪烁实验

#### 设计思路
[描述你的设计思路，包括时钟分频原理]

#### 代码实现
```verilog
[粘贴关键代码]
```

#### 仿真结果

[描述或截图展示仿真波形]

#### 上板验证

[描述实际运行效果，遇到的问题和解决方法]

---

### 3.2 基本门电路实验

#### 真值表验证

| A | B | AND | OR | NOT_A | XOR |
|---|---|-----|----|-------|-----|
| 0 | 0 |     |    |       |     |
| 0 | 1 |     |    |       |     |
| 1 | 0 |     |    |       |     |
| 1 | 1 |     |    |       |     |

[填写实际测试结果]

---

### 3.3 数码管显示实验

#### 扫描频率计算

- 时钟频率: 100MHz
- 目标扫描频率: ______ Hz
- 分频系数: ______

#### 显示效果

[描述数码管显示效果，是否稳定，有无闪烁]

---

## 四、遇到的问题及解决方案

| 问题 | 原因 | 解决方法 |
|------|------|----------|
|      |      |          |
|      |      |          |

---

## 五、思考题

### 5.1 如何将LED闪烁频率改为2Hz？

[回答]

### 5.2 为什么需要数码管动态扫描？

[回答]

### 5.3 解释同步复位和异步复位的区别

[回答]

---

## 六、实验总结

[总结本次实验的收获，对FPGA开发的理解，以及改进建议]

---

## 附录：代码清单

[附上所有编写的Verilog代码]

```

---

## 扩展练习

### 练习1：呼吸灯效果
实现LED亮度渐变效果（需要使用PWM）

```verilog
// 提示：使用PWM调制LED亮度
module breathing_led (
    input  wire clk,
    input  wire rst_n,
    output wire led
);
    // 实现占空比逐渐变化的PWM
    // 产生"呼吸"效果
endmodule
```

### 练习2：流水灯

实现16个LED依次点亮的流水灯效果

```verilog
module running_led (
    input  wire clk,
    input  wire rst_n,
    output reg  [15:0] led
);
    // 实现流水灯效果
endmodule
```

### 练习3：按键消抖

学习如何实现机械按键的消抖电路

```verilog
module debounce (
    input  wire clk,
    input  wire rst_n,
    input  wire btn_in,
    output reg  btn_out
);
    // 实现20ms消抖
endmodule
```

### 练习4：BCD码转换

实现二进制到BCD码的转换，在数码管上正确显示十进制数

```verilog
module bin_to_bcd (
    input  wire [7:0] bin,
    output reg  [11:0] bcd  // 3位BCD码
);
    // 实现加3算法或双DABble算法
endmodule
```

---

## 参考资料

1. **Vivado用户指南**
   - UG910: Vivado Design Suite User Guide

2. **Basys3参考手册**
   - <https://reference.digilentinc.com/reference/programmable-logic/basys-3/reference-manual>

3. **Verilog HDL教程**
   - 《Verilog数字系统设计教程》夏宇闻

4. **在线资源**
   - Xilinx官方论坛
   - Digilent技术社区

---

## 版本历史

| 版本 | 日期 | 修改内容 |
|------|------|----------|
| 1.0  | 2024 | 初始版本 |

---

*本文档由FPGA实验室编制，仅供教学使用*


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
