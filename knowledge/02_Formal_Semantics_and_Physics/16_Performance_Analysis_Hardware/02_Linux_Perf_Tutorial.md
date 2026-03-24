# Linux Perf工具完整实战教程


> **版本**: v1.0.0
> **最后更新**: 2026-03-25
> **作者**: C_Lang Team
> **难度**: L3 进阶

---

## 目录

- [Linux Perf工具完整实战教程](#linux-perf工具完整实战教程)
  - [目录](#目录)
  - [1. Perf工具概述](#1-perf工具概述)
    - [1.1 Perf简介](#11-perf简介)
    - [1.2 Perf的核心优势](#12-perf的核心优势)
    - [1.3 安装与配置](#13-安装与配置)
    - [1.4 Perf事件分类](#14-perf事件分类)
  - [2. Perf基础命令详解](#2-perf基础命令详解)
    - [2.1 perf stat - 性能计数统计](#21-perf-stat---性能计数统计)
      - [2.1.1 基本用法](#211-基本用法)
      - [2.1.2 详细示例与输出解析](#212-详细示例与输出解析)
      - [2.1.3 高级选项](#213-高级选项)
      - [2.1.4 输出字段详解](#214-输出字段详解)
    - [2.2 perf record - 性能数据采样](#22-perf-record---性能数据采样)
      - [2.2.1 基本用法](#221-基本用法)
      - [2.2.2 采样选项详解](#222-采样选项详解)
      - [2.2.3 输出文件分析](#223-输出文件分析)
    - [2.3 perf report - 生成性能报告](#23-perf-report---生成性能报告)
      - [2.3.1 基本用法](#231-基本用法)
      - [2.3.2 报告输出解析](#232-报告输出解析)
      - [2.3.3 报告选项详解](#233-报告选项详解)
    - [2.4 perf top - 实时性能监控](#24-perf-top---实时性能监控)
      - [2.4.1 基本用法](#241-基本用法)
      - [2.4.2 交互式命令](#242-交互式命令)
      - [2.4.3 输出示例](#243-输出示例)
  - [3. Perf高级功能](#3-perf高级功能)
    - [3.1 perf annotate - 代码级分析](#31-perf-annotate---代码级分析)
      - [3.1.1 基本用法](#311-基本用法)
      - [3.1.2 输出示例](#312-输出示例)
    - [3.2 perf script - 原始数据处理](#32-perf-script---原始数据处理)
      - [3.2.1 基本用法](#321-基本用法)
      - [3.2.2 脚本处理示例](#322-脚本处理示例)
    - [3.3 perf trace - 系统调用跟踪](#33-perf-trace---系统调用跟踪)
      - [3.3.1 基本用法](#331-基本用法)
      - [3.3.2 输出示例](#332-输出示例)
    - [3.4 perf probe - 动态探针](#34-perf-probe---动态探针)
      - [3.4.1 基本用法](#341-基本用法)
      - [3.4.2 探针使用示例](#342-探针使用示例)
  - [4. 事件选择与分析](#4-事件选择与分析)
    - [4.1 常用硬件事件](#41-常用硬件事件)
    - [4.2 Intel特定事件](#42-intel特定事件)
    - [4.3 事件选择策略](#43-事件选择策略)
    - [4.4 事件列表查询](#44-事件列表查询)
  - [5. Flame Graph火焰图生成](#5-flame-graph火焰图生成)
    - [5.1 火焰图简介](#51-火焰图简介)
    - [5.2 火焰图生成步骤](#52-火焰图生成步骤)
    - [5.3 针对特定程序的火焰图](#53-针对特定程序的火焰图)
    - [5.4 火焰图变体](#54-火焰图变体)
    - [5.5 生成火焰图的C程序示例](#55-生成火焰图的c程序示例)
    - [5.6 编译和生成火焰图](#56-编译和生成火焰图)
  - [6. 实际案例分析](#6-实际案例分析)
    - [6.1 案例一：矩阵乘法优化分析](#61-案例一矩阵乘法优化分析)
      - [6.1.1 朴素实现分析](#611-朴素实现分析)
      - [6.1.2 使用perf record定位热点](#612-使用perf-record定位热点)
      - [6.1.3 优化版本](#613-优化版本)
    - [6.2 案例二：内存泄漏与分配分析](#62-案例二内存泄漏与分配分析)
    - [6.3 案例三：多线程锁竞争分析](#63-案例三多线程锁竞争分析)
  - [7. Perf与C程序集成](#7-perf与c程序集成)
    - [7.1 程序内性能测量](#71-程序内性能测量)
    - [7.2 编译和运行](#72-编译和运行)
    - [7.3 Perf与CMake集成](#73-perf与cmake集成)
  - [8. 总结与最佳实践](#8-总结与最佳实践)
    - [8.1 Perf使用流程](#81-perf使用流程)
    - [8.2 常见问题与解决方案](#82-常见问题与解决方案)
    - [8.3 推荐学习资源](#83-推荐学习资源)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 1. Perf工具概述

### 1.1 Perf简介

Linux perf（Performance Counters for Linux）是Linux内核提供的强大性能分析工具，基于内核的perf_events子系统。
它利用硬件PMU（Performance Monitoring Unit）和内核跟踪点，提供低开销、高精度的性能分析能力。

```
┌─────────────────────────────────────────────────────────────────┐
│                      Perf工具架构                                │
├─────────────────────────────────────────────────────────────────┤
│  用户空间工具 (perf)                                            │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ perf stat  │  perf record │  perf report │  perf top   │   │
│  │ perf annotate│ perf script │ perf trace   │ perf probe  │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  系统调用接口                                                   │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │              perf_event_open()                          │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  内核子系统 (perf_events)                                       │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ 硬件PMU事件 │  软件事件  │  跟踪点   │  USDT探针     │   │
│  │ 硬件计数器  │  上下文切换 │  内核trace │  用户态探针   │   │
│  └─────────────────────────────────────────────────────────┘   │
├─────────────────────────────────────────────────────────────────┤
│  硬件层                                                         │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │        x86/ARM/RISC-V PMU 硬件计数器                    │   │
│  └─────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 Perf的核心优势

```
┌─────────────────────────────────────────────────────────────────┐
│                    Perf vs 其他工具                             │
├─────────────────┬──────────────────┬────────────────────────────┤
│     特性        │      Perf        │      其他工具              │
├─────────────────┼──────────────────┼────────────────────────────┤
│ 系统开销        │ 极低 (<1%)       │ gprof较高，Valgrind极高    │
│ 精度            │ 硬件级纳秒       │ 微秒级                     │
│ 侵入性          │ 无侵入           │ 通常需要插桩               │
│ 系统级分析      │ 完整支持         │ 通常仅用户态               │
│ 事件类型        │ 数百种           │ 有限                       │
│ 采样能力        │ 任意频率         │ 受限于实现                 │
│ 内核分析        │ 原生支持         │ 有限或需额外配置           │
└─────────────────┴──────────────────┴────────────────────────────┘
```

### 1.3 安装与配置

```bash
# Ubuntu/Debian安装
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
sudo apt-get update
sudo apt-get install linux-tools-common linux-tools-generic linux-tools-`uname -r`

### CentOS/RHEL安装
sudo yum install perf

### Arch Linux
sudo pacman -S perf

### 验证安装
perf --version
### 输出示例: perf version 5.15.0

### 配置权限 (允许非root用户使用)
echo 'kernel.perf_event_paranoid = 2' | sudo tee -a /etc/sysctl.conf
### 或者更宽松: echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid

### 增加最大采样频率限制
echo 'kernel.perf_event_max_sample_rate = 100000' | sudo tee -a /etc/sysctl.conf
sudo sysctl -p

### 检查当前配置
cat /proc/sys/kernel/perf_event_paranoid
cat /proc/sys/kernel/perf_event_max_sample_rate
```

### 1.4 Perf事件分类

```
┌─────────────────────────────────────────────────────────────────┐
│                    Perf事件类型分类                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 硬件事件 (Hardware Events)                                  │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • cycles           - CPU周期                         │    │
│     │ • instructions     - 退役指令                        │    │
│     │ • cache-references - 缓存引用                        │    │
│     │ • cache-misses     - 缓存未命中                      │    │
│     │ • branches         - 分支指令                        │    │
│     │ • branch-misses    - 分支预测失败                    │    │
│     │ • bus-cycles       - 总线周期                        │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  2. 硬件缓存事件 (Hardware Cache Events)                        │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ L1-dcache-loads, L1-dcache-load-misses              │    │
│     │ L1-dcache-stores, L1-dcache-store-misses            │    │
│     │ L1-icache-loads, L1-icache-load-misses              │    │
│     │ LLC-loads, LLC-load-misses                          │    │
│     │ dTLB-loads, dTLB-load-misses                        │    │
│     │ iTLB-loads, iTLB-load-misses                        │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  3. 软件事件 (Software Events)                                  │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • cpu-clock        - CPU时钟时间                     │    │
│     │ • task-clock       - 任务时钟时间                    │    │
│     │ • page-faults      - 缺页异常                        │    │
│     │ • context-switches - 上下文切换                      │    │
│     │ • cpu-migrations   - CPU迁移                         │    │
│     │ • minor-faults     - 轻微缺页                        │    │
│     │ • major-faults     - 严重缺页                        │    │
│     │ • alignment-faults - 对齐异常                        │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  4. 内核跟踪点 (Kernel Tracepoints)                             │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • sched:sched_switch       - 调度切换               │    │
│     │ • sched:sched_wakeup       - 任务唤醒               │    │
│     │ • irq:irq_handler_entry    - 中断进入               │    │
│     │ • syscalls:sys_enter_*     - 系统调用进入           │    │
│     │ • block:block_rq_insert    - 块设备请求             │    │
│     │ • kmem:kmem_cache_alloc    - 内存分配               │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
│  5. 动态探针 (Dynamic Probes)                                   │
│     ┌─────────────────────────────────────────────────────┐    │
│     │ • kprobes  - 内核动态探针                           │    │
│     │ • uprobes  - 用户态动态探针                         │    │
│     │ • USDT     - 用户态静态定义探针                     │    │
│     └─────────────────────────────────────────────────────┘    │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Perf基础命令详解

### 2.1 perf stat - 性能计数统计

`perf stat`用于执行命令并统计性能计数器数据，是最常用的perf命令之一。

#### 2.1.1 基本用法

```bash
### 基本统计 - 运行命令并显示统计信息
perf stat ls -la

### 统计自定义命令
perf stat ./my_program

### 统计系统范围 (所有CPU)
sudo perf stat -a sleep 5

### 统计特定PID
perf stat -p 1234 sleep 10

### 统计特定CPU
perf stat -C 0,1 sleep 5

### 重复运行多次取平均
perf stat -r 10 ./my_benchmark
```

#### 2.1.2 详细示例与输出解析

```bash
### 综合性能统计示例
$ perf stat -B -e cycles,instructions,cache-references,cache-misses,branches,branch-misses ./matrix_multiply

### 输出示例:
 Performance counter stats for './matrix_multiply':

     2,456,789,123      cycles                    #    2.891 GHz
     3,123,456,789      instructions              #    1.27  insn per cycle
       156,234,567      cache-references          #   18.432 M/sec
        12,345,678      cache-misses              #    7.898 % of all cache refs
       523,456,789      branches                  #   61.728 M/sec
         5,234,567      branch-misses             #    1.00% of all branches

       0.849234512 seconds time elapsed

       0.840123000 seconds user
       0.008000000 seconds sys
```

#### 2.1.3 高级选项

```bash
### 显示详细统计信息
perf stat -v ./program

### 显示CPU利用率
perf stat --cpu-util ./program

### 包含内核统计
perf stat -e cycles:k,cycles:u ./program
### :k = 仅内核态, :u = 仅用户态

### 以CSV格式输出
perf stat -x, -e cycles,instructions ./program

### 详细事件统计 (包含比例)
perf stat -d ./program

### 超详细统计
perf stat -ddd ./program

### 聚合多个事件的统计
perf stat -e '{cycles,instructions,cache-misses}' ./program

### 监控线程组
perf stat -t 1234,1235,1236 sleep 5
```

#### 2.1.4 输出字段详解

```
┌─────────────────────────────────────────────────────────────────┐
│                    perf stat 输出字段说明                        │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  原始计数                        说明                           │
│  ─────────                       ─────                          │
│  cycles          - CPU执行的时钟周期数                          │
│  instructions    - 成功退役的指令数                             │
│  cache-references - 缓存访问总次数                              │
│  cache-misses    - 缓存未命中次数                               │
│  branches        - 执行的分支指令数                             │
│  branch-misses   - 分支预测失败次数                             │
│                                                                 │
│  派生指标                                                       │
│  ─────────                                                      │
│  GHz (cycles/time)           - 实际运行频率                     │
│  insn per cycle (IPC)        - 每周期指令数                     │
│  cycles per insn (CPI)       - 每指令周期数                     │
│  cache-miss %                - 缓存未命中率                     │
│  branch-miss %               - 分支预测失败率                   │
│                                                                 │
│  时间指标                                                       │
│  ─────────                                                      │
│  time elapsed    -  wall-clock时间 (真实经过时间)               │
│  user            - 用户态CPU时间                                │
│  sys             - 内核态CPU时间                                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 2.2 perf record - 性能数据采样

`perf record`用于采样程序运行时的性能数据，生成`perf.data`文件供后续分析。

#### 2.2.1 基本用法

```bash
### 基本采样
perf record ./my_program

### 指定采样频率 (每秒1000次)
perf record -F 1000 ./my_program

### 指定采样周期 (每10000个事件采样一次)
perf record -c 10000 -e cycles ./my_program

### 采样特定事件
perf record -e cache-misses ./my_program

### 采样包括内核
perf record -a -g sleep 10

### 采样特定PID
perf record -p 1234 -g -- sleep 30

### 采样所有CPU
sudo perf record -a -g -- sleep 10
```

#### 2.2.2 采样选项详解

```bash
### 设置采样频率 (注意系统限制)
### 检查当前限制
cat /proc/sys/kernel/perf_event_max_sample_rate

### 常用采样频率
perf record -F 99 ./program      # 99Hz - 避免与定时器冲突
perf record -F 997 ./program     # 997Hz - 高频采样
perf record -F 10000 ./program   # 10kHz - 非常高频

### 采样call graph (调用栈)
perf record -g ./program         # 使用帧指针
perf record --call-graph dwarf ./program  # 使用DWARF信息
perf record --call-graph lbr ./program    # 使用Last Branch Record

### 限制采样范围
perf record -g -- ./program arg1 arg2

### 实时显示采样进度
perf record -v ./program

### 限制输出文件大小
perf record --data-size=1G ./program

### 采样特定CPU
perf record -C 0,1,2,3 ./program

### 组合多个事件采样
perf record -e cycles -e cache-misses -e branch-misses ./program
```

#### 2.2.3 输出文件分析

```bash
### 生成的perf.data文件信息
$ ls -lh perf.data
-rw------- 1 user user 2.3M Mar 19 10:00 perf.data

### 检查采样数据概要
perf report --header-only

### 输出示例:
### ========
### captured on    : Mon Mar 19 10:00:00 2026
### header version : 1
### data offset    : 392
### data size      : 2398472
### feat offset    : 2398864
### hostname       : workstation
### os release     : 5.15.0-91-generic
### arch           : x86_64
### nrcpus online  : 16
### nrcpus avail   : 16
### cpudesc        : Intel(R) Core(TM) i9-12900K
### total memory   : 65536000 kB
### cmdline        : /usr/lib/linux-tools/5.15.0-91-generic/perf record ./matrix_multiply
### event          : cycles (type: 0, config: 0)
### ...
```

### 2.3 perf report - 生成性能报告

`perf report`用于读取`perf.data`文件并生成可读的性能分析报告。

#### 2.3.1 基本用法

```bash
### 基本报告
perf report

### 指定输入文件
perf report -i perf.data.old

### 显示调用图
perf report -g

### 仅显示特定符号
perf report --symbols=main,calculate

### 按CPU分组显示
perf report --sort=cpu

### 显示调用链深度
perf report -g --max-stack=10

### 以累积方式显示
perf report --hierarchy

### 交互式TUI界面
perf report --tui

### 生成stdio报告 (非交互)
perf report --stdio
```

#### 2.3.2 报告输出解析

```bash
### 标准报告输出
$ perf report --stdio

### 输出示例:
### Samples: 12K of event 'cycles', Event count (approx.): 8945123456
### Overhead  Command          Shared Object        Symbol
### ........  .......  ..........................  ..............................
#
###    35.42%  matrix_m  matrix_multiply            [.] multiply_kernel
###             |
###             ---multiply_kernel
###                main
###                __libc_start_main
#
###    28.17%  matrix_m  matrix_multiply            [.] initialize_matrix
###             |
###             ---initialize_matrix
###                main
#
###    15.23%  matrix_m  libc-2.31.so               [.] __memset_avx2_unaligned_erms
###             |
###             ---__memset_avx2_unaligned_erms
###                initialize_matrix
###                main
#
###     8.91%  matrix_m  matrix_multiply            [.] main
###     4.56%  matrix_m  [kernel.vmlinux]           [k] smp_call_function_single
###     3.21%  matrix_m  [kernel.vmlinux]           [k] __softirqentry_text_start
###     2.87%  matrix_m  libc-2.31.so               [.] __random
###     1.63%  matrix_m  [kernel.vmlinux]           [k] finish_task_switch
#
```

#### 2.3.3 报告选项详解

```bash
### 按不同维度排序
perf report --sort=comm,dso,symbol        # 按命令、共享对象、符号排序
perf report --sort=pid,comm,symbol        # 按PID、命令、符号排序
perf report --sort=dso,symbol             # 按共享对象、符号排序

### 过滤选项
perf report --dsos=matrix_multiply        # 仅显示特定DSO
perf report --comms=matrix_m              # 仅显示特定命令
perf report --symbols=main,multiply_*     # 仅显示特定符号

### 显示百分比选项
perf report --percent-type=period-local   # 本地百分比
perf report --percent-type=period-global  # 全局百分比
perf report --percent-type=hits-local     # 命中本地百分比

### 调用图选项
perf report -g flat,0.5                   # 扁平显示，阈值0.5%
perf report -g graph,0.5,caller           # 图形显示，显示调用者
perf report -g graph,0.5,callee           # 图形显示，显示被调用者

### 生成特定格式输出
perf report -g --no-children              # 不包含子函数开销
perf report --inline                      # 显示内联函数
```

### 2.4 perf top - 实时性能监控

`perf top`类似于`top`命令，但显示的是CPU时间的消耗热点。

#### 2.4.1 基本用法

```bash
### 基本实时监控
sudo perf top

### 监控特定事件
sudo perf top -e cache-misses

### 监控用户态
sudo perf top --user-space-only

### 不显示内核符号
sudo perf top -K

### 不显示用户态符号
sudo perf top -U

### 显示调用图
sudo perf top -g

### 刷新间隔
sudo perf top -d 2    # 每2秒刷新

### 显示特定PID
sudo perf top -p 1234

### 显示特定DSO中的函数
sudo perf top --dsos=libc.so.6
```

#### 2.4.2 交互式命令

```
在perf top界面中可用的交互命令:

  d      - 切换DSO视图
  s      - 切换符号视图
  t      - 切换线程视图
  h      - 显示帮助
  H      - 切换层次显示
  /      - 搜索符号
  n      - 按符号名排序
  p      - 按百分比排序
  q      - 退出
  z      - 切换零开销显示
  a      - 切换累积模式
```

#### 2.4.3 输出示例

```
Samples: 23K of event 'cycles', 4000 Hz, Event count (approx.): 5789456123 lost: 0/0 drop: 0/0
Overhead  Shared Object                      Symbol
   8.45%  [kernel]                           [k] _raw_spin_unlock_irqrestore
   4.23%  libcrypto.so.3                     [.] aesni_ctr32_encrypt_blocks
   3.87%  [kernel]                           [k] copy_user_enhanced_fast_string
   3.21%  libc-2.31.so                       [.] __memcmp_avx2_movbe
   2.98%  [kernel]                           [k] fget_light
   2.76%  chrome                             [.] v8::internal::Scavenger::Process
   2.54%  [kernel]                           [k] _raw_spin_lock
   2.43%  [kernel]                           [k] syscall_exit_to_user_mode
   2.12%  libglib-2.0.so.0                   [.] g_hash_table_lookup
   1.98%  [kernel]                           [k] __fget_light
   1.87%  chrome                             [.] SkCanvas::drawRect
   ...
```

---

## 3. Perf高级功能

### 3.1 perf annotate - 代码级分析

`perf annotate`将性能数据与源代码/汇编代码对应，帮助理解热点代码。

#### 3.1.1 基本用法

```bash
### 基本注释显示
perf annotate

### 显示特定符号
perf annotate --symbol=main

### 显示源代码 (需要调试符号)
perf annotate --stdio --show-total-period

### 生成可浏览的HTML报告
perf annotate --stdio > annotated.txt

### 显示汇编和源代码混合
perf annotate --show-total-period --line

### 使用特定源代码搜索路径
perf annotate --prefix=~/source/myproj/src

### 显示特定DSO
perf annotate --dsos=./my_program
```

#### 3.1.2 输出示例

```
$ perf annotate --stdio --show-total-period --symbol=multiply_kernel

Percent│      Source code & Disassembly of matrix_multiply for cycles (599 events)
       │
       │      Disassembly of section .text:
       │
       │      00000000000012a0 <multiply_kernel>:
       │      multiply_kernel():
  2.50 │        push   %rbp
  0.00 │        mov    %rsp,%rbp
  0.17 │        push   %r15
  0.17 │        push   %r14
  0.00 │        push   %r13
       │      # 外层循环 i
 35.42 │10:    mov    0x0(%r13,%r14,8),%rax
  8.35 │        mov    (%rax,%r12,8),%rax
       │      # 内层循环 k
 28.17 │        mov    (%rbx,%rdx,8),%rcx
 12.54 │        mulsd  (%rcx,%r12,8),%xmm0
  4.32 │        add    $0x1,%r12
  5.23 │        cmp    %r15,%r12
       │      # 循环条件判断
  3.15 │        jne    10
       │
       │      # 结果存储
  0.00 │        movsd  %xmm0,(%r8,%r9,8)
```

### 3.2 perf script - 原始数据处理

`perf script`读取`perf.data`并输出原始事件数据，用于自定义分析。

#### 3.2.1 基本用法

```bash
### 输出所有原始事件
perf script

### 仅输出特定符号的事件
perf script --symbol=main

### 按时间排序输出
perf script -D

### 生成Chromium trace格式
perf script -g chromium

### 生成火焰图脚本格式
perf script | ./stackcollapse-perf.pl > out.perf-folded

### 显示时间戳
perf script -f time,comm,pid,symbol

### 自定义输出字段
perf script -F time,pid,comm,sym,dso,srcline

### 输出为JSON
perf script -j

### 指定脚本处理
perf script -s ./my-analysis-script.py
```

#### 3.2.2 脚本处理示例

```python
#!/usr/bin/env python3
### my-analysis-script.py - Perf脚本处理示例

import sys

def process_event(param_dict):
    """处理每个采样事件"""
    timestamp = param_dict.get('t', 'N/A')
    comm = param_dict.get('comm', 'N/A')
    pid = param_dict.get('pid', 'N/A')
    symbol = param_dict.get('symbol', 'N/A')
    dso = param_dict.get('dso', 'N/A')

    # 自定义分析逻辑
    if 'matrix' in symbol.lower():
        print(f"[MATRIX] {timestamp}: {comm}({pid}) in {symbol} @ {dso}")
    elif 'cache' in symbol.lower():
        print(f"[CACHE] {timestamp}: {comm}({pid}) in {symbol}")

def main():
    """主处理函数"""
    print("开始处理perf数据...")

    # perf script会自动传递事件数据
    # 这里只是示例框架
    event_count = 0

    for line in sys.stdin:
        # 解析perf script输出
        # 格式: command pid [cpu] timestamp: event: details
        event_count += 1

        # 简化的处理逻辑
        if event_count % 1000 == 0:
            print(f"已处理 {event_count} 个事件", file=sys.stderr)

    print(f"处理完成，共 {event_count} 个事件")

if __name__ == '__main__':
    main()
```

### 3.3 perf trace - 系统调用跟踪

`perf trace`类似于`strace`，但基于perf事件机制，开销更低。

#### 3.3.1 基本用法

```bash
### 跟踪命令的系统调用
sudo perf trace ./my_program

### 跟踪特定PID
sudo perf trace -p 1234

### 仅显示特定系统调用
sudo perf trace -e openat,read,write ./my_program

### 显示统计摘要
sudo perf trace --summary ./my_program

### 跟踪所有线程
sudo perf trace -a

### 显示时间戳
sudo perf trace -T ./my_program

### 显示持续时间
sudo perf trace --duration 10  # 只显示超过10ms的调用
```

#### 3.3.2 输出示例

```
$ sudo perf trace ./file_io_test

     0.000 ( 0.004 ms): file_io_test/12345 openat(dfd: CWD, filename: "test.dat", flags: RDWR|CREAT, mode: IRUGO|IWUSR) = 3
     0.012 ( 0.002 ms): file_io_test/12345 fstat(fd: 3, statbuf: 0x7ffd1234abcd) = 0
     0.018 (12.456 ms): file_io_test/12345 write(fd: 3, buf: 0x7f1234abcd0000, count: 104857600) = 104857600
    12.498 ( 0.003 ms): file_io_test/12345 fsync(fd: 3) = 0
    12.512 ( 0.002 ms): file_io_test/12345 lseek(fd: 3, whence: SET) = 0
    12.521 ( 8.234 ms): file_io_test/12345 read(fd: 3, buf: 0x7f1234abcd0000, count: 104857600) = 104857600
    20.765 ( 0.002 ms): file_io_test/12345 close(fd: 3) = 0
    20.773 ( 0.001 ms): file_io_test/12345 exit_group()

Summary of events:

file_io_test (12345), 7 events, 100.0%

   syscall            calls    total       min       avg       max      stddev
                               (msec)    (msec)    (msec)    (msec)        (%)
   --------------- -------- --------- --------- --------- ---------     ------
   write                 1   12.4560   12.4560   12.4560   12.4560      0.00%
   read                  1    8.2340    8.2340    8.2340    8.2340      0.00%
   fsync                 1    0.0030    0.0030    0.0030    0.0030      0.00%
   openat                1    0.0040    0.0040    0.0040    0.0040      0.00%
   fstat                 1    0.0020    0.0020    0.0020    0.0020      0.00%
   lseek                 1    0.0020    0.0020    0.0020    0.0020      0.00%
   close                 1    0.0020    0.0020    0.0020    0.0020      0.00%
```

### 3.4 perf probe - 动态探针

`perf probe`用于在运行时动态插入探针点。

#### 3.4.1 基本用法

```bash
### 列出可探测的函数
sudo perf probe --funcs -x ./my_program

### 在函数入口添加探针
sudo perf probe --add='my_function'

### 在特定行添加探针
sudo perf probe --add='my_file.c:42'

### 在特定偏移添加探针
sudo perf probe --add='my_function+10'

### 带参数捕获的探针
sudo perf probe --add='my_function arg1=%di arg2=%si'

### 列出现有探针
sudo perf probe --list

### 删除探针
sudo perf probe --del='probe:my_function'

### 使用探针采样
sudo perf record -e probe:my_function ./my_program
```

#### 3.4.2 探针使用示例

```bash
### 1. 查看可调用的函数
$ sudo perf probe --funcs -x ./test_program
main
calculate_sum
process_data
helper_function

### 2. 在calculate_sum添加探针
$ sudo perf probe --add='calculate_sum'
Added new event:
  probe_test:calculate_sum (on calculate_sum in /home/user/test_program)

### 3. 使用探针记录
$ sudo perf record -e probe_test:calculate_sum -aR sleep 10
[ perf record: Woken up 1 times to write data ]
[ perf record: Captured and wrote 0.152 MB perf.data ]

### 4. 查看探针触发情况
$ perf script
            test_program 12345 [000] 1234567.890123: probe_test:calculate_sum: (4005a0)
            test_program 12345 [000] 1234567.890234: probe_test:calculate_sum: (4005a0)

### 5. 清理探针
$ sudo perf probe --del='probe_test:calculate_sum'
```

---

## 4. 事件选择与分析

### 4.1 常用硬件事件

```bash
### 1. CPU周期与指令
perf stat -e cycles,instructions ./program

### 2. 缓存事件
perf stat -e L1-dcache-loads,L1-dcache-load-misses ./program
perf stat -e L1-dcache-stores,L1-dcache-store-misses ./program
perf stat -e LLC-loads,LLC-load-misses ./program

### 3. TLB事件
perf stat -e dTLB-loads,dTLB-load-misses ./program
perf stat -e iTLB-loads,iTLB-load-misses ./program

### 4. 分支事件
perf stat -e branches,branch-misses ./program

### 5. 总线与内存
perf stat -e bus-cycles,cache-references,cache-misses ./program
```

### 4.2 Intel特定事件

```bash
### Intel Top-Down分析事件
perf stat -e cycles,instructions \
          -e uops_issued.any,uops_retired.retire_slots \
          -e int_misc.recovery_cycles \
          -e br_misp_retired.all_branches \
          ./program

### Intel L1/L2/L3缓存
perf stat -e L1-dcache-loads,L1-dcache-load-misses \
          -e l2_rqsts.all_demand_data_rd,l2_rqsts.demand_data_rd_miss \
          -e LLC-loads,LLC-load-misses \
          ./program

### Intel前端/后端事件
perf stat -e icache_16b.ifdata_stall \
          -e idq_uops_not_delivered.core \
          -e resource_stalls.any \
          ./program
```

### 4.3 事件选择策略

```bash
## 策略1: 基础性能指标
perf stat -e cycles,instructions,cache-misses,branch-misses ./program

### 策略2: 缓存深度分析
perf stat -e L1-dcache-load-misses,L2_rqsts.all_demand_data_rd,LLC-load-misses ./program

### 策略3: 内存带宽分析
perf stat -e uncore_imc/clockticks/,uncore_imc/cas_count_read/ ./program

### 策略4: 前端分析 (Intel)
perf stat -e idq_uops_not_delivered.core,icache_16b.ifdata_stall ./program

### 策略5: 后端分析 (Intel)
perf stat -e resource_stalls.any,cycle_activity.stalls_l2_miss ./program
```

### 4.4 事件列表查询

```bash
### 列出所有可用事件
perf list

### 列出硬件事件
perf list hardware

### 列出软件事件
perf list software

### 列出缓存事件
perf list cache

### 列出跟踪点
perf list tracepoint

### 搜索特定事件
perf list | grep cache
perf list | grep sched

### 列出PMU事件 (特定于CPU)
perf list pmu

### 验证事件是否可用
perf stat -e cycles:u,event-that-does-not-exist true 2>&1
```

---

## 5. Flame Graph火焰图生成

### 5.1 火焰图简介

火焰图（Flame Graph）是Brendan Gregg开发的可视化工具，用于直观展示CPU时间消耗在哪些调用栈上。

```
┌─────────────────────────────────────────────────────────────────┐
│                    火焰图结构说明                               │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  Y轴: 调用栈深度                                                │
│       ▲                                                         │
│       │    ┌─────────────┐                                      │
│     3 │    │  function3  │ ← 栈顶 (叶函数)                     │
│       │    └─────────────┘                                      │
│     2 │  ┌─────────────────────┐                                │
│       │  │     function2       │                                │
│       │  └─────────────────────┘                                │
│     1 │┌───────────────────────────────┐                        │
│       ││         function1             │ ← 栈底 (根函数)       │
│       │└───────────────────────────────┘                        │
│       └────────────────────────────────────────────────────►    │
│                          X轴: 样本数量 (宽度 = 占用时间)        │
│                                                                 │
│  颜色: 随机分配，仅用于区分不同函数                             │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 火焰图生成步骤

```bash
### 步骤1: 克隆FlameGraph工具
 git clone https://github.com/brendangregg/FlameGraph.git
cd FlameGraph

### 步骤2: 使用perf记录数据
sudo perf record -F 99 -a -g -- sleep 30

### 步骤3: 生成折叠格式
perf script | ./stackcollapse-perf.pl > out.perf-folded

### 步骤4: 生成SVG火焰图
./flamegraph.pl out.perf-folded > kernel.svg

### 或者直接使用管道
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > kernel.svg

### 步骤5: 在浏览器中查看
firefox kernel.svg
### 或
google-chrome kernel.svg
```

### 5.3 针对特定程序的火焰图

```bash
### 用户程序火焰图
perf record -F 99 -g -- ./my_program
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > myapp.svg

### 包含内核的完整火焰图
perf record -F 99 -a -g -- ./my_program
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > full.svg

### 仅用户空间
perf record -F 99 --user-space-only -g -- ./my_program
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > userspace.svg

### 仅内核空间
sudo perf record -F 99 -a -g -- sleep 30
perf script --kallsyms=/proc/kallsyms | ./stackcollapse-perf.pl | ./flamegraph.pl > kernel.svg
```

### 5.4 火焰图变体

```bash
### 差分火焰图 (比较两个版本)
perf record -F 99 -g -- ./program_v1
perf script | ./stackcollapse-perf.pl > v1.folded

perf record -F 99 -g -- ./program_v2
perf script | ./stackcollapse-perf.pl > v2.folded

./difffolded.pl v1.folded v2.folded | ./flamegraph.pl > diff.svg

### 冷热火焰图
./stackcollapse-perf.pl < perf.data | \
  awk '{ print $0, ($1 > 100 ? "hot" : "cold") }' | \
  ./flamegraph.pl --color=hot > hotcold.svg

### 搜索高亮
./flamegraph.pl --search="main" out.perf-folded > highlighted.svg
```

### 5.5 生成火焰图的C程序示例

```c
/* flame_test.c - 用于生成火焰图的测试程序 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

/* 模拟不同层级的CPU消耗 */

void heavy_computation(int n) {
    volatile double result = 0;
    for (int i = 0; i < n * 1000; i++) {
        result += sin(i) * cos(i);
    }
    (void)result;
}

void medium_computation(int n) {
    volatile double result = 0;
    for (int i = 0; i < n * 500; i++) {
        result += log(i + 1) * sqrt(i + 1);
    }
    heavy_computation(n / 2);
    (void)result;
}

void light_computation(int n) {
    volatile int sum = 0;
    for (int i = 0; i < n * 100; i++) {
        sum += i;
    }
    medium_computation(n / 2);
    (void)sum;
}

/* 数据处理函数 */
void process_data_a(int *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = data[i] * 2 + 1;
    }
    heavy_computation(100);
}

void process_data_b(int *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = data[i] / 2 - 1;
    }
    medium_computation(100);
}

void process_data_c(int *data, int n) {
    for (int i = 0; i < n; i++) {
        data[i] = (data[i] << 2) ^ 0xFF;
    }
    light_computation(100);
}

/* 主处理流程 */
void data_processing_pipeline(int *data, int n, int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
        process_data_a(data, n);
        process_data_b(data, n);
        process_data_c(data, n);
    }
}

/* 内存操作函数 */
void memory_heavy_operation(int size) {
    int *buffer = malloc(size * sizeof(int));
    if (!buffer) return;

    /* 写入操作 */
    for (int i = 0; i < size; i++) {
        buffer[i] = i;
    }

    /* 读取操作 */
    volatile int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += buffer[i];
    }

    free(buffer);
    (void)sum;
}

void memory_light_operation(int size) {
    int *buffer = malloc(size * sizeof(int) / 10);
    if (!buffer) return;

    for (int i = 0; i < size / 10; i++) {
        buffer[i] = i;
    }

    free(buffer);
}

/* I/O操作模拟 */
void io_operation_simulation(int n) {
    FILE *fp = tmpfile();
    if (!fp) return;

    char buffer[4096];
    memset(buffer, 'A', sizeof(buffer));

    for (int i = 0; i < n; i++) {
        fwrite(buffer, 1, sizeof(buffer), fp);
    }

    fflush(fp);
    rewind(fp);

    for (int i = 0; i < n; i++) {
        fread(buffer, 1, sizeof(buffer), fp);
    }

    fclose(fp);
}

/* 混合工作负载 */
void mixed_workload(int iterations) {
    const int DATA_SIZE = 100000;
    int *data = malloc(DATA_SIZE * sizeof(int));

    for (int i = 0; i < iterations; i++) {
        /* 初始化数据 */
        for (int j = 0; j < DATA_SIZE; j++) {
            data[j] = rand();
        }

        /* CPU密集处理 */
        data_processing_pipeline(data, DATA_SIZE, 10);

        /* 内存操作 */
        memory_heavy_operation(100000);
        memory_light_operation(100000);

        /* I/O操作 */
        if (i % 10 == 0) {
            io_operation_simulation(100);
        }
    }

    free(data);
}

int main(int argc, char *argv[]) {
    printf("火焰图测试程序启动...\n");
    printf("请使用: perf record -F 99 -g -- ./flame_test\n");
    printf("然后: perf script | stackcollapse-perf.pl | flamegraph.pl > flame_test.svg\n\n");

    int iterations = 100;
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }

    printf("执行 %d 次迭代...\n", iterations);

    mixed_workload(iterations);

    printf("完成!\n");
    return 0;
}
```

### 5.6 编译和生成火焰图

```bash
### 编译测试程序
gcc -O2 -g -o flame_test flame_test.c -lm

### 生成火焰图
perf record -F 99 -g -- ./flame_test 50
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > flame_test.svg

### 分析热点
echo "火焰图已生成: flame_test.svg"
echo "在浏览器中打开查看热点函数"
```

---

## 6. 实际案例分析

### 6.1 案例一：矩阵乘法优化分析

#### 6.1.1 朴素实现分析

```c
/* matrix_naive.c - 朴素矩阵乘法 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024

void matmul_naive(double *a, double *b, double *c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }
}

int main() {
    double *a = aligned_alloc(64, N * N * sizeof(double));
    double *b = aligned_alloc(64, N * N * sizeof(double));
    double *c = aligned_alloc(64, N * N * sizeof(double));

    /* 初始化 */
    for (int i = 0; i < N * N; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0;
    }

    printf("Running naive matrix multiplication...\n");
    matmul_naive(a, b, c, N);
    printf("Done\n");

    free(a); free(b); free(c);
    return 0;
}
```

```bash
### 编译
gcc -O2 -o matrix_naive matrix_naive.c

### 性能统计
perf stat -e cycles,instructions,cache-references,cache-misses,L1-dcache-load-misses,LLC-load-misses ./matrix_naive

### 输出:
 Performance counter stats for './matrix_naive':

    23,456,789,123      cycles
    34,567,890,123      instructions              #    1.47  insn per cycle
     2,134,567,890      cache-references
     1,678,901,234      cache-misses              #   78.65% of all cache refs
     1,245,678,901      L1-dcache-load-misses
       892,345,678      LLC-load-misses

       7.823456789 seconds time elapsed

       7.800000000 seconds user
       0.020000000 seconds sys

### 分析: 缓存未命中率78.65% - 严重问题！
### LLC未命中接近9亿次
```

#### 6.1.2 使用perf record定位热点

```bash
### 记录热点
perf record -g ./matrix_naive

### 生成报告
perf report --stdio

### 输出:
### Samples: 8.5K of event 'cycles', Event count (approx.): 23456789123
### Overhead  Command       Shared Object        Symbol
### ........  .......  .....................  ......................
#
###    92.34%  matrix_na  matrix_naive         [.] matmul_naive
###             |
###             ---matmul_naive
###                main
#
###     4.56%  matrix_na  libc-2.31.so         [.] __random
###     2.10%  matrix_na  [kernel.vmlinux]     [k] ___bpf_prog_run
###     1.00%  matrix_na  matrix_naive         [.] main
```

#### 6.1.3 优化版本

```c
/* matrix_optimized.c - 优化的矩阵乘法 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 1024
#define BLOCK 64  /* 缓存块大小 */

/* 分块矩阵乘法 */
void matmul_blocked(double *a, double *b, double *c, int n, int block) {
    for (int ii = 0; ii < n; ii += block) {
        for (int jj = 0; jj < n; jj += block) {
            for (int kk = 0; kk < n; kk += block) {
                /* 块内计算 */
                for (int i = ii; i < ii + block && i < n; i++) {
                    for (int j = jj; j < jj + block && j < n; j++) {
                        double sum = c[i * n + j];
                        for (int k = kk; k < kk + block && k < n; k++) {
                            sum += a[i * n + k] * b[k * n + j];
                        }
                        c[i * n + j] = sum;
                    }
                }
            }
        }
    }
}

/* 转置优化版本 */
void matmul_transposed(double *a, double *b_transposed, double *c, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                /* 访问b_transposed[j][k]代替b[k][j]，提高空间局部性 */
                sum += a[i * n + k] * b_transposed[j * n + k];
            }
            c[i * n + j] = sum;
        }
    }
}

int main() {
    double *a = aligned_alloc(64, N * N * sizeof(double));
    double *b = aligned_alloc(64, N * N * sizeof(double));
    double *b_transposed = aligned_alloc(64, N * N * sizeof(double));
    double *c = aligned_alloc(64, N * N * sizeof(double));

    /* 初始化 */
    for (int i = 0; i < N * N; i++) {
        a[i] = (double)rand() / RAND_MAX;
        b[i] = (double)rand() / RAND_MAX;
        c[i] = 0;
    }

    /* 转置b矩阵 */
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            b_transposed[j * N + i] = b[i * N + j];
        }
    }

    printf("Running blocked matrix multiplication...\n");
    matmul_blocked(a, b, c, N, BLOCK);
    printf("Done\n");

    free(a); free(b); free(b_transposed); free(c);
    return 0;
}
```

```bash
### 编译
gcc -O2 -o matrix_optimized matrix_optimized.c

### 性能统计
perf stat -e cycles,instructions,cache-references,cache-misses,L1-dcache-load-misses,LLC-load-misses ./matrix_optimized

### 输出:
 Performance counter stats for './matrix_optimized':

     2,345,678,901      cycles
     8,234,567,890      instructions              #    3.51  insn per cycle
       134,567,890      cache-references
        23,456,789      cache-misses              #   17.43% of all cache refs
        18,901,234      L1-dcache-load-misses
         8,234,567      LLC-load-misses

       0.782345678 seconds time elapsed

       0.780000000 seconds user
       0.002000000 seconds sys

### 优化效果:
### - 执行时间: 7.82s -> 0.78s (10x加速)
### - IPC: 1.47 -> 3.51 (2.4x提升)
### - Cache未命中率: 78.65% -> 17.43%
### - LLC未命中: 8.92亿 -> 823万 (108x减少)
```

### 6.2 案例二：内存泄漏与分配分析

```bash
### 使用perf trace跟踪内存分配
sudo perf trace -e kmem:kmem_cache_alloc,kmem:kfree -p $(pidof myapp)

### 更详细的内存分析
sudo perf stat -e kmem:kmem_cache_alloc,kmem:kfree,kmem:mm_page_alloc,kmem:mm_page_free ./myapp

### 使用系统tap进行内存分析
perf record -e kmem:kmem_cache_alloc --filter 'bytes > 1024' -a -- sleep 10
```

### 6.3 案例三：多线程锁竞争分析

```c
/* lock_contention.c - 锁竞争测试 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_THREADS 8
#define ITERATIONS 10000000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
volatile long shared_counter = 0;

void *worker(void *arg) {
    for (int i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        shared_counter++;
        pthread_mutex_unlock(&mutex);
    }
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];

    printf("Starting lock contention test with %d threads...\n", NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, worker, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Final counter: %ld (expected: %d)\n",
           shared_counter, NUM_THREADS * ITERATIONS);

    return 0;
}
```

```bash
### 编译
gcc -O2 -pthread -o lock_contention lock_contention.c

### 分析锁竞争
sudo perf stat -e raw_syscalls:sys_enter_futex,cycles,instructions ./lock_contention

### 使用ftrace分析锁
sudo perf record -e probe:pthread_mutex_lock -a ./lock_contention

### 输出示例:
 Performance counter stats for './lock_contention':

        78,234,567      raw_syscalls:sys_enter_futex  # futex调用次数
    12,456,789,012      cycles
    15,678,901,234      instructions              #    1.26  insn per cycle

       3.456789012 seconds time elapsed

### futex调用7800万次！说明严重的锁竞争
### 优化方向: 使用原子操作或分段锁
```

---

## 7. Perf与C程序集成

### 7.1 程序内性能测量

```c
/* perf_integration.c - 在C程序中集成perf */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <asm/unistd.h>
#include <errno.h>
#include <stdint.h>
#include <inttypes.h>

/* 简单的perf封装库 */

typedef struct {
    int fd;
    const char *name;
    uint64_t start_value;
} perf_counter_t;

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags) {
    return syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
}

int perf_counter_init(perf_counter_t *counter, uint32_t type, uint64_t config,
                      const char *name) {
    struct perf_event_attr attr;

    memset(&attr, 0, sizeof(attr));
    attr.type = type;
    attr.size = sizeof(attr);
    attr.config = config;
    attr.disabled = 1;
    attr.exclude_kernel = 1;
    attr.exclude_hv = 1;
    attr.read_format = PERF_FORMAT_TOTAL_TIME_ENABLED | PERF_FORMAT_TOTAL_TIME_RUNNING;

    counter->fd = perf_event_open(&attr, 0, -1, -1, 0);
    if (counter->fd == -1) {
        fprintf(stderr, "Failed to open perf event %s: %s\n", name, strerror(errno));
        return -1;
    }

    counter->name = name;
    return 0;
}

void perf_counter_start(perf_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_RESET, 0);
    ioctl(counter->fd, PERF_EVENT_IOC_ENABLE, 0);
}

void perf_counter_stop(perf_counter_t *counter) {
    ioctl(counter->fd, PERF_EVENT_IOC_DISABLE, 0);
}

uint64_t perf_counter_read(perf_counter_t *counter) {
    uint64_t values[3];
    if (read(counter->fd, values, sizeof(values)) == sizeof(values)) {
        return values[0];
    }
    return 0;
}

void perf_counter_close(perf_counter_t *counter) {
    close(counter->fd);
}

/* 性能测量宏 */
#define PERF_MEASURE_START(name) \
    perf_counter_t _perf_##name; \
    perf_counter_init(&_perf_##name, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES, #name); \
    perf_counter_start(&_perf_##name); \
    printf("[PERF] Starting measurement: %s\n", #name)

#define PERF_MEASURE_END(name) \
    perf_counter_stop(&_perf_##name); \
    uint64_t _perf_##name##_cycles = perf_counter_read(&_perf_##name); \
    printf("[PERF] %s: %" PRIu64 " cycles\n", #name, _perf_##name##_cycles); \
    perf_counter_close(&_perf_##name)

/* 测试函数 */
void matrix_multiply(int n) {
    double *a = aligned_alloc(64, n * n * sizeof(double));
    double *b = aligned_alloc(64, n * n * sizeof(double));
    double *c = aligned_alloc(64, n * n * sizeof(double));

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            double sum = 0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }

    free(a); free(b); free(c);
}

void memory_intensive(int n) {
    int *buffer = malloc(n * sizeof(int));
    volatile int sum = 0;

    for (int i = 0; i < n; i++) {
        buffer[i] = i;
    }

    for (int i = 0; i < n; i++) {
        sum += buffer[i];
    }

    free(buffer);
    (void)sum;
}

int main() {
    printf("Perf Integration Demo\n");
    printf("=====================\n\n");

    /* 测量矩阵乘法 */
    {
        perf_counter_t cycles, instr, cache_miss;

        perf_counter_init(&cycles, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CPU_CYCLES, "cycles");
        perf_counter_init(&instr, PERF_TYPE_HARDWARE, PERF_COUNT_HW_INSTRUCTIONS, "instructions");
        perf_counter_init(&cache_miss, PERF_TYPE_HARDWARE, PERF_COUNT_HW_CACHE_MISSES, "cache-misses");

        printf("Testing matrix multiplication...\n");

        perf_counter_start(&cycles);
        perf_counter_start(&instr);
        perf_counter_start(&cache_miss);

        matrix_multiply(512);

        perf_counter_stop(&cycles);
        perf_counter_stop(&instr);
        perf_counter_stop(&cache_miss);

        uint64_t c = perf_counter_read(&cycles);
        uint64_t i = perf_counter_read(&instr);
        uint64_t m = perf_counter_read(&cache_miss);

        printf("Results:\n");
        printf("  Cycles:        %" PRIu64 "\n", c);
        printf("  Instructions:  %" PRIu64 "\n", i);
        printf("  Cache misses:  %" PRIu64 "\n", m);
        printf("  CPI:           %.3f\n", (double)c / i);

        perf_counter_close(&cycles);
        perf_counter_close(&instr);
        perf_counter_close(&cache_miss);
    }

    printf("\n");

    /* 使用宏测量 */
    PERF_MEASURE_START(memory_test);
    memory_intensive(10000000);
    PERF_MEASURE_END(memory_test);

    printf("\nDone!\n");
    return 0;
}
```

### 7.2 编译和运行

```bash
### 编译
gcc -O2 -o perf_integration perf_integration.c

### 运行 (可能需要调整paranoid设置)
echo 2 | sudo tee /proc/sys/kernel/perf_event_paranoid
./perf_integration

### 预期输出:
### Perf Integration Demo
### =====================
#
### Testing matrix multiplication...
### Results:
###   Cycles:        2456789012
###   Instructions:  3456789012
###   Cache misses:  123456789
###   CPI:           0.711
#
### [PERF] Starting measurement: memory_test
### [PERF] memory_test: 456789012 cycles
#
### Done!
```

### 7.3 Perf与CMake集成

```cmake
### CMakeLists.txt - 集成perf到构建系统
cmake_minimum_required(VERSION 3.10)
project(PerfDemo)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

### 添加perf分析目标
function(add_perf_target TARGET_NAME EXECUTABLE)
    # 生成perf报告
    add_custom_target(${TARGET_NAME}-perf-stat
        COMMAND perf stat -e cycles,instructions,cache-misses,branch-misses ./${EXECUTABLE}
        DEPENDS ${EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Running perf stat on ${EXECUTABLE}"
    )

    # 生成火焰图
    add_custom_target(${TARGET_NAME}-flamegraph
        COMMAND perf record -F 99 -g -- ./${EXECUTABLE}
        COMMAND perf script | ${CMAKE_SOURCE_DIR}/FlameGraph/stackcollapse-perf.pl | ${CMAKE_SOURCE_DIR}/FlameGraph/flamegraph.pl > ${EXECUTABLE}.svg
        DEPENDS ${EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating flame graph for ${EXECUTABLE}"
    )

    # 详细分析
    add_custom_target(${TARGET_NAME}-perf-report
        COMMAND perf record -g -- ./${EXECUTABLE}
        COMMAND perf report --stdio > ${EXECUTABLE}-report.txt
        DEPENDS ${EXECUTABLE}
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Generating perf report for ${EXECUTABLE}"
    )
endfunction()

### 示例可执行文件
add_executable(matrix_test matrix_test.c)
target_compile_options(matrix_test PRIVATE -O2 -g)

### 添加perf目标
add_perf_target(matrix matrix_test)

### 全部perf分析
add_custom_target(perf-all
    DEPENDS matrix-perf-stat matrix-flamegraph matrix-perf-report
    COMMENT "Running all perf analyses"
)
```

---

## 8. 总结与最佳实践

### 8.1 Perf使用流程

```
┌─────────────────────────────────────────────────────────────────┐
│                    Perf性能分析标准流程                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  1. 基础性能评估                                                 │
│     └─> perf stat -e cycles,instructions,cache-misses ./prog    │
│                                                                 │
│  2. 识别热点函数                                                 │
│     └─> perf record -g ./prog                                   │
│     └─> perf report --stdio                                     │
│                                                                 │
│  3. 代码级分析                                                   │
│     └─> perf annotate --symbol=hot_function                     │
│                                                                 │
│  4. 可视化分析                                                   │
│     └─> perf script | stackcollapse-perf.pl | flamegraph.pl     │
│                                                                 │
│  5. 针对性优化                                                   │
│     └─> 根据热点进行算法/缓存/分支优化                          │
│                                                                 │
│  6. 验证优化效果                                                 │
│     └─> 重复步骤1-4对比优化前后                                 │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 8.2 常见问题与解决方案

| 问题 | 原因 | 解决方案 |
|------|------|----------|
| `Permission denied` | paranoid设置 | `echo -1 | sudo tee /proc/sys/kernel/perf_event_paranoid` |
| `No symbols` | 缺少调试信息 | 使用`-g`编译选项 |
| `Sample rate too high` | 采样频率过高 | 降低`-F`值或增加系统限制 |
| `Lost samples` | 缓冲区溢出 | 增加`--mmap-pages`或使用`--aio` |
| 调用栈不完整 | 帧指针优化 | 使用`-fno-omit-frame-pointer`编译 |

### 8.3 推荐学习资源

- Brendan Gregg's Linux Performance: <http://www.brendangregg.com/linuxperf.html>
- Linux perf Documentation: <https://perf.wiki.kernel.org/>
- Intel VTune Profiler Documentation
- ARM Performance Monitoring Unit Guide


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
