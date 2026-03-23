# TLC模型检验器使用指南

## 1. TLC概述

### 1.1 什么是TLC？

TLC（Temporal Logic Checker）是TLA+的模型检验器，用于自动验证系统规范的正确性。

```
TLC核心功能:
├── 状态空间探索 - 枚举所有可能状态
├── 死锁检测 - 发现系统停滞状态
├── 不变式验证 - 检查安全属性
├── 时序性质验证 - 检查活性属性
├── 反例生成 - 提供失败轨迹
└── 覆盖率报告 - 状态空间统计

验证流程:
┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│  TLA+规范   │ -> │  TLC模型    │ -> │  验证报告   │
│             │    │  检验器     │    │             │
└─────────────┘    └─────────────┘    └─────────────┘
                          │
                    ┌─────┴─────┐
                    ▼           ▼
              ┌─────────┐  ┌─────────┐
              │  成功   │  │  失败   │
              │ ✓     │  │ 反例轨迹 │
              └─────────┘  └─────────┘
```

### 1.2 TLC与测试对比

| 特性 | 单元测试 | TLC模型检验 |
|------|----------|-------------|
| 覆盖范围 | 特定输入 | 所有可能输入 |
| 并发检查 | 困难 | 原生支持 |
| 非确定性 | 难以复现 | 显式枚举 |
| 边界条件 | 依赖测试设计 | 自动发现 |
| 形式化保证 | 无 | 数学级保证 |
| 可扩展性 | 好 | 受状态空间限制 |

---

## 2. TLC基础使用

### 2.1 命令行工具

```bash
# 基本用法
tlc ModuleName.tla

# 常用选项
tlc ModuleName.tla \
  -config ModuleName.cfg \    # 指定配置文件
  -workers 4 \                 # 并行工作线程
  -depth 100 \                 # 最大搜索深度
  -coverage 1 \                # 覆盖率分析
  -difftrace \                 # 显示差异轨迹
  -terse \                     # 简洁输出
  -nowarning                   # 忽略警告

# 示例
tlc Bakery.tla -workers 8 -coverage 3
```

### 2.2 配置文件格式

```properties
# ModuleName.cfg 配置文件示例

# 需要验证的规范
SPECIFICATION Spec

# 要检查的不变式
INVARIANT TypeInvariant
INVARIANT MutualExclusion

# 要检查的时序性质
PROPERTY EventuallyCritical

# 常量赋值
CONSTANTS
  N = 3
  MaxClock = 5

# 对称性定义（减少状态空间）
SYMMETRY symm

# 视图定义（用于状态压缩）
VIEW ViewDef

# 动作约束
ACTION_CONSTRAINT ActionConstraint

# 状态约束
CONSTRAINT StateConstraint
```

### 2.3 运行示例

```bash
$ tlc Bakery.tla -workers 4

TLC2 Version 2.18 of Day Month 20xx (rev: ffffc90)
Running breadth-first search Model-Checking with fp 6 and seed -5755327980209936551 with 4 workers on 8 cores with 1820MB heap and 64MB offheap memory...
Parsing file Bakery.tla
Parsing file /opt/toolbox/tla/tla2tools.jar!/tla2sany/StandardModules/Naturals.tla
Parsing file /opt/toolbox/tla/tla2tools.jar!/tla2sany/StandardModules/Sequences.tla
Semantic processing of module Naturals
Semantic processing of module Sequences
Semantic processing of module Bakery
Starting... (2026-03-19 10:00:00)
Computing initial states...
Finished computing initial states: 1 distinct state generated at 2026-03-19 10:00:01.
Model checking completed. No error has been found.
  Estimates of the probability that TLC did not check all reachable states
  because two distinct states had the same fingerprint:
  calculated (optimistic):  val = 1.1E-17
  based on the actual fingerprints:  val = 2.2E-17
  47 states generated, 16 distinct states found, 0 states left on queue.
  The depth of the complete state graph search is 6.
  The average outdegree of the complete state graph is 1 (minimum is 0, the maximum 4 and the 95th percentile is 3).
  Finished in 00s at (2026-03-19 10:00:02)
```

