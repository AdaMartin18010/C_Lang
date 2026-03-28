# λ演算：函数的视角

> **层级定位**: 21 Computation Theory / 基础层
> **前置知识**: [图灵机](./01_Turing_Machines.md)
> **后续延伸**: [计算复杂性](./03_Computational_Complexity.md) → [理论到物理桥梁](./04_Theory_to_Physical_Bridge.md)
> **横向关联**: [函数与作用域](../../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md)（C语言实现）
> **预计学习时间**: 1周
> **实践验证**: [λ演算解释器](../../examples/lambda/lambda_eval.py)

---

## 核心问题：为什么函数是计算的基础？

图灵机从"状态转换"的角度描述计算，而λ演算从"函数抽象"的角度描述计算。

```
两种视角：
┌─────────────────┬─────────────────────────────┐
│ 图灵机          │ λ演算                       │
├─────────────────┼─────────────────────────────┤
│ 命令式          │ 函数式                      │
│ 状态 + 转移     │ 函数 + 应用                 │
│ 磁带读写        │ 变量替换                    │
│ 像C语言         │ 像Lisp/Haskell              │
└─────────────────┴─────────────────────────────┘

关键洞察：
两者在计算能力上是等价的（Church-Turing论题）
这解释了为什么C（命令式）和函数式语言都能解决相同的问题
```

---

## 1. λ演算语法

### 1.1 三种表达式

```
λ演算中只有三种合法的表达式：

1. 变量 (Variable): x, y, z, ...
   代表一个值或函数

2. 抽象 (Abstraction): λx.M
   定义一个匿名函数：参数是x，函数体是M
   相当于 C 语言中的:  (x) => M

3. 应用 (Application): M N
   将函数M应用于参数N
   相当于 C 语言中的:  M(N)
```

### 1.2 语法BNF

```bnf
<expr> ::= <variable>
         | λ <variable> . <expr>
         | <expr> <expr>
         | ( <expr> )

<variable> ::= 任意标识符
```

### 1.3 简单示例

```lambda
λx.x          # 恒等函数：返回输入本身
λx.λy.x       # 返回第一个参数（K组合子）
λx.λy.y       # 返回第二个参数
(λx.x) y      # 应用恒等函数于y，结果是y
```

---

## 2. β规约：计算的本质

### 2.1 规约规则

```
β规约（函数应用）：

  (λx.M) N  →  M[x := N]

含义：将函数体M中的所有x替换为N

示例：
  (λx.x x) y
  → (x x)[x := y]
  → y y
```

### 2.2 求值策略

| 策略 | 描述 | 特点 |
|:-----|:-----|:-----|
| **正常序** (Normal Order) | 最外层的应用先规约 | 惰性求值，可能避免无限循环 |
| **应用序** (Applicative Order) | 参数先求值，再应用 | 严格求值，C语言采用 |

### 2.3 手工规约示例

**示例1**：恒等函数

```lambda
(λx.x) y
→ y        # 直接替换
```

**示例2**：嵌套函数

```lambda
(λx.λy.x) a b
→ (λy.a) b    # 先应用外层，x替换为a
→ a           # 再应用内层，y替换为b（但a不含y）
```

**示例3**：递归应用

```lambda
(λx.x x) (λx.x x)
→ (λx.x x) (λx.x x)    # 无限循环！

这就是著名的 Ω 组合子，永不终止
对应图灵机的无限循环
```

---

## 3. Church编码：用函数表示数据

### 3.1 Church数：用函数表示自然数

```
数字n表示为：应用函数f共n次

0 := λf.λx.x           # 应用0次f，返回x
1 := λf.λx.f x         # 应用1次f
2 := λf.λx.f (f x)     # 应用2次f
3 := λf.λx.f (f (f x)) # 应用3次f
...

直观理解：
- 数字n不是符号，而是"做n次某事"的操作
- 这正是循环的本质！
```

### 3.2 后继函数（Successor）

```lambda
SUCC := λn.λf.λx.f (n f x)

含义：在n的基础上，再多应用一次f

验证 SUCC 1:
  (λn.λf.λx.f (n f x)) 1
→ λf.λx.f (1 f x)
→ λf.λx.f ((λf.λx.f x) f x)
→ λf.λx.f (f x)        # = 2 ✓
```

### 3.3 加法

```lambda
ADD := λm.λn.λf.λx.m f (n f x)

含义：先应用n次f，再应用m次f，总共m+n次

验证 ADD 1 2:
  相当于 2 f (1 f x)
  = f (f (f x))        # 3次f，= 3 ✓
```

### 3.4 乘法

```lambda
MUL := λm.λn.λf.m (n f)

含义：将"应用n次f"重复m次
```

### 3.5 Church布尔值

