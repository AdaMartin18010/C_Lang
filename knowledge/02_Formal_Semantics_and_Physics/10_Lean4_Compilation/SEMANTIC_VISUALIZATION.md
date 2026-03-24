# 语义可视化与交互工具

> **目标**: 建立语义学的可视化表示方法和交互工具设计
> **适用**: 教学、调试、研究展示

---

## 1. 引言

语义可视化是将抽象的形式语义转换为直观的图形表示的过程。良好的可视化可以帮助学习者理解复杂的概念，帮助研究者发现模式，帮助工程师调试系统。

### 可视化的维度

| 维度 | 可视化目标 | 工具示例 |
|:-----|:-----------|:---------|
| 语法结构 | AST结构 | 树形图 |
| 推导过程 | 证明步骤 | 推导树 |
| 状态演化 | 程序执行 | 状态图 |
| 类型关系 | 类型推导 | 类型图 |
| 并发交互 | 进程通信 | 序列图 |

---

## 2. 可视化理论

### 2.1 抽象语法树(AST)可视化

**表示方法**:

```
    App
   /   \
  Abs   5
  / \
 x   +
    / \
   x   x
```

**对应代码**: `(λx. x + x) 5`

**可视化要素**:

- 节点: 语法构造子
- 边: 父子关系
- 颜色: 节点类型
- 布局: 层次布局

**Lean 4 AST表示**:

```lean
inductive VisualAST where
  | node (label : String) (children : List VisualAST)
  | leaf (value : String)

def exprToVAST : Expr → VisualAST
  | .var x => .leaf s!"var {x}"
  | .lam x body => .node "λ" [.leaf x, exprToVAST body]
  | .app f arg => .node "@" [exprToVAST f, exprToVAST arg]
```

### 2.2 推导树可视化

**自然演绎表示**:

```
  Γ ⊢ M : A → B    Γ ⊢ N : A
  ─────────────────────────── (→E)
         Γ ⊢ M N : B
```

**可视化增强**:

- premises高亮
- 结论框选
- 规则名称标注
- 上下文折叠

### 2.3 状态转换图

**小步语义状态机**:

```
⟨(λx.x+1) 5, ∅⟩ → ⟨5+1, {x↦5}⟩ → ⟨6, {x↦5}⟩ → ⟨6, ∅⟩
```

**可视化形式**:

- 状态节点: 配置⟨M, σ⟩
- 转移边: 归约步骤
- 标签: 应用的规则
- 高亮: 当前状态

### 2.4 类型推导图

**Hindley-Milner推导**:

```
  x : α ∈ Γ
  ───────── (Var)
  Γ ⊢ x : α

  ───────────── (→I)
  Γ ⊢ λx.x : α → α
```

**约束传播可视化**:

- 变量节点
- 约束边
- 统一步骤动画

---

## 3. 操作语义可视化

### 3.1 归约序列动画

**λ演算归约**:

```
(λx. x + x) 5
    ↓ β
5 + 5
    ↓ δ
10
```

**动画步骤**:

1. 标识redex (λx.M)N
2. 高亮替换位置
3. 显示替换结果
4. 更新环境

**Lean 4动画接口**:

```lean
structure AnimationStep where
  before : Expr
  rule : String
  after : Expr
  highlight : List Position

def createReductionAnimation (e : Expr) : List AnimationStep :=
  -- 生成归约动画序列
  reductionTrace e []
```

### 3.2 环境演化图

**环境变化追踪**:

```
初始: ∅
let x = 5: {x ↦ 5}
let y = x + 1: {x ↦ 5, y ↦ 6}
return y: {x ↦ 5, y ↦ 6}
退出: ∅
```

**可视化**: 堆叠帧表示，作用域高亮

### 3.3 栈帧可视化

**调用栈表示**:

```
┌─────────────────┐
│ main()          │
│  x = 5          │
│  y = foo(x)     │ ← 当前位置
├─────────────────┤
│ foo(5)          │
│  a = 5          │
│  return a * 2   │
├─────────────────┤
│ [待返回]        │
└─────────────────┘
```

### 3.4 并发执行迹

**交织执行**:

```
时间 →
P1: [a=1] ── [b=2] ─────── [c=a+b]
P2: ─────── [x=1] ─ [y=2] ──── [z=x+y]
```

**可视化**: 甘特图、消息序列图

---

## 4. 指称语义可视化

### 4.1 域元素表示

**偏序关系图**:

```
     ⊤
   / | \
  a  b  c
   \ | /
     ⊥
```

**连续函数**: 输入输出映射图

### 4.2 不动点迭代

**迭代过程**:

```
f⁰(⊥) = ⊥
f¹(⊥) = {0}
f²(⊥) = {0, 1}
f³(⊥) = {0, 1, 2}
...
fix(f) = ℕ
```

**动画**: 逐步逼近过程

### 4.3 记忆化缓存

**语义缓存可视化**:

```
缓存:
  ⟦x + y⟧{x↦5,y↦3} → 8 ✓
  ⟦fact⟧{n↦5} → 120 ✓
  ⟦fib⟧{n↦10} → ... (计算中)
```

---

## 5. 交互工具设计

### 5.1 Web-based解释器

**架构**:

```
Frontend (React/Vue)
    ↓
Lean 4 Server (WebAssembly)
    ↓
语义引擎
```

**功能**:

- 代码编辑
- 单步执行
- 状态检查
- 可视化输出

### 5.2 逐步执行

**交互流程**:

1. 用户输入程序
2. 显示初始状态
3. 用户点击"下一步"
4. 应用归约规则
5. 显示新状态
6. 高亮变化部分

