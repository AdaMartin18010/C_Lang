# POSIX线程编程 - 并发编程权威参考

> **层级定位**: 03 System Technology Domains / 14 Concurrency_Parallelism
> **对应标准**: POSIX.1-2008 (IEEE Std 1003.1), pthreads, ISO/IEC 9945
> **难度级别**: L3-L5
> **预估学习时间**: 16-20小时
> **最后更新**: 2026-03-16

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 线程模型、同步原语、内存序、并发安全、性能优化 |
| **前置知识** | C基础、内存管理、操作系统原理、计算机体系结构 |
| **后续延伸** | 无锁编程、并行算法、分布式系统、实时系统 |
| **权威来源** | POSIX.1-2008, Butenhof《Programming with POSIX Threads》, C11标准 |

---

## 🧠 第一部分：概念定义（严格形式化）

### 1.1 线程的数学模型

#### 1.1.1 线程作为抽象数据类型

**定义 1.1（线程上下文）**：线程上下文是一个五元组

$$
T = (PC, R, S, F, M)
$$

其中：

- $PC$ = 程序计数器（Program Counter），指向当前执行指令
- $R = \{r_0, r_1, ..., r_{n-1}\}$ = 通用寄存器集合
- $S = (SP, BP, \text{stack})$ = 栈状态（栈指针、基址指针、栈内容）
- $F = \{f_0, f_1, ..., f_{m-1}\}$ = 标志寄存器集合（条件码、浮点状态）
- $M$ = 线程私有内存区域（线程局部存储）

#### 1.1.2 进程与线程的关系

```
进程地址空间 (共享)
┌─────────────────────────────────────┐
│ 代码段 (.text) - 所有线程共享        │
├─────────────────────────────────────┤
│ 全局/静态数据段 (.data, .bss)       │
├─────────────────────────────────────┤
│ 堆 (Heap) - 动态分配，共享访问       │
├─────────────────────────────────────┤
│ 线程1栈 │ 线程2栈 │ ... │ 线程N栈   │
│ (私有)  │ (私有)  │     │ (私有)    │
└─────────────────────────────────────┘
```

**定义 1.2（线程状态空间）**：线程状态机定义为

$$
\mathcal{S} = \{ \text{NEW}, \text{READY}, \text{RUNNING}, \text{BLOCKED}, \text{TERMINATED} \}
$$

状态转换函数 $\delta: \mathcal{S} \times \mathcal{E} \rightarrow \mathcal{S}$，其中 $\mathcal{E}$ 是事件集合。

### 1.2 并发与并行的严格区分

| 维度 | 并发 (Concurrency) | 并行 (Parallelism) |
|:-----|:------------------|:------------------|
| **定义** | 任务在重叠时间段内执行 | 任务同时执行 |
| **核心问题** | 任务调度、资源共享、同步 | 性能扩展、负载均衡、通信 |
| **时间模型** | 交错执行 (interleaving) | 同时执行 (simultaneous) |
| **硬件要求** | 单核即可 | 多核/多处理器 |
| **关注焦点** | 正确性、响应性 | 吞吐量、加速比 |
| **形式化** | $\exists t: T_1(t) \land T_2(t)$ 不同时为真 | $\exists t: T_1(t) \land T_2(t)$ 同时为真 |

```
并发执行（单核时间片轮转）    并行执行（多核同时执行）
CPU: ┌─────────────────┐      CPU0: ┌─────┐     CPU1: ┌─────┐
     │ T1 │ T2 │ T1 │ T2│            │ T1  │            │ T2  │
     └─────────────────┘            └─────┘            └─────┘
时间: ─────────────────→            时间: ─────────────────────────→
```

### 1.3 线程状态的有限状态机

```mermaid
stateDiagram-v2
    [*] --> NEW : pthread_create
    NEW --> READY : 初始化完成
    READY --> RUNNING : 调度器选择
    RUNNING --> READY : 时间片用完/被抢占
    RUNNING --> BLOCKED : 等待资源/条件
    BLOCKED --> READY : 资源可用/条件满足
    RUNNING --> TERMINATED : 执行完成/pthread_exit
    BLOCKED --> TERMINATED : 被取消
    TERMINATED --> [*] : pthread_join/pthread_detach
```

**线程生命周期状态转换表**：

| 当前状态 | 事件/操作 | 下一状态 | 触发条件 |
|:--------:|:----------|:--------:|:---------|
| NEW | 创建完成 | READY | pthread_create返回成功 |
| READY | 被调度 | RUNNING | 调度器分配CPU时间片 |
| RUNNING | 时间片用完 | READY | 时钟中断 |
| RUNNING | 主动让出 | READY | pthread_yield() |
| RUNNING | 等待互斥锁 | BLOCKED | pthread_mutex_lock(被占用) |
| RUNNING | 等待条件变量 | BLOCKED | pthread_cond_wait() |
| RUNNING | I/O操作 | BLOCKED | 系统调用阻塞 |
| BLOCKED | 锁可用 | READY | 其他线程释放锁 |
| BLOCKED | 条件满足 | READY | pthread_cond_signal/broadcast |
| BLOCKED | I/O完成 | READY | 中断处理完成 |
| RUNNING | 执行完成 | TERMINATED | 线程函数返回 |
| RUNNING | 调用exit | TERMINATED | pthread_exit() |
| BLOCKED | 被取消 | TERMINATED | pthread_cancel() |
| READY | 被取消 | TERMINATED | 延迟取消生效 |

### 1.4 竞态条件的形式化定义

**定义 1.3（竞态条件）**：设程序 $P$ 有 $n$ 个线程 $T_1, T_2, ..., T_n$，共享状态空间 $S$。若存在执行轨迹（execution trace）$\sigma_1$ 和 $\sigma_2$，使得：

$$
\text{Exec}(P, \sigma_1) \neq \text{Exec}(P, \sigma_2)
$$