```lambda
TRUE  := λx.λy.x      # 返回第一个参数
FALSE := λx.λy.y      # 返回第二个参数

条件选择：
IF := λb.λt.λf.b t f

验证 IF TRUE a b:
  (λb.λt.λf.b t f) TRUE a b
→ TRUE a b
→ (λx.λy.x) a b
→ a                   # 返回a ✓
```

### 3.6 与C语言的映射

```c
// Church数在C中的体现

// 0: 不执行
void zero(void (*f)(int), int x) {
    // 直接返回x
}

// 1: 执行一次
void one(void (*f)(int), int x) {
    f(x);
}

// 2: 执行两次
void two(void (*f)(int), int x) {
    f(f(x));
}

// 这实际上就是我们写的循环！
for (int i = 0; i < n; i++) {  // Church数n的应用
    f(x);
}
```

---

## 4. 递归与Y组合子

### 4.1 递归的问题

```lambda
# 试图定义阶乘
FACT := λn.IF (IS_ZERO n) 1 (MUL n (FACT (PRED n)))

问题：FACT在定义内部引用自己！
这在λ演算中是非法的（没有全局命名）
```

### 4.2 Y组合子：不动点算子

```lambda
Y := λf.(λx.f (x x)) (λx.f (x x))

魔法：Y g = g (Y g)

这意味着Y g是g的不动点：
应用g后，结果还是Y g本身
```

### 4.3 用Y组合子实现递归

```lambda
# 阶乘的递归部分
FACT_GEN := λfact.λn.
  IF (IS_ZERO n)
     1
     (MUL n (fact (PRED n)))

# 真正的阶乘
FACT := Y FACT_GEN

执行 FACT 3:
  Y FACT_GEN 3
→ FACT_GEN (Y FACT_GEN) 3    # Y的魔法
→ IF (IS_ZERO 3) 1 (MUL 3 ((Y FACT_GEN) 2))
→ MUL 3 ((Y FACT_GEN) 2)
→ MUL 3 (FACT_GEN (Y FACT_GEN) 2)
→ ...
→ MUL 3 (MUL 2 (MUL 1 1))
→ 6
```

### 4.4 Y组合子的直观理解

```
Y组合子是一个"自我复制机"：

当你给Y一个函数生成器g，
Y会构造一个"无限展开"的表达式：

Y g = g (Y g)
    = g (g (Y g))
    = g (g (g (Y g)))
    = ...

这正是递归需要的：无限的函数调用链
但每次展开都延迟到实际需要时才发生（惰性求值）
```

---

## 5. λ演算与图灵机的等价性

### 5.1 Church-Turing论题的再确认

```
定理：
1. 任何λ演算表达式可计算的函数，图灵机也可计算
2. 任何图灵机可计算的函数，λ演算也可计算

证明思路：
→ 方向：将λ项编码为图灵机的磁带状态
← 方向：将图灵机编码为λ项（用Church数表示状态和配置）

意义：
两种完全不同的计算描述方式，描述的是同一类计算！
```

### 5.2 对比总结

| 特性 | 图灵机 | λ演算 |
|:-----|:-------|:------|
| **基础操作** | 状态转移 | 函数应用 |
| **数据** | 磁带上的符号 | 函数本身 |
| **控制流** | 显式状态 | 规约策略 |
| **自然对应** | 硬件/汇编 | 函数式语言 |
| **易于理解** | 直观（机械操作） | 抽象（数学概念） |
| **表达能力** | 图灵完备 | 图灵完备 |

---

## 6. 与现有知识库的连接

### 6.1 与C语言的函数对比

```
λ演算概念          C语言实现
─────────────────────────────────────────
λx.M               (x) => M  或 函数指针
M N                M(N) 函数调用
变量绑定            参数传递
β规约               函数调用时的参数替换
高阶函数            函数指针作为参数/返回值
```

**深入阅读**：[C语言函数与作用域](../../01_Core_Knowledge_System/02_Core_Layer/04_Functions_Scope.md)

### 6.2 与函数指针的关联

```c
// λ演算的函数作为值的体现

// λx.x + 1 的C实现
typedef int (*func_t)(int);

int add_one(int x) { return x + 1; }

// 高阶函数：接受函数作为参数
// 相当于 λf.λx.f x
int apply(func_t f, int x) {
    return f(x);
}

// 使用
int result = apply(add_one, 5);  // → 6
```

### 6.3 与编译原理的连接

```
λ演算 → 类型系统 → C类型系统

在编译原理中：
- λ演算是函数式语言的核心
- 类型推导（Hindley-Milner）基于λ演算
- C++的模板和泛型有λ演算的影子

相关阅读：
→ [C类型理论](../../02_Formal_Semantics_and_Physics/00_Core_Semantics_Foundations/04_C_Type_Theory.md)
```

---

## 7. Python实现：λ演算解释器

