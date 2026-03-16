# C语言视频教程完整大纲

> **总集数**: 270集 | **总时长**: 约180小时 | **难度梯度**: 入门 → 进阶 → 专家 → 大师

---

## 📋 目录

- [第一季：C语言基础 (40集)](#第一季c语言基础)
- [第二季：C语言核心 (50集)](#第二季c语言核心)
- [第三季：系统编程 (60集)](#第三季系统编程)
- [第四季：高级主题 (40集)](#第四季高级主题)
- [第五季：形式化方法 (30集)](#第五季形式化方法)
- [第六季：工业实战 (50集)](#第六季工业实战)

---

## 第一季：C语言基础

> **集数**: 40集 | **预计时长**: 26小时 | **目标**: 掌握C语言基础语法与编程思维

### 第1章：开发环境搭建 (5集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S01E01 | Windows下VS Code + GCC环境配置 | 25min | 1. 安装MinGW-w64工具链<br>2. 配置VS Code C/C++扩展<br>3. 配置调试环境launch.json<br>4. 编译运行第一个程序 | 无 | `hello_world.c` | 配置个人开发环境 |
| S01E02 | Linux下GCC开发环境搭建 | 20min | 1. 安装build-essential<br>2. 配置Makefile基础<br>3. 使用gdb调试<br>4. 包管理器使用 | Linux基础命令 | `linux_setup/` | 在虚拟机中配置环境 |
| S01E03 | macOS/Xcode命令行工具配置 | 20min | 1. 安装Command Line Tools<br>2. Homebrew安装GCC<br>3. VS Code配置<br>4. clang vs gcc对比 | macOS基础 | `mac_setup.c` | 配置clang环境 |
| S01E04 | CMake跨平台构建入门 | 35min | 1. CMakeLists.txt编写<br>2. 多文件项目管理<br>3. 生成器表达式<br>4. 跨平台编译实践 | S01E01-03 | `cmake_demo/` | 编写CMakeLists.txt |
| S01E05 | 开发工具链深度解析 | 30min | 1. 编译四阶段详解<br>2. 静态分析工具<br>3. 代码格式化工具<br>4. Git版本控制集成 | 无 | `toolchain_demo/` | 配置pre-commit hook |

**参考文档**: [GCC官方文档](https://gcc.gnu.org/onlinedocs/) | [CMake教程](https://cmake.org/cmake/help/latest/guide/tutorial/) | [VS Code C++](https://code.visualstudio.com/docs/languages/cpp)

---

### 第2章：C语言基础语法 (8集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S01E06 | Hello World深度剖析 | 25min | 1. 程序结构解析<br>2. #include机制<br>3. main函数返回值<br>4. 标准I/O原理 | S01E01 | `hello_deep.c` | 编写带参数的main |
| S01E07 | 变量与常量详解 | 30min | 1. 变量声明与定义<br>2. 作用域与生命周期<br>3. const与volatile<br>4. 字面量类型推导 | 无 | `variables.c` | 常量优化实验 |
| S01E08 | 基本数据类型(上) | 35min | 1. 整数类型家族<br>2. 有符号vs无符号<br>3. 类型转换规则<br>4. 整型提升机制 | S01E07 | `integer_types.c` | 类型边界测试 |
| S01E09 | 基本数据类型(下) | 35min | 1. 浮点数表示(IEEE 754)<br>2. 精度与舍入<br>3. char类型本质<br>4. _Bool与stdbool.h | S01E08 | `float_types.c` | 浮点精度实验 |
| S01E10 | 类型限定与修饰符 | 30min | 1. signed/unsigned<br>2. short/long/long long<br>3. sizeof运算符<br>4. 对齐与填充 | S01E08-09 | `type_qualifiers.c` | 结构体内存布局分析 |
| S01E11 | typedef与类型别名 | 25min | 1. typedef语法<br>2. 与#define区别<br>3. 复杂类型简化<br>4. 标准类型别名 | S01E10 | `typedef_demo.c` | 定义跨平台类型 |
| S01E12 | 输入输出基础 | 35min | 1. printf格式化详解<br>2. scanf安全使用<br>3. 缓冲区机制<br>4. 常见I/O错误 | S01E06 | `io_basics.c` | 格式化输出练习 |
| S01E13 | 注释与代码风格 | 20min | 1. 注释最佳实践<br>2. Doxygen文档<br>3. 命名规范<br>4. 代码审查基础 | 无 | `style_guide/` | 规范化现有代码 |

**参考文档**: [C11标准草案](https://www.open-std.org/jtc1/sc22/wg14/www/docs/n1548.pdf) | [IEEE 754标准](https://ieeexplore.ieee.org/document/4610935)

---

### 第3章：运算符与表达式 (6集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S01E14 | 算术运算符 | 25min | 1. 四则运算<br>2. 整数除法陷阱<br>3. 溢出检测<br>4. 运算优先级 | S01E08 | `arithmetic.c` | 安全算术运算库 |
| S01E15 | 关系与逻辑运算符 | 25min | 1. 比较运算符<br>2. 短路求值<br>3. 布尔上下文<br>4. 常见逻辑错误 | S01E14 | `logical_ops.c` | 复杂条件重构 |
| S01E16 | 位运算符详解(上) | 30min | 1. 与或非异或<br>2. 移位运算<br>3. 掩码技术<br>4. 位标志设计 | S01E08 | `bitwise_1.c` | 权限系统设计 |
| S01E17 | 位运算符详解(下) | 35min | 1. 位字段vs移位<br>2. 位运算技巧<br>3. 奇偶判断<br>4. 交换变量 | S01E16 | `bitwise_2.c` | 位运算算法题 |
| S01E18 | 赋值与复合运算符 | 25min | 1. 赋值表达式<br>2. 复合赋值<br>3. 连续赋值<br>4. 副作用与序列点 | S01E14 | `assignment.c` | 表达式求值分析 |
| S01E19 | 其他运算符与优先级 | 30min | 1. 三目运算符<br>2. sizeof/sizeof_<br>3. 逗号运算符<br>4. 优先级表记忆法 | S01E15-18 | `other_ops.c` | 运算符重排练习 |

**参考文档**: [运算符优先级表](https://en.cppreference.com/w/c/language/operator_precedence) | [C陷阱与缺陷第2章](https://baike.baidu.com/item/C%E9%99%B7%E9%98%B1%E4%B8%8E%E7%BC%BA%E9%99%B7)

---

### 第4章：控制流程 (8集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S01E20 | 顺序结构与代码块 | 20min | 1. 语句与表达式<br>2. 复合语句<br>3. 空语句陷阱<br>4. 代码块作用域 | S01E07 | `sequential.c` | 作用域分析练习 |
| S01E21 | if-else条件分支 | 30min | 1. 单分支/多分支<br>2. else悬挂问题<br>3. 嵌套优化<br>4. 防御式编程 | S01E15 | `if_else.c` | 重构复杂条件 |
| S01E22 | switch-case详解 | 35min | 1. switch工作原理<br>2. case标签规则<br>3. break与fall-through<br>4. 跳转表优化 | S01E21 | `switch_case.c` | 状态机实现 |
| S01E23 | while循环精讲 | 30min | 1. while语法<br>2. 循环不变式<br>3. 终止条件设计<br>4. 无限循环处理 | S01E21 | `while_loop.c` | 迭代算法实现 |
| S01E24 | for循环深度剖析 | 35min | 1. for三要素<br>2. 逗号表达式<br>3. 空for语句<br>4. 与while选择 | S01E23 | `for_loop.c` | 嵌套循环优化 |
| S01E25 | do-while与循环对比 | 25min | 1. do-while特点<br>2. 至少执行一次<br>3. 三种循环对比<br>4. 性能考量 | S01E24 | `do_while.c` | 菜单系统实现 |
| S01E26 | break与continue | 25min | 1. break跳出循环<br>2. continue跳过<br>3. 嵌套循环控制<br>4. goto替代方案 | S01E23-25 | `break_continue.c` | 循环重构练习 |
| S01E27 | goto与标签(上) | 30min | 1. goto语法<br>2. 跳出深层嵌套<br>3. 错误处理模式<br>4. Dijkstra争议 | S01E26 | `goto_demo.c` | 错误处理框架 |

**参考文档**: [结构化程序设计](https://dl.acm.org/doi/10.1145/362929.362947) | [Linux内核goto使用](https://www.kernel.org/doc/Documentation/process/coding-style.rst)

---

### 第5章：函数基础 (7集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S01E28 | 函数定义与调用 | 30min | 1. 函数声明定义<br>2. 参数与返回值<br>3. 调用约定<br>4. 栈帧基础 | S01E20 | `function_basics.c` | 数学函数库 |
| S01E29 | 函数参数传递 | 35min | 1. 值传递本质<br>2. 数组参数退化<br>3. const参数<br>4. 可变参数入门 | S01E28 | `parameters.c` | 交换函数实现 |
| S01E30 | 递归函数 | 35min | 1. 递归原理<br>2. 递归vs迭代<br>3. 尾递归优化<br>4. 递归深度限制 | S01E28 | `recursion.c` | 经典递归问题 |
| S01E31 | 作用域与链接属性 | 35min | 1. 作用域层级<br>2. extern/static<br>3. 内部/外部链接<br>4. 头文件保护 | S01E28 | `linkage/` | 多文件项目 |
| S01E32 | 变量存储类别 | 35min | 1. auto/static<br>2. 存储期概念<br>3. 静态局部变量<br>4. 线程存储期 | S01E31 | `storage.c` | 状态保持设计 |
| S01E33 | 函数指针入门 | 40min | 1. 函数指针语法<br>2. 回调函数<br>3. qsort使用<br>4. 函数指针数组 | S01E28 | `func_ptr_basics.c` | 回调机制实现 |
| S01E34 | 内联函数 | 30min | 1. inline关键字<br>2. 内联vs宏<br>3. 编译器优化<br>4. 使用限制 | S01E28 | `inline_demo.c` | 性能对比测试 |

**参考文档**: [C函数调用约定](https://en.wikipedia.org/wiki/X86_calling_conventions) | [尾调用优化](https://llvm.org/docs/CodeGenerator.html#tail-call-optimization)

---

### 第6章：数组基础 (6集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S01E35 | 一维数组 | 30min | 1. 数组定义初始化<br>2. 数组访问<br>3. 数组越界<br>4. 柔性数组入门 | S01E29 | `array_1d.c` | 数组工具函数 |
| S01E36 | 二维数组与多维数组 | 35min | 1. 多维数组布局<br>2. 数组指针参数<br>3. 矩阵操作<br>4. 动态多维数组 | S01E35 | `array_2d.c` | 矩阵运算库 |
| S01E37 | 字符数组与字符串 | 35min | 1. 字符串存储<br>2. 终止符'\0'<br>3. 字符数组vs字符串<br>4. 字符串字面量 | S01E35 | `char_array.c` | 字符串处理函数 |
| S01E38 | 字符串处理函数(上) | 35min | 1. strlen/strcpy<br>2. strcat/strcmp<br>3. 安全版本函数<br>4. 缓冲区溢出 | S01E37 | `string_funcs_1.c` | 安全字符串库 |
| S01E39 | 字符串处理函数(下) | 35min | 1. strchr/strstr<br>2. strtok使用<br>3. 内存操作函数<br>4. 自定义字符串库 | S01E38 | `string_funcs_2.c` | 字符串解析器 |
| S01E40 | 数组与指针初步 | 40min | 1. 数组名本质<br>2. 指针算术<br>3. 数组遍历<br>4. 指针与数组区别 | S01E33,35 | `array_ptr_intro.c` | 通用迭代器 |

**参考文档**: [C字符串处理](https://en.cppreference.com/w/c/string/byte) | [缓冲区溢出攻击](https://owasp.org/www-community/vulnerabilities/Buffer_Overflow)

---

**第一季总结**: 完成本季学习后，你将能够编写结构化的C程序，理解变量、运算符、控制流程、函数和数组的基本使用，为后续深入学习打下坚实基础。

**项目作业**: 实现一个带菜单的学生成绩管理系统，包含增删改查、排序、统计功能。

---