其中差异源于线程执行顺序的不确定性，则称程序 $P$ 存在**竞态条件**。

**定义 1.4（数据竞争）**：对于共享内存位置 $x$，若线程 $T_i$ 和 $T_j$（$i \neq j$）满足：

1. $T_i$ 写入 $x$ 且 $T_j$ 读取 $x$（RAW - Read After Write）
2. $T_i$ 读取 $x$ 且 $T_j$ 写入 $x$（WAR - Write After Read）
3. $T_i$ 写入 $x$ 且 $T_j$ 写入 $x$（WAW - Write After Write）

且两个访问之间没有**happens-before**关系，则存在**数据竞争**。

```c
// 竞态条件示例：非确定性结果
int counter = 0;

void* increment(void* arg) {
    for (int i = 0; i < 100000; i++) {
        counter++;  // 非原子操作: LOAD → ADD → STORE
    }
    return NULL;
}
// 两个线程执行后，counter 值不确定，在 [100000, 200000] 范围内
```

### 1.5 死锁的形式化条件（Coffman条件）

**定理 1.1（Coffman死锁条件）**：系统发生死锁当且仅当同时满足以下四个必要条件：

| 条件 | 形式化描述 | 解释 |
|:-----|:-----------|:-----|
| **互斥** (Mutual Exclusion) | $\forall r \in R: |Allocation(r)| \leq 1$ | 资源非共享，一次只能被一个线程占用 |
| **占有并等待** (Hold and Wait) | $\exists t \in T, r_1, r_2 \in R: Holds(t, r_1) \land Requests(t, r_2)$ | 线程持有资源同时请求新资源 |
| **不可剥夺** (No Preemption) | $\forall t \in T, r \in R: Holds(t, r) \Rightarrow \neg Preemptible(r)$ | 资源只能由持有者显式释放 |
| **循环等待** (Circular Wait) | $\exists \{t_1, t_2, ..., t_n\}: Requests(t_i, r_{i+1}) \land Holds(t_{i+1}, r_{i+1})$ | 存在线程-资源的循环等待链 |

**死锁资源分配图**：

```
        线程T1          线程T2          线程T3
           │              │              │
           ▼              ▼              ▼
        持有R1 ──────── 请求R1        持有R3
           │           ▲   │           │
           │           │   │           ▼
           ▼           │   ▼        请求R1
        请求R2 ────────┘ 持有R2 ────────┘
                         请求R3
                           │
                           ▼
                        形成循环等待

循环等待链: T1 → R2 → T2 → R1 → T1 (死锁！)
```

---

## 📊 第二部分：属性维度矩阵

### 2.1 同步原语对比矩阵

| 特性 | 互斥锁 (Mutex) | 读写锁 (RWLock) | 条件变量 (CondVar) | 信号量 (Semaphore) | 屏障 (Barrier) |
|:-----|:---------------|:----------------|:-------------------|:-------------------|:---------------|
| **核心用途** | 互斥访问临界区 | 读多写少场景 | 等待特定条件 | 资源计数控制 | 多线程同步点 |
| **获取语义** | 独占 | 读：共享<br>写：独占 | 与互斥锁配合使用 | 计数减一 | 计数递减，达到阈值统一释放 |
| **释放语义** | 持有者释放 | 持有者释放 | 信号方释放 | 任意线程释放 | 自动释放 |
| **阻塞特性** | 是（可配置） | 是 | 是 | 是 | 是 |
| **超时支持** | pthread_mutex_timedlock | pthread_rwlock_timedrdlock/wrlock | pthread_cond_timedwait | sem_timedwait | 无标准超时 |
| **递归支持** | 可配置 | 否 | N/A | N/A | N/A |
| **公平性** | 依赖实现 | 通常写者优先 | 依赖实现 | 依赖实现 | 到达即释放 |
| **典型场景** | 保护共享数据 | 缓存、配置表 | 生产者-消费者 | 连接池、限流 | 并行计算分阶段 |
| **性能开销** | 低 | 中（读高写低） | 低 | 中 | 中 |
| **死锁风险** | 中（顺序不当） | 中 | 高（信号丢失） | 中 | 低 |

### 2.2 互斥锁类型对比矩阵

| 类型 | 同线程重复加锁 | 其他线程解锁 | 使用场景 | 性能特点 |
|:-----|:--------------:|:------------:|:---------|:---------|
| **普通锁** (NORMAL) | 死锁 | 未定义行为 | 简单互斥 | 最快 |
| **错误检查锁** (ERRORCHECK) | 返回错误 | 返回错误 | 调试 | 较慢 |
| **递归锁** (RECURSIVE) | 允许，计数增加 | 同线程递减 | 递归算法 | 中等 |
| **自适应锁** (ADAPTIVE_NP) | 死锁 | 未定义行为 | 高竞争场景 | 自适应自旋 |

### 2.3 线程属性配置矩阵

| 属性 | 可选值 | 默认值 | 作用 | 配置函数 |
|:-----|:-------|:-------|:-----|:---------|
| **分离状态** | JOINABLE, DETACHED | JOINABLE | 决定线程终止后资源回收方式 | pthread_attr_setdetachstate |
| **栈大小** | ≥ PTHREAD_STACK_MIN | 系统默认 | 控制线程栈空间 | pthread_attr_setstacksize |
| **栈地址** | 用户分配地址 | NULL | 指定栈位置 | pthread_attr_setstack |
| **调度策略** | SCHED_FIFO, SCHED_RR, SCHED_OTHER | SCHED_OTHER | 实时调度策略 | pthread_attr_setschedpolicy |
| **调度参数** | sched_param.sched_priority | 0 | 优先级设置 | pthread_attr_setschedparam |
| **继承调度** | INHERIT, EXPLICIT | EXPLICIT | 是否继承创建者调度 | pthread_attr_setinheritsched |
| **作用域** | SYSTEM, PROCESS | SYSTEM | 竞争范围 | pthread_attr_setscope |
| **CPU亲和性** | CPU掩码 | 无限制 | 绑定特定CPU核心 | pthread_setaffinity_np |