---

## 3. 状态空间探索原理

### 3.1 状态空间概念

```
状态空间构成:
┌─────────────────────────────────────────────────────────┐
│  State = (变量1值, 变量2值, ..., 变量N值, pc值)        │
│                                                         │
│  Behavior = State₀ → State₁ → State₂ → ...             │
│                ↑                                        │
│           通过动作(Action)转移                          │
└─────────────────────────────────────────────────────────┘

示例: 两个布尔变量的状态空间
                    ┌─────────┐
                    │ (0,0)   │ ← 初始状态
                    └────┬────┘
           ┌─────────────┼─────────────┐
           ▼             ▼             ▼
      ┌─────────┐   ┌─────────┐   ┌─────────┐
      │ (1,0)   │   │ (0,1)   │   │ (1,1)   │
      └────┬────┘   └────┬────┘   └────┬────┘
           │             │             │
           └─────────────┴─────────────┘
                         ▼
                    ┌─────────┐
                    │ (1,1)   │ ← 终端状态
                    └─────────┘
```

### 3.2 BFS vs DFS搜索

```
BFS (广度优先搜索):
├── 按层遍历状态
├── 找到最短反例
├── 内存消耗大
└── 适合找不变式违反

DFS (深度优先搜索):
├── 沿单条路径深入
├── 内存消耗小
├── 可能错过浅层错误
└── 适合找活性违反

TLC默认使用BFS，可通过选项调整:
  -dfid  : 使用深度优先迭代加深
  -fpmem : 设置指纹内存大小
```

### 3.3 状态压缩与优化

```tla
(* ===== 对称性减少 ===== *)
(* 如果进程是对称的，可以声明对称性 *)
Perms == Permutations(Procs)  (* 进程排列 *)

(* 在配置文件中 *)
(* SYMMETRY Perms *)

(* ===== 视图定义 ===== *)
(* 只关注关心的状态部分 *)
View == <<lock, resource>>  (* 忽略其他变量 *)

(* ===== 状态约束 ===== *)
(* 限制搜索范围 *)
StateConstraint ==
  /\ clock <= MaxClock
  /\ queue_len <= MaxQueue
```

---

## 4. 死锁检测

### 4.1 死锁的定义

```
死锁条件 (Coffman条件):
┌────────────────────────────────────────────────────┐
│ 1. 互斥: 资源不可共享                              │
│ 2. 占有并等待: 进程持有资源同时请求新资源          │
│ 3. 非抢占: 资源不能被强制释放                      │
│ 4. 循环等待: 存在进程循环等待链                    │
└────────────────────────────────────────────────────┘

PlusCal中的死锁:
┌────────────────────────────────────────────────────┐
│ - 所有进程都在await非真条件                        │
│ - 没有进程可以推进                                 │
│ - 系统状态不再变化                                 │
└────────────────────────────────────────────────────┘
```

### 4.2 死锁检测示例

```tla
---- MODULE DeadlockExample ----
EXTENDS Naturals, TLC

(* 简单的死锁示例: 两个进程互相等待 *)

(* --algorithm DeadlockDemo
variables
  resource_a = 0,  (* 0=空闲, 其他=持有者 *)
  resource_b = 0;

define
  (* 类型不变式 *)
  TypeInvariant ==
    /\ resource_a \in {0, 1, 2}
    /\ resource_b \in {0, 1, 2}
end define

process P1 = 1
begin
L1:
  resource_a := 1;   (* 获取资源A *)

L2:
  await resource_b = 0;  (* 等待资源B *)
  resource_b := 1;

L3:
  (* 释放资源 *)
  resource_a := 0;
  resource_b := 0;
end process;

process P2 = 2
begin
M1:
  resource_b := 2;   (* 获取资源B *)

M2:
  await resource_a = 0;  (* 等待资源A *)
  resource_a := 2;

M3:
  (* 释放资源 *)
  resource_b := 0;
  resource_a := 0;
end process;

end algorithm *)
====
```

