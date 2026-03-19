# 硬件调试技术文档库

本目录包含硬件调试技术的完整文档集，涵盖从基础原理到高级调试技巧的各个方面。

## 文档列表

### 1. 01_JTAG_Debug.md (2,109 行)

**JTAG 调试技术详解**

- JTAG 协议详解
- TAP 控制器状态机
- 指令寄存器/数据寄存器
- 边界扫描测试
- OpenOCD 使用指南
- GDB + JTAG 调试

### 2. 02_ILA_Integrated_Logic_Analyzer.md (1,918 行)

**Vivado ILA 集成逻辑分析仪使用指南**

- Vivado ILA 使用
- 探针插入
- 触发条件设置
- 波形分析
- 高级调试技巧

### 3. 03_SignalTap_Quartus.md (1,514 行)

**SignalTap II 逻辑分析仪使用指南**

- SignalTap II 使用
- Quartus 调试工具
- 与 Vivado ILA 对比

### 4. 04_Hardware_Debug_Methodology.md (1,631 行)

**硬件调试方法论**

- 硬件调试方法论
- 仿真 vs 在线调试
- 常见问题诊断
- 时序问题分析
- 信号完整性基础

## 适用对象

- FPGA 开发工程师
- 硬件设计工程师
- 嵌入式系统开发者
- 电子工程专业学生

## 学习目标

通过阅读本系列文档，读者将掌握：

1. JTAG 协议原理和实际调试操作
2. Xilinx 和 Intel FPGA 的片上逻辑分析仪使用
3. 系统化的硬件调试方法论
4. 时序分析和信号完整性基础知识
5. 实际调试案例的分析和解决能力

## 推荐学习顺序

```
01_JTAG_Debug.md
       ↓
02_ILA_Integrated_Logic_Analyzer.md 或 03_SignalTap_Quartus.md
       ↓
04_Hardware_Debug_Methodology.md
```

## 补充资源

- 每个文档末尾都附有参考资源和延伸阅读
- 实际调试时需要配合相应的硬件开发板和调试工具
- 建议结合具体项目实践以加深理解

---

*文档创建日期: 2026-03-19*
*总文档数: 4*
*总行数: 7,172 行*