### 2.4 取消类型对比矩阵

| 取消类型 | 取消状态 | 取消时机 | 清理处理 | 适用场景 |
|:---------|:---------|:---------|:---------|:---------|
| **延迟取消** (DEFERRED) | ENABLE/DISABLE | 在取消点检查 | 在取消点执行 | 一般应用，推荐 |
| **异步取消** (ASYNCHRONOUS) | ENABLE/DISABLE | 立即执行 | 立即执行 | 紧急终止 |

**取消点函数表**：

| 类别 | 函数列表 |
|:-----|:---------|
| I/O操作 | read(), write(), open(), close(), select(), poll(), accept() |
| 线程操作 | pthread_testcancel(), pthread_join(), pthread_cond_wait() |
| 进程控制 | system(), wait(), waitpid() |
| 信号操作 | pause(), sigsuspend(), sigtimedwait() |
| 定时器 | sleep(), nanosleep(), usleep() |
| 消息队列 | mq_receive(), mq_send() |
| 标准I/O | printf(), fprintf(), fgets(), fputs() |

### 2.5 线程安全级别矩阵

| 级别 | 定义 | 可重入性 | 并发安全 | 示例 |
|:-----|:-----|:--------:|:--------:|:-----|
| **线程不安全** | 访问共享状态无保护 | 否 | 否 | strerror(), rand(), strtok() |
| **线程安全** | 内部同步或TLS保护 | 可能 | 是 | strtok_r(), rand_r(), strerror_r() |
| **可重入** | 不依赖共享状态 | 是 | 是 | sin(), strlen(), memcpy() |
| **异步信号安全** | 可被信号处理程序调用 | 是 | 是 | write(), _exit(), sem_post() |
| **原子操作** | 硬件保证不可分割 | 是 | 是 | atomic_fetch_add(), CAS操作 |

### 2.6 pthread函数错误码矩阵

| 函数 | 常见错误码 | 含义 | 处理建议 |
|:-----|:-----------|:-----|:---------|
| **pthread_create** | EAGAIN | 资源不足（线程/内存） | 减少线程数或增加限制 |
| | EINVAL | 无效属性 | 检查attr配置 |
| | EPERM | 权限不足 | 检查调度策略权限 |
| **pthread_join** | EDEADLK | 检测到死锁 | 检查线程依赖关系 |
| | EINVAL | 线程不可join | 已是detached或自身 |
| | ESRCH | 线程不存在 | 检查线程ID有效性 |
| **pthread_mutex_lock** | EINVAL | 互斥锁未初始化 | 调用pthread_mutex_init |
| | EDEADLK | 死锁检测（递归） | 检查加锁顺序 |
| **pthread_cond_wait** | EINVAL | 条件变量/互斥锁无效 | 检查初始化状态 |
| | EPERM | 互斥锁非当前线程持有 | 检查加锁逻辑 |

### 2.7 调度策略对比矩阵

| 策略 | 优先级范围 | 特性 | 抢占行为 | 适用场景 |
|:-----|:-----------|:-----|:---------|:---------|
| **SCHED_OTHER** | 0 | 分时调度，动态优先级 | 时间片用完 | 普通应用 |
| **SCHED_FIFO** | 1-99 (实时) | 先进先出，同优先级不抢占 | 永不 | 硬实时任务 |
| **SCHED_RR** | 1-99 (实时) | 轮转，同优先级时间片轮转 | 时间片用完 | 软实时任务 |

---

## 📝 第三部分：形式化描述

### 3.1 线程创建的抽象数据类型

```
ADT Thread {
    // 类型定义
    type ThreadID = opaque handle
    type ThreadAttr = record {
        detachstate: {JOINABLE, DETACHED}
        stacksize: size_t
        stackaddr: void*
        guardsize: size_t
        schedpolicy: {FIFO, RR, OTHER}
        schedparam: struct sched_param
        inheritsched: {INHERIT, EXPLICIT}
        scope: {SYSTEM, PROCESS}
    }

    // 操作
    Create: (start_routine: void* → void*, arg: void*, attr: ThreadAttr) → (ThreadID, ErrorCode)
    Join: (thread: ThreadID) → (void* return_value, ErrorCode)
    Detach: (thread: ThreadID) → ErrorCode
    Exit: (retval: void*) → ⊥
    Self: () → ThreadID
    Equal: (t1: ThreadID, t2: ThreadID) → bool
    Cancel: (thread: ThreadID) → ErrorCode
    SetCancelState: (state: {ENABLE, DISABLE}) → (oldstate, ErrorCode)
    SetCancelType: (type: {DEFERRED, ASYNCHRONOUS}) → (oldtype, ErrorCode)

    // 不变式
    invariant ∀t: ThreadID ::
        is_valid(t) ⇒
            (state(t) ∈ {NEW, READY, RUNNING, BLOCKED, TERMINATED}) ∧
            (state(t) = TERMINATED ⇒ ¬is_joinable(t) ∨ return_value_defined(t))
}
```

### 3.2 互斥锁的不变式

