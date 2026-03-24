# 安全语义学

> **层级定位**: 02 Formal Semantics and Physics / 10 Lean4 Compilation
> **难度级别**: L6+++ (系统级元理论深度)
> **目标**: 建立程序安全性的语义学框架，包括信息流控制、内存安全和漏洞模型
> **符号标准**: 严格遵循 SEMANTIC_NOTATIONS.md

---

## 目录

- [安全语义学](#安全语义学)
  - [目录](#目录)
  - [1. 引言](#1-引言)
  - [2. 信息流控制](#2-信息流控制)
    - [2.1 安全等级](#21-安全等级)
    - [2.2 非干涉性](#22-非干涉性)
    - [2.3 类型系统](#23-类型系统)
    - [2.4 隐通道](#24-隐通道)
  - [3. 内存安全](#3-内存安全)
    - [3.1 缓冲区溢出](#31-缓冲区溢出)
    - [3.2 使用-after-free](#32-使用-after-free)
    - [3.3 空指针](#33-空指针)
    - [3.4 类型安全](#34-类型安全)
  - [4. 漏洞模型](#4-漏洞模型)
    - [4.1 漏洞分类](#41-漏洞分类)
    - [4.2 利用语义](#42-利用语义)
    - [4.3 缓解技术](#43-缓解技术)
    - [4.4 形式化定义](#44-形式化定义)
  - [5. 安全验证](#5-安全验证)
    - [5.1 安全属性](#51-安全属性)
    - [5.2 不变量验证](#52-不变量验证)
    - [5.3 污点分析](#53-污点分析)
    - [5.4 符号执行](#54-符号执行)
  - [6. 形式化定理与证明](#6-形式化定理与证明)
    - [定理1 (非干涉性)](#定理1-非干涉性)
    - [定理2 (类型安全蕴含内存安全)](#定理2-类型安全蕴含内存安全)
    - [定理3 (沙箱隔离)](#定理3-沙箱隔离)
    - [定理4 (栈保护有效性)](#定理4-栈保护有效性)
    - [定理5 (ASLR有效性)](#定理5-aslr有效性)
  - [7. Lean 4实现](#7-lean-4实现)
    - [示例1: 信息流类型系统](#示例1-信息流类型系统)
    - [示例2: 内存安全验证](#示例2-内存安全验证)
    - [示例3: 污点分析实现](#示例3-污点分析实现)
    - [示例4: 漏洞检测模型](#示例4-漏洞检测模型)
    - [示例5: 符号执行引擎](#示例5-符号执行引擎)
    - [示例6: 安全验证框架](#示例6-安全验证框架)
  - [8. 参考文献](#8-参考文献)
  - [9. 交叉引用](#9-交叉引用)
    - [符号标准](#符号标准)
    - [相关文档](#相关文档)
    - [概念依赖图](#概念依赖图)
    - [版本信息](#版本信息)

---

## 1. 引言

安全语义学是研究程序安全性属性的形式化理论框架。它为分析程序的信息流安全性、内存安全性和漏洞特征提供了严格的数学基础。

**核心问题**:

| 问题域 | 核心问题 | 语义方法 |
|:-----|:---------|:---------|
| **信息流控制** | 高安全数据是否泄露到低安全通道？ | 类型系统、非干涉性 |
| **内存安全** | 程序是否存在危险的内存操作？ | 操作语义、类型保持 |
| **漏洞分析** | 程序是否存在可被利用的弱点？ | 模型检测、符号执行 |

**形式化目标**:

1. 建立安全等级的形式化模型
2. 定义并证明非干涉性定理
3. 形式化常见漏洞的语义特征
4. 提供安全验证的技术基础

---

## 2. 信息流控制

### 2.1 安全等级

**定义 2.1.1** (安全格): 安全等级构成一个完全格 $(\mathcal{L}, \sqsubseteq, \sqcup, \sqcap, \bot, \top)$，其中：

- $\mathcal{L}$: 安全等级集合
- $\sqsubseteq$: 安全级别偏序（低 $\sqsubseteq$ 高）
- $\sqcup$: 最小上界（join，取更高安全级）
- $\sqcap$: 最大下界（meet）
- $\bot$: 最低安全级（公开，Public/Low）
- $\top$: 最高安全级（机密，Secret/High）

**标准安全格**:

```
        ⊤ (High/Secret)
       /   \
      /     \
   ClassA   ClassB
      \     /
       \   /
        ⊥ (Low/Public)
```

**赋值语义**:

$$\langle x := E, \sigma, \Gamma \rangle \to \langle \text{skip}, \sigma[x \mapsto \mathcal{E}⟦E⟧\sigma], \Gamma \rangle$$

其中 $\Gamma$ 是安全类型环境，$\Gamma(x) \in \mathcal{L}$ 表示变量 $x$ 的安全级别。

**安全级别约束**:

**定义 2.1.2** (安全赋值): 赋值 $x := E$ 是安全的，如果：

$$\text{level}(E) \sqsubseteq \Gamma(x)$$

其中 $\text{level}(E)$ 是表达式 $E$ 中所有变量的最小上界：

$$\text{level}(E) = \bigsqcup_{y \in \text{fv}(E)} \Gamma(y)$$

### 2.2 非干涉性

**定义 2.2.1** (低等价): 两个状态 $\sigma_1, \sigma_2$ 是低等价的（$\sigma_1 \approx_L \sigma_2$），如果它们在所有低安全级变量上取值相同：

$$\sigma_1 \approx_L \sigma_2 \iff \forall x. \Gamma(x) \sqsubseteq L \implies \sigma_1(x) = \sigma_2(x)$$

**定义 2.2.2** (非干涉性): 程序 $P$ 满足非干涉性，如果：

$$\forall \sigma_1, \sigma_2. \sigma_1 \approx_L \sigma_2 \implies \mathcal{C}⟦P⟧\sigma_1 \approx_L \mathcal{C}⟦P⟧\sigma_2$$

即：高安全输入的变化不会影响低安全输出。

**非干涉性的语义表述**:

$$
\frac{\sigma_1 \approx_L \sigma_2 \quad \langle C, \sigma_1 \rangle \Downarrow \sigma_1' \quad \langle C, \sigma_2 \rangle \Downarrow \sigma_2'}{\sigma_1' \approx_L \sigma_2'}
$$

### 2.3 类型系统

**安全类型判断**:

$$\Gamma \vdash M : \tau \text{ @ } \ell$$

表示在环境 $\Gamma$ 下，项 $M$ 具有类型 $\tau$ 和安全级别 $\ell$。

**类型规则**:

**变量 (VAR)**:
$$\frac{\Gamma(x) = \tau \text{ @ } \ell}{\Gamma \vdash x : \tau \text{ @ } \ell}$$

**常量 (CONST)**:
$$\frac{}{\Gamma \vdash c : \tau \text{ @ } \bot}$$

常量总是低安全级（公开）。

**赋值 (ASSIGN)**:
$$\frac{\Gamma \vdash E : \tau \text{ @ } \ell_E \quad \ell_E \sqsubseteq \Gamma(x)}{\Gamma \vdash x := E : \text{unit} \text{ @ } \bot}$$

**条件 (IF)**:
$$\frac{\Gamma \vdash B : \text{bool} \text{ @ } \ell_B \quad \Gamma \vdash C_1 : \tau \text{ @ } \ell_1 \quad \Gamma \vdash C_2 : \tau \text{ @ } \ell_2 \quad \ell_B \sqsubseteq \ell_1 \sqcap \ell_2}{\Gamma \vdash \text{if } B \text{ then } C_1 \text{ else } C_2 : \tau \text{ @ } \ell_1 \sqcup \ell_2}$$

条件表达式的安全级别是分支安全级别的上界。

**类型保持定理**:

**定理** (安全类型保持): 若 $\Gamma \vdash M : \tau \text{ @ } \ell$ 且 $M \to M'$，则 $\Gamma \vdash M' : \tau \text{ @ } \ell$。

### 2.4 隐通道

**定义 2.4.1** (隐通道): 隐通道是通过非直接数据流泄露信息的机制。

**存储通道**:
通过共享存储单元的状态传递信息：

```
High:  if secret then x := 1 else x := 0
Low:   if x = 1 then output "1" else output "0"
```

**时间通道**:
通过计算时间差异传递信息：

```
High:  if secret then busy_wait(1000) else skip
Low:   measure_time()
```

**终止通道**:
通过程序是否终止传递信息：

```
High:  if secret then while true do skip else skip
Low:   if program_terminates then ...
```

**概率通道**:
通过概率分布差异传递信息：

```
High:  if secret then random(0.9) else random(0.1)
Low:   observe_distribution()
```

---

## 3. 内存安全

### 3.1 缓冲区溢出

**定义 3.1.1** (缓冲区溢出): 缓冲区溢出发生在写入操作超出分配的内存边界时。

**操作语义**:

**溢出错误 (OVERFLOW)**:
$$\frac{H(a) = \langle \text{buf}, n, \vec{v} \rangle \quad i \geq n}{\langle \text{write}(a, i, v), H \rangle \to \text{error}_{\text{overflow}}}$$

**有效写入 (WRITE-OK)**:
$$\frac{H(a) = \langle \text{buf}, n, \vec{v} \rangle \quad i < n \quad \vec{v}' = \vec{v}[i \mapsto v]}{\langle \text{write}(a, i, v), H \rangle \to \langle \text{ok}, H[a \mapsto \langle \text{buf}, n, \vec{v}' \rangle] \rangle}$$

**堆缓冲区溢出**:

```c
char *buf = malloc(10);
buf[10] = 'A';  // 溢出！有效索引 0-9
```

**栈缓冲区溢出**:

```c
void func() {
    char buf[10];
    strcpy(buf, "AAAAAAAAAAA");  // 溢出！
}
```

**形式化安全条件**:

**定义 3.1.2** (缓冲区安全): 缓冲区访问 $\text{access}(a, i)$ 是安全的，如果：

$$\exists n. H(a) = \langle \text{buf}, n, \_ \rangle \land 0 \leq i < n$$

### 3.2 使用-after-free

**定义 3.2.1** (使用-after-free): 使用-after-free发生在程序释放内存后仍继续使用该内存。

**操作语义**:

**释放 (FREE)**:
$$\frac{a \in \text{dom}(H)}{\langle \text{free}(a), H \rangle \to \langle \text{ok}, H|_{\text{dom}(H) \setminus \{a\}} \rangle}$$

**非法访问 (USE-AFTER-FREE)**:
$$\frac{a \notin \text{dom}(H)}{\langle \text{read}(a, i), H \rangle \to \text{error}_{\text{uaf}}}$$

**悬空指针类型**:

```lean
/-- 指针状态 -/
inductive PtrStatus
  | valid       -- 指向有效内存
  | freed       -- 已释放（悬空）
  | null        -- 空指针
  deriving DecidableEq

structure Pointer where
  addr : Addr
  status : PtrStatus
```

**安全访问条件**:

$$\text{safe-deref}(p) \iff p.\text{status} = \text{valid} \land p.\text{addr} \in \text{dom}(H)$$

### 3.3 空指针

**定义 3.3.1** (空指针): 空指针是不指向任何有效内存的特殊指针值。

**操作语义**:

**空指针解引用 (NULL-DEREF)**:
$$\frac{p = \text{null}}{\langle \text{deref}(p), H \rangle \to \text{error}_{\text{null}}}$$

**C语言示例**:

```c
int *p = NULL;
int x = *p;  // 空指针解引用，未定义行为
```

**Lean中的安全处理**:

```lean
/-- 安全解引用 -/
def safeDeref {α : Type} (p : Option (Ref α)) : Option α :=
  match p with
  | none => none  -- 空指针
  | some r => r.get?  -- 可能已释放
```

### 3.4 类型安全

**定义 3.4.1** (类型安全): 程序是类型安全的，如果它不会违反类型系统的约束。

**进步定理 (Progress)**: 若 $\Gamma \vdash M : \tau$，则 $M$ 是值，或存在 $M'$ 使得 $M \to M'$。

**保持定理 (Preservation)**: 若 $\Gamma \vdash M : \tau$ 且 $M \to M'$，则 $\Gamma \vdash M' : \tau$。

**类型安全蕴含内存安全**:

**定理**: 良类型程序不会发生：

1. 缓冲区溢出
2. 使用-after-free
3. 空指针解引用

**证明概要**:

类型系统通过以下机制保证内存安全：

1. **数组类型**: `Array τ n` 确保索引 `i : Fin n`
2. **所有权类型**: 确保每块内存只有一个所有者
3. **生命周期**: 编译时验证引用有效性
4. **空指针**: `Option` 类型强制空检查

---

## 4. 漏洞模型

### 4.1 漏洞分类

**CWE/SANS Top 25漏洞**:

| 排名 | 漏洞类型 | 语义特征 | 风险等级 |
|:-----|:---------|:---------|:---------|
| 1 | 缓冲区溢出 | $i \geq n$ 时写入 | 严重 |
| 2 | 使用-after-free | $a \notin \text{dom}(H)$ 时访问 | 严重 |
| 3 | 整数溢出 | $x + y > \text{MAX}$ | 高 |
| 4 | 格式字符串 | 用户控制格式串 | 高 |
| 5 | 命令注入 | 拼接用户输入到命令 | 高 |

**漏洞语义模型**:

**定义 4.1.1** (漏洞状态): 程序状态 $(M, H, \mathcal{V})$ 包含漏洞配置 $\mathcal{V}$，记录潜在弱点。

$$\mathcal{V} = \{ (a, t, d) \mid \text{地址 } a \text{ 存在类型 } t \text{ 的漏洞，可利用性 } d \}$$

### 4.2 利用语义

**控制流劫持**:

**定义 4.2.1** (控制流劫持): 攻击者通过覆盖返回地址或函数指针改变程序控制流。

**栈溢出利用**:

```
[ 局部变量  ]  <- buf (10字节)
[ 填充      ]  <- 覆盖 (2字节)
[ 旧EBP     ]  <- 覆盖
[ 返回地址  ]  <- 覆盖为 shellcode_addr
[ 参数      ]
```

**形式化**:

$$\frac{H(\text{retAddr}) = v_{\text{old}} \quad \text{buf溢出覆盖retAddr}}{\langle \text{return}, H \rangle \to \langle \text{jump}(v_{\text{shellcode}}), H \rangle}$$

**ROP链 (Return-Oriented Programming)**:

利用现有代码片段（gadgets）构造攻击：

```
覆盖返回地址 → gadget1地址
              → gadget2地址
              → ...
              → system("/bin/sh")
```

### 4.3 缓解技术

**金丝雀值 (Stack Canaries)**:

在返回地址前插入随机值，检查是否被修改：

```
[ 局部变量  ]
[ 金丝雀    ]  <- 随机值
[ 旧EBP     ]
[ 返回地址  ]
```

**语义**:

$$\frac{\text{canary}_\text{stored} = \text{canary}_\text{expected}}{\langle \text{return}, S \rangle \to \text{正常返回}}$$

$$\frac{\text{canary}_\text{stored} \neq \text{canary}_\text{expected}}{\langle \text{return}, S \rangle \to \text{abort}}$$

**ASLR (地址空间布局随机化)**:

随机化内存布局增加利用难度：

$$\text{base}_\text{rand} = \text{base}_\text{orig} + \text{random_offset}$$

**NX (不可执行栈)**:

标记栈内存为不可执行：

$$\text{permission}(\text{stack}) = \text{READ} \mid \text{WRITE} \quad \text{（无EXECUTE）}$$

**RELRO (重定位只读)**:

在程序启动后使GOT表只读：

$$\text{permission}(\text{GOT}) = \text{READ} \quad \text{（启动后不可WRITE）}$$

### 4.4 形式化定义

**漏洞存在性**:

**定义 4.4.1** (漏洞存在): 程序 $P$ 存在漏洞 $v$，如果存在输入 $I$ 使得执行到达错误状态：

$$\exists I. \langle P, I \rangle \to^* \text{error}_v$$

**可利用性**:

**定义 4.4.2** (可利用性): 漏洞 $v$ 是可利用的，如果攻击者可以构造输入 $I$ 实现恶意目标 $G$：

$$\exists I. \langle P, I \rangle \to^* \sigma' \land \sigma' \vDash G$$

**漏洞度量**:

$$\text{severity}(v) = f(\text{exploitability}, \text{impact}, \text{prevalence})$$

---

## 5. 安全验证

### 5.1 安全属性

**安全性的时序逻辑表述**:

**安全性 (Safety)**: "坏事永不会发生"

$$\Box \neg \text{Bad}$$

**活性 (Liveness)**: "好事最终会发生"

$$\Diamond \text{Good}$$

**保密性 (Confidentiality)**: 非干涉性的时序逻辑表述：

$$\forall \pi_1, \pi_2. \pi_1 \approx_L \pi_2 \implies O(\pi_1) \approx_L O(\pi_2)$$

其中 $\pi$ 是执行路径，$O$ 是观察函数。

**完整性 (Integrity)**: 高完整性数据不会被低完整性操作修改：

$$\Box (\text{high-integrity}(x) \implies \text{modified-by-trusted}(x))$$

**可用性 (Availability)**: 服务始终可用：

$$\Box \text{service-available}$$

### 5.2 不变量验证

**定义 5.2.1** (安全不变量): 安全不变量 $I$ 是在所有可达状态上成立的条件：

$$\forall s \in \text{Reachable}. I(s)$$

**Hoare逻辑中的不变量**:

**循环不变量**:
$$\frac{\{I \land B\} C \{I\}}{\{I\} \text{ while } B \text{ do } C \{I \land \neg B\}}$$

**示例：数组访问安全不变量**:

```c
// 不变量: 0 <= i && i < n
for (int i = 0; i < n; i++) {
    // {0 <= i && i < n}
    arr[i] = 0;
}
```

**不变量生成**:

自动发现程序不变量的方法：

1. **抽象解释**: 通过抽象域计算不动点
2. **约束求解**: 使用SMT求解器验证候选不变量
3. **机器学习**: 从示例中推断不变量模式

### 5.3 污点分析

**定义 5.3.1** (污点分析): 污点分析追踪不可信输入（污点源）在程序中的传播，确保它们不会到达敏感位置（污点汇）。

**污点语义**:

**污点标签**:
$$\text{taint}(x) \in \{\text{clean}, \text{tainted}\}$$

**传播规则**:

**赋值传播**:
$$\frac{\text{taint}(E) = \text{tainted}}{\langle x := E, \sigma \rangle \to \sigma[x \mapsto v, \text{taint}(x) \mapsto \text{tainted}]}$$

**净化 (Sanitization)**:
$$\frac{}{\langle x := \text{sanitize}(E), \sigma \rangle \to \sigma[x \mapsto v, \text{taint}(x) \mapsto \text{clean}]}$$

**污点汇检查**:

$$\frac{\text{taint}(E) = \text{tainted} \quad \text{sink}(x := E)}{\text{alarm: tainted data reaches sink}}$$

**示例**:

```c
// 污点源: 用户输入
char *user_input = read_from_network();

// 传播
char query[100];
strcpy(query, user_input);  // query 被污染

// 污点汇: SQL执行
execute_sql(query);  // 警报！污点到达SQL执行
```

### 5.4 符号执行

**定义 5.4.1** (符号执行): 符号执行使用符号值代替具体值执行程序，收集路径约束。

**符号状态**:

$$\sigma^s = (\text{pc}, \Pi, \mathcal{M})$$

- $\text{pc}$: 程序计数器
- $\Pi$: 路径约束（布尔公式）
- $\mathcal{M}$: 符号内存映射

**符号执行语义**:

**符号赋值**:
$$\langle x := E, (\text{pc}, \Pi, \mathcal{M}) \rangle \to (\text{pc}+1, \Pi, \mathcal{M}[x \mapsto \mathcal{E}^s⟦E⟧\mathcal{M}])$$

**条件分支**:

$$\frac{\langle \text{if } B \text{ then } C_1 \text{ else } C_2, \sigma^s \rangle \quad \Pi_1 = \Pi \land \mathcal{E}^s⟦B⟧ \quad \Pi_2 = \Pi \land \neg\mathcal{E}^s⟦B⟧}{\langle C_1, (\text{pc}_1, \Pi_1, \mathcal{M}) \rangle \mid \langle C_2, (\text{pc}_2, \Pi_2, \mathcal{M}) \rangle}$$

**约束求解**:

使用SMT求解器检查路径可行性：

$$\text{SAT}(\Pi) \iff \exists \text{具体输入满足路径约束}$$

**漏洞检测**:

$$\frac{\Pi \land \text{vuln-condition} \text{ is SAT}}{\text{发现漏洞，生成触发输入}}$$

---

## 6. 形式化定理与证明

### 定理1 (非干涉性)

**定理**: 高安全输入不影响低安全输出。

**形式化**: 设 $C$ 是符合安全类型系统的命令，则 $C$ 满足非干涉性：

$$\forall \sigma_1, \sigma_2. \sigma_1 \approx_L \sigma_2 \implies \mathcal{C}⟦C⟧\sigma_1 \approx_L \mathcal{C}⟦C⟧\sigma_2$$

**证明**:

**步骤1**: 定义低等价关系。

$$\sigma_1 \approx_L \sigma_2 \iff \forall x. \Gamma(x) \sqsubseteq L \implies \sigma_1(x) = \sigma_2(x)$$

**步骤2**: 对命令结构进行归纳。

**Case 赋值** $x := E$:

假设 $\sigma_1 \approx_L \sigma_2$。

根据安全类型规则，$\text{level}(E) \sqsubseteq \Gamma(x)$。

- 若 $\Gamma(x) \sqsubseteq L$：则 $\text{level}(E) \sqsubseteq L$，$E$ 只依赖低安全级变量。
  由归纳假设，$\mathcal{E}⟦E⟧\sigma_1 = \mathcal{E}⟦E⟧\sigma_2$。
  因此 $\sigma_1[x \mapsto v] \approx_L \sigma_2[x \mapsto v]$。

- 若 $\Gamma(x) \not\sqsubseteq L$：高安全级变量变化不影响低等价。

**Case 条件** $\text{if } B \text{ then } C_1 \text{ else } C_2$:

根据类型规则，$\text{level}(B) \sqsubseteq \text{level}(C_1) \sqcap \text{level}(C_2)$。

- 若 $\text{level}(B) \sqsubseteq L$：$B$ 取值相同，选择相同分支。
- 若 $\text{level}(B) \not\sqsubseteq L$：分支可能不同，但输出都是高安全级。

由归纳假设，两种情况下低等价保持。

**步骤3**: 组合所有情况，非干涉性成立。
∎

### 定理2 (类型安全蕴含内存安全)

**定理**: 良类型程序无缓冲区溢出。

**形式化**: 若 $\Gamma \vdash M : \tau$ 且 $\tau$ 包含数组类型约束，则执行中不会发生缓冲区溢出。

**证明**:

**步骤1**: 数组类型确保边界信息。

对于数组访问 $a[i]$，类型系统要求 $i : \text{Fin}(n)$，其中 $n$ 是数组长度。

**步骤2**: $\text{Fin}(n)$ 类型保证 $0 \leq i < n$。

由构造，$\text{Fin}(n) = \{ k : \mathbb{N} \mid k < n \}$。

**步骤3**: 操作语义检查。

数组访问规则：
$$\frac{H(a) = \langle \text{arr}, n, \vec{v} \rangle \quad i : \text{Fin}(n)}{\langle a[i], H \rangle \to \langle v_i, H \rangle}$$

若 $i \geq n$，则 $i$ 无法构造为 $\text{Fin}(n)$ 类型，程序无法通过类型检查。

**步骤4**: 由类型保持定理，归约保持类型。

因此所有数组访问都满足边界检查。
∎

### 定理3 (沙箱隔离)

**定理**: 沙箱化代码不能访问外部资源。

**形式化**: 设 $\text{Sandbox}(C)$ 是将命令 $C$ 沙箱化的操作，则：

$$\forall \sigma. \text{resources-accessed}(\mathcal{C}⟦\text{Sandbox}(C)⟧\sigma) \subseteq \text{sandbox-resources}$$

**证明**:

**步骤1**: 定义沙箱语义。

沙箱通过以下机制限制访问：

- 内存隔离：独立的地址空间 $H_{\text{sandbox}}$
- 系统调用过滤：允许列表 $S_{\text{allowed}}$
- 资源配额：限制CPU/内存使用

**步骤2**: 内存隔离保证。

$$\frac{\text{addr} \notin \text{dom}(H_{\text{sandbox}})}{\langle \text{access}(\text{addr}), H_{\text{sandbox}} \rangle \to \text{error}_{\text{isolation}}}$$

**步骤3**: 系统调用过滤。

$$\frac{\text{syscall} \notin S_{\text{allowed}}}{\langle \text{syscall}, C \rangle \to \text{error}_{\text{forbidden}}}$$

**步骤4**: 因此沙箱化代码只能访问沙箱内资源。
∎

### 定理4 (栈保护有效性)

**定理**: 金丝雀值检测栈溢出。

**形式化**: 若栈溢出覆盖返回地址，则金丝雀值必定被修改。

$$\text{overflow}(\text{buf}) \land \text{buf} < \text{canary} < \text{retAddr} \implies \text{canary-modified}$$

**证明**:

**步骤1**: 栈布局分析。

```
高地址
--------
retAddr    <- 目标覆盖地址
canary     <- 保护值
buf[n]     <- 缓冲区
--------
低地址
```

**步骤2**: 溢出条件。

缓冲区溢出需要写入超过 $n$ 字节：
$$\text{write}(\text{buf}, i, v) \text{ where } i \geq n$$

**步骤3**: 覆盖顺序。

由于栈向低地址增长，向高地址的溢出先覆盖金丝雀：
$$\text{overflow}(n + \text{sizeof}(\text{canary})) \implies \text{canary-overwritten}$$

**步骤4**: 检测机制。

返回前检查：
$$\frac{\text{canary} \neq \text{canary}_0}{\text{abort}()}$$

因此任何覆盖返回地址的溢出都会被检测。
∎

### 定理5 (ASLR有效性)

**定理**: 地址空间随机化增加利用难度。

**形式化**: 设 $P_{\text{success}}$ 是成功利用的概率，$N$ 是随机化熵（位数），则：

$$P_{\text{success}} \leq \frac{1}{2^N} + P_{\text{info-leak}}$$

**证明**:

**步骤1**: 地址空间布局。

ASLR随机化以下基址：

- 栈基址：$\text{stack}_\text{base} + R_1$
- 堆基址：$\text{heap}_\text{base} + R_2$
- 库基址：$\text{lib}_\text{base} + R_3$

其中 $R_i \in [0, 2^N)$。

**步骤2**: 攻击模型。

成功利用需要知道目标地址 $A$：

- 覆盖返回地址为 $A$
- 或跳转至 $A$

**步骤3**: 随机化效果。

无信息泄露时，猜测 $A$ 的概率：
$$P(\text{guess}) = \frac{1}{2^N}$$

**步骤4**: 信息泄露影响。

若存在信息泄露漏洞泄露地址，攻击者可以计算 $A$：
$$P_{\text{success}} = P_{\text{info-leak}} + (1 - P_{\text{info-leak}}) \cdot \frac{1}{2^N}$$

因此ASLR显著降低利用概率，除非存在信息泄露。
∎

---

## 7. Lean 4实现

### 示例1: 信息流类型系统

```lean4
-- 安全等级定义
namespace InfoFlow

/-- 安全等级 -/
inductive SecurityLevel
  | low      -- 公开信息
  | medium   -- 内部信息
  | high     -- 机密信息
  deriving DecidableEq, BEq

/-- 安全格结构 -/
def SecurityLevel.leq : SecurityLevel → SecurityLevel → Bool
  | low, _ => true
  | medium, low => false
  | medium, _ => true
  | high, high => true
  | high, _ => false

/-- 最小上界 -/
def SecurityLevel.join : SecurityLevel → SecurityLevel → SecurityLevel
  | low, l => l
  | l, low => l
  | medium, _ => high
  | high, _ => high

/-- 带安全类型的值 -/
structure SecValue (α : Type) where
  value : α
  level : SecurityLevel
  deriving Inhabited

/-- 安全类型环境 -/
abbrev SecEnv (α : Type) := String → SecurityLevel

/-- 表达式安全级别 -/
def exprLevel {α} (env : SecEnv α) (vars : List String) : SecurityLevel :=
  vars.foldl (fun acc v => acc.join (env v)) SecurityLevel.low

/-- 安全赋值检查 -/
def secureAssign {α} (env : SecEnv α) (x : String) (vars : List String) : Bool :=
  exprLevel env vars |>.leq (env x)

/-- 非干涉性: 低等价定义 -/
def lowEquiv {α} [BEq α] (env : SecEnv α) (σ₁ σ₂ : String → α) : Prop :=
  ∀ x, (env x).leq SecurityLevel.low → σ₁ x = σ₂ x

end InfoFlow
```

### 示例2: 内存安全验证

```lean4
-- 内存安全的形式化验证
namespace MemorySafety

/-- 地址 -/
abbrev Addr := Nat

/-- 内存对象 -/
structure MemObj where
  size : Nat
  data : Fin size → Nat
  deriving Inhabited

/-- 堆内存 -/
abbrev Heap := Addr → Option MemObj

/-- 有效地址检查 -/
def validAddr (h : Heap) (a : Addr) (offset : Nat) : Bool :=
  match h a with
  | some obj => offset < obj.size
  | none => false

/-- 安全内存读取 -/
def safeRead (h : Heap) (a : Addr) (i : Nat) : Option Nat :=
  if validAddr h a i then
    match h a with
    | some obj =>
        if h : i < obj.size then
          some (obj.data ⟨i, h⟩)
        else
          none
    | none => none
  else
    none

/-- 安全内存写入 -/
def safeWrite (h : Heap) (a : Addr) (i : Nat) (v : Nat) : Heap :=
  if validAddr h a i then
    match h a with
    | some obj =>
        if h : i < obj.size then
          let newData := fun j =>
            if j.val = i then v else obj.data j
          fun a' => if a' = a then some { obj with data := newData } else h a'
        else
          h
    | none => h
  else
    h

/-- 缓冲区溢出检查定理 -/
theorem no_buffer_overflow_read (h : Heap) (a : Addr) (i : Nat) :
    validAddr h a i → ∃ v, safeRead h a i = some v := by
  intro hvalid
  simp [safeRead, hvalid]
  split <;> simp_all [validAddr]
  split_ifs <;> simp_all

end MemorySafety
```

### 示例3: 污点分析实现

```lean4
-- 污点分析实现
namespace TaintAnalysis

/-- 污点状态 -/
inductive TaintStatus
  | clean     -- 可信数据
  | tainted   -- 不可信数据
  deriving DecidableEq, BEq

/-- 带污点标记的值 -/
structure TaintValue (α : Type) where
  value : α
  taint : TaintStatus
  deriving Inhabited

/-- 污点源标记 -/
def source (v : α) : TaintValue α :=
  { value := v, taint := .tainted }

/-- 净化函数 -/
def sanitize (tv : TaintValue α) : TaintValue α :=
  { tv with taint := .clean }

/-- 污点传播 -/
def taintJoin {α β : Type} (op : α → β → γ)
    (tv1 : TaintValue α) (tv2 : TaintValue β) : TaintValue γ :=
  let newTaint :=
    if tv1.taint == .tainted || tv2.taint == .tainted then
      .tainted
    else
      .clean
  { value := op tv1.value tv2.value, taint := newTaint }

/-- 污点检查 -/
def checkSink {α} (tv : TaintValue α) (sinkName : String) : Option String :=
  match tv.taint with
  | .tainted => some s!"Security Alert: Tainted data reaches sink {sinkName}"
  | .clean => none

/-- SQL注入检测示例 -/
inductive SQLQuery
  | literal (s : String)
  | concat (q1 q2 : SQLQuery)

/-- 安全SQL构造 -/
def safeSQL (userInput : TaintValue String) : Option SQLQuery :=
  match checkSink userInput "SQL" with
  | some warning =>
      dbg_trace warning
      none  -- 拒绝污点输入
  | none =>
      some (SQLQuery.literal (sanitize userInput).value)

end TaintAnalysis
```

### 示例4: 漏洞检测模型

```lean4
-- 漏洞形式化模型
namespace VulnerabilityModel

/-- 漏洞类型 -/
inductive VulnType
  | bufferOverflow
  | useAfterFree
  | nullDeref
  | integerOverflow
  | formatString
  deriving DecidableEq, BEq

/-- 严重程度 -/
inductive Severity
  | low
  | medium
  | high
  | critical
  deriving DecidableEq, Ord

/-- 漏洞实例 -/
structure Vulnerability where
  vulnType : VulnType
  location : String
  severity : Severity
  description : String
  deriving Inhabited

/-- 程序状态 -/
structure ProgramState where
  pc : Nat        -- 程序计数器
  heap : Nat → Option Nat
  stack : List Nat
  freed : List Nat  -- 已释放地址

/-- 漏洞检测谓词 -/
def detectUseAfterFree (s : ProgramState) (addr : Nat) : Bool :=
  addr ∈ s.freed

/-- 缓冲区溢出检测 -/
def detectBufferOverflow (size : Nat) (index : Nat) : Option VulnType :=
  if index >= size then
    some .bufferOverflow
  else
    none

/-- 漏洞利用条件 -/
def isExploitable (v : Vulnerability) : Bool :=
  match v.vulnType, v.severity with
  | .bufferOverflow, .critical => true
  | .useAfterFree, .high => true
  | .nullDeref, .high => true
  | _, _ => false

/-- 缓解技术效果 -/
inductive Mitigation
  | canary
  | aslr
  | nx
  | relro

/-- 缓解有效性子句 -/
def mitigationEffective (v : VulnType) (m : Mitigation) : Bool :=
  match v, m with
  | .bufferOverflow, .canary => true
  | .bufferOverflow, .aslr => true
  | .bufferOverflow, .nx => true
  | _, _ => false

end VulnerabilityModel
```

### 示例5: 符号执行引擎

```lean4
-- 符号执行基础
namespace SymbolicExecution

/-- 符号值 -/
inductive SymValue
  | concrete (n : Nat)
  | symbolic (id : Nat)
  | add (v1 v2 : SymValue)
  | lt (v1 v2 : SymValue)
  deriving DecidableEq

/-- 路径约束 -/
abbrev PathConstraint := List SymValue

/-- 符号状态 -/
structure SymState where
  pc : Nat                    -- 程序计数器
  constraints : PathConstraint
  memory : Nat → SymValue

/-- 约束求解结果 -/
inductive SolveResult
  | sat (model : Nat → Nat)   -- 可满足，给出模型
  | unsat                     -- 不可满足
  | unknown

/-- 符号执行步骤 -/
inductive SymStep : SymState → SymState → Prop
  | assign {s x v} :
      SymStep s { s with
        pc := s.pc + 1,
        memory := fun addr => if addr = x then v else s.memory addr
      }
  | branchTrue {s cond} :
      SymStep s { s with
        pc := s.pc + 1,
        constraints := cond :: s.constraints
      }
  | branchFalse {s cond} :
      SymStep s { s with
        pc := s.pc + 1,
        constraints := SymValue.lt cond (SymValue.concrete 1) :: s.constraints
      }

/-- 漏洞条件检查 -/
def checkVulnCondition (s : SymState) (condition : SymValue) : Prop :=
  -- 检查约束是否满足漏洞条件
  ∃ model, condition = SymValue.concrete 1

end SymbolicExecution
```

### 示例6: 安全验证框架

```lean4
-- 安全属性验证框架
namespace SecurityVerification

/-- 安全属性 -/
inductive SecurityProperty (State : Type)
  | confidentiality (pred : State → State → Prop)
  | integrity (pred : State → Prop)
  | availability (pred : State → Prop)
  | safety (inv : State → Prop)

/-- 程序执行关系 -/
def Execution (State Input : Type) := State → Input → State

/-- 非干涉性验证 -/
def verifyNonInterference {State Input : Type}
    (execute : Execution State Input)
    (lowEquiv : State → State → Prop)
    (getLowOutput : State → Output)
    (inputs : List Input) : Prop :=
  ∀ s₁ s₂, lowEquiv s₁ s₂ →
    ∀ inp, getLowOutput (execute s₁ inp) = getLowOutput (execute s₂ inp)

/-- 不变量验证 -/
def verifyInvariant {State : Type}
    (init : State)
    (step : State → State)
    (inv : State → Prop) : Prop :=
  inv init ∧ ∀ s, inv s → inv (step s)

/-- Hoare三元组 -/
structure HoareTriple (State : Type) where
  pre : State → Prop
  cmd : State → State
  post : State → Prop

/-- Hoare逻辑有效 -/
def hoareValid {State} (t : HoareTriple State) : Prop :=
  ∀ s, t.pre s → t.post (t.cmd s)

/-- 最弱前置条件 -/
def weakestPrecondition {State} (cmd : State → State) (post : State → Prop) :
    State → Prop :=
  fun s => post (cmd s)

/-- 安全性定理: 若前置条件蕴含WP，则Hoare三元组有效 -/
theorem hoare_soundness {State} (t : HoareTriple State) :
    (∀ s, t.pre s → weakestPrecondition t.cmd t.post s) →
    hoareValid t := by
  intro h s hpre
  exact h s hpre

/-- 安全边界条件 -/
inductive SecurityBoundary
  | trustZone
  | sandbox
  | privilegeRing (level : Nat)
  | network

/-- 边界穿越检查 -/
def boundaryCheck (src dst : SecurityBoundary) (data : α) : Bool :=
  match src, dst with
  | .trustZone, .sandbox => false  -- 敏感数据不出域
  | .sandbox, .trustZone => true    -- 但可入域
  | .privilegeRing n, .privilegeRing m => n ≤ m  -- 特权不降级
  | _, _ => true

end SecurityVerification
```

---

## 8. 参考文献

1. Denning, D. E. (1976). A lattice model of secure information flow. *Communications of the ACM*, 19(5), 236-243.

2. Goguen, J. A., & Meseguer, J. (1982). Security policies and security models. *IEEE Symposium on Security and Privacy*, 11-20.

3. Volpano, D., Irvine, C., & Smith, G. (1996). A sound type system for secure flow analysis. *Journal of Computer Security*, 4(2-3), 167-187.

4. One, A. (1996). Smashing The Stack For Fun And Profit. *Phrack Magazine*, 7(49).

5. Shacham, H. (2007). The geometry of innocent flesh on the bone: Return-into-libc without function calls (on the x86). *ACM CCS*, 552-561.

6. Checkoway, S., et al. (2010). Return-oriented programming without returns. *ACM CCS*, 559-572.

7. King, J. C. (1976). Symbolic execution and program testing. *Communications of the ACM*, 19(7), 385-394.

8. Cadar, C., & Sen, K. (2013). Symbolic execution for software testing: three decades later. *Communications of the ACM*, 56(2), 82-90.

9. CWE/SANS Top 25 Most Dangerous Software Errors (2023). *MITRE Corporation*.

10. OWASP Top 10 (2021). *Open Web Application Security Project*.

---

## 9. 交叉引用

### 符号标准

- **SEMANTIC_NOTATIONS.md** - 本文档严格遵循的符号标准
  - 操作语义符号 (→, ⇓, ⟨M, σ⟩)
  - 指称语义符号 (⟦M⟧, ρ, σ)
  - 类型系统符号 (Γ ⊢ M : τ)
  - 公理语义符号 ({P} C {Q})

### 相关文档

| 文档 | 关系 | 说明 |
|:-----|:-----|:-----|
| **EDGE_CASES.md** | 扩展 | 安全边界情况分析，包括内存越界、类型错误等 |
| **VERIFICATION_WORKSHOP.md** | 应用 | 形式化验证实践，可用于验证安全属性 |
| **RUNTIME_SEMANTICS.md** | 基础 | 运行时系统语义，为内存安全提供基础 |
| **SEMANTIC_MODELS_UNIFICATION.md** | 理论 | 语义模型统一框架 |

### 概念依赖图

```
安全语义学
├── 信息流控制
│   ├── 安全等级 [依赖: 格论]
│   ├── 非干涉性 [依赖: 双模拟]
│   └── 隐通道分析 [依赖: 信息流类型]
├── 内存安全
│   ├── 缓冲区溢出 [依赖: 数组语义]
│   ├── 使用-after-free [依赖: 堆语义]
│   └── 类型安全保证 [依赖: 类型保持]
├── 漏洞模型
│   ├── 漏洞分类 [依赖: CWE标准]
│   ├── 利用语义 [依赖: 控制流语义]
│   └── 缓解技术 [依赖: 运行时保护]
└── 安全验证
    ├── 污点分析 [依赖: 数据流分析]
    └── 符号执行 [依赖: 约束求解]
```

### 版本信息

| 版本 | 日期 | 变更 | 作者 |
|:-----|:-----|:-----|:-----|
| 1.0 | 2026-03-24 | 初始版本 | 项目团队 |

---

**文档状态**: 完成
**质量等级**: L6+++
**大小**: ~15 KB
**定理数量**: 5
**Lean代码片段**: 6
