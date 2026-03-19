# RISC-V CPU完整实现

本目录提供RISC-V处理器的完整实现指南，从简单的单周期设计到支持缓存的完整系统。

## 内容概览

| 文件 | 描述 |
|------|------|
| `01_RV32I_Single_Cycle.md` | RV32I指令集单周期CPU设计 |
| `02_RV32I_Pipelined.md` | RV32I流水线CPU设计与冒险处理 |
| `03_RV32IM_with_Cache.md` | 带缓存的RV32IM完整实现 |
| `04_RISCV_Simulator_in_C.md` | 使用C语言编写RISC-V模拟器 |

## 项目演进

```
单周期CPU → 流水线CPU → 带缓存CPU
   ↓              ↓            ↓
 最简单        性能提升       完整系统
```

## 核心特性

- **RV32I**: 完整实现32位RISC-V整数指令集
- **RV32M**: 乘除法扩展支持
- **五级流水线**: 取指、译码、执行、访存、写回
- **冒险处理**: 数据冒险、控制冒险的解决方案
- **缓存集成**: 指令缓存和数据缓存的设计

## 前置知识

- `09_Physical_Machine_Layer` 的硬件基础
- `06_Control_Unit_Design` 的控制器设计

---

[← 返回上级目录](../README.md)