```
ADT Mutex {
    // 状态
    State = {UNLOCKED, LOCKED_BY(t: ThreadID)}

    // 操作
    Init: (attr: MutexAttr) → (Mutex, ErrorCode)
    Lock: (m: Mutex) → ErrorCode
    TryLock: (m: Mutex) → (bool success, ErrorCode)
    TimedLock: (m: Mutex, timeout: timespec) → (bool success, ErrorCode)
    Unlock: (m: Mutex) → ErrorCode
    Destroy: (m: Mutex) → ErrorCode

    // 前置条件
    pre Lock(m): m.state = UNLOCKED ∨ (m.type = RECURSIVE ∧ m.owner = current_thread)
    pre Unlock(m): m.state = LOCKED_BY(current_thread)
    pre Destroy(m): m.state = UNLOCKED

    // 后置条件
    post Lock(m): m.state = LOCKED_BY(current_thread) ∧ m.lock_count = old(m.lock_count) + 1
    post Unlock(m): m.state = UNLOCKED ∨ (m.type = RECURSIVE ∧ m.lock_count > 0)

    // 不变式
    invariant ∀m: Mutex ::
        (m.state = LOCKED_BY(t) ⇒ m.owner = t) ∧
        (m.type ≠ RECURSIVE ⇒ m.lock_count ≤ 1) ∧
        (m.lock_count > 0 ⇔ m.state ≠ UNLOCKED)
}
```

### 3.3 条件变量的等待-通知协议

```
ADT ConditionVariable {
    // 协议规则
    protocol Wait(cv, mutex) {
        // 必须持有互斥锁
        require mutex.owner = current_thread

        // 原子操作序列
        atomically {
            mutex.Unlock()
            enqueue(current_thread, cv.wait_queue)
            sleep(current_thread)
        }

        // 被唤醒后
        upon_wakeup {
            mutex.Lock()
        }

        // 必须重新检查条件（虚假唤醒）
        ensure predicate_condition  // 循环检查
    }

    protocol Signal(cv) {
        if (cv.wait_queue not empty) {
            dequeue_and_wake_one(cv.wait_queue)
        }
    }

    protocol Broadcast(cv) {
        while (cv.wait_queue not empty) {
            dequeue_and_wake_one(cv.wait_queue)
        }
    }
}
```

**关键规则**：

1. **总是使用while循环**：`while (!condition) pthread_cond_wait(&cv, &mutex);`
2. **修改条件前必须加锁**
3. `pthread_cond_wait` 自动释放并重新获取互斥锁
4. **信号可能丢失**：若无线程等待，信号消失

### 3.4 内存序的形式化（happens-before关系）

**定义 3.1（Happens-Before关系，→hb）**：对于程序中的事件 $e_1$ 和 $e_2$，$e_1 \rightarrow_{hb} e_2$ 当且仅当：

1. **程序序**：在同一线程中，$e_1$ 在程序顺序上先于 $e_2$
2. **锁序**：$e_1$ 是解锁操作，$e_2$ 是后续对同一锁的加锁操作
3. **线程创建**：$e_1$ 是 `pthread_create`，$e_2$ 是新线程的第一个操作
4. **线程合并**：$e_1$ 是被join线程的最后一个操作，$e_2$ 是 `pthread_join` 返回
5. **传递性**：$e_1 \rightarrow_{hb} e_k$ 且 $e_k \rightarrow_{hb} e_2$

```
线程T1                    线程T2
  │                         │
  ▼                         ▼
  a ───────→hb──────────────┐
  │                    pthread_join
  ▼                         │
unlock(M) ──→hb───► lock(M) │
  │                    (同步点)
  ▼                         ▼
  b ◄───────────────────────┘

T1的操作a →hb T2的操作b
```

### 3.5 数据竞争的形式化定义

**定义 3.2（无数据竞争程序）**：程序 $P$ 是无数据竞争的，当且仅当对于任意两个对共享内存位置 $x$ 的访问 $a_1$ 和 $a_2$：

$$
(a_1 \text{ writes } x \lor a_2 \text{ writes } x) \land \neg(a_1 \rightarrow_{hb} a_2) \land \neg(a_2 \rightarrow_{hb} a_1) \Rightarrow \text{false}
$$

即：**任意两个冲突访问（至少一个写）必须存在 happens-before 关系**。

---

## 💻 第四部分：示例矩阵

### 4.1 基本线程创建和终止模式

#### 模式1：基本线程创建

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/*========================================
 * 模式1.1：简单线程创建与等待
 *========================================*/
typedef struct {
    int thread_id;
    int input_value;
} thread_arg_t;

void *simple_worker(void *arg) {
    thread_arg_t *ta = (thread_arg_t *)arg;
    printf("[Thread %d] Started with input=%d\n", ta->thread_id, ta->input_value);

    // 模拟工作
    int result = ta->input_value * ta->input_value;

    printf("[Thread %d] Completed, result=%d\n", ta->thread_id, result);

    // 返回动态分配的结果
    int *retval = malloc(sizeof(int));
    *retval = result;
    pthread_exit(retval);
}

int demo_basic_thread() {
    #define NUM_THREADS 4
    pthread_t threads[NUM_THREADS];
    thread_arg_t args[NUM_THREADS];

    // 创建线程
    for (int i = 0; i < NUM_THREADS; i++) {
        args[i].thread_id = i;
        args[i].input_value = i + 1;

        int rc = pthread_create(&threads[i], NULL, simple_worker, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %d\n", rc);
            return -1;
        }
    }

    // 等待并收集结果
    for (int i = 0; i < NUM_THREADS; i++) {
        void *retval;
        int rc = pthread_join(threads[i], &retval);
        if (rc == 0 && retval != NULL) {
            printf("[Main] Thread %d returned: %d\n", i, *(int*)retval);
            free(retval);
        }
    }

    return 0;
}

/*========================================
 * 模式1.2：分离线程（后台任务）
 *========================================*/
void *background_task(void *arg) {
    const char *task_name = (const char *)arg;
    printf("[Background] %s started\n", task_name);
    sleep(2);
    printf("[Background] %s completed\n", task_name);
    return NULL;
}

int demo_detached_thread() {
    pthread_t thread;
    pthread_attr_t attr;

    // 设置分离属性
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&thread, &attr, background_task, "CleanupTask");
    pthread_attr_destroy(&attr);

    // 主线程继续，无需join
    printf("[Main] Detached thread created, continuing...\n");
    sleep(3);  // 确保后台线程完成（实际应用不需要）

    return 0;
}

