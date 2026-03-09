# 01 Core Knowledge System - 核心知识体系

> **对应标准**: ISO/IEC 9899:2018 (C17), CERT C, MISRA C:2012
> **完成度**: 95% | **预估学习时间**: 80-100小时

---

## 目录结构

### 01_Basic_Layer - 基础层

C语言入门基础，零基础学习起点。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_Hello_World.md](./01_Basic_Layer/01_Hello_World.md) | 第一个程序 | L1 | ISO C §5.1.2.2.1 |
| [02_Compilation_Process.md](./01_Basic_Layer/02_Compilation_Process.md) | 编译过程 | L1 | ISO C §5.1.1.2 |
| [03_Variables_Constants.md](./01_Basic_Layer/03_Variables_Constants.md) | 变量与常量 | L1 | ISO C §6.2.1, §6.4.4 |
| [04_Operators_Expressions.md](./01_Basic_Layer/04_Operators_Expressions.md) | 运算符与表达式 | L2 | ISO C §6.5 |
| [05_Input_Output.md](./01_Basic_Layer/05_Input_Output.md) | 基本输入输出 | L2 | ISO C §7.21 |
| [06_Comments_Documentation.md](./01_Basic_Layer/06_Comments_Documentation.md) | 注释与文档 | L1 | ISO C §6.4.9, Doxygen |

**前置知识**: 无
**后续延伸**: [02_Core_Layer](./02_Core_Layer/README.md)

---

### 02_Core_Layer - 核心层

C语言核心语法和语义，程序设计基础。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_Data_Types.md](./02_Core_Layer/01_Data_Types.md) | 数据类型系统 | L2 | ISO C §6.2.5, §6.7.2 |
| [02_Type_Conversion.md](./02_Core_Layer/02_Type_Conversion.md) | 类型转换 | L3 | ISO C §6.3 |
| [03_Control_Flow.md](./02_Core_Layer/03_Control_Flow.md) | 控制流 | L2 | ISO C §6.8 |
| [04_Pointers.md](./02_Core_Layer/04_Pointers.md) | 指针 | L3 | ISO C §6.5.3.2, §6.7.6.1 |
| [05_Arrays.md](./02_Core_Layer/05_Arrays.md) | 数组 | L3 | ISO C §6.7.6.2 |
| [06_Memory_Management.md](./02_Core_Layer/06_Memory_Management.md) | 内存管理 | L3 | ISO C §7.22.3 |
| [07_Functions.md](./02_Core_Layer/07_Functions.md) | 函数 | L3 | ISO C §6.7.6.3, §6.9.1 |
| [08_Strings.md](./02_Core_Layer/08_Strings.md) | 字符串处理 | L3 | ISO C §7.24 |

**前置知识**: [01_Basic_Layer](./01_Basic_Layer/README.md)
**后续延伸**: [03_Construction_Layer](./03_Construction_Layer/README.md)

---

### 03_Construction_Layer - 构造层

复杂数据结构和程序组织。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_Structures_Unions.md](./03_Construction_Layer/01_Structures_Unions.md) | 结构体与联合 | L3 | ISO C §6.7.2.1 |
| [02_Enumerations.md](./03_Construction_Layer/02_Enumerations.md) | 枚举类型 | L2 | ISO C §6.7.2.2 |
| [03_Type_Definitions.md](./03_Construction_Layer/03_Type_Definitions.md) | 类型定义 | L3 | ISO C §6.7.8 |
| [04_Bit_Fields.md](./03_Construction_Layer/04_Bit_Fields.md) | 位域 | L3 | ISO C §6.7.2.1 |
| [05_Linked_Lists.md](./03_Construction_Layer/05_Linked_Lists.md) | 链表实现 | L3 | 经典数据结构 |
| [06_Stacks_Queues.md](./03_Construction_Layer/06_Stacks_Queues.md) | 栈与队列 | L3 | 经典数据结构 |
| [07_Binary_Trees.md](./03_Construction_Layer/07_Binary_Trees.md) | 二叉树 | L4 | 经典数据结构 |
| [08_Hash_Tables.md](./03_Construction_Layer/08_Hash_Tables.md) | 哈希表 | L4 | 经典数据结构 |

**前置知识**: [02_Core_Layer](./02_Core_Layer/README.md)
**后续延伸**: [04_Standard_Library](./04_Standard_Library/README.md)

