# C代码验证与TLA+的关联

---

## 🔗 知识关联网络

### 1. 全局导航

| 层级 | 文档 | 作用 |
|:-----|:-----|:-----|
| 总索引 | [../../00_GLOBAL_INDEX.md](../../00_GLOBAL_INDEX.md) | 完整知识图谱入口 |
| 本模块 | [../../README.md](../../README.md) | 模块总览与导航 |
| 学习路径 | [../../06_Thinking_Representation/06_Learning_Paths/README.md](../../06_Thinking_Representation/06_Learning_Paths/README.md) | 推荐学习路线 |

### 2. 前置知识依赖

| 文档 | 关系 | 掌握要求 |
|:-----|:-----|:---------|
| [../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md](../../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 语言基础 | 必须掌握 |
| [../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md](../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心机制 | 必须掌握 |
| [../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 内存基础 | 必须掌握 |

### 3. 同层横向关联

| 文档 | 关系 | 互补内容 |
|:-----|:-----|:---------|
| [../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md](../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 技术扩展 | 并发编程技术 |
| [../../02_Formal_Semantics_and_Physics/README.md](../../02_Formal_Semantics_and_Physics/README.md) | 理论支撑 | 形式化方法 |
| [../../04_Industrial_Scenarios/README.md](../../04_Industrial_Scenarios/README.md) | 实践应用 | 工业实践案例 |

### 4. 深层理论关联

| 文档 | 关系 | 理论深度 |
|:-----|:-----|:---------|
| [../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/README.md) | 语义基础 | 操作语义、类型理论 |
| [../../06_Thinking_Representation/05_Concept_Mappings/README.md](../../06_Thinking_Representation/05_Concept_Mappings/README.md) | 概念映射 | 知识关联网络 |

### 5. 后续进阶延伸

| 文档 | 关系 | 进阶方向 |
|:-----|:-----|:---------|
| [../../03_System_Technology_Domains/README.md](../../03_System_Technology_Domains/README.md) | 系统技术 | 系统级开发 |
| [../../01_Core_Knowledge_System/09_Safety_Standards/README.md](../../01_Core_Knowledge_System/09_Safety_Standards/README.md) | 安全标准 | 安全编码规范 |
| [../../07_Modern_Toolchain/README.md](../../07_Modern_Toolchain/README.md) | 工具链 | 现代开发工具 |

### 6. 阶段学习定位

```
当前位置: 根据文档主题确定学习阶段
├─ 入门阶段: 基础语法、数据类型
├─ 基础阶段: 控制流程、函数
├─ 进阶阶段: 指针、内存管理 ⬅️ 可能在此
├─ 高级阶段: 并发、系统编程
└─ 专家阶段: 形式验证、编译器
```

### 7. 局部索引

本文件所属模块的详细内容：

- 参见本模块 [README.md](../../README.md)
- 相关子目录文档


## 1. C并发代码到TLA+的映射

### 1.1 基本概念映射

```
C语言并发概念 → TLA+规范概念
═══════════════════════════════════════════════════════════════

┌─────────────────────┬────────────────────────────────────────────┐
│ C语言               │ TLA+对应概念                                 │
├─────────────────────┼────────────────────────────────────────────┤
│ pthread_t           │ process定义                                 │
│ pthread_create()    │ process启动 (初始状态的一部分)               │
│ pthread_join()      │ 进程终止检测                                 │
│ mutex/lock          │ 原子变量或await条件                          │
│ condition variable  │ await语句                                    │
│ semaphore           │ 计数器变量 + await条件                       │
│ atomic variable     │ TLA+变量 (所有赋值原子)                      │
│ memory barrier      │ 时序依赖关系                                 │
│ shared memory       │ VARIABLES声明                                │
│ thread-local        │ process variables                            │
└─────────────────────┴────────────────────────────────────────────┘
```

### 1.2 状态映射模型

```
C程序执行状态 → TLA+状态
═══════════════════════════════════════════════════════════════

C程序状态构成:
┌──────────────────────────────────────────────────────────────┐
│  1. 全局变量值                                               │
│  2. 各线程栈帧 (局部变量)                                    │
│  3. 程序计数器 (PC) 位置                                     │
│  4. 锁/信号量状态                                            │
│  5. 条件变量等待队列                                         │
│  6. 内存内容                                                 │
└──────────────────────────────────────────────────────────────┘

TLA+状态表示:
┌──────────────────────────────────────────────────────────────┐
│  VARIABLES                                                   │
│    global_vars,      ← 对应C全局变量                        │
│    pc,               ← 程序计数器 (每个进程的当前标签)      │
│    stack,            ← 调用栈状态                           │
│    lock_owners;      ← 锁持有者                             │
│                                                              │
│  process Thread \in ThreadIDs                                │
│    variables                                                 │
│      local_vars;     ← 对应线程局部变量                     │
│    begin ... end                                            │
└──────────────────────────────────────────────────────────────┘
```

### 1.3 代码结构映射示例

```c
/* ========== C语言版本 ========== */
/* 简单的计数器 - C实现 */
#include <pthread.h>

int counter = 0;
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void* increment_thread(void* arg) {
    for (int i = 0; i < 100; i++) {
        pthread_mutex_lock(&counter_mutex);
        counter++;
        pthread_mutex_unlock(&counter_mutex);
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, increment_thread, NULL);
    pthread_create(&t2, NULL, increment_thread, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    printf("counter = %d\n", counter);
    return 0;
}
```

```tla
(* ========== TLA+版本 ========== *)
---- MODULE CounterVerification ----
EXTENDS Naturals, TLC

CONSTANTS N, MaxCount

(* --algorithm Counter
variables
  counter = 0,
  lock = 0;  (* 0 = 未锁定, 非0 = 持有者ID *)

define
  (* 类型不变式 *)
  TypeInvariant ==
    /\ counter \in Nat
    /\ lock \in 0..N

  (* 最终结果: counter应该是200 *)
  FinalResult ==
    (\A i \in 1..N : pc[i] = "Done") => counter = N * MaxCount
end define

process Worker \in 1..N
variables
  i = 0
begin
Loop:
  while i < MaxCount do
Acquire:
    await lock = 0;
    lock := self;

Increment:
    counter := counter + 1;
    i := i + 1;

Release:
    lock := 0;
  end while;

Done:
  skip;
end process

end algorithm *)
====
```

---

## 2. 内存模型映射

### 2.1 C内存模型概述

```
C11内存模型层级:
┌────────────────────────────────────────────────────────────┐
│ memory_order_relaxed: 无同步或顺序保证                     │
│ memory_order_consume: 数据依赖顺序 (很少使用)              │
│ memory_order_acquire: 载入后读操作不重排到此载入前         │
│ memory_order_release: 存储前写操作不重排到此存储后         │
│ memory_order_acq_rel: 同时具有acquire和release语义         │
│ memory_order_seq_cst: 顺序一致性 (默认)                    │
└────────────────────────────────────────────────────────────┘

内存序与TLA+对应:
┌─────────────────────────┬──────────────────────────────────┐
│ C内存序                 │ TLA+表示                          │
├─────────────────────────┼──────────────────────────────────┤
│ relaxed                 │ 普通变量访问                      │
│ acquire/release         │ 消息传递模式                      │
│ seq_cst                 │ TLA+默认语义 (原子)               │
└─────────────────────────┴──────────────────────────────────┘
```

### 2.2 验证C内存序的TLA+模型

```tla
---- MODULE CMemoryModel ----
EXTENDS Naturals, Sequences, TLC

(* 验证C11 Release-Acquire序的TLA+模型 *)

CONSTANTS Writer, Reader

(* --algorithm ReleaseAcquire
variables
  data = 0,        (* 共享数据 *)
  flag = 0;        (* 同步标志 *)

define
  (* 关键性质: Reader看到flag=1时，data必须已写入 *)
  SyncCorrectness ==
    pc[Reader] = "ReadData" =>
      (flag = 1 => data = 42)

  (* 数据竞争自由 *)
  DataRaceFree ==
    ~((pc[Writer] = "WriteData" \/ pc[Writer] = "WriteFlag") /\
      pc[Reader] = "ReadData")
end define

process WriterProc = Writer
begin
WriteData:
  (* 先写数据 - Release前 *)
  data := 42;

WriteFlag:
  (* Release操作: flag存储 *)
  flag := 1;

DoneW:
  skip;
end process;

process ReaderProc = Reader
begin
WaitFlag:
  (* Acquire操作: 等待flag *)
  await flag = 1;

ReadData:
  (* 此时保证看到data=42 *)
  assert data = 42;

DoneR:
  skip;
end process;

end algorithm *)
====
```

### 2.3 内存序错误检测

```tla
(* 检测错误的内存序使用 *)
---- MODULE MemoryOrderBug ----
EXTENDS Naturals, TLC

(* 有bug的实现: 缺少适当的内存屏障 *)

(* --algorithm BuggyImplementation
variables
  x = 0,
  y = 0,
  z = 0;

define
  (* 期望的性质: z=1时x应该为1 *)
  ExpectedBehavior ==
    z = 1 => x = 1

  (* 实际可能违反 - 由于重排序 *)
  BugCondition ==
    z = 1 /\ x = 0  (* 数据竞争! *)
end define

process Writer = "Writer"
begin
W1: x := 1;        (* 写x *)
W2: y := 1;        (* 写y - 可能被重排到x之前 *)
end process;

process Reader = "Reader"
begin
R1: await y = 1;   (* 等待y *)
R2: z := 1;        (* 设置z *)
R3: assert x = 1;  (* 可能失败! *)
end process;

end algorithm *)

(*
 * TLC会发现违反ExpectedBehavior的情况
 * 原因: C编译器和CPU可能重排序W1和W2
 * 修复: 在W1后添加Release屏障，在R1后添加Acquire屏障
 *)
```

---

## 3. 验证C并发原语

### 3.1 互斥锁验证

```tla
(* 验证pthread_mutex_lock实现的正确性 *)
---- MODULE MutexVerification ----
EXTENDS Naturals, FiniteSets, TLC

CONSTANTS N
ASSUME N \in Nat /\ N > 0

Procs == 1..N

(* --algorithm PthreadMutex
variables
  mutex_owner = 0,        (* 0 = 未锁定 *)
  mutex_count = 0,        (* 递归计数 *)
  wait_queue = << >>;     (* 等待队列 *)

define
  (* 互斥: 最多一个拥有者 *)
  MutualExclusion ==
    Cardinality({p \in Procs : pc[p] = "Critical"}) <= 1

  (* 类型不变式 *)
  TypeInvariant ==
    /\ mutex_owner \in 0..N
    /\ mutex_count \in Nat

  (* 无死锁: 如果有人想进入，最终能进入 *)
  NoDeadlock ==
    \A p \in Procs : (pc[p] = "LockWait") ~> (pc[p] = "Critical")
end define

macro pthread_mutex_lock(p) begin
  if mutex_owner = 0 then
    mutex_owner := p;
    mutex_count := 1;
  elsif mutex_owner = p then
    mutex_count := mutex_count + 1;  (* 递归锁定 *)
  else
    wait_queue := Append(wait_queue, p);
    await mutex_owner = 0;  (* 等待解锁 *)
    mutex_owner := p;
    mutex_count := 1;
  end if;
end macro;

macro pthread_mutex_unlock(p) begin
  if mutex_owner = p then
    if mutex_count > 1 then
      mutex_count := mutex_count - 1;
    else
      mutex_owner := 0;
      mutex_count := 0;
      (* 唤醒等待者 *)
      if wait_queue /= << >> then
        wait_queue := Tail(wait_queue);
      end if;
    end if;
  end if;
end macro;

process Thread \in Procs
begin
Lock:
  pthread_mutex_lock(self);

Critical:
  skip;

Unlock:
  pthread_mutex_unlock(self);

Done:
  skip;
end process;

end algorithm *)
====
```

### 3.2 读写锁验证

```tla
(* 验证pthread_rwlock的读者优先实现 *)
---- MODULE RWLockVerification ----
EXTENDS Naturals, TLC

CONSTANTS NReaders, NWriters

(* --algorithm PthreadRWLock
variables
  read_count = 0,      (* 活跃读者数 *)
  write_lock = 0,      (* 写锁状态 *)
  reader_wait = 0;     (* 等待的读者数 *)

define
  (* 读者-写者互斥 *)
  ReaderWriterMutex ==
    ~(read_count > 0 /\ write_lock /= 0)

  (* 单一写者 *)
  SingleWriter ==
    write_lock \in {0, 1}

  (* 读者安全: 写锁时无读者 *)
  ReaderSafety ==
    write_lock = 1 => read_count = 0
end define

macro pthread_rwlock_rdlock() begin
  reader_wait := reader_wait + 1;
  await write_lock = 0;  (* 等待无写者 *)
  reader_wait := reader_wait - 1;
  read_count := read_count + 1;
end macro;

macro pthread_rwlock_wrlock() begin
  await read_count = 0 /\ write_lock = 0;
  write_lock := 1;
end macro;

macro pthread_rwlock_unlock() begin
  if write_lock = 1 then
    write_lock := 0;
  else
    read_count := read_count - 1;
  end if;
end macro;

process Reader \in 1..NReaders
begin
RLock:
  pthread_rwlock_rdlock();

RRead:
  skip;

RUnlock:
  pthread_rwlock_unlock();
end process;

process Writer \in 1..NWriters
begin
WLock:
  pthread_rwlock_wrlock();

WWrite:
  skip;

WUnlock:
  pthread_rwlock_unlock();
end process;

end algorithm *)
====
```

### 3.3 条件变量验证

```tla
(* 验证条件变量的正确使用 *)
---- MODULE ConditionVariable ----
EXTENDS Naturals, Sequences, TLC

CONSTANTS BufferSize

(* --algorithm CondVar
variables
  buffer = << >>,
  count = 0,
  mutex = 0,
  not_full_waiters = 0,   (* 等待not_full的线程数 *)
  not_empty_waiters = 0;  (* 等待not_empty的线程数 *)

define
  (* 缓冲区状态正确 *)
  BufferInvariant ==
    /\ Len(buffer) = count
    /\ count \in 0..BufferSize

  (* 条件变量谓词 *)
  NotFull == count < BufferSize
  NotEmpty == count > 0
end define

macro pthread_mutex_lock() begin
  await mutex = 0;
  mutex := 1;
end macro;

macro pthread_mutex_unlock() begin
  mutex := 0;
end macro;

(* 条件变量等待 - 原子性释放锁并等待 *)
macro pthread_cond_wait(cond_var, pred) begin
  pthread_mutex_unlock();
  await pred;
  pthread_mutex_lock();
end macro;

macro pthread_cond_signal(cond_var) begin
  skip;  (* 唤醒一个等待者 *)
end macro;

process Producer = "Producer"
variables
  item = 0
begin
ProdLoop:
  while TRUE do
    (* 生产数据 *)
    item := item + 1;

ProdLock:
    pthread_mutex_lock();

ProdWait:
    (* 等待缓冲区有空间 *)
    if ~NotFull then
      not_full_waiters := not_full_waiters + 1;
      pthread_cond_wait("not_full", NotFull);
      not_full_waiters := not_full_waiters - 1;
    end if;

ProdAdd:
    buffer := Append(buffer, item);
    count := count + 1;

ProdSignal:
    (* 通知消费者 *)
    if not_empty_waiters > 0 then
      pthread_cond_signal("not_empty");
    end if;

ProdUnlock:
    pthread_mutex_unlock();
  end while;
end process;

process Consumer = "Consumer"
variables
  item = 0
begin
ConsLoop:
  while TRUE do
ConsLock:
    pthread_mutex_lock();

ConsWait:
    (* 等待缓冲区有数据 *)
    if ~NotEmpty then
      not_empty_waiters := not_empty_waiters + 1;
      pthread_cond_wait("not_empty", NotEmpty);
      not_empty_waiters := not_empty_waiters - 1;
    end if;

ConsRemove:
    item := Head(buffer);
    buffer := Tail(buffer);
    count := count - 1;

ConsSignal:
    (* 通知生产者 *)
    if not_full_waiters > 0 then
      pthread_cond_signal("not_full");
    end if;

ConsUnlock:
    pthread_mutex_unlock();

ConsProcess:
    (* 消费数据 *)
    skip;
  end while;
end process;

end algorithm *)
====
```

---

## 4. TLA+与CBMC对比

### 4.1 工具对比

```
TLA+ vs CBMC 对比:
═══════════════════════════════════════════════════════════════

┌─────────────────────┬───────────────────┬───────────────────┐
│ 特性                │ TLA+ / TLC        │ CBMC              │
├─────────────────────┼───────────────────┼───────────────────┤
│ 输入                │ 形式化规范        │ C代码             │
│ 验证对象            │ 算法/协议设计     │ 具体实现          │
│ 状态空间            │ 显式枚举          │ 有界模型检验      │
│ 循环处理            │ 无需展开          │ 需要设置unwind    │
│ 数组大小            │ 有界集合          │ 需要具体化        │
│ 浮点数              │ 不支持            │ 支持(近似)        │
│ 指针分析            │ 不支持            │ 支持              │
│ 内存安全            │ 不验证            │ 验证              │
│ 并发验证            │ 原生支持          │ 支持(复杂)        │
│ 反例长度            │ 最短反例          │ 取决于unwind      │
│ 学习曲线            │ 陡峭              │ 中等              │
└─────────────────────┴───────────────────┴───────────────────┘

使用场景:
┌─────────────────────────────────────────────────────────────┐
│ TLA+适合:                                                   │
│   - 设计新算法/协议                                         │
│   - 高层架构验证                                            │
│   - 并发交互分析                                            │
│   - 分布式系统                                              │
│                                                             │
│ CBMC适合:                                                   │
│   - 验证现有C代码                                           │
│   - 内存安全检查                                            │
│   - 数组越界检测                                            │
│   - 与TLA+验证后的设计对照                                  │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 相同算法的对比验证

```c
/* ========== CBMC验证示例 ========== */
/* 使用CBMC验证自旋锁 */

#include <stdatomic.h>

atomic_int lock = 0;
int shared_data = 0;

void acquire_lock(void) {
    while (atomic_exchange(&lock, 1) == 1) {
        // 自旋等待
    }
}

void release_lock(void) {
    atomic_store(&lock, 0);
}

void thread_func(void) {
    acquire_lock();
    shared_data++;  // 临界区
    release_lock();
}

// CBMC验证命令:
// cbmc --unwind 10 --unwinding-assertions \
//      --threads 2 --no-libc \
//      spinlock.c

// 验证性质:
// 1. 数据竞争自由 (--race-check)
// 2. 无断言失败 (--bounds-check)
// 3. 无内存泄漏 (--memory-leak-check)
```

```tla
(* ========== TLA+对应规范 ========== *)
---- MODULE SpinlockCBMCComparison ----
EXTENDS Naturals, TLC

CONSTANTS N

(* --algorithm Spinlock
variables
  lock = 0,
  shared_data = 0;

define
  (* 数据竞争自由 *)
  DataRaceFree ==
    Cardinality({p \in 1..N : pc[p] = "Critical"}) <= 1

  (* 最终正确性 *)
  Correctness ==
    (\A p \in 1..N : pc[p] = "Done") =>
      shared_data = N
end define

process Thread \in 1..N
begin
Acquire:
  await lock = 0;
  lock := 1;

Critical:
  shared_data := shared_data + 1;

Release:
  lock := 0;

Done:
  skip;
end process;

end algorithm *)
====
```

### 4.3 组合使用策略

```
推荐的验证流程:
═══════════════════════════════════════════════════════════════

┌─────────────────┐
│ 1. TLA+设计验证 │ ← 验证算法正确性，探索设计空间
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 2. C代码实现    │ ← 基于验证过的设计编写代码
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 3. CBMC验证     │ ← 验证实现符合设计，检查内存安全
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ 4. 运行时验证   │ ← 断言、Sanitizer测试
└─────────────────┘

具体实践:
├── TLA+阶段
│   ├── 编写算法高层规范
│   ├── 验证安全性和活性
│   └── 探索边界条件
│
├── 编码阶段
│   ├── 将TLA+不变式转为C断言
│   ├── 保持代码结构与规范一致
│   └── 添加运行时检查
│
├── CBMC阶段
│   ├── 验证无内存错误
│   ├── 检查数组边界
│   └── 验证并发安全
│
└── 测试阶段
    ├── 基于TLA+反例设计测试
    ├── 压力测试边界条件
    └── 混沌测试
```

---

## 5. 实践案例：验证C实现的一致性

### 5.1 简单状态机验证

```c
/* C实现: 简单的连接状态机 */
/* connection.c */

#include <stdatomic.h>

typedef enum {
    STATE_DISCONNECTED,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING
} ConnState;

atomic_int state = STATE_DISCONNECTED;

int connect(void) {
    int expected = STATE_DISCONNECTED;
    if (!atomic_compare_exchange_strong(&state, &expected,
                                        STATE_CONNECTING)) {
        return -1;  // 已经在连接中或已连接
    }

    // 执行连接操作...

    atomic_store(&state, STATE_CONNECTED);
    return 0;
}

int disconnect(void) {
    int expected = STATE_CONNECTED;
    if (!atomic_compare_exchange_strong(&state, &expected,
                                        STATE_DISCONNECTING)) {
        return -1;
    }

    // 执行断开操作...

    atomic_store(&state, STATE_DISCONNECTED);
    return 0;
}
```

```tla
(* TLA+验证: 状态机正确性 *)
---- MODULE ConnectionStateMachine ----
EXTENDS Naturals, TLC

CONSTANTS Client, Server

States == {"DISCONNECTED", "CONNECTING", "CONNECTED", "DISCONNECTING"}

(* --algorithm Connection
variables
  state = [Client |-> "DISCONNECTED", Server |-> "DISCONNECTED"];

define
  (* 有效状态转换 *)
  ValidTransition(p, old, new) ==
    \/ (old = "DISCONNECTED" /\ new = "CONNECTING")
    \/ (old = "CONNECTING" /\ new = "CONNECTED")
    \/ (old = "CONNECTED" /\ new = "DISCONNECTING")
    \/ (old = "DISCONNECTING" /\ new = "DISCONNECTED")

  (* 状态机安全: 只允许有效转换 *)
  StateMachineSafety ==
    [][\A p \in {Client, Server} :
        ValidTransition(p, state[p], state'[p])]_state

  (* 连接协议: 两端状态一致 *)
  ConnectionAgreement ==
    (state[Client] = "CONNECTED") <=> (state[Server] = "CONNECTED")
end define

process Peer \in {Client, Server}
begin
MainLoop:
  while TRUE do
    either
      (* 尝试连接 *)
      await state[self] = "DISCONNECTED";
      state[self] := "CONNECTING";

      (* 模拟连接延迟 *)
      state[self] := "CONNECTED";

    or
      (* 断开连接 *)
      await state[self] = "CONNECTED";
      state[self] := "DISCONNECTING";
      state[self] := "DISCONNECTED";
    end either;
  end while;
end process;

end algorithm *)
====
```

### 5.2 生产者-消费者验证

```c
/* C实现: 有界缓冲区 */
/* bounded_buffer.c */

#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 10

typedef struct {
    int buffer[BUFFER_SIZE];
    int count;
    int head;
    int tail;
    pthread_mutex_t mutex;
    sem_t not_full;
    sem_t not_empty;
} BoundedBuffer;

void buffer_init(BoundedBuffer* b) {
    b->count = 0;
    b->head = 0;
    b->tail = 0;
    pthread_mutex_init(&b->mutex, NULL);
    sem_init(&b->not_full, 0, BUFFER_SIZE);
    sem_init(&b->not_empty, 0, 0);
}

void buffer_put(BoundedBuffer* b, int item) {
    sem_wait(&b->not_full);          // P(not_full)
    pthread_mutex_lock(&b->mutex);    // P(mutex)

    // 断言: 缓冲区未满
    assert(b->count < BUFFER_SIZE);

    b->buffer[b->tail] = item;
    b->tail = (b->tail + 1) % BUFFER_SIZE;
    b->count++;

    pthread_mutex_unlock(&b->mutex);  // V(mutex)
    sem_post(&b->not_empty);          // V(not_empty)
}

int buffer_get(BoundedBuffer* b) {
    sem_wait(&b->not_empty);          // P(not_empty)
    pthread_mutex_lock(&b->mutex);    // P(mutex)

    // 断言: 缓冲区非空
    assert(b->count > 0);

    int item = b->buffer[b->head];
    b->head = (b->head + 1) % BUFFER_SIZE;
    b->count--;

    pthread_mutex_unlock(&b->mutex);  // V(mutex)
    sem_post(&b->not_full);           // V(not_full)

    return item;
}
```

```tla
(* TLA+验证: 有界缓冲区正确性 *)
---- MODULE BoundedBufferVerification ----
EXTENDS Naturals, Sequences, TLC

CONSTANTS BufferSize, NumItems

(* --algorithm BoundedBuffer
variables
  buffer = << >>,
  produced = 0,
  consumed = 0;

define
  (* 类型不变式 *)
  TypeInvariant ==
    /\ buffer \in Seq(Nat)
    /\ Len(buffer) \in 0..BufferSize
    /\ produced \in 0..NumItems
    /\ consumed \in 0..NumItems

  (* 安全: 缓冲区不溢出 *)
  NoOverflow ==
    Len(buffer) <= BufferSize

  (* 安全: 数据一致性 *)
  DataConsistency ==
    consumed <= produced

  (* 活性: 所有生产的数据最终都被消费 *)
  AllConsumed ==
    (produced = NumItems) ~> (consumed = NumItems)

  (* 信号量不变式 *)
  SemaphoreInvariant ==
    (* not_full = BufferSize - Len(buffer) *)
    (* not_empty = Len(buffer) *)
    TRUE
end define

process Producer = "Producer"
variables
  next_item = 1
begin
Produce:
  while produced < NumItems do
    await Len(buffer) < BufferSize;  (* P(not_full) *)

    buffer := Append(buffer, next_item);
    produced := produced + 1;
    next_item := next_item + 1;
    (* V(not_empty) - 隐式在Consumer的await中 *)
  end while;
end process;

process Consumer = "Consumer"
variables
  item
begin
Consume:
  while consumed < NumItems do
    await buffer /= << >>;  (* P(not_empty) *)

    item := Head(buffer);
    buffer := Tail(buffer);
    consumed := consumed + 1;
    (* V(not_full) - 隐式在Producer的await中 *)
  end while;
end process;

end algorithm *)
====
```

### 5.3 从TLA+到C的检查清单

```
验证C实现的检查清单:
═══════════════════════════════════════════════════════════════

□ 状态对应
  - C中的状态变量与TLA+ VARIABLES一一对应
  - 初始值一致
  - 状态转换逻辑等价

□ 不变式映射
  - 将TLA+不变式转为C断言
  - 在关键位置插入运行时检查
  - 编译时启用断言

□ 并发原语
  - mutex_lock对应await+赋值
  - condition_wait对应await条件
  - atomic操作对应TLA+原子语义

□ 边界条件
  - 缓冲区满/空
  - 计数器溢出
  - 资源耗尽

□ 错误处理
  - TLA+中要么省略错误路径
  - 要么显式建模，C代码需匹配

□ 内存管理
  - TLA+不验证内存安全
  - CBMC补充验证
  - 使用AddressSanitizer测试
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**关联文件**: 01_TLA_Introduction.md, 02_PlusCal_Language.md, 03_Model_Checking.md, 04_Distributed_Systems_Specs.md
**参考工具**: CBMC (<https://github.com/diffblue/cbmc>), TLA+ Toolbox


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