/*========================================
 * 模式1.3：线程取消（优雅终止）
 *========================================*/
volatile int cancel_flag = 0;

void *cancellable_worker(void *arg) {
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    while (!cancel_flag) {
        // 工作循环，定期调用测试点
        printf("[Worker] Working...\n");
        pthread_testcancel();  // 显式取消点
        sleep(1);
    }

    printf("[Worker] Gracefully exiting\n");
    pthread_cleanup_pop(0);
    return NULL;
}

int demo_cancel_thread() {
    pthread_t thread;
    pthread_create(&thread, NULL, cancellable_worker, NULL);

    sleep(3);
    printf("[Main] Requesting cancel...\n");
    pthread_cancel(thread);

    pthread_join(thread, NULL);
    printf("[Main] Thread cancelled\n");

    return 0;
}
```

### 4.2 线程池完整实现

```c
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

/*========================================
 * 完整线程池实现
 *========================================*/

typedef struct task {
    void (*function)(void *);
    void *argument;
    struct task *next;
} task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t notify;       // 通知新任务
    pthread_cond_t finished;     // 通知全部完成
    pthread_t *threads;
    task_t *head, *tail;
    int thread_count;
    int queue_size;
    int shutdown;
    int active_tasks;
    int completed_tasks;
} threadpool_t;

static void *worker_thread(void *arg);

threadpool_t *threadpool_create(int num_threads, int queue_size) {
    threadpool_t *pool = calloc(1, sizeof(threadpool_t));
    if (!pool) return NULL;

    pool->thread_count = num_threads;
    pool->queue_size = queue_size;

    pthread_mutex_init(&pool->lock, NULL);
    pthread_cond_init(&pool->notify, NULL);
    pthread_cond_init(&pool->finished, NULL);

    pool->threads = calloc(num_threads, sizeof(pthread_t));
    if (!pool->threads) goto err;

    for (int i = 0; i < num_threads; i++) {
        if (pthread_create(&pool->threads[i], NULL, worker_thread, pool) != 0) {
            goto err;
        }
    }

    return pool;

err:
    threadpool_destroy(pool, 0);
    return NULL;
}

int threadpool_submit(threadpool_t *pool, void (*function)(void *), void *arg) {
    if (!pool || !function) return -1;

    task_t *task = malloc(sizeof(task_t));
    if (!task) return -1;

    task->function = function;
    task->argument = arg;
    task->next = NULL;

    pthread_mutex_lock(&pool->lock);

    if (pool->shutdown) {
        pthread_mutex_unlock(&pool->lock);
        free(task);
        return -1;
    }

    // 添加到队列尾部
    if (pool->tail) {
        pool->tail->next = task;
    } else {
        pool->head = task;
    }
    pool->tail = task;
    pool->queue_size++;

    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    return 0;
}

int threadpool_wait_all(threadpool_t *pool) {
    pthread_mutex_lock(&pool->lock);
    while (pool->active_tasks > 0 || pool->queue_size > 0) {
        pthread_cond_wait(&pool->finished, &pool->lock);
    }
    pthread_mutex_unlock(&pool->lock);
    return 0;
}

int threadpool_destroy(threadpool_t *pool, int graceful) {
    if (!pool) return -1;

    pthread_mutex_lock(&pool->lock);
    pool->shutdown = graceful ? 2 : 1;  // 2=优雅关闭，1=立即
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < pool->thread_count; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    // 清理未处理任务
    while (pool->head) {
        task_t *task = pool->head;
        pool->head = task->next;
        free(task);
    }

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    pthread_cond_destroy(&pool->finished);
    free(pool->threads);
    free(pool);

    return 0;
}

static void *worker_thread(void *arg) {
    threadpool_t *pool = arg;

    while (1) {
        pthread_mutex_lock(&pool->lock);

        // 等待任务或关闭信号
        while (!pool->head && pool->shutdown != 1) {
            if (pool->shutdown == 2 && !pool->active_tasks) {
                pthread_mutex_unlock(&pool->lock);
                return NULL;
            }
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        if (pool->shutdown == 1) {
            pthread_mutex_unlock(&pool->lock);
            return NULL;
        }

        // 获取任务
        task_t *task = pool->head;
        if (task) {
            pool->head = task->next;
            if (!pool->head) pool->tail = NULL;
            pool->queue_size--;
            pool->active_tasks++;
        }

        pthread_mutex_unlock(&pool->lock);

        if (task) {
            // 执行任务
            task->function(task->argument);

            pthread_mutex_lock(&pool->lock);
            pool->active_tasks--;
            pool->completed_tasks++;
            if (pool->active_tasks == 0 && pool->queue_size == 0) {
                pthread_cond_broadcast(&pool->finished);
            }
            pthread_mutex_unlock(&pool->lock);

            free(task);
        }
    }

    return NULL;
}

/* 使用示例 */
void example_task(void *arg) {
    int num = *(int*)arg;
    printf("[Task %d] Executing on thread %lu\n", num, pthread_self());
    usleep(100000);  // 模拟工作
}

int demo_threadpool() {
    threadpool_t *pool = threadpool_create(4, 100);
    if (!pool) return -1;

    int tasks[20];
    for (int i = 0; i < 20; i++) {
        tasks[i] = i;
        threadpool_submit(pool, example_task, &tasks[i]);
    }

    threadpool_wait_all(pool);
    printf("All tasks completed\n");

    threadpool_destroy(pool, 1);
    return 0;
}
```

### 4.3 生产者-消费者模式（多种实现）

#### 实现1：单生产者-单消费者（有界缓冲区）

```c
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int head;           // 写入位置
    int tail;           // 读取位置
    int count;          // 当前数量
    pthread_mutex_t lock;
    pthread_cond_t not_full;   // 缓冲区不满
    pthread_cond_t not_empty;  // 缓冲区不空
} bounded_buffer_t;

