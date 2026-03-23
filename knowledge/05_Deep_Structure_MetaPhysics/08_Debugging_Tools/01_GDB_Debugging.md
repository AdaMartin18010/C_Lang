# GDB调试技术

> **层级定位**: 05 Deep_Structure_MetaPhysics / 08 Debugging_Tools
> **对应标准**: GNU Debugger
> **难度级别**: L2-L4
> **预估学习时间**: 6-8小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 断点、单步执行、堆栈回溯、内存检查、多线程调试 |
| **前置知识** | C基础、编译过程 |
| **后续延伸** | 性能分析、逆向工程 |
| **权威来源** | GDB Manual, "Debugging with GDB" |

---

## 📖 1. GDB基础

### 1.1 编译调试版本

```bash
# 启用调试信息
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
gcc -g -o program program.c

# 优化级别推荐
gcc -g -O0 -o program program.c  # 无优化，便于调试
gcc -g -Og -o program program.c  # 适度优化，保留调试信息

# 禁用栈保护（某些情况下需要）
gcc -g -fno-stack-protector -o program program.c
```

### 1.2 启动GDB

```bash
# 直接启动
gdb ./program

# 带参数启动
gdb --args ./program arg1 arg2

# 附加到运行进程
gdb -p <pid>

# 分析core dump
gdb ./program core
```

---

## 📖 2. 基本命令

### 2.1 断点设置

```gdb
# 设置断点
break main           # 在main函数处
break 42             # 在第42行
break file.c:func    # 在指定文件的函数
break *0x401000      # 在指定地址

# 条件断点
break 42 if i == 10

# 临时断点（命中一次后删除）
tbreak main

# 禁用/启用断点
disable 1
enable 1

# 删除断点
delete 1
clear main
```

### 2.2 程序控制

```gdb
# 运行程序
run
run arg1 arg2       # 带参数运行

# 单步执行
step                # 进入函数 (s)
next                # 跳过函数 (n)
finish              # 完成当前函数
until               # 运行到下一行（跳过循环）

# 继续执行
continue            # (c)
continue 5          # 继续并忽略接下来5个断点

# 停止程序
kill                # 终止程序
quit                # 退出GDB
```

---

## 📖 3. 查看状态

### 3.1 查看变量

```gdb
# 打印变量
print var
print/x var         # 十六进制
print/d var         # 十进制
print/t var         # 二进制
print/a var         # 地址

# 打印数组
print *arr@10       # 打印arr的前10个元素

# 打印结构体
print *struct_ptr
print struct_ptr->field

# 设置变量
set var x = 10
set {int}0x1000 = 42

# 监视变量变化
watch var           # 变量变化时停止
rwatch var          # 变量被读取时停止
awatch var          # 变量被读写时停止
```

### 3.2 查看调用栈

```gdb
# 堆栈回溯
backtrace           # (bt)
backtrace full      # 包含局部变量

# 切换栈帧
frame 2             # 切换到第2帧
up                  # 向上一层
down                # 向下一层

# 查看帧信息
info frame
info locals         # 局部变量
info args           # 函数参数
```

---

## 📖 4. 内存检查

```gdb
# 检查内存
x/10x &var          # 以十六进制显示10个单元
x/10d &var          # 十进制
x/10s &var          # 字符串
x/10i $pc           # 显示10条指令

# 内存单元大小
x/b addr            # 字节
x/h addr            # 半字 (2字节)
x/w addr            # 字 (4字节)
x/g addr            # 双字 (8字节)
```

---

## 📖 5. 多线程调试

```gdb
# 查看线程
info threads

# 切换线程
thread 2

# 对所有线程执行命令
thread apply all bt
thread apply 1-3 print x

# 设置线程特定断点
break func thread 2
```

---

## 📖 6. 实用技巧

### 6.1 GDB脚本

```gdb
# .gdbinit 文件
set pagination off
set confirm off
set print pretty on

# 定义命令
define hook-stop
    info registers
    x/5i $pc
end

# 自动化调试
define my_debug
    break main
    run
    while 1
        step
        print i
    end
end
```

### 6.2 TUI模式

```gdb
# 启动TUI
gdb -tui ./program

# 快捷键
Ctrl+x a            # 切换TUI模式
Ctrl+x 2            # 切换布局（源码/汇编/寄存器）
Ctrl+x o            # 切换窗口
```

---

## ⚠️ 常见陷阱

### 陷阱 GDB01: 优化导致调试困难

```c
// -O2优化可能删除变量或重排代码
gcc -g -O2 program.c  // 可能无法打印某些变量

// 推荐
gcc -g -O0 program.c  // 无优化
```

### 陷阱 GDB02: 缓冲区溢出检测

```gdb
# 使用canary检测栈溢出
gcc -fstack-protector-all -g program.c

# 在GDB中查看canary值
print __stack_chk_guard
```

---

## ✅ 质量验收清单

- [x] GDB启动与基本命令
- [x] 断点设置（条件/临时）
- [x] 单步执行控制
- [x] 变量查看与修改
- [x] 调用栈回溯
- [x] 内存检查
- [x] 多线程调试
- [x] GDB脚本与TUI

---

> **更新记录**
>
> - 2025-03-09: 创建，补充核心缺失主题


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