### 4.3 死锁分析报告

```
TLC死锁检测输出:
=============================================================
Deadlock reached.
The behavior up to this point is:

State 1: <Initial predicate>
/\ resource_a = 0
/\ resource_b = 0
/\ pc = <<"L1", "M1">>

State 2: <P1 line 23, col 5 to line 23, col 21 of module DeadlockDemo>
/\ resource_a = 1
/\ resource_b = 0
/\ pc = <<"L2", "M1">>

State 3: <P2 line 35, col 5 to line 35, col 21 of module DeadlockDemo>
/\ resource_a = 1
/\ resource_b = 2
/\ pc = <<"L2", "M2">>

(* 死锁状态: P1等待resource_b=0，但P2持有它
             P2等待resource_a=0，但P1持有它 *)

解决方案:
1. 资源有序分配: 总是按相同顺序获取资源
2. 超时机制: 等待一段时间后放弃
3. 死锁检测与恢复: 检测循环等待并打破
=============================================================
```

### 4.4 预防死锁的设计模式

```tla
(* 资源有序分配策略 *)
---- MODULE NoDeadlock ----
EXTENDS Naturals, TLC

(* --algorithm OrderedResources
variables
  resources = [i \in 1..2 |-> 0];  (* 资源数组 *)

define
  (* 不变式: 无死锁 *)
  NoDeadlock ==
    ~(/\ resources[1] = 1 /\ resources[2] = 2
      /\ pc[1] = "WaitB" /\ pc[2] = "WaitA")
end define

process P1 = 1
begin
L1:
  resources[1] := 1;  (* 先获取资源1 *)
L2:
  await resources[2] = 0;
  resources[2] := 1;  (* 再获取资源2 *)
L3:
  resources := [i \in 1..2 |-> 0];
end process;

process P2 = 2
begin
M1:
  resources[1] := 2;  (* P2也先尝试获取资源1 *)
M2:
  await resources[2] = 0;
  resources[2] := 2;
M3:
  resources := [i \in 1..2 |-> 0];
end process;

end algorithm *)
====
```

---

## 5. 不变式验证

### 5.1 不变式的类型

```tla
(* ===== 类型不变式 ===== *)
(* 确保变量始终持有正确类型的值 *)
TypeInvariant ==
  /\ x \in Nat
  /\ y \in 1..100
  /\ buffer \in Seq(Data)

(* ===== 安全不变式 ===== *)
(* 确保系统不发生错误状态 *)
SafetyInvariant ==
  /\ counter >= 0           (* 计数器非负 *)
  /\ Len(buffer) <= MaxSize (* 缓冲区不溢出 *)
  /\ ~(flag[A] = TRUE /\ flag[B] = TRUE)  (* 互斥 *)

(* ===== 一致性不变式 ===== *)
(* 确保数据一致性 *)
ConsistencyInvariant ==
  /\ sum_parts = total
  /\ version_count >= committed_count
  /\ replication_factor >= min_replicas

(* ===== 业务逻辑不变式 ===== *)
BusinessInvariant ==
  /\ account_balance >= 0
  /\ transfer_amount <= source_balance
```

### 5.2 互斥锁验证案例

```tla
---- MODULE MutexVerification ----
EXTENDS Naturals, FiniteSets, TLC

CONSTANTS N
ASSUME N \in Nat /\ N > 0

Procs == 1..N

(* --algorithm Mutex
variables
  lock = 0;

define
  (* 类型不变式 *)
  TypeInvariant ==
    lock \in 0..N

  (* 关键安全不变式: 互斥 *)
  MutualExclusion ==
    Cardinality({i \in Procs : pc[i] = "CS"}) <= 1

  (* 辅助不变式 *)
  LockHolderInCS ==
    lock /= 0 => pc[lock] = "CS"

  LockConsistency ==
    (\E i \in Procs : pc[i] = "CS") => lock /= 0
end define

process Proc \in Procs
begin
L1:
  while TRUE do
L2:
    await lock = 0;
    lock := self;
CS:
    skip;
L3:
    lock := 0;
  end while;
end process;

end algorithm *)
====
```