void bounded_buffer_init(bounded_buffer_t *bb) {
    bb->head = bb->tail = bb->count = 0;
    pthread_mutex_init(&bb->lock, NULL);
    pthread_cond_init(&bb->not_full, NULL);
    pthread_cond_init(&bb->not_empty, NULL);
}

void bounded_buffer_put(bounded_buffer_t *bb, int item) {
    pthread_mutex_lock(&bb->lock);

    // 等待直到缓冲区不满
    while (bb->count == BUFFER_SIZE) {
        pthread_cond_wait(&bb->not_full, &bb->lock);
    }

    // 放入数据
    bb->buffer[bb->head] = item;
    bb->head = (bb->head + 1) % BUFFER_SIZE;
    bb->count++;

    printf("[Producer] Put %d, count=%d\n", item, bb->count);

    // 通知消费者
    pthread_cond_signal(&bb->not_empty);
    pthread_mutex_unlock(&bb->lock);
}

int bounded_buffer_get(bounded_buffer_t *bb) {
    pthread_mutex_lock(&bb->lock);

    // 等待直到缓冲区不空
    while (bb->count == 0) {
        pthread_cond_wait(&bb->not_empty, &bb->lock);
    }

    // 取出数据
    int item = bb->buffer[bb->tail];
    bb->tail = (bb->tail + 1) % BUFFER_SIZE;
    bb->count--;

    printf("[Consumer] Got %d, count=%d\n", item, bb->count);

    // 通知生产者
    pthread_cond_signal(&bb->not_full);
    pthread_mutex_unlock(&bb->lock);

    return item;
}
```

#### 实现2：多生产者-多消费者（使用信号量）

```c
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define QUEUE_SIZE 100

typedef struct {
    void **buffer;
    int head, tail;
    sem_t empty;    // 空槽数量
    sem_t full;     // 填充槽数量
    pthread_mutex_t head_lock;  // 保护head
    pthread_mutex_t tail_lock;  // 保护tail
} mpmc_queue_t;

mpmc_queue_t *mpmc_queue_create(int size) {
    mpmc_queue_t *q = calloc(1, sizeof(mpmc_queue_t));
    q->buffer = calloc(size, sizeof(void*));
    q->head = q->tail = 0;
    sem_init(&q->empty, 0, size);
    sem_init(&q->full, 0, 0);
    pthread_mutex_init(&q->head_lock, NULL);
    pthread_mutex_init(&q->tail_lock, NULL);
    return q;
}

void mpmc_queue_put(mpmc_queue_t *q, void *item) {
    sem_wait(&q->empty);  // 等待空槽

    pthread_mutex_lock(&q->head_lock);
    q->buffer[q->head] = item;
    q->head = (q->head + 1) % QUEUE_SIZE;
    pthread_mutex_unlock(&q->head_lock);

    sem_post(&q->full);   // 增加已填充槽计数
}

void *mpmc_queue_get(mpmc_queue_t *q) {
    sem_wait(&q->full);   // 等待已填充槽

    pthread_mutex_lock(&q->tail_lock);
    void *item = q->buffer[q->tail];
    q->buffer[q->tail] = NULL;
    q->tail = (q->tail + 1) % QUEUE_SIZE;
    pthread_mutex_unlock(&q->tail_lock);

    sem_post(&q->empty);  // 增加空槽计数
    return item;
}
```

### 4.4 读者-写者问题（多种策略）

#### 策略1：读者优先

```c
/* 读者优先策略：读者可以插队，写者可能饿死 */
typedef struct {
    pthread_mutex_t lock;
    pthread_mutex_t write_lock;  // 写者互斥
    int reader_count;
} read_prefer_rw_t;

void read_prefer_init(read_prefer_rw_t *rw) {
    pthread_mutex_init(&rw->lock, NULL);
    pthread_mutex_init(&rw->write_lock, NULL);
    rw->reader_count = 0;
}

void read_prefer_read_lock(read_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->reader_count++;
    if (rw->reader_count == 1) {
        pthread_mutex_lock(&rw->write_lock);  // 第一个读者阻止写者
    }
    pthread_mutex_unlock(&rw->lock);
}

void read_prefer_read_unlock(read_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->reader_count--;
    if (rw->reader_count == 0) {
        pthread_mutex_unlock(&rw->write_lock);  // 最后一个读者允许写者
    }
    pthread_mutex_unlock(&rw->lock);
}

void read_prefer_write_lock(read_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->write_lock);  // 独占访问
}

void read_prefer_write_unlock(read_prefer_rw_t *rw) {
    pthread_mutex_unlock(&rw->write_lock);
}
```

#### 策略2：写者优先

```c
/* 写者优先策略：写者到达后阻塞新读者，防止写者饿死 */
typedef struct {
    pthread_mutex_t lock;
    pthread_mutex_t write_lock;
    pthread_cond_t reader_queue;   // 读者等待队列
    int reader_count;
    int writer_waiting;            // 等待的写者数量
    int writer_active;             // 活跃的写者
} write_prefer_rw_t;

void write_prefer_read_lock(write_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    // 等待所有写者完成
    while (rw->writer_active || rw->writer_waiting > 0) {
        pthread_cond_wait(&rw->reader_queue, &rw->lock);
    }
    rw->reader_count++;
    pthread_mutex_unlock(&rw->lock);
}

void write_prefer_write_lock(write_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writer_waiting++;
    // 等待所有读者和其他写者完成
    while (rw->reader_count > 0 || rw->writer_active) {
        pthread_mutex_unlock(&rw->lock);
        pthread_mutex_lock(&rw->write_lock);
        pthread_mutex_lock(&rw->lock);
    }
    rw->writer_waiting--;
    rw->writer_active = 1;
    pthread_mutex_unlock(&rw->lock);
}

