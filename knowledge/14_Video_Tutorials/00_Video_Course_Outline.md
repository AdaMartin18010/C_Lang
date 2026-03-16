# C语言视频教程完整大纲

> **总集数**: 270集 | **总时长**: 约180小时 | **难度梯度**: 入门 → 进阶 → 专家 → 大师

---

## 📋 目录

- [C语言视频教程完整大纲](#c语言视频教程完整大纲)
  - [📋 目录](#-目录)
  - [第一季：C语言基础](#第一季c语言基础)
    - [第1章：开发环境搭建 (5集)](#第1章开发环境搭建-5集)
    - [第2章：C语言基础语法 (8集)](#第2章c语言基础语法-8集)
    - [第3章：运算符与表达式 (6集)](#第3章运算符与表达式-6集)
    - [第4章：控制流程 (8集)](#第4章控制流程-8集)
    - [第5章：函数基础 (7集)](#第5章函数基础-7集)
    - [第6章：数组基础 (6集)](#第6章数组基础-6集)
  - [第二季：C语言核心](#第二季c语言核心)
    - [第7章：指针深度解析 (12集)](#第7章指针深度解析-12集)
    - [第8章：内存管理 (10集)](#第8章内存管理-10集)
    - [第9章：字符串高级处理 (6集)](#第9章字符串高级处理-6集)
    - [第10章：结构体与联合体 (8集)](#第10章结构体与联合体-8集)
    - [第11章：文件I/O与序列化 (7集)](#第11章文件io与序列化-7集)
    - [第12章：预处理器与宏 (7集)](#第12章预处理器与宏-7集)

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


## 第二季：C语言核心

> **集数**: 50集 | **预计时长**: 35小时 | **目标**: 深入理解C语言核心机制，掌握内存管理与高级数据结构

### 第7章：指针深度解析 (12集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S02E01 | 指针的本质 | 40min | 1. 内存地址概念<br>2. 指针变量定义<br>3. 取址与解引用<br>4. NULL指针 | S01E40 | `pointer_essence.c` | 指针可视化工具 |
| S02E02 | 指针的类型系统 | 35min | 1. 指针类型意义<br>2. void*指针<br>3. 指针转换规则<br>4. 类型安全 | S02E01 | `pointer_types.c` | 通用指针容器 |
| S02E03 | 指针与const | 35min | 1. const指针<br>2. 指向const<br>3. 双重const<br>4. 只读语义 | S02E02 | `const_pointer.c` | 只读接口设计 |
| S02E04 | 指针算术运算 | 40min | 1. 指针加减<br>2. 指针比较<br>3. 指针差值<br>4. 越界检查 | S02E01 | `pointer_arith.c` | 内存扫描工具 |
| S02E05 | 指针与数组深入 | 45min | 1. 数组指针<br>2. 指针数组<br>3. 复杂声明解析<br>4. 右左法则 | S02E04 | `array_ptr_deep.c` | 复杂声明解码器 |
| S02E06 | 函数指针高级 | 45min | 1. 复杂函数指针<br>2. typedef简化<br>3. 回调设计模式<br>4. 信号处理 | S02E05 | `func_ptr_adv.c` | 事件驱动框架 |
| S02E07 | 多级指针 | 40min | 1. 二级指针<br>2. 多级间接<br>3. 指针参数传递<br>4. 动态二维数组 | S02E05 | `multi_pointer.c` | 通用矩阵库 |
| S02E08 | 函数返回指针 | 35min | 1. 返回局部变量陷阱<br>2. 返回静态变量<br>3. 返回动态内存<br>4. 所有权语义 | S02E07 | `return_pointer.c` | 字符串工厂模式 |
| S02E09 | 复杂声明解析 | 40min | 1. 声明语法<br>2. cdecl工具<br>3. 螺旋法则<br>4. 实际项目案例 | S02E05 | `declarations.c` | 声明生成器 |
| S02E10 | 指针与字符串 | 35min | 1. 字符串指针<br>2. 字符数组vs指针<br>3. 字符串常量池<br>4. 字符串数组 | S02E08 | `ptr_string.c` | 命令行解析器 |
| S02E11 | 指针数组与main参数 | 35min | 1. argv解析<br>2. 环境变量<br>3. 参数验证<br>4. getopt使用 | S02E10 | `main_args.c` | 完整CLI工具 |
| S02E12 | 指针安全与调试 | 40min | 1. 常见指针错误<br>2. 野指针检测<br>3. ASan使用<br>4. 调试技巧 | S02E01-11 | `pointer_debug.c` | 指针安全检查工具 |

**参考文档**: [C指针深度解析](https://en.wikipedia.org/wiki/Pointer_(computer_programming)) | [AddressSanitizer](https://github.com/google/sanitizers/wiki/AddressSanitizer)

---

### 第8章：内存管理 (10集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S02E13 | 进程内存布局 | 45min | 1. 虚拟地址空间<br>2. 代码/数据/堆/栈<br>3. BSS段<br>4. 内存映射 | S02E01 | `memory_layout.c` | 内存使用分析 |
| S02E14 | 动态内存分配基础 | 40min | 1. malloc/calloc<br>2. free释放<br>3. 分配失败处理<br>4. 内存碎片 | S02E13 | `malloc_basics.c` | 内存池基础版 |
| S02E15 | realloc与内存调整 | 35min | 1. realloc语义<br>2. 原地扩展vs移动<br>3. 动态数组<br>4. 过度分配策略 | S02E14 | `realloc_demo.c` | 动态数组实现 |
| S02E16 | 内存泄漏检测 | 40min | 1. 内存泄漏原因<br>2. Valgrind使用<br>3. 智能指针思想<br>4. 所有权设计 | S02E14 | `leak_check.c` | 内存泄漏检测器 |
| S02E17 | 双重释放与使用已释放内存 | 35min | 1. use-after-free<br>2. double-free<br>3. 野指针防护<br>4. 防御式编程 | S02E14 | `double_free.c` | 安全防护包装器 |
| S02E18 | 自定义内存分配器(上) | 50min | 1. 固定大小分配器<br>2. 空闲列表<br>3. 内存对齐<br>4. 位图管理 | S02E14 | `allocator_1.c` | 对象池实现 |
| S02E19 | 自定义内存分配器(下) | 50min | 1. 伙伴系统<br>2. slab分配器<br>3. 内存映射<br>4. 性能测试 | S02E18 | `allocator_2.c` | 高性能分配器 |
| S02E20 | 栈内存与变长数组 | 35min | 1. 栈溢出<br>2. VLA使用<br>3. alloca函数<br>4. 栈保护机制 | S02E13 | `stack_memory.c` | 递归安全检测 |
| S02E21 | 内存对齐与填充 | 40min | 1. 对齐要求<br>2. alignof/aligned_alloc<br>3. 结构体填充<br>4. 缓存行优化 | S02E18 | `alignment.c` | 缓存友好数据结构 |
| S02E22 | mmap与内存映射 | 45min | 1. mmap系统调用<br>2. 文件映射<br>3. 共享内存<br>4. 大页支持 | S02E19 | `mmap_demo.c` | 内存映射数据库 |

**参考文档**: [glibc内存分配](https://www.gnu.org/software/libc/manual/html_node/The-GNU-Allocator.html) | [Linux内存管理](https://www.kernel.org/doc/html/latest/admin-guide/mm/index.html)

---

### 第9章：字符串高级处理 (6集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S02E23 | 字符串实现原理 | 40min | 1. C字符串缺陷<br>2. 长度前缀字符串<br>3. SDS实现<br>4. 现代字符串库 | S01E39 | `string_impl.c` | 简易SDS实现 |
| S02E24 | 宽字符与国际化 | 40min | 1. wchar_t类型<br>2. Unicode基础<br>3. 编码转换<br>4. locale设置 | S02E23 | `wide_char.c` | UTF-8处理工具 |
| S02E25 | 正则表达式入门 | 45min | 1. regex.h使用<br>2. 基本模式匹配<br>3. 捕获组<br>4. 性能考量 | S02E23 | `regex_demo.c` | 日志分析器 |
| S02E26 | 字符串解析技术 | 45min | 1. 有限状态机<br>2. 递归下降<br>3. strtok_r安全<br>4. 解析器生成器 | S02E25 | `parsing.c` | 配置文件解析器 |
| S02E27 | 字符串算法 | 50min | 1. KMP算法<br>2. Boyer-Moore<br>3. 字符串哈希<br>4. 实际应用 | S02E26 | `string_algo.c` | 文本搜索引擎 |
| S02E28 | 格式化字符串安全 | 40min | 1. printf家族<br>2. 格式字符串漏洞<br>3. snprintf安全<br>4. 自定义格式化 | S02E27 | `fmt_security.c` | 安全日志库 |

**参考文档**: [Redis SDS](https://github.com/redis/redis/blob/unstable/src/sds.h) | [PCRE库](https://www.pcre.org/)

---

### 第10章：结构体与联合体 (8集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S02E29 | 结构体基础 | 35min | 1. struct定义<br>2. 成员访问<br>3. 初始化语法<br>4. 匿名结构体 | S01E10 | `struct_basics.c` | 学生信息结构 |
| S02E30 | 结构体内存布局 | 45min | 1. 成员对齐<br>2. 填充字节<br>3. 重排优化<br>4. packed属性 | S02E21,29 | `struct_layout.c` | 内存优化分析 |
| S02E31 | 结构体数组与指针 | 35min | 1. 结构体数组<br>2. 指向结构体指针<br>3. 箭头运算符<br>4. 自引用结构体 | S02E30 | `struct_ptr.c` | 链表节点设计 |
| S02E32 | 位域详解 | 40min | 1. 位域语法<br>2. 布局规则<br>3. 可移植性<br>4. 硬件寄存器映射 | S02E16,30 | `bit_fields.c` | 硬件抽象层 |
| S02E33 | 联合体与类型双关 | 40min | 1. union定义<br>2. 内存共享<br>3. 类型双关<br>4. 字节序检测 | S02E32 | `union_demo.c` | 通用数据容器 |
| S02E34 | 变体类型实现 | 45min | 1. tagged union<br>2. 类型判别<br>3. 访问安全<br>4. 模拟代数类型 | S02E33 | `variant_type.c` | JSON值类型 |
| S02E35 | 结构体与函数 | 35min | 1. 结构体传参<br>2. 返回结构体<br>3. 结构体指针参数<br>4. 构造函数模式 | S02E31 | `struct_func.c` | 面向对象模拟 |
| S02E36 | 复杂数据结构实战 | 50min | 1. 动态数组<br>2. 哈希表<br>3. 树结构<br>4. 图结构 | S02E31,34 | `data_structures/` | 完整数据结构库 |

**参考文档**: [结构体内存对齐](https://en.cppreference.com/w/c/language/alignas) | [Tagged Union模式](https://en.wikipedia.org/wiki/Tagged_union)

---

### 第11章：文件I/O与序列化 (7集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S02E37 | 文件操作基础 | 35min | 1. fopen/fclose<br>2. 文件模式<br>3. 文本vs二进制<br>4. 错误处理 | S01E12 | `file_basics.c` | 文件复制工具 |
| S02E38 | 顺序读写操作 | 35min | 1. fgetc/fputc<br>2. fgets/fputs<br>3. fread/fwrite<br>4. 缓冲区控制 | S02E37 | `file_seq.c` | 文本处理器 |
| S02E39 | 随机访问与定位 | 35min | 1. fseek/ftell<br>2. rewind<br>3. 大文件支持<br>4. 内存映射文件 | S02E38 | `file_random.c` | 数据库索引 |
| S02E40 | 文件元数据与目录 | 40min | 1. stat函数<br>2. 权限检查<br>3. 目录遍历<br>4. 通配匹配 | S02E39 | `file_meta.c` | 文件管理器 |
| S02E41 | 序列化与反序列化 | 45min | 1. 二进制序列化<br>2. 字节序处理<br>3. 版本兼容<br>4. 安全检查 | S02E39 | `serialization.c` | 配置保存系统 |
| S02E42 | JSON处理实战 | 50min | 1. cJSON库使用<br>2. 解析与生成<br>3. 内存管理<br>4. 错误处理 | S02E41 | `json_handling.c` | 配置管理器 |
| S02E43 | 内存流与自定义I/O | 40min | 1. fmemopen<br>2. 自定义stream<br>3. 压缩I/O<br>4. 加密I/O | S02E42 | `custom_io.c` | 流过滤器框架 |

**参考文档**: [POSIX文件I/O](https://pubs.opengroup.org/onlinepubs/9699919799/functions/stdio.h.html) | [cJSON库](https://github.com/DaveGamble/cJSON)

---

### 第12章：预处理器与宏 (7集)

| 编号 | 标题 | 时长 | 学习目标 | 前置知识 | 配套代码 | 练习 |
|------|------|------|----------|----------|----------|------|
| S02E44 | 预处理器基础 | 30min | 1. 编译流程<br>2. 预处理指令<br>3. 宏定义基础<br>4. 条件编译 | S01E06 | `preprocessor.c` | 调试宏系统 |
| S02E45 | 宏定义高级技巧 | 40min | 1. 多行宏<br>2. 变参宏<br>3. 字符串化<br>4. 连接符 | S02E44 | `macro_adv.c` | 日志宏系统 |
| S02E46 | 条件编译实战 | 35min | 1. #ifdef/#ifndef<br>2. 平台检测<br>3. 功能开关<br>4. 头文件保护 | S02E45 | `conditional.c` | 跨平台抽象层 |
| S02E47 | 预定义宏与调试 | 35min | 1. __FILE__等<br>2. 调试信息<br>3. 断言assert<br>4. 静态断言 | S02E46 | `predefined.c` | 调试信息宏 |
| S02E48 | 宏的陷阱与最佳实践 | 40min | 1. 副作用问题<br>2. 括号使用<br>3. do-while技巧<br>4. 宏vs函数选择 | S02E45 | `macro_traps.c` | 安全宏库 |
| S02E49 | 代码生成与元编程 | 45min | 1. X-Macros<br>2. 重复模式<br>3. 表驱动生成<br>4. 代码模板 | S02E48 | `codegen.c` | 枚举字符串化 |
| S02E50 | 现代C替代方案 | 35min | 1. const vs #define<br>2. inline vs 宏<br>3. enum vs 宏常量<br>4. _Generic选择 | S02E49 | `modern_alt.c` | 宏重构项目 |

**参考文档**: [C预处理器](https://gcc.gnu.org/onlinedocs/cpp/) | [X-Macros技术](https://en.wikipedia.org/wiki/X_Macro)

---

**第二季总结**: 完成本季后，你将深入理解C语言的核心机制，包括指针、内存管理、高级字符串处理、结构体设计和文件I/O。能够编写健壮、高效的C程序。

**项目作业**: 实现一个带垃圾回收标记的内存分配器，包含内存泄漏检测功能。

---
