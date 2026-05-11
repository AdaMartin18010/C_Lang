# 进程调度算法深度分析

> **层级定位**: 03 System Technology Domains / 16 OS Kernel
> **参考课程**: MIT 6.828, CMU 15-410, Berkeley CS162
> **难度级别**: L5 专家
> **预估学习时间**: 12-15 小时

---

## 目录

- [进程调度算法深度分析](#进程调度算法深度分析)
  - [目录](#目录)
  - [1. 调度基础](#1-调度基础)
    - [1.1 调度目标与度量](#11-调度目标与度量)
    - [1.2 调度分类](#12-调度分类)
  - [2. 经典调度算法](#2-经典调度算法)
    - [2.1 先来先服务 (FCFS)](#21-先来先服务-fcfs)
    - [2.2 最短作业优先 (SJF)](#22-最短作业优先-sjf)
    - [2.3 轮转调度 (Round Robin)](#23-轮转调度-round-robin)
    - [2.4 优先级调度](#24-优先级调度)
  - [3. Linux CFS调度器](#3-linux-cfs调度器)
    - [3.1 CFS设计原理](#31-cfs设计原理)
    - [3.2 CFS关键算法](#32-cfs关键算法)
    - [3.3 CFS唤醒抢占](#33-cfs唤醒抢占)
  - [4. 实时调度](#4-实时调度)
    - [4.1 实时调度策略](#41-实时调度策略)
    - [4.2 EDF调度](#42-edf调度)
    - [4.3 SCHED\_DEADLINE实现](#43-sched_deadline实现)
  - [5. 多核调度](#5-多核调度)
    - [5.1 亲和性与负载均衡](#51-亲和性与负载均衡)
    - [5.2 Linux负载均衡算法](#52-linux负载均衡算法)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 调度基础

### 1.1 调度目标与度量

```
调度器设计目标：
┌─────────────────────────────────────────────────────────────┐
│ 目标冲突                                                    │
├─────────────────────────────────────────────────────────────┤
│ 响应时间(Response Time)  ←────→  吞吐量(Throughput)        │
│   交互式任务                矛盾    批处理任务               │
├─────────────────────────────────────────────────────────────┤
│ 公平性(Fairness)        ←────→  优先级(Priority)           │
│   平等分配                  矛盾    关键任务优先             │
├─────────────────────────────────────────────────────────────┤
│ 可预测性(Predictability) ←────→  利用率(Utilization)        │
│   实时系统                  矛盾    通用系统                 │
└─────────────────────────────────────────────────────────────┘

关键度量指标：
• 周转时间(Turnaround Time) = 完成时间 - 到达时间
• 响应时间(Response Time) = 首次运行时间 - 到达时间
• 等待时间(Waiting Time) = 周转时间 - 执行时间
• 吞吐量(Throughput) = 完成任务数 / 单位时间
• CPU利用率(CPU Utilization) = 忙碌时间 / 总时间
```

### 1.2 调度分类

| 维度 | 类型 | 说明 |
|:-----|:-----|:-----|
| **抢占性** | 抢占式 / 非抢占式 | 能否强制收回CPU |
| **作业长度** | 长作业 / 短作业 | 执行时间预估 |
| **I/O模式** | CPU密集型 / I/O密集型 | 资源使用特征 |
| **实时性** | 硬实时 / 软实时 / 尽力而为 | 截止时间保证 |
| **环境** | 单机 / 多处理器 / 分布式 | 执行环境 |

---

## 2. 经典调度算法

### 2.1 先来先服务 (FCFS)

```
特点：最简单，非抢占式，FIFO队列

示例：
进程   到达时间   执行时间
P1     0          24
P2     0          3
P3     0          3

调度顺序：P1 → P2 → P3
甘特图：
0                    24  27  30
├────────────────────┼───┼───┤
│        P1          │P2 │P3 │
└────────────────────┴───┴───┘

周转时间：P1=24, P2=27, P3=30
平均周转时间：(24+27+30)/3 = 27

护航效应(Convoy Effect)：
短进程等待长进程，类似高速公路慢车阻塞
```

### 2.2 最短作业优先 (SJF)

```
特点：最优平均等待时间，需要预知执行时间

示例（非抢占式）：
进程   到达时间   执行时间
P1     0          7
P2     2          4
P3     4          1
P4     5          4

调度顺序：P1(0-7) → P3(7-8) → P2(8-12) → P4(12-16)

平均等待时间：
P1: 0, P2: 8-2=6, P3: 7-4=3, P4: 12-5=7
平均：(0+6+3+7)/4 = 4

最短剩余时间优先(SRTF) - 抢占式版本：
进程   到达时间   执行时间
P1     0          8
P2     1          4
P3     2          2
P4     3          1

t=0: P1运行
t=1: P2到达(4<P1剩余7)，抢占，P2运行
t=2: P3到达(2<P2剩余3)，抢占，P3运行
t=3: P4到达(1<P3剩余1)，抢占，P4运行
t=4: P4完成，P3剩余1最短，运行
t=5: P3完成，P2剩余3最短，运行
t=8: P2完成，P1运行到结束
```

### 2.3 轮转调度 (Round Robin)

```
特点：时间片轮转，抢占式，公平响应

示例（时间片=4）：
进程   执行时间
P1     24
P2     3
P3     3

调度序列：
P1(0-4) → P2(4-7) → P3(7-10) → P1(10-14) → P1(14-18)...

时间片大小影响：
时间片太大 → 退化为FCFS
时间片太小 → 上下文切换开销大（典型：10-100ms）

上下文切换成本：
• 保存/恢复寄存器
• 切换地址空间（TLB刷新）
• 缓存失效
• 典型开销：1-10μs
```

### 2.4 优先级调度

```
特点：为每个进程分配优先级，高优先级先执行

问题：饥饿(Starvation) - 低优先级进程永远得不到执行

解决方案：老化(Aging)
• 等待时间增加时，逐步提高优先级
• priority = base_priority + age_factor × wait_time

多级队列(MLQ)：
┌─────────────────┐  最高优先级
│  系统进程       │  固定时间片（如8ms）
├─────────────────┤
│  交互式进程     │  前台任务
├─────────────────┤
│  批处理进程     │  后台任务
├─────────────────┤
│  学生进程       │  最低优先级
└─────────────────┘

多级反馈队列(MLFQ)：
• 进程可在队列间移动
• I/O密集型 → 高优先级队列
• CPU密集型 → 低优先级队列
```

---

## 3. Linux CFS调度器

### 3.1 CFS设计原理

```
完全公平调度器(Completely Fair Scheduler)：

核心思想：
• 为每个进程分配"虚拟运行时间"(vruntime)
• 总是选择vruntime最小的进程运行
• 保证所有进程获得公平的CPU时间份额

红黑树数据结构：
          [vruntime=10]
         /            \
   [vruntime=5]   [vruntime=15]
       /    \
  [vruntime=3] [vruntime=8]

左侧节点总是vruntime最小的，O(log n)操作
```

### 3.2 CFS关键算法

```c
// 虚拟运行时间计算
// vruntime += delta_exec * (NICE_0_LOAD / weight)
//
// weight来自优先级权重表：
// nice 0: weight 1024
// nice -5: weight 3121 (3x)
// nice 5: weight 335 (0.3x)

// 调度延迟与最小粒度
#define SCHED_LATENCY 6ms      // 目标调度延迟
#define MIN_GRANULARITY 0.75ms // 最小时间片

// 时间片计算
// time_slice = (weight / sum_weights) * SCHED_LATENCY
// 但至少为MIN_GRANULARITY

// CFS调度实体 (简化)
struct sched_entity {
    struct load_weight  load;       // 权重
    struct rb_node      run_node;   // 红黑树节点
    u64                 vruntime;   // 虚拟运行时间
    u64                 exec_start; // 开始执行时间
    u64                 sum_exec_runtime; // 总运行时间
};

// 更新vruntime
static void update_curr(struct cfs_rq *cfs_rq)
{
    struct sched_entity *curr = cfs_rq->curr;
    u64 now = rq_clock_task(rq_of(cfs_rq));
    u64 delta_exec;

    delta_exec = now - curr->exec_start;
    curr->exec_start = now;

    curr->sum_exec_runtime += delta_exec;
    curr->vruntime += calc_delta_fair(delta_exec, curr);
}
```

### 3.3 CFS唤醒抢占

```c
// 唤醒时检查是否应该抢占当前进程
static void check_preempt_wakeup(struct rq *rq, struct task_struct *p)
{
    struct task_struct *curr = rq->curr;
    struct sched_entity *se = &curr->se;
    struct sched_entity *p_se = &p->se;

    // 如果唤醒进程的vruntime足够小，抢占当前进程
    // wakeup_granularity = 1ms
    s64 vdiff = se->vruntime - p_se->vruntime;

    if (vdiff > wakeup_granularity) {
        // 抢占当前进程
        resched_curr(rq);
    }
}
```

---

## 4. 实时调度

### 4.1 实时调度策略

| 策略 | 类型 | 特点 |
|:-----|:-----|:-----|
| **SCHED_FIFO** | 先进先出 | 无时间片，高优先级抢占，直到阻塞或主动释放 |
| **SCHED_RR** | 轮转 | 同优先级RR调度，有时间片 |
| **SCHED_DEADLINE** | 截止时间 | 基于EDF算法，保证截止时间 |

### 4.2 EDF调度

```
最早截止时间优先(Earliest Deadline First)：

示例：
任务   周期(T)   执行时间(C)   截止时间(D)
T1     20        5            20
T2     50        12           50

CPU利用率：U = 5/20 + 12/50 = 0.25 + 0.24 = 0.49 < 1
可调度！

t=0: T1截止20，T2截止50 → 选T1
     T1执行到t=5完成

t=5: 只有T2，选T2
     T2执行到t=17完成

t=17: 空闲到t=20

t=20: T1新实例截止40，T2截止50 → 选T1
      ...

可调度性判定：
对于隐式截止时间任务（D=T），可调度条件：
∑(C_i / T_i) ≤ 1
```

### 4.3 SCHED_DEADLINE实现

```c
#include <sched.h>
#include <sys/syscall.h>
#include <linux/sched.h>

// 设置DEADLINE调度参数
struct sched_attr {
    u32 size;
    u32 sched_policy;
    u64 sched_flags;
    s32 sched_nice;
    u32 sched_priority;
    u64 sched_runtime;   // 执行时间 (ns)
    u64 sched_deadline;  // 相对截止时间 (ns)
    u64 sched_period;    // 周期 (ns)
};

int set_deadline_sched(u64 runtime, u64 deadline, u64 period)
{
    struct sched_attr attr = {
        .size = sizeof(attr),
        .sched_policy = SCHED_DEADLINE,
        .sched_runtime = runtime,
        .sched_deadline = deadline,
        .sched_period = period,
    };

    return syscall(__NR_sched_setattr, 0, &attr, 0);
}

// 使用示例
// 每100ms周期，需要20ms执行时间，50ms相对截止时间
set_deadline_sched(20*1000000, 50*1000000, 100*1000000);
```

---

## 5. 多核调度

### 5.1 亲和性与负载均衡

```
CPU亲和性：
• 硬亲和性：进程只能在指定CPU上运行
• 软亲和性：倾向于在同一CPU上运行（缓存友好）

负载均衡：
• 推迁移：过载CPU推送任务到空闲CPU
• 拉迁移：空闲CPU从过载CPU拉取任务
• 周期性均衡：定时检查并重新分配

调度域(Scheduling Domain)：
┌─────────────────────────────────────────┐
│           NUMA Node 0                    │
│  ┌─────────────────────────────────┐    │
│  │        物理核心 0-3              │    │
│  │  ┌─────────┐    ┌─────────┐    │    │
│  │  │ L3共享  │    │ L3共享  │    │    │
│  │  │核心0-1  │    │核心2-3  │    │    │
│  │  │L1-L2私有│    │L1-L2私有│    │    │
│  │  └─────────┘    └─────────┘    │    │
│  └─────────────────────────────────┘    │
└─────────────────────────────────────────┘

层级：
1. SMT域（超线程）
2. 核心域（共享L3）
3. 插槽域（NUMA节点）
4. 全局域（所有CPU）
```

### 5.2 Linux负载均衡算法

```c
// 负载计算：runnable_load_avg
// 考虑CPU频率、任务优先级

// 负载均衡触发时机：
// 1. 当前运行队列为空
// 2. 周期性负载均衡（每4-64ms）
// 3. 进程fork/exec唤醒时

// 迁移决策
static int migrate_task(struct task_struct *p, int src_cpu, int dst_cpu)
{
    // 检查亲和性
    if (!cpumask_test_cpu(dst_cpu, &p->cpus_allowed))
        return -EINVAL;

    // 检查缓存热度
    if (task_hot(p, src_cpu))
        return -EAGAIN;

    // 检查负载不平衡度
    long imbalance = weighted_cpuload(src_cpu) - weighted_cpuload(dst_cpu);
    if (imbalance < migration_cost)
        return -EAGAIN;

    return 0;
}
```

---

## 关联导航

### 前置知识

- [Linux内核模块编程](./01_Linux_Kernel_Module_Programming.md)
- [并发编程](../14_Concurrency_Parallelism/README.md)

### 后续延伸

- [虚拟内存子系统](./03_Virtual_Memory_Subsystem.md)
- [实时系统编程](../03_Embedded_Systems/04_Real_Time_Systems.md)

### 参考

- Operating Systems: Three Easy Pieces, Remzi & Andrea Arpaci-Dusseau
- Linux Kernel Development, Robert Love
- Understanding the Linux Kernel, Bovet & Cesati
