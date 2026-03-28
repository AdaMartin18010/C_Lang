# 图灵机：计算的理论模型

> **层级定位**: 21 Computation Theory / 基础层
> **前置知识**: 无（本层起点）
> **后续延伸**: [λ演算](./02_Lambda_Calculus.md) → [复杂性理论](./03_Computational_Complexity.md) → [理论到物理桥梁](./04_Theory_to_Physical_Bridge.md)
> **横向关联**: [数字逻辑基础](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/README.md)（实现层面）
> **预计学习时间**: 1周
> **实践验证**: [图灵机模拟器](../../examples/turing_machine/tm_simulator.py)

---

## 核心问题：什么是可计算的？

在深入物理实现之前，我们必须回答一个根本问题：**什么是可计算的？**

```
直觉理解：
- 计算 = 一系列明确的步骤，将输入转换为输出
- 可计算 = 存在这样的步骤序列
- 图灵机 = 描述这种步骤序列的数学模型

关键洞察：
图灵机不是一台真实的机器，而是一个数学定义。
但它告诉我们：任何可计算的问题，都可以用图灵机描述。
```

---

## 1. 图灵机的形式化定义

### 1.1 七元组定义

一个图灵机 $M$ 是一个七元组：

$$M = (Q, \Sigma, \Gamma, \delta, q_0, q_{accept}, q_{reject})$$

| 元素 | 含义 | 直观理解 |
|:-----|:-----|:---------|
| $Q$ | 有限状态集 | 机器的"心智状态" |
| $\Sigma$ | 输入字母表（不含空白符） | 允许输入的符号 |
| $\Gamma$ | 磁带字母表（$\Sigma \subset \Gamma$，含空白符 $\sqcup$） | 磁带上可以出现的所有符号 |
| $\delta$ | 转移函数 $Q \times \Gamma \rightarrow Q \times \Gamma \times \{L, R\}$ | "如果处于状态q，读到符号x，则..." |
| $q_0 \in Q$ | 起始状态 | 开始时的状态 |
| $q_{accept} \in Q$ | 接受状态 | 成功结束 |
| $q_{reject} \in Q$ | 拒绝状态 | 失败结束 |

### 1.2 转移函数的直观理解

```
δ(q, a) = (p, b, R) 的含义：

当前状态: q
读到符号: a
动作：
  1. 转移到新状态 p
  2. 在磁带当前位置写入 b（替换 a）
  3. 磁头向右移动一格 (R) / 向左移动一格 (L)
```

### 1.3 配置（Configuration）

图灵机的"快照"，描述某一时刻的完整状态：

$$u q v$$

其中：

- $u \in \Gamma^*$：磁头左侧的内容
- $q \in Q$：当前状态
- $v \in \Gamma^*$：磁头位置及右侧的内容

**示例配置**：$1011\ q_7\ 01101$

表示：

- 磁带内容为 ...101101101...
- 当前状态为 $q_7$
- 磁头指向第一个 0

---

## 2. 图灵机的工作原理

### 2.1 计算过程

```
开始配置: q₀w （w是输入字符串）

循环:
  1. 根据当前状态和磁头下的符号，查找δ
  2. 执行转移函数指定的动作
  3. 如果进入 q_accept → 接受（停机）
  4. 如果进入 q_reject → 拒绝（停机）
  5. 否则继续
```

### 2.2 判定 vs 识别

| 类型 | 定义 | 停机保证 | 示例 |
|:-----|:-----|:--------:|:-----|
| **判定器** (Decider) | 对所有输入都停机 | ✅ 总是停机 | 判断回文 |
| **识别器** (Recognizer) | 对接受输入停机，对拒绝输入可能不停机 | ❌ 可能不停机 | 判断程序是否会停机 |

**关键区别**：判定器必须在有限步内给出答案，识别器可以"永远思考"。

---

## 3. 手工模拟示例

### 3.1 示例1：判断二进制是否为0

**问题**：判断输入是否全是0

**图灵机设计**：

```
Q = {q₀, q₁, q_accept, q_reject}
Σ = {0, 1}
Γ = {0, 1, □}

转移函数 δ：
1. δ(q₀, 0) = (q₀, 0, R)  // 读到0，继续右移
2. δ(q₀, 1) = (q_reject, 1, R)  // 读到1，拒绝
3. δ(q₀, □) = (q_accept, □, R)  // 读到空白，接受（全是0）
```

**模拟执行**（输入 "000"）：

```
步骤0: q₀0 0 0 □
       ↑
步骤1: 0 q₀0 0 □
         ↑
步骤2: 0 0 q₀0 □
           ↑
步骤3: 0 0 0 q₀□
             ↑
步骤4: δ(q₀, □) → q_accept
结果: 接受 ✓
```

