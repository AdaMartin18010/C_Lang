# 调试工具与技术 (Debugging Tools & Techniques)

## 目录

- [调试工具与技术 (Debugging Tools \& Techniques)](#调试工具与技术-debugging-tools--techniques)
  - [目录](#目录)
  - [概述](#概述)
    - [调试的层次](#调试的层次)
  - [GDB 高级技巧](#gdb-高级技巧)
    - [基本配置与启动](#基本配置与启动)
    - [高级断点技巧](#高级断点技巧)
    - [数据检查技巧](#数据检查技巧)
    - [栈帧与回溯](#栈帧与回溯)
    - [内存调试](#内存调试)
    - [多线程调试](#多线程调试)
    - [GDB脚本自动化](#gdb脚本自动化)
    - [Python扩展（GDB 7.0+）](#python扩展gdb-70)
  - [LLDB 调试器](#lldb-调试器)
    - [LLDB vs GDB 命令对照](#lldb-vs-gdb-命令对照)
    - [LLDB 高级特性](#lldb-高级特性)
    - [LLDB Python脚本](#lldb-python脚本)
  - [Valgrind 内存检测](#valgrind-内存检测)
    - [工具集概览](#工具集概览)
    - [Memcheck 深度使用](#memcheck-深度使用)
    - [常见错误检测示例](#常见错误检测示例)
    - [Valgrind 抑制文件](#valgrind-抑制文件)
  - [AddressSanitizer](#addresssanitizer)
    - [简介](#简介)
    - [使用方法](#使用方法)
    - [环境变量配置](#环境变量配置)
    - [示例输出解读](#示例输出解读)
    - [与Valgrind对比](#与valgrind对比)
  - [调试策略与方法论](#调试策略与方法论)
    - [科学调试法](#科学调试法)
    - [二分法定位](#二分法定位)
    - [防御式编程与调试支持](#防御式编程与调试支持)
  - [实战案例分析](#实战案例分析)
    - [案例1：堆内存损坏定位](#案例1堆内存损坏定位)
    - [案例2：并发死锁分析](#案例2并发死锁分析)
    - [案例3：性能问题定位](#案例3性能问题定位)
  - [参考资源](#参考资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 概述

调试是软件开发中最具挑战性的环节之一。
据研究表明，程序员平均花费 **50%-75%** 的时间在调试和测试上。
掌握高效的调试工具和技术，是成为优秀C程序员的关键。

### 调试的层次

```
┌─────────────────────────────────────────────────────────────────┐
│                      调试层次金字塔                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│                         ┌───────┐                               │
│                         │ 静态  │  ← 编译期检查、静态分析         │
│                         │ 分析  │                               │
│                         └───┬───┘                               │
│                     ┌───────┴───────┐                           │
│                     │   动态分析    │  ← 运行时检查、内存检测     │
│                     │ (Valgrind/ASan)│                           │
│                     └───────┬───────┘                           │
│                 ┌───────────┴───────────┐                       │
│                 │      交互式调试       │  ← GDB/LLDB断点调试   │
│                 │    (Interactive)      │                       │
│                 └───────────┬───────────┘                       │
│             ┌───────────────┴───────────────┐                   │
│             │          日志追踪            │  ← printf/日志系统  │
│             │      (Logging/Tracing)       │                   │
│             └───────────────────────────────┘                   │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## GDB 高级技巧

### 基本配置与启动

```bash
# 编译时启用调试信息
gcc -g -O0 program.c -o program    # -O0 关闭优化，便于调试
gcc -ggdb3 program.c -o program    # 最大调试信息

# 启动GDB
gdb ./program                        # 直接启动
gdb ./program core                  # 分析core dump
gdb -p <pid>                        # 附加到运行中的进程
```

### 高级断点技巧

```gdb
# 1. 条件断点 - 只在特定条件下停止
break main.c:45 if i == 100
break process_data if ptr != NULL && ptr->size > 1024

# 2. 临时断点 - 只触发一次
tbreak parse_header

# 3. 忽略前N次触发
ignore 1 10    # 忽略断点1的前10次触发

# 4. 硬件断点（数量限制但速度快）
hbreak *0x400500    # 监视特定地址
watch -l global_var # 监视变量修改

# 5. 断点命令序列
break process_packet
commands
  silent                    # 不显示断点信息
  print packet->type
  print packet->length
  if packet->type == ERROR
    backtrace full
  end
  continue                  # 自动继续
end
```

### 数据检查技巧

```gdb
# 1. 数组打印
p array@10              # 打印前10个元素
p *array@10            # 同上
p/x (int[10])array     # 以十六进制打印数组

# 2. 结构体打印
p *ptr                 # 展开指针内容
p/t structure          # 以树形结构打印
set print pretty on    # 美化结构体输出
set print array on     # 美化数组输出

# 3. 字符串显示
p/s char_ptr           # 作为字符串打印
x/s &buffer            # 检查内存中的字符串
x/20wx &buffer         # 以十六进制显示20个字

# 4. 类型检查
whatis variable        # 显示变量类型
ptype struct_name      # 显示结构体定义
info types regex       # 列出匹配的类型

# 5. 表达式求值
p function(arg)        # 调用函数并打印返回值
p sizeof(struct node)  # 计算类型大小
```

### 栈帧与回溯

```gdb
# 栈帧操作
bt                     # 显示完整调用栈
bt full                # 包含局部变量
frame 3                # 切换到第3层栈帧
up / down              # 上移/下移栈帧

# 查看特定栈帧的信息
info frame             # 当前栈帧详情
info locals            # 局部变量
info args              # 函数参数
info registers         # 寄存器值

# 选择性回溯
backtrace 10           # 只显示前10层
backtrace -10          # 只显示后10层
```

### 内存调试

```gdb
# 内存检查
x/10gx &array          # 以64位十六进制显示10个单元
x/20i $pc              # 显示接下来的20条汇编指令
x/10wx 0x7fff0000      # 检查特定地址

# 内存断点
watch variable         # 变量修改时停止
rw variable            # 变量读取时停止
awatch variable        # 读取或修改时停止

# 内存映射检查
info proc mappings     # 显示进程内存映射
maintenance info sections  # 显示ELF段信息
```

### 多线程调试

```gdb
# 线程管理
info threads                    # 列出所有线程
thread 3                        # 切换到线程3
thread apply all bt             # 所有线程的调用栈
thread apply 1 3 5 info locals  # 指定线程的信息

# 线程断点
break process.c:50 thread 2     # 只在特定线程中断
break process.c:50 thread 2 if x > 10  # 条件+线程

# 线程锁调试
set scheduler-locking on        # 只运行当前线程
set scheduler-locking step      # 单步时锁定
set scheduler-locking off       # 恢复多线程

# 观察线程创建
catch syscall clone             # 捕获线程创建
set print thread-events on      # 显示线程创建/退出
```

### GDB脚本自动化

```gdb
# save as debug.gdb
define log_backtrace
    set logging file backtrace.log
    set logging on
    echo \n=== Backtrace at
    print time(NULL)
    bt full
    set logging off
end

define heap_check
    if $argc != 1
        help heap_check
    else
        set $heap = $arg0
        printf "Heap status at %p:\n", $heap
        x/4wx $heap
    end
end

document heap_check
检查堆内存状态
用法: heap_check heap_pointer
end

# 加载脚本
gdb -x debug.gdb ./program
# 或在GDB中：
# (gdb) source debug.gdb
```

### Python扩展（GDB 7.0+）

```python
# save as gdb_printers.py
import gdb

class StringPrinter:
    """打印自定义字符串类型"""
    def __init__(self, val):
        self.val = val

    def to_string(self):
        length = int(self.val['length'])
        data = self.val['data']
        return '"' + data.string(length=length) + '"'

    def display_hint(self):
        return 'string'

class VectorPrinter:
    """打印动态数组"""
    def __init__(self, val):
        self.val = val

    def to_string(self):
        size = int(self.val['size'])
        capacity = int(self.val['capacity'])
        return f"Vector(size={size}, capacity={capacity})"

    def children(self):
        size = int(self.val['size'])
        data = self.val['data']
        for i in range(size):
            yield f"[{i}]", data[i]

    def display_hint(self):
        return 'array'

# 注册打印器
def build_pretty_printer():
    pp = gdb.printing.RegexpCollectionPrettyPrinter("mylib")
    pp.add_printer('String', '^String$', StringPrinter)
    pp.add_printer('Vector', '^Vector$', VectorPrinter)
    return pp

gdb.printing.register_pretty_printer(
    gdb.current_objfile(),
    build_pretty_printer()
)
```

---

## LLDB 调试器

### LLDB vs GDB 命令对照

| 功能 | GDB | LLDB |
|------|-----|------|
| 启动 | `gdb program` | `lldb program` |
| 断点 | `b main` | `b main` / `breakpoint set -n main` |
| 运行 | `run` | `run` / `r` |
| 单步 | `step` / `next` | `s` / `n` / `step` / `next` |
| 继续 | `continue` | `c` / `continue` |
| 打印 | `p var` | `p var` / `expr var` |
| 调用栈 | `bt` | `bt` / `thread backtrace` |
| 线程 | `info threads` | `thread list` |
| 反汇编 | `disas` | `disassemble` / `di` |
| 寄存器 | `info reg` | `register read` |

### LLDB 高级特性

```bash
# 表达式求值
expr ptr->data[5] = 100           # 修改变量
expr -- function_call(arg)        # 调用函数
expr -f x -- var                  # 十六进制格式

# Python脚本
script print("Hello from Python")
script for t in lldb.thread_list: print(t)

# 条件断点
breakpoint set -n process -c "i > 10"

# 命令别名
command alias plb platform select remote-ios
command alias bfl breakpoint set -f %1 -l %2

# 持久化设置
settings set target.x86-disassembly-flavor intel
settings set target.inline-breakpoint-strategy always
```

### LLDB Python脚本

```python
# save as lldb_script.py
import lldb

def trace_function(debugger, command, result, internal_dict):
    """跟踪函数调用"""
    target = debugger.GetSelectedTarget()
    breakpoint = target.BreakpointCreateByName(command)

    # 添加回调
    def callback(frame, bp_loc, dict):
        function = frame.GetFunctionName()
        args = []
        for i in range(frame.GetNumArguments()):
            val = frame.GetArgumentAtIndex(i)
            args.append(f"{val.GetName()}={val.GetValue()}")
        print(f"[TRACE] {function}({', '.join(args)})")
        return False  # 不停止

    breakpoint.SetScriptCallbackFunction("lldb_script.callback")

def print_heap(debugger, command, result, internal_dict):
    """打印堆信息"""
    target = debugger.GetSelectedTarget()
    process = target.GetProcess()

    for region in process.GetMemoryRegions():
        if region.IsWritable() and region.IsReadable():
            print(f"Heap: {region.GetRegionBase()} - {region.GetRegionEnd()}")

# 注册命令
def __lldb_init_module(debugger, internal_dict):
    debugger.HandleCommand(
        'command script add -f lldb_script.trace_function trace'
    )
    debugger.HandleCommand(
        'command script add -f lldb_script.print_heap heap'
    )
```

---

## Valgrind 内存检测

### 工具集概览

| 工具 | 用途 |
|------|------|
| Memcheck | 内存错误检测（最常用） |
| Cachegrind | 缓存和分支预测分析 |
| Callgrind | 函数调用分析 |
| Helgrind | 线程错误检测 |
| DRD | 线程错误检测（替代Helgrind） |
| Massif | 堆分析 |

### Memcheck 深度使用

```bash
# 基本用法
valgrind --leak-check=full --show-leak-kinds=all ./program

# 完整参数
valgrind \
    --tool=memcheck \
    --leak-check=full \
    --show-leak-kinds=definite,indirect \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind.log \
    --suppressions=custom.supp \
    ./program arg1 arg2
```

### 常见错误检测示例

```c
// example_buggy.c - 故意包含各种内存错误
#include <stdlib.h>
#include <string.h>

void illegal_read() {
    int *p = malloc(10 * sizeof(int));
    int x = p[20];  // 越界读取
    free(p);
}

void illegal_write() {
    int *p = malloc(10 * sizeof(int));
    p[20] = 42;     // 越界写入
    free(p);
}

void use_after_free() {
    int *p = malloc(sizeof(int));
    free(p);
    *p = 42;        // 使用已释放内存
}

void double_free() {
    int *p = malloc(sizeof(int));
    free(p);
    free(p);        // 双重释放
}

void memory_leak() {
    int *p = malloc(sizeof(int));  // 未释放
    *p = 42;
}

void uninitialized() {
    int x;
    if (x > 0) {    // 使用未初始化变量
        printf("x is positive\n");
    }
}

int main() {
    illegal_read();
    illegal_write();
    use_after_free();
    double_free();
    memory_leak();
    uninitialized();
    return 0;
}
```

```bash
# 编译并检测
gcc -g example_buggy.c -o buggy
valgrind --leak-check=full ./buggy
```

### Valgrind 抑制文件

```
# custom.supp - 抑制已知/可接受的错误
{
   known_glibc_leak
   Memcheck:Leak
   match-leak-kinds: reachable
   ...
   fun:allocate_dtv
   fun:_dl_allocate_tls
   ...
}

{
   openssl_uninitialized
   Memcheck:Cond
   obj:/usr/lib/libcrypto.so*
}

{
   custom_suppression
   Memcheck:Addr4
   fun:specific_function
   obj:*/mylib.so*
}
```

---

## AddressSanitizer

### 简介

AddressSanitizer (ASan) 是Clang/GCC内置的内存错误检测器，相比Valgrind：

- **速度更快** (2-3x slowdown vs 10-20x)
- **检测范围更广** (栈、堆、全局变量)
- **集成更方便** (编译器内置)

### 使用方法

```bash
# 编译启用ASan
clang -fsanitize=address -g program.c -o program
gcc -fsanitize=address -g program.c -o program

# 可选：同时使用UBSan检测未定义行为
clang -fsanitize=address,undefined -g program.c -o program
```

### 环境变量配置

```bash
# ASAN_OPTIONS 常用选项
export ASAN_OPTIONS=\
    detect_stack_use_after_return=1:\
    detect_container_overflow=1:\
    detect_leaks=1:\
    halt_on_error=0:\
    print_stats=1:\
    log_path=asan.log

# 禁用特定检查
export ASAN_OPTIONS=detect_odr_violation=0
```

### 示例输出解读

```
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000014
WRITE of size 4 at 0x602000000014 thread T0
    #0 0x55a3e8 in illegal_write example.c:14
    #1 0x55a4f2 in main example.c:35
    #2 0x7f8b2c in __libc_start_main

0x602000000014 is located 40 bytes to the right of 10-byte region [0x602000000010,0x602000000028)
allocated by thread T0 here:
    #0 0x7f8a10 in malloc
    #1 0x55a3d0 in illegal_write example.c:13
```

### 与Valgrind对比

| 特性 | Valgrind | AddressSanitizer |
|------|----------|------------------|
| 速度 | 慢 (10-20x) | 较快 (2-3x) |
| 内存 | 高 | 中等 |
| 侵入性 | 无 | 需要重新编译 |
| 检测范围 | 堆为主 | 堆+栈+全局 |
| 误报率 | 低 | 极低 |
| 平台 | 多平台 | Linux/Mac |

---

## 调试策略与方法论

### 科学调试法

```
┌─────────────────────────────────────────────────────────────┐
│                    科学调试流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   1. 复现问题        ───────────────────────┐               │
│      - 创建最小可复现示例                    │               │
│      - 记录触发条件和环境                    │               │
│      - 确认问题的确定性/随机性              │               │
│                                            ▼               │
│   2. 提出假设 ◄─────────────────────  6. 验证修复          │
│      - 基于代码分析提出可能原因             ▲               │
│      - 设计实验验证每个假设                 │               │
│      - 一次只改变一个变量                   │               │
│                                            │               │
│   3. 预测结果                              │               │
│      - 如果假设正确，应该观察到什么？       │               │
│      - 设计可观察的验证方法                 │               │
│                                            │               │
│   4. 验证实验                              │               │
│      - 运行实验收集数据                     │               │
│      - 比较实际结果与预测                   │               │
│                                            │               │
│   5. 分析结论                              │               │
│      - 如果符合预测：假设成立 → 定位问题    │               │
│      - 如果不符合：返回步骤2                │               │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 二分法定位

```bash
# 使用git bisect自动定位引入bug的提交
git bisect start
git bisect bad HEAD          # 当前版本有问题
git bisect good v1.0         # 1.0版本是好的

# Git自动checkout中间版本，测试后标记
git bisect good    # 或 bad

# 自动化的二分查找
git bisect run ./test_script.sh
```

### 防御式编程与调试支持

```c
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* 调试宏定义 */
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) fprintf(stderr, "[DEBUG] %s:%d " fmt "\n", \
                                          __FILE__, __LINE__, ##__VA_ARGS__)
    #define DEBUG_ASSERT(cond) assert(cond)
    #define TRACE_ENTRY() DEBUG_PRINT("--> Entering %s", __func__)
    #define TRACE_EXIT()  DEBUG_PRINT("<-- Exiting %s", __func__)
#else
    #define DEBUG_PRINT(fmt, ...)
    #define DEBUG_ASSERT(cond)
    #define TRACE_ENTRY()
    #define TRACE_EXIT()
#endif

/* 运行时检查宏 */
#define CHECK_NONNULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "[ERROR] %s:%d Null pointer: %s\n", \
                __FILE__, __LINE__, #ptr); \
        abort(); \
    } \
} while(0)

#define CHECK_BOUNDS(index, size) do { \
    if ((index) < 0 || (index) >= (size)) { \
        fprintf(stderr, "[ERROR] %s:%d Index out of bounds: %d (size=%d)\n", \
                __FILE__, __LINE__, (int)(index), (int)(size)); \
        abort(); \
    } \
} while(0)

/* 使用示例 */
void process_array(int *arr, size_t size, size_t index) {
    TRACE_ENTRY();

    CHECK_NONNULL(arr);
    CHECK_BOUNDS(index, size);

    DEBUG_PRINT("Processing arr[%zu] = %d", index, arr[index]);

    // 处理逻辑...
    arr[index] *= 2;

    DEBUG_ASSERT(arr[index] % 2 == 0);

    TRACE_EXIT();
}
```

---

## 实战案例分析

### 案例1：堆内存损坏定位

```c
// 症状：程序随机崩溃，崩溃位置不固定
// 原因：缓冲区溢出破坏了堆元数据

void buggy_function() {
    char *buffer = malloc(100);

    // 错误：复制了超过100字节
    strcpy(buffer, "This is a very long string that exceeds "
                   "the allocated buffer size of 100 bytes...");

    free(buffer);  // 崩溃可能在这里，因为元数据被破坏
}

// 调试方法：
// 1. Valgrind: valgrind --tool=memcheck ./program
//    输出：Invalid write of size 1 at ...
//
// 2. ASan: 编译时加 -fsanitize=address
//    输出：ERROR: AddressSanitizer: heap-buffer-overflow
//
// 3. GDB: 设置 watchpoint 监视 malloc 返回的地址+100
//    (gdb) watch *(char*)(buffer + 100)
```

### 案例2：并发死锁分析

```c
#include <pthread.h>

pthread_mutex_t mutex_a = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_b = PTHREAD_MUTEX_INITIALIZER;

void *thread1(void *arg) {
    pthread_mutex_lock(&mutex_a);
    // ... 一些操作
    pthread_mutex_lock(&mutex_b);  // 等待mutex_b
    // ...
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
    return NULL;
}

void *thread2(void *arg) {
    pthread_mutex_lock(&mutex_b);
    // ... 一些操作
    pthread_mutex_lock(&mutex_a);  // 等待mutex_a - 死锁！
    // ...
    pthread_mutex_unlock(&mutex_a);
    pthread_mutex_unlock(&mutex_b);
    return NULL;
}

// 调试方法：
// 1. GDB 多线程调试：
//    (gdb) info threads  # 查看所有线程
//    (gdb) thread apply all bt  # 所有线程的调用栈
//
// 2. Valgrind + Helgrind：
//    valgrind --tool=helgrind ./program
//    输出：Lock order violated
//
// 3. 修复方案：统一加锁顺序
//    总是先锁A再锁B，或者使用 trylock
```

### 案例3：性能问题定位

```bash
# 使用Valgrind Cachegrind分析缓存性能
valgrind --tool=cachegrind ./program
cg_annotate cachegrind.out.*

# 使用gprof
gcc -pg program.c -o program
./program
gprof ./program gmon.out > profile.txt

# 使用perf (Linux)
perf record -g ./program
perf report
perf annotate

# 使用Intel VTune (商业软件)
vtune -collect hotspots ./program
```

---

## 参考资源

- [GDB Documentation](https://sourceware.org/gdb/documentation/)
- [LLDB Documentation](https://lldb.llvm.org/)
- [Valgrind Manual](https://valgrind.org/docs/manual/)
- [AddressSanitizer Wiki](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [The Art of Debugging - Norm Matloff & Peter Jay Salzman]

---

*"调试就像是犯罪电影中的侦探工作，而你同时还是凶手。"* —— Filipe Fortes


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