### 5.3 复杂不变式验证

```tla
(* ===== 银行账户系统不变式 ===== *)
---- MODULE BankInvariant ----
EXTENDS Naturals, Sequences, TLC

CONSTANTS
  Accounts,
  MaxBalance

ASSUME MaxBalance \in Nat

(* --algorithm BankSystem
variables
  balances = [a \in Accounts |-> 100],  (* 初始余额 *)
  total_deposits = 100 * Cardinality(Accounts);

define
  (* 类型不变式 *)
  TypeInvariant ==
    /\ balances \in [Accounts -> 0..MaxBalance]
    /\ total_deposits \in Nat

  (* 资金安全: 总资金守恒 *)
  FundConservation ==
    total_deposits = Sum([a \in Accounts |-> balances[a]])

  (* 账户安全: 无透支 *)
  NoOverdraft ==
    \A a \in Accounts : balances[a] >= 0

  (* 转账安全: 余额充足 *)
  TransferSafety ==
    \A a \in Accounts :
      pc[a] = "CompleteTransfer" =>
        balances[a] >= transfer_amount[a]
end define

macro Transfer(from, to, amount) begin
  if balances[from] >= amount then
    balances[from] := balances[from] - amount;
    balances[to] := balances[to] + amount;
  end if;
end macro;

process Account \in Accounts
variables
  transfer_amount = 0,
  target \in Accounts
begin
Transfer:
  while TRUE do
    with amt \in 1..50, tgt \in Accounts \ {self} do
      transfer_amount := amt;
      target := tgt;
    end with;

CompleteTransfer:
    Transfer(self, target, transfer_amount);
  end while;
end process;

end algorithm *)
====
```

---

## 6. 时序性质验证

### 6.1 时序逻辑基础

```
TLA+时序运算符:
┌─────────────────┬────────────────────────────────────────┐
│ 运算符          │ 含义                                   │
├─────────────────┼────────────────────────────────────────┤
│ □F (Box)        │ F在所有未来状态为真                    │
│ ◇F (Diamond)    │ F在某个未来状态为真                    │
│ F ~> G (LeadsTo)│ F为真最终会导致G为真                   │
│ ○F (Next)       │ F在下一状态为真                        │
│ F U G (Until)   │ G最终为真，且此前F为真                 │
│ WF_v(A)         │ 弱公平性: 如果A持续可用则最终执行      │
│ SF_v(A)         │ 强公平性: 如果A无限次可用则无限次执行  │
└─────────────────┴────────────────────────────────────────┘

活性性质示例:
  □(request => ◇response)     (* 请求最终得到响应 *)
  □◇scheduled                 (* 任务无限次被调度 *)
  state = "start" ~> state = "end"  (* 开始最终结束 *)
```

### 6.2 活性验证示例

```tla
---- MODULE LivenessExample ----
EXTENDS Naturals, TLC

(* --algorithm LivenessDemo
variables
  request = FALSE,
  response = FALSE,
  count = 0;

define
  (* 安全性: 没有请求就没有响应 *)
  Safety ==
    response = TRUE => request = TRUE

  (* 活性: 请求最终得到响应 *)
  ResponseLiveness ==
    request = TRUE ~> response = TRUE

  (* 公平性假设 *)
  Fairness ==
    WF_vars(Responder)
end define

process Requester = "Requester"
begin
L1:
  while TRUE do
    request := TRUE;
L2:
    await response = TRUE;
    request := FALSE;
    response := FALSE;
  end while;
end process;

process Responder = "Responder"
begin
L3:
  while TRUE do
    await request = TRUE;
    response := TRUE;
  end while;
end process;

end algorithm *)
====
```