**模拟执行**（输入 "010"）：

```
步骤0: q₀0 1 0 □
       ↑
步骤1: 0 q₀1 0 □
         ↑
步骤2: δ(q₀, 1) → q_reject
结果: 拒绝 ✗
```

### 3.2 示例2：判断回文（简化版）

**问题**：判断二进制字符串是否为回文

**思路**：

1. 记住第一个符号（0或1），标记为X
2. 移动到最右端，检查是否匹配
3. 如果匹配，标记为X，返回左端
4. 重复直到所有符号都被标记

**关键转移**（伪代码）：

```
δ(q₀, 0) = (q_find_last_0, X, R)  // 记住是0，开始找最右的0
δ(q₀, 1) = (q_find_last_1, X, R)  // 记住是1，开始找最右的1
δ(q₀, X) = (q_accept, X, R)       // 全是X，接受

// 在找最后一个0的过程中...
δ(q_find_last_0, 0/1) = (q_find_last_0, 0/1, R)  // 继续右移
δ(q_find_last_0, □) = (q_check_0, □, L)          // 到末尾，左移检查
δ(q_check_0, 0) = (q_return, X, L)               // 是0，标记，返回
δ(q_check_0, 1) = (q_reject, 1, L)               // 是1，不匹配，拒绝
```

---

## 4. 图灵机模拟器（Python实现）

### 4.1 代码实现

```python
# examples/turing_machine/tm_simulator.py

class TuringMachine:
    """
    图灵机模拟器

    用法:
        tm = TuringMachine(transitions, start_state, accept_state, reject_state)
        result = tm.run("输入字符串")
    """

    def __init__(self, transitions, start_state, accept_state, reject_state):
        """
        transitions: dict, 键为 (state, symbol), 值为 (new_state, write_symbol, direction)
        start_state: 起始状态
        accept_state: 接受状态
        reject_state: 拒绝状态
        """
        self.transitions = transitions
        self.start_state = start_state
        self.accept_state = accept_state
        self.reject_state = reject_state
        self.blank = '□'

    def run(self, input_string, max_steps=1000, verbose=False):
        """
        运行图灵机

        返回: (结果, 步数, 最终磁带内容)
        结果: 'accept', 'reject', 或 'timeout'
        """
        # 初始化
        tape = list(input_string) if input_string else [self.blank]
        head = 0
        state = self.start_state
        steps = 0

        if verbose:
            print(f"初始: {self._config_str(tape, head, state)}")

        while steps < max_steps:
            # 确保磁头在磁带范围内
            if head < 0:
                tape.insert(0, self.blank)
                head = 0
            elif head >= len(tape):
                tape.append(self.blank)

            symbol = tape[head]
            key = (state, symbol)

            # 检查是否有转移
            if key not in self.transitions:
                # 默认拒绝
                if verbose:
                    print(f"步骤{steps}: 无转移，隐式拒绝")
                return ('reject', steps, ''.join(tape))

            new_state, write_symbol, direction = self.transitions[key]

            # 执行转移
            tape[head] = write_symbol
            state = new_state

            if direction == 'R':
                head += 1
            elif direction == 'L':
                head -= 1

            steps += 1

            if verbose:
                print(f"步骤{steps}: {self._config_str(tape, head, state)}")

            # 检查是否停机
            if state == self.accept_state:
                return ('accept', steps, ''.join(tape).strip(self.blank))
            elif state == self.reject_state:
                return ('reject', steps, ''.join(tape).strip(self.blank))

        return ('timeout', steps, ''.join(tape))

    def _config_str(self, tape, head, state):
        """生成配置字符串"""
        tape_str = ''.join(tape)
        pointer = ' ' * head + '↑'
        return f"{tape_str}\n{pointer}\n状态: {state}"


# ===== 预定义的图灵机 =====

def create_zero_checker():
    """创建判断全0的图灵机"""
    transitions = {
        ('q0', '0'): ('q0', '0', 'R'),
        ('q0', '1'): ('q_reject', '1', 'R'),
        ('q0', '□'): ('q_accept', '□', 'R'),
    }
    return TuringMachine(transitions, 'q0', 'q_accept', 'q_reject')


def create_palindrome_checker():
    """创建判断回文的图灵机（简化版）"""
    transitions = {
        # 开始状态：读取第一个符号并记住
        ('q0', '0'): ('q_find_0', 'X', 'R'),  # 是0，去找最右的0
        ('q0', '1'): ('q_find_1', 'X', 'R'),  # 是1，去找最右的1
        ('q0', 'X'): ('q_accept', 'X', 'R'),  # 全是X，接受
        ('q0', '□'): ('q_accept', '□', 'R'),  # 空输入，接受

        # 找最后一个0
        ('q_find_0', '0'): ('q_find_0', '0', 'R'),
        ('q_find_0', '1'): ('q_find_0', '1', 'R'),
        ('q_find_0', 'X'): ('q_find_0', 'X', 'R'),
        ('q_find_0', '□'): ('q_check_0', '□', 'L'),

        # 检查是否是0
        ('q_check_0', '0'): ('q_return', 'X', 'L'),
        ('q_check_0', '1'): ('q_reject', '1', 'L'),

        # 找最后一个1
        ('q_find_1', '0'): ('q_find_1', '0', 'R'),
        ('q_find_1', '1'): ('q_find_1', '1', 'R'),
        ('q_find_1', 'X'): ('q_find_1', 'X', 'R'),
        ('q_find_1', '□'): ('q_check_1', '□', 'L'),

        # 检查是否是1
        ('q_check_1', '0'): ('q_reject', '0', 'L'),
        ('q_check_1', '1'): ('q_return', 'X', 'L'),

        # 返回到最左边的非X符号
        ('q_return', '0'): ('q_return', '0', 'L'),
        ('q_return', '1'): ('q_return', '1', 'L'),
        ('q_return', 'X'): ('q_return', 'X', 'L'),
        ('q_return', '□'): ('q0', '□', 'R'),  # 回到q0继续
    }
    return TuringMachine(transitions, 'q0', 'q_accept', 'q_reject')


# ===== 测试 =====

if __name__ == "__main__":
    print("=" * 50)
    print("图灵机模拟器测试")
    print("=" * 50)

    # 测试全0检查器
    print("\n1. 全0检查器")
    print("-" * 30)
    tm = create_zero_checker()

    for test in ["000", "010", "", "111"]:
        result, steps, tape = tm.run(test, verbose=False)
        status = "✓ 接受" if result == 'accept' else "✗ 拒绝"
        print(f"  输入 '{test}': {status} ({steps}步)")

    # 详细演示
    print("\n  详细演示 '000':")
    tm.run("000", verbose=True)

    # 测试回文检查器
    print("\n2. 回文检查器")
    print("-" * 30)
    tm = create_palindrome_checker()

    for test in ["0110", "010", "1", "", "0101"]:
        result, steps, tape = tm.run(test, verbose=False)
        status = "✓ 接受" if result == 'accept' else "✗ 拒绝"
        print(f"  输入 '{test}': {status} ({steps}步)")
```