---

### 04_Standard_Library - 标准库

ISO C标准库详解。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_File_Operations.md](./04_Standard_Library/01_File_Operations.md) | 文件操作 | L3 | ISO C §7.21 |
| [02_String_Manipulation.md](./04_Standard_Library/02_String_Manipulation.md) | 字符串处理 | L3 | ISO C §7.24 |
| [03_Mathematical_Functions.md](./04_Standard_Library/03_Mathematical_Functions.md) | 数学函数 | L3 | ISO C §7.12 |
| [04_Time_Date.md](./04_Standard_Library/04_Time_Date.md) | 时间日期 | L3 | ISO C §7.27 |
| [05_Error_Handling.md](./04_Standard_Library/05_Error_Handling.md) | 错误处理 | L3 | ISO C §7.5, §7.21.6.3 |
| [06_Dynamic_Memory.md](./04_Standard_Library/06_Dynamic_Memory.md) | 动态内存 | L3 | ISO C §7.22.3 |
| [07_Locale_Internationalization.md](./04_Standard_Library/07_Locale_Internationalization.md) | 本地化 | L4 | ISO C §7.11 |
| [08_Standard_Library_Overview.md](./04_Standard_Library/08_Standard_Library_Overview.md) | 标准库概览 | L2 | ISO C §7 |
| [09_Library_Implementation.md](./04_Standard_Library/09_Library_Implementation.md) | 库实现 | L4 | glibc, musl |
| [10_Threads_C11.md](./04_Standard_Library/10_Threads_C11.md) | C11线程 | L4 | ISO C §7.26 |

**前置知识**: [03_Construction_Layer](./03_Construction_Layer/README.md)
**后续延伸**: [05_Engineering](./05_Engineering/README.md)

---

### 05_Engineering - 工程化

专业软件开发实践。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_Build_Systems_Make.md](./05_Engineering/01_Build_Systems_Make.md) | 构建系统 | L3 | POSIX Make, CMake |
| [02_Preprocessor.md](./05_Engineering/02_Preprocessor.md) | 预处理器 | L3 | ISO C §6.10 |
| [03_Header_Files.md](./05_Engineering/03_Header_Files.md) | 头文件管理 | L3 | ISO C §6.10.2 |
| [04_Static_Libraries.md](./05_Engineering/04_Static_Libraries.md) | 静态库 | L3 | ar, ranlib |
| [05_Dynamic_Libraries.md](./05_Engineering/05_Dynamic_Libraries.md) | 动态库 | L4 | POSIX dlopen |
| [06_Debugging_Techniques.md](./05_Engineering/06_Debugging_Techniques.md) | 调试技术 | L3 | GDB, LLDB |
| [07_Performance_Profiling.md](./05_Engineering/07_Performance_Profiling.md) | 性能分析 | L4 | perf, valgrind |
| [08_Code_Review_Checklist.md](./05_Engineering/08_Code_Review_Checklist.md) | 代码审查 | L3 | CERT C, MISRA C |

**前置知识**: [04_Standard_Library](./04_Standard_Library/README.md)
**后续延伸**: [06_Advanced_Layer](./06_Advanced_Layer/README.md)

---

### 06_Advanced_Layer - 高级层

高级编程技术。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_Advanced_Pointers.md](./06_Advanced_Layer/01_Advanced_Pointers.md) | 高级指针 | L4 | ISO C §6.5.3 |
| [02_Function_Pointers.md](./06_Advanced_Layer/02_Function_Pointers.md) | 函数指针 | L4 | ISO C §6.7.6.3 |
| [03_Variadic_Functions.md](./06_Advanced_Layer/03_Variadic_Functions.md) | 可变参数 | L4 | ISO C §7.16 |
| [04_Signal_Handling.md](./06_Advanced_Layer/04_Signal_Handling.md) | 信号处理 | L4 | POSIX Signal, ISO C §7.14 |
| [05_Inter_Process_Communication.md](./06_Advanced_Layer/05_Inter_Process_Communication.md) | IPC | L4 | POSIX IPC |
| [06_Network_Programming.md](./06_Advanced_Layer/06_Network_Programming.md) | 网络编程 | L4 | POSIX Socket |
| [07_MultiThreading.md](./06_Advanced_Layer/07_MultiThreading.md) | 多线程 | L4 | POSIX Threads |
| [08_Memory_Mapped_Files.md](./06_Advanced_Layer/08_Memory_Mapped_Files.md) | 内存映射 | L4 | POSIX mmap |
| [09_POSIX_API.md](./06_Advanced_Layer/09_POSIX_API.md) | POSIX API | L4 | IEEE Std 1003.1 |
| [10_Systems_Programming.md](./06_Advanced_Layer/10_Systems_Programming.md) | 系统编程 | L5 | Linux/Unix API |