void write_prefer_write_unlock(write_prefer_rw_t *rw) {
    pthread_mutex_lock(&rw->lock);
    rw->writer_active = 0;
    pthread_cond_broadcast(&rw->reader_queue);  // 唤醒等待的读者
    pthread_mutex_unlock(&rw->lock);
    pthread_mutex_unlock(&rw->write_lock);
}
```

#### 策略3：公平策略（使用POSIX读写锁）

```c
/* 使用标准POSIX读写锁（通常实现公平策略） */
#include <pthread.h>

typedef pthread_rwlock_t fair_rw_t;

void fair_rw_init(fair_rw_t *rw) {
    pthread_rwlockattr_t attr;
    pthread_rwlockattr_init(&attr);
    // 设置写者优先（如果支持）
    #ifdef PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP
    pthread_rwlockattr_setkind_np(&attr,
        PTHREAD_RWLOCK_PREFER_WRITER_NONRECURSIVE_NP);
    #endif
    pthread_rwlock_init(rw, &attr);
    pthread_rwlockattr_destroy(&attr);
}

void fair_read_lock(fair_rw_t *rw) {
    pthread_rwlock_rdlock(rw);
}

void fair_write_lock(fair_rw_t *rw) {
    pthread_rwlock_wrlock(rw);
}

void fair_unlock(fair_rw_t *rw) {
    pthread_rwlock_unlock(rw);
}
```

### 4.5 哲学家就餐问题（死锁避免）

```c
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

#define N 5  // 哲学家数量

/*========================================
 * 策略1：资源分级（全局排序）
 *========================================*/
typedef struct {
    pthread_mutex_t forks[N];
} table_t;

table_t table;

void *philosopher_hierarchy(void *arg) {
    int id = *(int*)arg;
    int left = id;
    int right = (id + 1) % N;
    int first, second;

    // 资源分级：总是先拿编号小的叉子
    if (left < right) {
        first = left; second = right;
    } else {
        first = right; second = left;
    }

    for (int i = 0; i < 10; i++) {
        printf("Philosopher %d thinking...\n", id);
        usleep(100000);

        // 按序加锁
        pthread_mutex_lock(&table.forks[first]);
        printf("Philosopher %d picked fork %d\n", id, first);
        pthread_mutex_lock(&table.forks[second]);
        printf("Philosopher %d picked fork %d, eating...\n", id, second);

        usleep(100000);  // 就餐

        pthread_mutex_unlock(&table.forks[second]);
        pthread_mutex_unlock(&table.forks[first]);
        printf("Philosopher %d finished eating\n", id);
    }

    return NULL;
}

/*========================================
 * 策略2：互斥进入（限制同时就餐人数）
 *========================================*/
typedef struct {
    pthread_mutex_t forks[N];
    sem_t room;  // 同时只允许N-1人就餐
} table_limit_t;

table_limit_t table2;

void *philosopher_limit(void *arg) {
    int id = *(int*)arg;
    int left = id;
    int right = (id + 1) % N;

    for (int i = 0; i < 10; i++) {
        printf("Philosopher %d thinking...\n", id);
        usleep(100000);

        sem_wait(&table2.room);  // 申请进入餐厅

        pthread_mutex_lock(&table2.forks[left]);
        pthread_mutex_lock(&table2.forks[right]);

        printf("Philosopher %d eating...\n", id);
        usleep(100000);

        pthread_mutex_unlock(&table2.forks[right]);
        pthread_mutex_unlock(&table2.forks[left]);

        sem_post(&table2.room);  // 离开餐厅
        printf("Philosopher %d finished\n", id);
    }

    return NULL;
}

int demo_dining_philosophers() {
    pthread_t philosophers[N];
    int ids[N];

    // 初始化
    for (int i = 0; i < N; i++) {
        pthread_mutex_init(&table.forks[i], NULL);
    }

    // 创建哲学家线程
    for (int i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&philosophers[i], NULL, philosopher_hierarchy, &ids[i]);
    }

    // 等待
    for (int i = 0; i < N; i++) {
        pthread_join(philosophers[i], NULL);
    }

    return 0;
}
```

### 4.6 屏障同步模式

```c
#include <pthread.h>
#include <stdio.h>

/*========================================
 * 自定义屏障实现（兼容旧系统）
 *========================================*/
typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t cond;
    int count;           // 当前等待线程数
    int threshold;       // 触发阈值
    int generation;      // 代计数（处理虚假唤醒）
    int sense;           // 本地感应变量
} custom_barrier_t;

int custom_barrier_init(custom_barrier_t *b, int threshold) {
    pthread_mutex_init(&b->lock, NULL);
    pthread_cond_init(&b->cond, NULL);
    b->count = 0;
    b->threshold = threshold;
    b->generation = 0;
    b->sense = 0;
    return 0;
}

int custom_barrier_wait(custom_barrier_t *b) {
    pthread_mutex_lock(&b->lock);

    int my_gen = b->generation;
    b->count++;

    if (b->count == b->threshold) {
        // 最后一个到达的线程
        b->generation++;   // 进入下一代
        b->count = 0;
        pthread_cond_broadcast(&b->cond);
        pthread_mutex_unlock(&b->lock);
        return 1;  // 返回特殊值表示"序列领导者"
    } else {
        // 等待其他线程
        while (b->generation == my_gen) {
            pthread_cond_wait(&b->cond, &b->lock);
        }
        pthread_mutex_unlock(&b->lock);
        return 0;
    }
}

/*========================================
 * 使用POSIX屏障（现代系统）
 *========================================*/
void demo_posix_barrier() {
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, 4);  // 4个线程同步

    // 工作函数中
    // pthread_barrier_wait(&barrier);  // 所有线程到达后才能继续

    pthread_barrier_destroy(&barrier);
}

/*========================================
 * 分阶段并行计算示例
 *========================================*/
#define NUM_WORKERS 4
#define DATA_SIZE 1000000

double data[DATA_SIZE];
pthread_barrier_t phase_barrier;