### 4.2 运行方法

```bash
# 在项目根目录执行
cd e:\_src\C_Lang
python examples/turing_machine/tm_simulator.py

# 预期输出:
# ==================================================
# 图灵机模拟器测试
# ==================================================
#
# 1. 全0检查器
# ------------------------------
#   输入 '000': ✓ 接受 (4步)
#   输入 '010': ✗ 拒绝 (2步)
#   ...
```

---

## 5. 与现有知识库的连接

### 5.1 向上连接（理论来源）

这是计算理论的起点，没有前置理论要求。

### 5.2 向下连接（物理实现）

这是最关键的部分！每个学习图灵机的人应该思考：

```
核心问题：图灵机如何映射到物理计算机？

映射关系：
┌─────────────────┬──────────────────────────────┐
│ 图灵机          │ 物理实现                       │
├─────────────────┼──────────────────────────────┤
│ 无限磁带         │ 有限内存 + 外部存储（虚拟无限）   │
│ 磁头            │ 内存地址寄存器 + 数据总线        │
│ 当前状态         │ CPU的状态寄存器                 │
│ 转移函数 δ       │ 控制单元 + 微码/硬连线逻辑       │
│ 读写操作         │ 内存读写周期                    │
│ 状态转换         │ 时钟驱动的状态机                 │
└─────────────────┴──────────────────────────────┘

关键洞察：
- 图灵机是数学抽象（无限磁带）
- 物理计算机是工程实现（有限资源 + 分层抽象）
- 两者在"可计算性"上是等价的（Church-Turing论题）
```

### 5.3 显式链接到现有文档