**前置知识**: [05_Engineering](./05_Engineering/README.md)
**后续延伸**: [07_Modern_C](./07_Modern_C/README.md)

---

### 07_Modern_C - 现代C

C11/C17现代特性。

| 文件 | 主题 | 难度 | 国际标准 |
|:-----|:-----|:----:|:---------|
| [01_C11_Standard.md](./07_Modern_C/01_C11_Standard.md) | C11标准 | L4 | ISO/IEC 9899:2011 |
| [02_Static_Assert.md](./07_Modern_C/02_Static_Assert.md) | 静态断言 | L3 | ISO C §7.2 |
| [03_Anonymous_Structures.md](./07_Modern_C/03_Anonymous_Structures.md) | 匿名结构 | L3 | ISO C §6.7.2.1 |
| [04_Alignas_Alignof.md](./07_Modern_C/04_Alignas_Alignof.md) | 对齐支持 | L4 | ISO C §6.7.5, §6.5.3.4 |
| [05_Noreturn.md](./07_Modern_C/05_Noreturn.md) | noreturn | L3 | ISO C §7.23 |
| [06_Generic_Selection.md](./07_Modern_C/06_Generic_Selection.md) | 泛型选择 | L4 | ISO C §6.5.1.1 |
| [07_Atomic_Operations.md](./07_Modern_C/07_Atomic_Operations.md) | 原子操作 | L5 | ISO C §7.17 |
| [08_Threads_Mutexes.md](./07_Modern_C/08_Threads_Mutexes.md) | 线程互斥 | L4 | ISO C §7.26 |
| [09_Complex_Numbers.md](./07_Modern_C/09_Complex_Numbers.md) | 复数 | L3 | ISO C §7.3 |
| [10_Unicode_Support.md](./07_Modern_C/10_Unicode_Support.md) | Unicode | L4 | ISO C §6.4.5, §7.28 |
| [11_Static_Analysis.md](./07_Modern_C/11_Static_Analysis.md) | 静态分析 | L4 | Clang Analyzer, Frama-C |
| [12_Safety_Extensions.md](./07_Modern_C/12_Safety_Extensions.md) | 安全扩展 | L4 | Annex K, ISO/IEC TR 24731 |

**前置知识**: [06_Advanced_Layer](./06_Advanced_Layer/README.md)
**后续延伸**: [02_Formal_Semantics_and_Physics](../02_Formal_Semantics_and_Physics/README.md)

---

## 学习路径

```
01_Basic_Layer (20h)
    ↓
02_Core_Layer (30h)
    ↓
03_Construction_Layer (20h)
    ↓
04_Standard_Library (15h)
    ↓
05_Engineering (15h)
    ↓
06_Advanced_Layer (20h)
    ↓
07_Modern_C (10h)
```

---

## 关联知识库

| 目标 | 路径 |
|:-----|:-----|
| 系统实现 | [03_System_Technology_Domains](../03_System_Technology_Domains/README.md) |
| 工业应用 | [04_Industrial_Scenarios](../04_Industrial_Scenarios/README.md) |
| 理论基础 | [05_Deep_Structure_MetaPhysics](../05_Deep_Structure_MetaPhysics/README.md) |
| 思维工具 | [06_Thinking_Representation](../06_Thinking_Representation/README.md) |

---

## 参考标准

- **ISO/IEC 9899:2018** - C17 Programming Language Standard
- **ISO/IEC 9899:2011** - C11 Programming Language Standard
- **CERT C Coding Standard** - SEI CERT C Secure Coding Standard
- **MISRA C:2012** - Motor Industry Software Reliability Association
- **IEEE Std 1003.1-2017** - POSIX.1-2017 (Base Definitions)
- **IEC 61508** - Functional Safety of Electrical/Electronic Systems

---

> **最后更新**: 2025-03-09
