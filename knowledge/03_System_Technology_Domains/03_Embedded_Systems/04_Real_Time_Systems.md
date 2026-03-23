# 实时系统设计与实现

> **层级定位**: 03 System Technology Domains / 03 Embedded Systems
> **参考标准**: ISO 26262, DO-178C, IEC 61508
> **难度级别**: L5 专家
> **预估学习时间**: 15-20 小时

---

## 目录

- [实时系统设计与实现](#实时系统设计与实现)
  - [目录](#目录)
  - [1. 实时系统概述](#1-实时系统概述)
    - [1.1 实时性分类](#11-实时性分类)
    - [1.2 关键概念](#12-关键概念)
  - [2. 实时调度算法](#2-实时调度算法)
    - [2.1 速率单调调度 (RMS)](#21-速率单调调度-rms)
    - [2.2 最早截止时间优先 (EDF)](#22-最早截止时间优先-edf)
    - [2.3 最少松弛时间优先 (LLF)](#23-最少松弛时间优先-llf)
  - [3. Linux实时扩展](#3-linux实时扩展)
    - [3.1 PREEMPT\_RT补丁](#31-preempt_rt补丁)
    - [3.2 Linux实时编程](#32-linux实时编程)
  - [4. WCET分析](#4-wcet分析)
    - [4.1 静态WCET分析](#41-静态wcet分析)
    - [4.2 测量WCET](#42-测量wcet)
  - [5. 安全关键系统标准](#5-安全关键系统标准)
    - [5.1 DO-178C (航空)](#51-do-178c-航空)
    - [5.2 ISO 26262 (汽车)](#52-iso-26262-汽车)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 实时系统概述

### 1.1 实时性分类

```
实时系统 (Real-Time Systems):

┌─────────────────────────────────────────────────────────────┐
│  硬实时 (Hard Real-Time)                                    │
│  • 截止时间必须满足                                          │
│  • 错过截止时间是系统失败                                    │
│  • 应用：汽车安全气囊、飞机控制系统、医疗设备                │
│  • 最坏情况执行时间(WCET)分析必须                            │
├─────────────────────────────────────────────────────────────┤
│  固实时 (Firm Real-Time)                                    │
│  • 偶尔错过截止时间可接受                                    │
│  • 但结果价值随延迟迅速下降                                  │
│  • 应用：多媒体流、在线游戏                                  │
├─────────────────────────────────────────────────────────────┤
│  软实时 (Soft Real-Time)                                    │
│  • 平均性能重要                                              │
│  • 偶尔错过截止时间可接受                                    │
│  • 应用：视频会议、网络路由                                  │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 关键概念

```
任务参数：
┌──────────────┬──────────────────────────────────────────────┐
│  参数         │  说明                                        │
├──────────────┼──────────────────────────────────────────────┤
│  到达时间(A)  │  任务就绪时刻                                │
│  截止时间(D)  │  必须完成的时刻                              │
│  执行时间(C)  │  最坏情况执行时间(WCET)                      │
│  周期(T)      │  周期性任务的周期                            │
│  优先级(P)    │  调度优先级                                  │
│  利用率(U)    │  U = C/T                                     │
└──────────────┴──────────────────────────────────────────────┘

实时调度指标：
• 可调度性：所有任务是否能在截止时间内完成
• 响应时间：从就绪到完成的延迟
• 抖动：执行时间的变异
```

---

## 2. 实时调度算法

### 2.1 速率单调调度 (RMS)

```c
/*
 * RMS (Rate Monotonic Scheduling)
 * • 静态优先级
 * • 周期越短，优先级越高
 * • 最优的固定优先级算法
 */

// RMS可调度性测试（充分条件）
// 对于n个任务，如果总利用率 U <= n*(2^(1/n) - 1)，则可调度
// n=2: U <= 0.828
// n=3: U <= 0.779
// n->inf: U <= ln(2) ≈ 0.693

// 精确响应时间分析
// R_i = C_i + Σ[j∈hp(i)] ceil(R_i / T_j) * C_j
// 其中hp(i)是优先级高于i的任务集合

typedef struct {
    int id;
    int period;      // T (ms)
    int wcet;        // C (ms)
    int deadline;    // D (ms)，通常D=T
    int priority;    // RMS: 越小优先级越高
} rms_task_t;

// RMS优先级分配
void rms_assign_priorities(rms_task_t *tasks, int n) {
    // 按周期排序，周期小的优先级高
    for (int i = 0; i < n; i++) {
        tasks[i].priority = i;  // 已按周期排序
    }
}

// 响应时间分析
int rta_response_time(rms_task_t *tasks, int n, int task_idx) {
    int R = tasks[task_idx].wcet;
    int prev_R;

    do {
        prev_R = R;
        int interference = 0;

        for (int i = 0; i < n; i++) {
            if (tasks[i].priority < tasks[task_idx].priority) {
                // 高优先级任务干扰
                interference += ((R + tasks[i].period - 1) / tasks[i].period)
                               * tasks[i].wcet;
            }
        }

        R = tasks[task_idx].wcet + interference;

        if (R > tasks[task_idx].deadline) {
            return -1;  // 不可调度
        }
    } while (R != prev_R);

    return R;
}
```

### 2.2 最早截止时间优先 (EDF)

```c
/*
 * EDF (Earliest Deadline First)
 * • 动态优先级
 * • 截止时间最早的优先执行
 * • 最优的动态优先级算法
 * • 可调度条件：Σ(Ci/Ti) <= 1
 */

typedef struct {
    int id;
    int release_time;
    int absolute_deadline;
    int remaining_time;
    int period;
    int wcet;
} edf_task_t;

// EDF调度器（模拟）
void edf_schedule(edf_task_t *tasks, int n, int simulation_time) {
    // 使用最小堆按绝对截止时间排序
    for (int t = 0; t < simulation_time; t++) {
        // 1. 释放新任务实例
        for (int i = 0; i < n; i++) {
            if (t % tasks[i].period == 0) {
                // 释放新实例
                tasks[i].release_time = t;
                tasks[i].absolute_deadline = t + tasks[i].period;
                tasks[i].remaining_time = tasks[i].wcet;
            }
        }

        // 2. 选择截止时间最早的就绪任务
        int selected = -1;
        int earliest_deadline = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (tasks[i].remaining_time > 0 &&
                tasks[i].absolute_deadline < earliest_deadline) {
                earliest_deadline = tasks[i].absolute_deadline;
                selected = i;
            }
        }

        // 3. 执行一个时间单位
        if (selected >= 0) {
            tasks[selected].remaining_time--;
            printf("t=%d: Task %d executing\n", t, selected);
        }

        // 4. 检查截止时间错过
        for (int i = 0; i < n; i++) {
            if (tasks[i].remaining_time > 0 && t >= tasks[i].absolute_deadline) {
                printf("DEADLINE MISS: Task %d at t=%d\n", i, t);
            }
        }
    }
}
```

### 2.3 最少松弛时间优先 (LLF)

```c
/*
 * LLF (Least Laxity First)
 * • 松弛时间 = 截止时间 - 当前时间 - 剩余执行时间
 * • 松弛时间最小的优先
 * • 需要频繁重新计算优先级
 */

typedef struct {
    int id;
    int deadline;
    int remaining;
} llf_task_t;

// 计算松弛时间
int laxity(llf_task_t *task, int current_time) {
    return task->deadline - current_time - task->remaining;
}

// LLF调度
void llf_schedule(llf_task_t *tasks, int n, int current_time) {
    int min_laxity = INT_MAX;
    int selected = -1;

    for (int i = 0; i < n; i++) {
        if (tasks[i].remaining > 0) {
            int l = laxity(&tasks[i], current_time);
            if (l < min_laxity) {
                min_laxity = l;
                selected = i;
            }
        }
    }

    // 执行选中的任务
    if (selected >= 0) {
        tasks[selected].remaining--;
    }
}
```

---

## 3. Linux实时扩展

### 3.1 PREEMPT_RT补丁

```
标准Linux vs PREEMPT_RT:

特性                    标准Linux      PREEMPT_RT
─────────────────────────────────────────────────
内核抢占               有限           完全抢占
中断处理               顶半部/底半部   线程化
自旋锁                 自旋           互斥锁（可睡眠）
调度延迟               100μs-10ms     <10μs
定时器精度             ~1ms           ~1μs

PREEMPT_RT关键修改：
1. 线程化中断处理程序
2. 可睡眠自旋锁 (rt_mutex)
3. 高精度定时器 (hrtimers)
4. 优先级继承协议
```

### 3.2 Linux实时编程

```c
#define _GNU_SOURCE
#include <sched.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>

// ========== 实时调度策略设置 ==========

int set_realtime_priority(pthread_t thread, int priority) {
    struct sched_param param;
    param.sched_priority = priority;

    // SCHED_FIFO: 先进先出，无时间片
    // SCHED_RR:   轮转，有时间片
    return pthread_setschedparam(thread, SCHED_FIFO, &param);
}

// ========== 内存锁定 ==========

#include <sys/mman.h>

void lock_memory(void) {
    // 锁定所有当前和未来分配的内存
    // 防止页面交换导致的延迟
    if (mlockall(MCL_CURRENT | MCL_FUTURE) == -1) {
        perror("mlockall failed");
    }
}

// ========== 实时任务模板 ==========

typedef struct {
    struct timespec period;      // 周期
    void (*task_func)(void);     // 任务函数
    int priority;                // 优先级
} rt_task_config_t;

void *rt_task_thread(void *arg) {
    rt_task_config_t *config = (rt_task_config_t *)arg;
    struct timespec next;

    // 设置实时优先级
    set_realtime_priority(pthread_self(), config->priority);

    // 获取当前时间
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (1) {
        // 执行任务
        config->task_func();

        // 计算下一个周期
        next.tv_sec += config->period.tv_sec;
        next.tv_nsec += config->period.tv_nsec;

        // 规范化
        if (next.tv_nsec >= 1000000000) {
            next.tv_sec++;
            next.tv_nsec -= 1000000000;
        }

        // 等待下一个周期
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }

    return NULL;
}

// ========== SCHED_DEADLINE ==========

#include <linux/sched.h>
#include <sys/syscall.h>

struct sched_attr {
    u32 size;
    u32 sched_policy;
    u64 sched_flags;
    s32 sched_nice;
    u32 sched_priority;
    u64 sched_runtime;    // 执行时间预算 (ns)
    u64 sched_deadline;   // 相对截止时间 (ns)
    u64 sched_period;     // 周期 (ns)
};

int sched_setattr(pid_t pid, const struct sched_attr *attr, unsigned int flags) {
    return syscall(__NR_sched_setattr, pid, attr, flags);
}

int setup_deadline_sched(u64 runtime_ns, u64 deadline_ns, u64 period_ns) {
    struct sched_attr attr = {
        .size = sizeof(attr),
        .sched_policy = SCHED_DEADLINE,
        .sched_runtime = runtime_ns,
        .sched_deadline = deadline_ns,
        .sched_period = period_ns,
    };

    return sched_setattr(0, &attr, 0);
}

// 使用示例：每10ms周期，需要1ms执行时间，5ms截止时间
// setup_deadline_sched(1000000, 5000000, 10000000);
```

---

## 4. WCET分析

### 4.1 静态WCET分析

```
WCET (Worst-Case Execution Time)分析技术：

┌─────────────────────────────────────────────────────────────┐
│  1. 基于树的分析                                             │
│     • 控制流图(CFG)遍历                                      │
│     • 每条路径执行时间累加                                   │
│     • 问题：路径爆炸                                         │
├─────────────────────────────────────────────────────────────┤
│  2. 隐式路径枚举 (IPET)                                      │
│     • 整数线性规划(ILP)                                      │
│     • 流事实约束                                             │
│     • 工业标准方法                                           │
├─────────────────────────────────────────────────────────────┤
│  3. 抽象解释                                                 │
│     • 处理器行为抽象                                         │
│     • 缓存、流水线分析                                       │
│     • Astree工具使用此方法                                   │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 测量WCET

```c
#include <stdint.h>
#include <time.h>

// 使用性能计数器测量执行时间

#ifdef __x86_64__
static inline uint64_t rdtsc(void) {
    unsigned int lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif

typedef struct {
    uint64_t min;
    uint64_t max;
    uint64_t sum;
    uint64_t count;
} wcet_stats_t;

void wcet_record(wcet_stats_t *stats, uint64_t elapsed) {
    if (elapsed < stats->min || stats->count == 0)
        stats->min = elapsed;
    if (elapsed > stats->max)
        stats->max = elapsed;
    stats->sum += elapsed;
    stats->count++;
}

// 考虑缓存状态的测量
void measure_task_wcet(void (*task)(void), int iterations) {
    wcet_stats_t stats = {0};

    for (int i = 0; i < iterations; i++) {
        // 刷新缓存（模拟最坏情况）
        // clflush_cache_range(...);

        uint64_t start = rdtsc();
        task();
        uint64_t end = rdtsc();

        wcet_record(&stats, end - start);
    }

    printf("WCET measurement:\n");
    printf("  Min: %lu cycles\n", stats.min);
    printf("  Max: %lu cycles\n", stats.max);
    printf("  Avg: %lu cycles\n", stats.sum / stats.count);

    // WCET估计 = Max + 安全余量
}
```

---

## 5. 安全关键系统标准

### 5.1 DO-178C (航空)

```
DO-178C软件等级：

等级   失效影响                    验证要求
─────────────────────────────────────────────────
A     灾难性（飞机坠毁）           MC/DC覆盖率
B     危险/严重                   决策覆盖率
C     重大                        语句覆盖率
D     轻微                        部分验证
E     无影响                      无验证要求

MC/DC (Modified Condition/Decision Coverage):
• 每个条件的独立影响必须被测试
• 条件组合数量：2^n
• 测试用例优化：n+1个用例
```

### 5.2 ISO 26262 (汽车)

```
ASIL等级 (Automotive Safety Integrity Level):

┌────────┬────────────────┬────────────────┐
│  ASIL  │  硬件指标       │  软件措施      │
├────────┼────────────────┼────────────────┤
│  ASIL D│  最高           │  形式化方法    │
│  ASIL C│  高             │  静态分析      │
│  ASIL B│  中             │  代码审查      │
│  ASIL A│  低             │  测试          │
│  QM    │  质量管理       │  标准实践      │
└────────┴────────────────┴────────────────┘
```

---

## 关联导航

### 前置知识

- [进程调度算法深度](../../16_OS_Kernel/02_Process_Scheduling_Deep.md)
- [Linux内核模块编程](../../16_OS_Kernel/01_Linux_Kernel_Module_Programming.md)

### 后续延伸

- [安全关键系统验证](../10_Security_Engineering/11_Safety_Critical_Verification.md)
- [AUTOSAR架构](../05_Automotive/AUTOSAR_Architecture.md)

### 参考

- Hard Real-Time Computing Systems, Buttazzo
- Real-Time Systems, Jane W. S. Liu
- DO-178C Standard
- ISO 26262 Road vehicles - Functional safety