void *phased_worker(void *arg) {
    int id = *(int*)arg;
    int chunk = DATA_SIZE / NUM_WORKERS;
    int start = id * chunk;
    int end = (id == NUM_WORKERS - 1) ? DATA_SIZE : start + chunk;

    // 阶段1：局部计算
    for (int i = start; i < end; i++) {
        data[i] = i * 0.5;  // 初始化
    }

    pthread_barrier_wait(&phase_barrier);  // 等待所有线程完成阶段1

    // 阶段2：需要全局数据的计算
    double local_sum = 0;
    for (int i = start; i < end; i++) {
        local_sum += data[i];
    }

    // 存储局部结果到全局（需要同步）
    // ...

    pthread_barrier_wait(&phase_barrier);  // 等待阶段2

    return NULL;
}
```

### 4.7 工作窃取队列实现

```c
#include <pthread.h>
#include <stdlib.h>
#include <stdatomic.h>

/*========================================
 * Chase-Lev 工作窃取双端队列
 * 支持：
 * - 单线程owner从尾部push/pop
 * - 多线程stealer从头部steal
 *========================================*/

typedef struct {
    void **buffer;
    atomic_size_t top;      // 窃取端（由 thieves 使用）
    atomic_size_t bottom;   // 工作端（由 owner 使用）
    size_t capacity;
    pthread_mutex_t resize_lock;
} work_steal_queue_t;

work_steal_queue_t *wsq_create(size_t initial_capacity) {
    work_steal_queue_t *q = calloc(1, sizeof(work_steal_queue_t));
    q->capacity = initial_capacity;
    q->buffer = calloc(initial_capacity, sizeof(void*));
    atomic_init(&q->top, 0);
    atomic_init(&q->bottom, 0);
    pthread_mutex_init(&q->resize_lock, NULL);
    return q;
}

// Owner操作：从尾部压入（无锁）
void wsq_push(work_steal_queue_t *q, void *item) {
    size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed);
    size_t t = atomic_load_explicit(&q->top, memory_order_acquire);

    // 检查是否需要扩容
    if (b - t >= q->capacity - 1) {
        pthread_mutex_lock(&q->resize_lock);
        // 重新检查并扩容
        // ... 实现省略
        pthread_mutex_unlock(&q->resize_lock);
    }

    q->buffer[b % q->capacity] = item;
    atomic_thread_fence(memory_order_release);
    atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
}

// Owner操作：从尾部弹出
void *wsq_pop(work_steal_queue_t *q) {
    size_t b = atomic_load_explicit(&q->bottom, memory_order_relaxed) - 1;
    atomic_store_explicit(&q->bottom, b, memory_order_relaxed);
    atomic_thread_fence(memory_order_seq_cst);

    size_t t = atomic_load_explicit(&q->top, memory_order_relaxed);
    void *item;

    if (t <= b) {
        // 队列非空
        item = q->buffer[b % q->capacity];
        if (t == b) {
            // 最后一个元素，需要与stealer竞争
            if (!atomic_compare_exchange_strong_explicit(
                    &q->top, &t, t + 1,
                    memory_order_seq_cst, memory_order_relaxed)) {
                // 竞争失败，stealer已取走
                atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
                return NULL;
            }
            atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
        }
        return item;
    } else {
        // 队列为空
        atomic_store_explicit(&q->bottom, b + 1, memory_order_relaxed);
        return NULL;
    }
}

// Stealer操作：从头部窃取（多线程安全）
void *wsq_steal(work_steal_queue_t *q) {
    size_t t = atomic_load_explicit(&q->top, memory_order_acquire);
    atomic_thread_fence(memory_order_seq_cst);
    size_t b = atomic_load_explicit(&q->bottom, memory_order_acquire);

    if (t < b) {
        // 队列非空，尝试窃取
        void *item = q->buffer[t % q->capacity];
        if (atomic_compare_exchange_strong_explicit(
                &q->top, &t, t + 1,
                memory_order_seq_cst, memory_order_relaxed)) {
            return item;
        }
        // 竞争失败
        return NULL;  // 可以重试或返回空
    }
    return NULL;  // 队列为空
}
```

### 4.8 线程局部存储使用

```c
#include <pthread.h>
#include <stdio.h>

/*========================================
 * 方法1：GCC __thread扩展（编译器级）
 *========================================*/
__thread int thread_local_counter = 0;
__thread char thread_local_buffer[1024];

void demo_compiler_tls() {
    thread_local_counter++;
    printf("Thread %lu: counter = %d\n", pthread_self(), thread_local_counter);
}

/*========================================
 * 方法2：POSIX pthread_key（标准可移植）
 *========================================*/
pthread_key_t tls_key;

typedef struct {
    int id;
    int call_count;
    void *private_data;
} thread_context_t;

void tls_destructor(void *value) {
    thread_context_t *ctx = value;
    printf("[Destructor] Cleaning up thread context %d\n", ctx->id);
    free(ctx);
}

void init_tls() {
    pthread_key_create(&tls_key, tls_destructor);
}

thread_context_t *get_thread_context() {
    thread_context_t *ctx = pthread_getspecific(tls_key);
    if (ctx == NULL) {
        ctx = calloc(1, sizeof(thread_context_t));
        pthread_setspecific(tls_key, ctx);
    }
    return ctx;
}

void *tls_worker(void *arg) {
    int id = *(int*)arg;
    thread_context_t *ctx = get_thread_context();
    ctx->id = id;

    for (int i = 0; i < 5; i++) {
        ctx->call_count++;
        printf("[Thread %d] Call count: %d\n", id, ctx->call_count);
    }

    return NULL;
}

/*========================================
 * 方法3：C11 _Thread_local（标准C11）
 *========================================*/
#include <threads.h>

_Thread_local int c11_thread_local = 0;

int c11_worker(void *arg) {
    c11_thread_local = *(int*)arg;
    printf("C11 TLS value: %d\n", c11_thread_local);
    return 0;
}
```
