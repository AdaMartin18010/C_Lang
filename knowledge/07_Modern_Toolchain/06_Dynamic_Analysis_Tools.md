# 动态分析工具链

> **层级定位**: 07 Modern Toolchain
> **参考工具**: Valgrind, AddressSanitizer, ThreadSanitizer, DrMemory
> **难度级别**: L4
> **预估学习时间**: 10-12 小时

---

## 1. 动态分析概述

### 1.1 动态分析vs静态分析

```
┌─────────────────────────────────────────────────────────────┐
│              动态分析与静态分析对比                          │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  静态分析                    动态分析                        │
│  ─────────                   ─────────                      │
│  • 不执行程序               • 实际执行程序                    │
│  • 覆盖所有代码路径         • 仅覆盖执行路径                  │
│  • 可能有误报               • 无误报（真阳性）                │
│  • 可能有漏报               • 可能有漏报（未执行代码）        │
│  • 分析速度快               • 分析速度慢（10x-100x）          │
│  • 适合CI/CD                • 适合测试阶段                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘

动态分析类型：
• 内存错误检测（ASan, Valgrind）
• 线程错误检测（TSan, Helgrind）
• 未定义行为检测（UBSan）
• 覆盖率分析（gcov, llvm-cov）
• 性能分析（perf, Valgrind Callgrind）
```

---

## 2. AddressSanitizer (ASan)

### 2.1 ASan原理

```
ASan内存布局（影子内存）：

应用程序内存               影子内存（Shadow Memory）
┌─────────────────┐       ┌─────────────────┐
│  正常堆内存      │       │  元数据          │
│  (redzone)      │──────>│  (1:8比例)       │
├─────────────────┤       ├─────────────────┤
│  可用内存        │       │                 │
│  (用户分配)      │──────>│  0 = 可访问      │
├─────────────────┤       │  1-7 = 部分访问  │
│  redzone        │──────>│  negative = 毒化 │
└─────────────────┘       └─────────────────┘

检测的错误类型：
• 使用已释放内存（Use-after-free）
• 堆缓冲区溢出（Heap buffer overflow）
• 栈缓冲区溢出（Stack buffer overflow）
• 全局缓冲区溢出（Global buffer overflow）
• 使用后返回（Use-after-return）
• 作用域后使用（Use-after-scope）
• 内存泄漏（Memory leaks）- 需要LSan
```

### 2.2 使用ASan

```bash
# ========== 编译选项 ==========

# GCC/Clang
gcc -fsanitize=address -g -O1 program.c -o program

# 推荐选项
gcc -fsanitize=address \
    -fsanitize-address-use-after-scope \
    -fno-omit-frame-pointer \
    -g -O1 \
    program.c -o program

# 禁用某些检查（性能考虑）
gcc -fsanitize=address \
    -mllvm -asan-instrument-reads=false \  # 不检查读操作
    program.c -o program

# ========== 运行 ==========

# 基本运行
./program

# 环境变量控制
ASAN_OPTIONS=detect_stack_use_after_return=1 ./program
ASAN_OPTIONS=detect_container_overflow=0 ./program

# 详细输出
ASAN_OPTIONS=verbosity=1 ./program

# 生成报告文件
ASAN_OPTIONS=log_path=asan.log ./program
```

### 2.3 ASan环境变量

```bash
# 常用ASAN_OPTIONS

# 检测类型
ASAN_OPTIONS=detect_stack_use_after_return=1      # 检测返回后使用
ASAN_OPTIONS=detect_container_overflow=1          # 检测容器溢出
ASAN_OPTIONS=detect_leaks=1                       # 检测内存泄漏（需LSan）

# 行为控制
ASAN_OPTIONS=halt_on_error=0                      # 不停止，继续检测
ASAN_OPTIONS=print_stats=1                        # 打印统计信息
ASAN_OPTIONS=verbosity=1                          # 详细输出

# 报告控制
ASAN_OPTIONS=log_path=/tmp/asan                   # 报告文件路径
ASAN_OPTIONS=symbolize=1                          # 符号化堆栈

# 抑制误报
ASAN_OPTIONS=suppressions=asan.supp               # 抑制文件
```

### 2.4 ASan抑制文件

```
# asan.supp 示例

# 抑制特定函数
interceptor_via_fun:my_known_leaky_function

# 抑制特定库
interceptor_via_lib:libthird_party.so

# 抑制特定源文件
src:*/third_party/*

# 抑制特定内存范围
# (需要代码中使用__asan_poison_memory_region)
```

---

## 3. MemorySanitizer (MSan)

### 3.1 MSan原理