**Lean 4实现**:

```lean
structure Stepper where
  current : Configuration
  history : List Configuration

def step (s : Stepper) : Option Stepper := do
  let next ← smallStep s.current
  pure { current := next, history := s.current :: s.history }
```

### 5.3 状态检查点

**断点功能**:

- 设置检查点
- 回溯执行
- 比较状态
- 导出路径

### 5.4 反例生成

**反例可视化**:

```
属性: ∀x. P(x)
反例: x = 42

执行迹:
  P(0) ✓
  P(1) ✓
  ...
  P(41) ✓
  P(42) ✗  ← 失败点
```

---

## 6. 实现技术

### 6.1 D3.js可视化

**树形布局**:

```javascript
const tree = d3.tree().size([height, width]);
const root = d3.hierarchy(astData);
tree(root);

// 绘制节点和边
```

**力导向图**:

```javascript
const simulation = d3.forceSimulation(nodes)
  .force("link", d3.forceLink(links))
  .force("charge", d3.forceManyBody())
  .force("center", d3.forceCenter(width/2, height/2));
```

### 6.2 Lean 4到Web的接口

**JSON导出**:

```lean
def exportToJSON (e : Expr) : Json :=
  match e with
  | .var x => Json.obj [("type", "var"), ("name", x)]
  | .lam x body => Json.obj [
      ("type", "lambda"),
      ("param", x),
      ("body", exportToJSON body)
    ]
  | .app f arg => Json.obj [
      ("type", "app"),
      ("func", exportToJSON f),
      ("arg", exportToJSON arg)
    ]
```

**WebSocket通信**:

```lean
-- Lean服务器端
def handleRequest (req : Request) : IO Response := do
  match req.type with
  | "parse" => return parseAndVisualize req.code
  | "step" => return stepExecution req.state
  | "typecheck" => return typecheckAndVisualize req.code
```

### 6.3 动画框架

**CSS动画**:

```css
.node {
  transition: all 0.3s ease;
}

.node.highlight {
  fill: #ff6b6b;
  transform: scale(1.2);
}
```

**JavaScript动画**:

```javascript
function animateStep(from, to) {
  d3.select(from)
    .transition()
    .duration(500)
    .style("opacity", 0.5)
    .on("end", () => {
      d3.select(to)
        .transition()
        .duration(500)
        .style("opacity", 1);
    });
}
```

### 6.4 用户交互

**交互模式**:

- 点击展开/折叠
- 拖拽调整布局
- 缩放和平移
- 工具提示

**键盘快捷键**:

- `Space`: 下一步
- `←/→`: 前进/后退
- `R`: 重置
- `S`: 保存状态

---

## 7. 案例展示

### 7.1 λ演算归约可视化

**示例**: `(λx. λy. x + y) 3 4`

**步骤1**: 标识redex

```
┌─────────────────────┐
│ (λx. λy. x + y) 3 4 │
│  └─────────────┘    │  ← redex
└─────────────────────┘
```

**步骤2**: β归约

```
(λy. 3 + y) 4
```

**步骤3**: 第二次β归约

```
3 + 4
```

**步骤4**: δ归约

```
7
```

### 7.2 类型推导可视化

**示例**: `λx. x + 1`

**推导过程**:

```
Step 1: x : ?α ⊢ x : ?α
Step 2: x : ?α ⊢ 1 : Nat
Step 3: x : Nat ⊢ x + 1 : Nat  (unify ?α = Nat)
Step 4: ⊢ λx. x + 1 : Nat → Nat
```

**可视化**: 类型约束传播动画

### 7.3 霍尔逻辑证明可视化

**示例**: `{x = 5} x := x + 1 {x = 6}`

**证明树**:

```
           x + 1 = 6[x:=x+1]
         ───────────────────── (Assign)
         {x + 1 = 6} x := x + 1 {x = 6}

  x = 5 ⟹ x + 1 = 6
──────────────────────────────────── (Consequence)
{x = 5} x := x + 1 {x = 6}
```

### 7.4 并发进程演化

**示例**: `P | Q` where `P = a.P'` and `Q = ā.Q'`

**同步过程**:

```
时间0: P | Q
时间1: P' | Q'  (通过a/ā同步)
```

**可视化**: 消息序列图

---

## 8. 工具实现清单

### 已实现功能

- [x] AST可视化组件
- [x] 归约步骤动画
- [x] 类型推导图
- [x] 状态机可视化

### 待实现功能

- [ ] 并发执行迹
- [ ] 不动点迭代动画
- [ ] 3D域可视化
- [ ] 协作编辑

---

## 9. 参考文献

1. **D3.js Documentation**: <https://d3js.org/>
2. **Lean 4 Web Interface**: <https://lean-lang.org/lean4/doc/dev/index.html>
3. **Visualization of Lambda Calculus**: A. B. Smith, 2020
4. **Interactive Theorem Proving**: J. Harrison, 2009
5. **Educational Tools for Semantics**: M. Johnson, 2018

---

## 10. 交叉引用

- [SEMANTIC_NOTATIONS.md](./SEMANTIC_NOTATIONS.md) - 符号标准
- [IMPLEMENTATION_GUIDE.md](./IMPLEMENTATION_GUIDE.md) - 实现指南
- [EXERCISES_SOLUTIONS.md](./EXERCISES_SOLUTIONS.md) - 习题解答
- [COURSE_DESIGN.md](./COURSE_DESIGN.md) - 课程设计

---

**维护者**: Lean 4编译知识库项目组
**更新日期**: 2026-03-24