### 6.3 公平性约束

```tla
(* ===== 公平性类型 ===== *)

(* 弱公平性: 如果动作持续可用，则必须执行 *)
WF_vars(Action) ==
  □(ENABLED <<Action>>_vars => ◇<<Action>>_vars)

(* 强公平性: 如果动作无限次可用，则无限次执行 *)
SF_vars(Action) ==
  □◇ENABLED <<Action>>_vars => □◇<<Action>>_vars

(* ===== 在规范中使用 ===== *)
Spec ==
  Init
  /\ □[Next]_vars
  /\ WF_vars(Action1)   (* 弱公平性 *)
  /\ SF_vars(Action2)   (* 强公平性 *)
```

### 6.4 完整时序验证案例

```tla
---- MODULE DiningPhilosophers ----
EXTENDS Naturals, TLC

CONSTANTS N
Philosophers == 0..N-1
Left(i) == (i + 1) % N
Right(i) == (i + N - 1) % N

(* --algorithm DiningPhilosophers
variables
  forks = [i \in Philosophers |-> TRUE];  (* TRUE=可用 *)

define
  (* 类型不变式 *)
  TypeInvariant ==
    forks \in [Philosophers -> BOOLEAN]

  (* 安全: 相邻哲学家不能同时用餐 *)
  NoAdjacentEating ==
    \A i \in Philosophers :
      ~(pc[i] = "Eating" /\ pc[Left(i)] = "Eating")

  (* 安全: 用餐需要两把叉子 *)
  EatingHasBothForks ==
    \A i \in Philosophers :
      pc[i] = "Eating" =>
        forks[i] = FALSE /\ forks[Right(i)] = FALSE
end define

(* 需要公平性假设 *)
fair process Philosopher \in Philosophers
begin
Thinking:
  while TRUE do

PickLeft:
    await forks[self] = TRUE;
    forks[self] := FALSE;

PickRight:
    await forks[Right(self)] = TRUE;
    forks[Right(self)] := FALSE;

Eating:
    skip;

PutDown:
    forks[self] := TRUE;
    forks[Right(self)] := TRUE;
  end while;
end process;

end algorithm *)

(* 时序性质 *)
NoStarvation ==
  \A i \in Philosophers :
    pc[i] = "Thinking" ~> pc[i] = "Eating"
====
```

---

## 7. 实际案例分析

### 7.1 两阶段提交验证

```tla
---- MODULE TwoPhaseVerification ----
EXTENDS Naturals, FiniteSets, Sequences, TLC

CONSTANTS RM, TM
ASSUME RM /= {} /\ TM /= {}

(* --algorithm TwoPhase
variables
  rmState = [r \in RM |-> "working"],
  tmState = "init",
  msgs = {};

define
  (* 消息类型 *)
  PrepareMsg == [type : {"Prepared"}, rm : RM]
  CommitMsg == [type : {"Commit"}]
  AbortMsg == [type : {"Abort"}]

  (* 类型不变式 *)
  TypeInvariant ==
    /\ rmState \in [RM -> {"working", "prepared", "committed", "aborted"}]
    /\ tmState \in {"init", "committing", "committed", "aborted"}
    /\ msgs \subseteq PrepareMsg \union CommitMsg \union AbortMsg

  (* 安全: 不能同时提交和终止 *)
  Consistency ==
    ~\E r1, r2 \in RM :
      rmState[r1] = "committed" /\ rmState[r2] = "aborted"

  (* 安全: TM决定后的RM一致性 *)
  TMDecisionImpliesRM ==
    (tmState = "committed" => \A r \in RM : rmState[r] \in {"prepared", "committed"})
    /\ (tmState = "aborted" => \A r \in RM : rmState[r] \in {"working", "aborted"})
end define

macro Send(m) begin
  msgs := msgs \union {m};
end macro;

process ResourceManager \in RM
begin
RMPrepare:
  await rmState[self] = "working";
  rmState[self] := "prepared";
  Send([type |-> "Prepared", rm |-> self]);

RMCommit:
  await [type |-> "Commit"] \in msgs;
  rmState[self] := "committed";
  goto RMDone;

RMAbort:
  await [type |-> "Abort"] \in msgs;
  rmState[self] := "aborted";

RMDone:
  skip;
end process;

process TransactionManager = TM
variables
  preparedRMs = {};
begin
TMInit:
  while TRUE do
    either
      (* 收集Prepared消息 *)
      with r \in RM \ preparedRMs do
        await [type |-> "Prepared", rm |-> r] \in msgs;
        preparedRMs := preparedRMs \union {r};
      end with;
    or
      (* 决定提交 *)
      await preparedRMs = RM;
      tmState := "committed";
      Send([type |-> "Commit"]);
      goto TMDone;
    or
      (* 决定终止 *)
      await tmState = "init";
      tmState := "aborted";
      Send([type |-> "Abort"]);
      goto TMDone;
    end either;
  end while;

TMDone:
  skip;
end process;

end algorithm *)
====
```