| 本文档概念 | 链接到现有内容 | 连接说明 |
|:-----------|:---------------|:---------|
| 图灵机状态 | [有限状态机](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/03_Memory_Circuits/01_SR_Latch_and_D_FlipFlop.md) | CPU状态由触发器实现 |
| 磁带存储 | [存储器层次](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/README.md#2-内存层次结构) | 内存层次模拟无限磁带 |
| 转移函数 | [控制单元](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/06_Control_Unit_Design/README.md) | δ函数由控制单元实现 |
| 计算能力 | [RISC-V CPU](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/README.md) | CPU是图灵完备的物理实现 |

---

## 6. Church-Turing论题

### 6.1 核心内容

> **Church-Turing论题**：任何在直觉上可计算的函数，都可以被图灵机计算。

这不是一个数学定理（无法证明），而是一个**计算范式**：

- 它定义了"可计算"的边界
- 所有已知的计算模型（λ演算、递归函数、随机存取机）都被证明与图灵机等价
- 物理学家认为：即使使用量子力学，计算能力也不会超越图灵机（只能加速）

### 6.2 物理版本（预告）

在 [理论到物理桥梁](./04_Theory_to_Physical_Bridge.md) 中，我们将探讨：

```
物理Church-Turing论题（Deutsch 1985）：

"任何有限物理系统都可以被有限资源的图灵机模拟到任意精度。"

这回答了一个深层问题：
为什么数学模型（图灵机）能描述物理世界（计算机）？
```

---

## 7. 停机问题（Halting Problem）

### 7.1 问题陈述

**停机问题**：给定一个程序P和输入w，判断P在w上是否会停机。

**结论**：不存在通用的图灵机能解决所有停机问题实例。

### 7.2 不可判定性证明（对角化法）

**假设**：存在判定停机问题的图灵机 $H$。

**构造**：

```
定义机器 D：
  输入：一个图灵机 M 的描述 <M>
  操作：
    1. 在 <M> 上运行 H
    2. 如果 H 接受（M在<M>上停机），则 D 无限循环
    3. 如果 H 拒绝（M在<M>上不停机），则 D 停机并接受
```

**矛盾**：

- 如果 $D$ 在 $<D>$ 上停机 → $H$ 会接受 → $D$ 会无限循环（矛盾！）
- 如果 $D$ 在 $<D>$ 上不停机 → $H$ 会拒绝 → $D$ 会停机（矛盾！）

**结论**：假设错误，$H$ 不存在。

### 7.3 实际意义

```
1. 不存在完美的程序分析器
   - 无法静态检测所有程序是否会死循环
   - 杀毒软件、静态分析工具都有固有局限

2. 编程中的体现
   - C语言中的停机问题是无法自动解决的
   - 这就是需要程序员干预的根本原因

3. 与物理层的联系
   - 物理计算机也会"卡住"（死锁、活锁）
   - 操作系统用看门狗定时器等工程方法处理
```

---

## 8. 学习检查点

完成本章后，您应该能够：

### 基础理解

- [ ] 用七元组描述一个图灵机
- [ ] 手工模拟简单图灵机的执行过程
- [ ] 解释判定器与识别器的区别

### 实践技能

- [ ] 运行图灵机模拟器，观察执行过程
- [ ] 修改模拟器实现新的图灵机（如二进制加法）
- [ ] 用Python实现一个简单的图灵机

### 深层理解

- [ ] 解释"图灵机为什么能代表所有计算"
- [ ] 描述停机问题的不可判定性及其含义
- [ ] 初步思考：图灵机如何映射到数字电路

### 连接建立

- [ ] 列出图灵机组件与CPU组件的对应关系
- [ ] 阅读 [数字逻辑基础](../02_Formal_Semantics_and_Physics/09_Physical_Machine_Layer/01_Digital_Logic_Foundations/README.md)，寻找实现图灵机的线索

---

## 9. 延伸阅读与资源

### 必读

- Sipser《计算理论导引》第3章
- "The Church-Turing Thesis" (Stanford Encyclopedia of Philosophy)

### 推荐视频

- Computerphile: "Turing Machines Explained"
- 3Blue1Brown: "Hilbert's Curve"（理解无限与计算的关系）

### 实践项目

- [ ] 用模拟器实现二进制加法图灵机
- [ ] 实现图灵机通用模拟器（模拟其他图灵机）
- [ ] 阅读 [RISC-V CPU实现](../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/08_RISCV_CPU_Complete/01_RV32I_Single_Cycle.md)，标记哪些组件对应图灵机的哪些部分

---

## 10. 下一步预告

### 下一章：[λ演算](./02_Lambda_Calculus.md)

我们将学习另一种计算模型，并证明它与图灵机的等价性。

**核心问题**：

- 函数抽象如何成为计算的基础？
- 没有变量赋值，如何实现递归？
- Y组合子是什么魔法？

---

> **学习笔记区**（供您个人记录）：
>
> _在此记录您的理解、疑问和洞察..._
>
> ---
>
> **关键洞察**:
>
> **未解疑问**:
>
> **与物理层的连接思考**:

---

_文档创建: 2026-03-29_
_状态: 完成，可立即使用_
_配套代码: [tm_simulator.py](../../examples/turing_machine/tm_simulator.py)_