```
MemorySanitizer: 检测未初始化内存使用

跟踪每个内存位的初始化状态：
• 0 = 已初始化
• 1 = 未初始化（毒化）

传播规则：
• 未初始化数据参与运算 → 结果未初始化
• 条件分支使用未初始化数据 → 报告错误
• 系统调用使用未初始化数据 → 报告错误

ASan vs MSan：
• ASan: 检测越界和释放后使用
• MSan: 检测未初始化内存使用
• 两者互补，通常都需要运行
```

### 3.2 使用MSan

```bash
# 注意：MSan需要完整的MSan-instrumented库链
# 通常需要Chromium的MSan工具链或自定义构建

# 编译
clang -fsanitize=memory -fno-omit-frame-pointer -g -O2 program.c -o program

# 运行
./program

# 环境变量
MSAN_OPTIONS=print_stats=1 ./program
```

---

## 4. ThreadSanitizer (TSan)

### 4.1 TSan原理

```
ThreadSanitizer: 数据竞争检测

Vector Clock (happens-before关系)：
• 每个线程维护一个逻辑时钟
• 同步操作传递向量时钟
• 检测没有happens-before关系的并发内存访问

检测的错误：
• 数据竞争（Data races）
• 死锁（Deadlocks）
• 线程泄漏

报告示例：
WARNING: ThreadSanitizer: data race
  Write of size 4 at 0x7fff1234 by thread T1:
    #0 foo thread.c:10
  Previous read of size 4 at 0x7fff1234 by thread T2:
    #0 bar thread.c:20
```

### 4.2 使用TSan

```bash
# ========== 编译 ==========

# GCC 5.1+ 或 Clang 3.2+
gcc -fsanitize=thread -g -O1 program.c -o program -pthread

# ========== 运行 ==========

./program

# 环境变量
TSAN_OPTIONS=second_deadlock_stack=1 ./program
TSAN_OPTIONS=detect_deadlocks=1 ./program

# 忽略特定数据竞争
TSAN_OPTIONS=suppressions=tsan.supp ./program
```

### 4.3 TSan抑制文件

```
# tsan.supp 示例

# 抑制特定race
race:my_racy_function
race:*/third_party/*

# 抑制特定库的所有race
race:libpthread.so

# 抑制特定mutex的误报
mutex:my_special_mutex
```

---

## 5. UndefinedBehaviorSanitizer (UBSan)

### 5.1 UBSan检测的错误

```
UBSan检测的未定义行为：

┌─────────────────────────────────────────────────────────────┐
│  类别              │  示例                                   │
├────────────────────┼─────────────────────────────────────────┤
│  有符号溢出         │  INT_MAX + 1                           │
│  无符号溢出         │  (可选，通常有定义)                     │
│  除零               │  x / 0                                 │
│  移位溢出           │  1 << 32                               │
│  无效移位           │  1 << -1                               │
│  空指针解引用       │  *NULL                                 │
│  未对齐指针         │  *(int*)0x1                            │
│  无效浮点转换       │  NaN转换为整数                          │
│  越界数组访问       │  int a[10]; a[10]                      │
│  无效布尔值         │  int x = 2; bool b = x;                │
│  无效枚举值         │  enum E {A, B}; E e = (E)5;            │
│  函数类型不匹配     │  通过错误函数指针调用                   │
│  对象大小检查       │  访问超出对象边界                       │
└────────────────────┴─────────────────────────────────────────┘
```

### 5.2 使用UBSan

```bash
# ========== 编译 ==========

# 所有检查
gcc -fsanitize=undefined -g -O1 program.c -o program

# 特定检查
gcc -fsanitize=signed-integer-overflow program.c -o program
gcc -fsanitize=shift program.c -o program
gcc -fsanitize=null program.c -o program
gcc -fsanitize=alignment program.c -o program

# 运行时错误恢复（继续执行）
gcc -fsanitize=undefined -fsanitize-recover=all program.c -o program

# ========== 运行 ==========

./program

# 不中止
UBSAN_OPTIONS=halt_on_error=0 ./program
```

---

## 6. Valgrind

### 6.1 Valgrind工具集

```
Valgrind工具：

┌─────────────────────────────────────────────────────────────┐
│  Memcheck        │  内存错误检测（默认）                    │
│  Cachegrind      │  缓存性能分析                            │
│  Callgrind       │  函数调用分析                            │
│  Helgrind        │  线程错误检测                            │
│  DRD             │  线程错误检测（替代Helgrind）            │
│  Massif          │  堆分析                                  │
│  DHAT            │  动态堆分析                              │
│  SGCheck         │  栈/全局数组越界检测                     │
│  BBV             │  基本块向量（模拟）                      │
│  Lackey          │  示例工具                                │
└─────────────────────────────────────────────────────────────┘
```