```python
# examples/lambda/lambda_eval.py

"""
λ演算解释器（简化版）

支持：
- 变量、抽象、应用
- β规约
- Church数
- Y组合子
"""

from dataclasses import dataclass
from typing import Union, Callable

# AST节点
@dataclass
class Var:
    name: str

@dataclass
class Abs:
    param: str
    body: 'Expr'

@dataclass
class App:
    func: 'Expr'
    arg: 'Expr'

Expr = Union[Var, Abs, App]

# 打印
def show(e: Expr) -> str:
    if isinstance(e, Var):
        return e.name
    elif isinstance(e, Abs):
        return f"(λ{e.param}.{show(e.body)})"
    elif isinstance(e, App):
        return f"({show(e.func)} {show(e.arg)})"

# 替换: expr[var := replacement]
def substitute(expr: Expr, var: str, replacement: Expr) -> Expr:
    if isinstance(expr, Var):
        return replacement if expr.name == var else expr
    elif isinstance(expr, Abs):
        if expr.param == var:
            return expr  # 变量被绑定，不替换
        else:
            return Abs(expr.param, substitute(expr.body, var, replacement))
    elif isinstance(expr, App):
        return App(
            substitute(expr.func, var, replacement),
            substitute(expr.arg, var, replacement)
        )

# β规约（一次）
def beta_reduce(expr: Expr) -> Expr:
    if isinstance(expr, App):
        if isinstance(expr.func, Abs):
            # (λx.M) N → M[x := N]
            return substitute(expr.func.body, expr.func.param, expr.arg)
        else:
            # 尝试规约函数部分
            new_func = beta_reduce(expr.func)
            if new_func is not expr.func:
                return App(new_func, expr.arg)
            # 尝试规约参数部分
            new_arg = beta_reduce(expr.arg)
            if new_arg is not expr.arg:
                return App(expr.func, new_arg)
    return expr

# 求值（多步规约）
def eval_expr(expr: Expr, max_steps: int = 100) -> Expr:
    for _ in range(max_steps):
        new_expr = beta_reduce(expr)
        if new_expr == expr:
            return expr  # 无法继续规约
        expr = new_expr
    return expr

# Church编码
def church_num(n: int) -> Expr:
    """将Python整数转换为Church数"""
    # n = λf.λx.f^n x
    body = Var("x")
    for _ in range(n):
        body = App(Var("f"), body)
    return Abs("f", Abs("x", body))

def church_add() -> Expr:
    """加法: λm.λn.λf.λx.m f (n f x)"""
    return Abs("m", Abs("n", Abs("f", Abs("x",
        App(
            App(Var("m"), Var("f")),
            App(App(Var("n"), Var("f")), Var("x"))
        )
    ))))

# 测试
if __name__ == "__main__":
    # 测试: (λx.x) y → y
    id_func = Abs("x", Var("x"))
    test1 = App(id_func, Var("y"))
    print(f"测试1: {show(test1)}")
    print(f"结果:  {show(eval_expr(test1))}")
    print()

    # 测试: 1 + 2 = 3
    add = church_add()
    one = church_num(1)
    two = church_num(2)

    # 应用加法: ((add 1) 2)
    test2 = App(App(add, one), two)
    print(f"测试2: 1 + 2")
    result = eval_expr(test2)
    print(f"结果:  {show(result)}")
    # 验证：应该等价于 church_num(3)
```

---

## 8. 学习检查点

完成本章后，您应该能够：

### 基础理解

- [ ] 解释λ演算的三种表达式
- [ ] 执行简单的β规约
- [ ] 描述Church数的原理

### 实践技能

- [ ] 手工将简单λ表达式规约到范式
- [ ] 用Church编码实现基本运算
- [ ] 理解Y组合子的工作原理

### 深层理解

- [ ] 解释为什么λ演算与图灵机等价
- [ ] 描述函数式编程的理论基础
- [ ] 思考：C语言与λ演算的关系

### 连接建立

- [ ] 找到C语言中对应λ抽象的实现
- [ ] 理解函数指针作为"函数值"的意义

---

## 9. 关键洞察总结

```
通过λ演算，我们理解：

1. 函数本身就是数据
   - C的函数指针、回调函数都源于此

2. 循环可以用递归表示
   - Church数就是"重复n次"的操作
   - C的for循环本质上是Church数的语法糖

3. 递归不需要自引用
   - Y组合子提供了通用的递归机制
   - 这启发了现代编程语言的递归实现

4. 两种计算模型等价
   - 命令式（图灵机）和函数式（λ演算）描述同一事物
   - 这解释了为什么多种编程语言都能解决问题
```

---

## 10. 下一步预告

### 下一章：[计算复杂性](./03_Computational_Complexity.md)

我们将探讨：

- P vs NP问题
- 计算难度的本质
- 为什么有些问题"难以计算"
- 物理限制对计算的影响

---

*文档创建: 2026-03-29*
*配套代码: [lambda_eval.py](../../examples/lambda/lambda_eval.py)*
*与图灵机的连接: 两种等价的计算模型*