### 7.2 验证结果分析

```
=============================================================
TLC验证报告: TwoPhaseCommit
=============================================================

验证的配置:
  - RM = {r1, r2, r3}
  - 最大搜索深度: 无限制
  - 工作线程: 4

发现的错误:
  ❌ 活性违反: TMCommitDecision
     描述: 事务管理器可能永远无法做出决定
     原因: 如果某个RM不发送Prepared消息

  反例轨迹:
    State 1: 初始状态
      rmState = [r1 |-> "working", r2 |-> "working", r3 |-> "working"]
      tmState = "init"

    State 2: r1发送Prepared
      rmState[r1] = "prepared"
      msgs包含Prepared(r1)

    State 3: r2发送Prepared
      rmState[r2] = "prepared"
      msgs包含Prepared(r1), Prepared(r2)

    [r3永远不发送Prepared]

    死锁: TM等待所有RM的Prepared

修复方案:
  1. 添加超时机制
  2. 假设所有RM最终都会响应 (SF)
  3. TM可以单方面决定终止
=============================================================
```

### 7.3 状态空间统计

```
=============================================================
TLC状态空间统计
=============================================================

模型: BakeryLock (N=3)
状态空间大小: 47个状态 (16个不同状态)
搜索深度: 6
生成速率: ~1000 states/sec

内存使用:
  - 指纹内存: 64MB
  - 堆内存: 1820MB
  - 平均每状态: ~38KB

覆盖率:
  - 状态覆盖率: 100%
  - 动作覆盖率: 100%
  - 分支覆盖率: 100%

对称性减少:
  - 原始状态数: 47
  - 减少后状态数: 16
  - 减少比例: 66%

时间统计:
  - 初始状态计算: 0.1s
  - 模型检验: 0.5s
  - 总时间: 0.6s
=============================================================
```

---

## 8. TLC高级技巧

### 8.1 调试技巧

```bash
# 生成反例轨迹
$ tlc Module.tla -dump dot,actionlabels,out.dot
$ dot -Tpng out.dot -o out.png

# 查看状态空间图
$ tlc Module.tla -dump json,out.json

# 从特定状态开始
$ tlc Module.tla -recover ID

# 限制状态数（用于调试）
$ tlc Module.tla -maxsetsize 10000
```

### 8.2 性能优化

```tla
(* 1. 使用VIEW减少状态 *)
View == <<var1, var2>>  (* 忽略不重要变量 *)

(* 2. 使用对称性 *)
Perms == Permutations(Procs)
(* cfg: SYMMETRY Perms *)

(* 3. 状态约束 *)
StateConstraint ==
  /\ x < MaxX
  /\ Len(queue) < MaxQueue

(* 4. 动作约束 *)
ActionConstraint ==
  count < MaxCount
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**关联文件**: 01_TLA_Introduction.md, 02_PlusCal_Language.md, 04_Distributed_Systems_Specs.md


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
