# 约束文件

本目录存放FPGA设计的XDC（Xilinx Design Constraints）约束文件。

## 约束类型

- **引脚约束**: 指定信号与FPGA引脚的映射
- **时序约束**: 定义时钟频率和时序要求
- **物理约束**: 指定布局和布线约束

## 常用约束

```tcl
# 时钟约束
create_clock -period 10.000 -name sys_clk [get_ports clk]

# 引脚约束
set_property PACKAGE_PIN W5 [get_ports clk]
set_property IOSTANDARD LVCMOS33 [get_ports clk]
```

---

[← 返回上级目录](../README.md)