### 6.2 Memcheck使用

```bash
# ========== 基本用法 ==========

# 运行程序
valgrind ./program

# 详细泄漏检查
valgrind --leak-check=full ./program

# 显示可达的泄漏（可能是误报）
valgrind --leak-check=full --show-leak-kinds=all ./program

# 生成报告文件
valgrind --leak-check=full --log-file=valgrind.log ./program

# 检测未初始化值使用
valgrind --track-origins=yes ./program

# ========== 常用选项 ==========

valgrind \
    --tool=memcheck \           # 使用Memcheck（默认）
    --leak-check=full \         # 详细泄漏检查
    --show-leak-kinds=all \     # 显示所有泄漏类型
    --track-origins=yes \       # 追踪未初始化值的来源
    --verbose \                 # 详细输出
    --log-file=valgrind.log \   # 输出到文件
    --suppressions=valgrind.supp \  # 抑制文件
    --gen-suppressions=yes \    # 自动生成抑制规则
    ./program
```

### 6.3 Valgrind抑制文件

```
# valgrind.supp 示例

# 抑制特定函数
{
   my_known_leak
   Memcheck:Leak
   match-leak-kinds: definite
   fun:my_known_leaky_function
}

# 抑制特定库的泄漏
{
   third_party_leak
   Memcheck:Leak
   ...
   obj:*/libthird_party.so*
}

# 抑制OpenSSL的误报
{
   openssl_error
   Memcheck:Cond
   obj:*/libcrypto.so*
}
```

### 6.4 其他Valgrind工具

```bash
# Cachegrind - 缓存分析
valgrind --tool=cachegrind ./program
cg_annotate cachegrind.out.*

# Callgrind - 调用图分析
valgrind --tool=callgrind ./program
callgrind_annotate callgrind.out.*
kcachegrind callgrind.out.*  # GUI查看

# Helgrind - 线程错误
valgrind --tool=helgrind ./program

# Massif - 堆分析
valgrind --tool=massif ./program
ms_print massif.out.*
```

---

## 7. 工具选择指南

```
场景选择：

内存错误检测：
• 开发阶段 → ASan（快，精确）
• 测试阶段 → ASan + Valgrind（互补）
• 生产环境 → 无法直接运行

未初始化内存：
• 有MSan工具链 → MSan
• 否则 → Valgrind

线程错误：
• 开发阶段 → TSan
• 否则 → Helgrind

未定义行为：
• 必须 → UBSan

性能分析：
• CPU → perf
• 缓存 → Cachegrind
• 调用图 → Callgrind, perf
• 内存分配 → Massif

组合推荐：
• 日常开发：ASan + UBSan
• CI测试：ASan + TSan + UBSan（分别运行）
• 发布前：Valgrind完整检查
```

---

## 8. CI/CD集成

```yaml
# .github/workflows/dynamic-analysis.yml
name: Dynamic Analysis

on: [push, pull_request]

jobs:
  asan:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Build with ASan
      run: |
        gcc -fsanitize=address -fno-omit-frame-pointer -g \
            -fsanitize-address-use-after-scope \
            program.c -o program

    - name: Run tests with ASan
      run: |
        ASAN_OPTIONS=detect_leaks=1:log_path=asan \
          ./run_tests.sh

    - name: Upload ASan logs
      if: failure()
      uses: actions/upload-artifact@v3
      with:
        name: asan-logs
        path: asan.*

  tsan:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Build with TSan
      run: |
        gcc -fsanitize=thread -g program.c -o program -pthread

    - name: Run tests with TSan
      run: ./run_tests.sh

  ubsan:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Build with UBSan
      run: |
        gcc -fsanitize=undefined -fno-sanitize-recover=all -g \
            program.c -o program

    - name: Run tests with UBSan
      run: ./run_tests.sh

  valgrind:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Install Valgrind
      run: sudo apt-get install -y valgrind

    - name: Build
      run: gcc -g -O0 program.c -o program

    - name: Run Valgrind
      run: |
        valgrind --leak-check=full --error-exitcode=1 \
          --suppressions=valgrind.supp \
          ./run_tests.sh
```

---

## 关联导航

### 前置知识

- [内存安全](../../01_Core_Knowledge_System/02_Core_Layer/01_Memory_Model.md)
- [并发编程](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md)

### 后续延伸

- [性能分析工具](./07_Performance_Profiling.md)
- [Linux运行时调试](./04_Linux_Runtime_Debugging.md)

### 参考

- AddressSanitizer Wiki
- Valgrind User Manual
- ThreadSanitizer Documentation
