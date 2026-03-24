# 交互式语义工具

> **目标**: 设计和实现交互式语义学工具
> **范围**: Web工具、IDE插件、调试器

---

## 1. 引言

交互式工具将静态的语义学文档转化为动态的学习和研究环境。通过实时反馈、可视化和交互，用户可以更深入地理解语义概念。

### 工具分类

| 类型 | 功能 | 目标用户 |
|:-----|:-----|:---------|
| Web解释器 | 在线执行和可视化 | 学习者 |
| IDE插件 | 集成开发支持 | 工程师 |
| 调试器 | 程序分析 | 研究者 |
| 证明助手 | 交互式证明 | 形式化验证 |

---

## 2. Web解释器

### 2.1 系统架构

```
┌─────────────────────────────────────────┐
│          Frontend (React/Vue)           │
│  ┌─────────┐ ┌─────────┐ ┌──────────┐  │
│  │ 编辑器   │ │ 可视化  │ │ 控制台   │  │
│  └────┬────┘ └────┬────┘ └────┬─────┘  │
└───────┼───────────┼───────────┼────────┘
        │           │           │
        └───────────┼───────────┘
                    │ WebSocket
┌───────────────────┼─────────────────────┐
│         Lean 4 Web Server              │
│  ┌──────────────┐ ┌─────────────────┐  │
│  │   Parser     │ │   Type Checker   │  │
│  └──────────────┘ └─────────────────┘  │
│  ┌──────────────┐ ┌─────────────────┐  │
│  │  Evaluator   │ │  Visualizer     │  │
│  └──────────────┘ └─────────────────┘  │
└─────────────────────────────────────────┘
```

### 2.2 功能模块

#### 代码编辑器

- **特性**:
  - 语法高亮
  - 自动补全
  - 错误提示
  - 代码折叠

- **Lean 4支持**:

```javascript
import { leanLanguage } from 'codemirror-lang-lean';

const editor = new EditorView({
  doc: 'def main := print "Hello"',
  extensions: [basicSetup, leanLanguage()]
});
```

#### 执行引擎

- **解释器模式**:
  - 单步执行
  - 连续执行
  - 断点调试

- **Lean接口**:

```lean
structure ExecutionRequest where
  code : String
  mode : ExecutionMode  -- Step | Continuous
  breakpoints : List Position

def execute (req : ExecutionRequest) : IO ExecutionResult := do
  let ast ← parse req.code
  match req.mode with
  | .step => stepExecution ast
  | .continuous => continuousExecution ast
```

#### 可视化面板

- **显示内容**:
  - AST树
  - 状态转换
  - 类型推导
  - 内存布局

### 2.3 用户界面

**主界面布局**:

```
┌──────────────────────────────────────┐
│  工具栏                               │
├──────────┬───────────────┬───────────┤
│          │               │           │
│  编辑器   │   可视化区    │  信息面板  │
│          │               │           │
│          │               │           │
├──────────┴───────────────┴───────────┤
│  控制台/日志                          │
└──────────────────────────────────────┘
```

**控制按钮**:

- ▶ 运行
- ⏸ 暂停
- ⏹ 停止
- ⏮ 单步回退
- ⏭ 单步前进
- 🔁 重置

---

## 3. IDE插件

### 3.1 VS Code插件

**功能清单**:

- [x] 语法高亮
- [x] 错误诊断
- [x] 代码补全
- [x] 定义跳转
- [x] 悬停提示
- [x] 重命名重构
- [ ] 语义可视化
- [ ] 交互式证明

**配置示例**:

```json
{
  "name": "lean4-semantics",
  "displayName": "Lean 4 Semantics",
  "contributes": {
    "languages": [{
      "id": "lean4-semantics",
      "extensions": [".l4s"]
    }],
    "commands": [{
      "command": "semantics.visualize",
      "title": "Visualize Semantics"
    }]
  }
}
```

### 3.2 Emacs模式

```elisp
(define-derived-mode lean4-semantics-mode prog-mode "L4S"
  "Major mode for Lean 4 Semantics."
  (setq font-lock-defaults '(lean4-semantics-keywords))
  (lean4-semantics-setup-lsp))

(define-key lean4-semantics-mode-map (kbd "C-c C-v") 'semantics-visualize)
(define-key lean4-semantics-mode-map (kbd "C-c C-s") 'semantics-step)
```

### 3.3 LSP服务器

**功能**:

- 诊断
- 补全
- 悬停
- 定义
- 语义Token

**Lean实现**:

```lean
def handleRequest (method : String) (params : Json) : IO Json :=
  match method with
  | "textDocument/hover" => handleHover params
  | "textDocument/completion" => handleCompletion params
  | "textDocument/definition" => handleDefinition params
  | "semantics/visualize" => handleVisualize params
  | _ => throw (.userError s!"Unknown method: {method}")
```

---

## 4. 调试器

### 4.1 调试功能

**断点类型**:

- 代码断点
- 数据断点
- 条件断点
- 函数断点

**调试操作**:

- 继续(Continue)
- 单步跳过(Step Over)
- 单步进入(Step Into)
- 单步跳出(Step Out)
- 运行到光标(Run to Cursor)

### 4.2 调用栈

**栈帧表示**:

```lean
structure StackFrame where
  function : Name
  location : Position
  locals : List (Name × Value)
  arguments : List (Name × Value)

def getCallStack (state : DebugState) : List StackFrame :=
  state.executionStack.map λframe =>
    { function := frame.fn,
      location := frame.pos,
      locals := frame.env.toList,
      arguments := frame.args }
```

**可视化**:

```
调用栈:
┌─────────────────────────────────────┐
│ 1. fib(5) at fib.l4s:10             │ ← 当前帧
│    n = 5                            │
├─────────────────────────────────────┤
│ 2. fib(4) at fib.l4s:10             │
│    n = 4                            │
├─────────────────────────────────────┤
│ 3. main at main.l4s:5               │
└─────────────────────────────────────┘
```

### 4.3 变量检查

**检查器功能**:

- 局部变量
- 全局变量
- 表达式求值
- 内存查看
- 修改值

**表达式求值**:

```lean
def evaluateExpression (expr : String) (context : Context) : IO Value := do
  let ast ← parseExpression expr
  eval ast context
```

---

## 5. 证明助手

### 5.1 交互式证明

**证明状态**:

```lean
structure ProofState where
  goals : List Goal
  context : Context
  completed : List Tactic

def applyTactic (state : ProofState) (tactic : Tactic) : IO ProofState :=
  match tactic with
  | .intro names => applyIntro state names
  | .apply expr => applyApply state expr
  | .simp => applySimp state
  | .exact expr => applyExact state expr
```

**证明树可视化**:

```
⊢ ∀n, n + 0 = n
│
├─ intro n
│  │
│  ├─ induction n
│  │  │
│  │  ├─ case 0
│  │  │  │
│  │  │  └─ simp ✓
│  │  │
│  │  └─ case succ n ih
│  │     │
│  │     └─ simp [ih] ✓
```

### 5.2 自动化策略

**策略组合**:

```lean
macro "auto" : tactic =>
  `(tactic| try { simp } <;> try { aesop } <;> try { omega })

macro "induct_auto" : tactic =>
  `(tactic| intro x; induction x <;> auto)
```

**反例搜索**:

```lean
def findCounterexample (prop : Expr) (bounds : List Bound) : IO (Option Assignment) :=
  -- 使用SMT求解器或穷举搜索
  slimCheck prop bounds
```

---

## 6. 工具集成

### 6.1 与文档集成

**功能**:

- 文档内嵌交互式代码
- 一键打开工具
- 示例直接运行
- 习题自动验证

**实现**:

```markdown
## 示例

```lean4-interactive
-- 点击运行按钮执行
#eval 1 + 1
```

<button onclick="openInTool('code-id')">在工具中打开</button>

```

### 6.2 与CI/CD集成

**持续验证**:
```yaml
# .github/workflows/semantics.yml
name: Semantics Check
on: [push, pull_request]
jobs:
  verify:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Run Semantic Verification
        run: lean4-semantics verify --all
      - name: Generate Report
        run: lean4-semantics report --output report.html
```

### 6.3 API接口

**REST API**:

```
POST /api/parse
{ "code": "def main := 1 + 1" }
→ { "ast": {...}, "errors": [] }

POST /api/evaluate
{ "code": "1 + 1", "mode": "step" }
→ { "steps": [...], "result": "2" }

POST /api/typecheck
{ "code": "λx.x" }
→ { "type": "∀α, α → α" }

POST /api/visualize
{ "type": "ast", "code": "1 + 1" }
→ { "svg": "..." }
```

---

## 7. 性能优化

### 7.1 增量计算

**缓存策略**:

```lean
structure IncrementalCache where
  parsed : HashMap String AST
  typechecked : HashMap String TypeInfo
  evaluated : HashMap String Value

def incrementalCheck (code : String) (cache : IncrementalCache) : IO (Result × IncrementalCache) :=
  if let some ast := cache.parsed.find? code then
    -- 使用缓存
    pure (ast, cache)
  else
    -- 重新解析
    let ast ← parse code
    pure (ast, { cache with parsed := cache.parsed.insert code ast })
```

### 7.2 懒加载

**模块懒加载**:

```javascript
const Visualizer = React.lazy(() => import('./Visualizer'));

function App() {
  return (
    <Suspense fallback={<Loading />}>
      <Visualizer />
    </Suspense>
  );
}
```

### 7.3 并行化

**并行类型检查**:

```lean
def parallelTypeCheck (files : List File) : IO (List TypeInfo) :=
  files.mapM (fun f => Task.spawn (typeCheckFile f))
  |> Task.collect
  |> Task.waitAll
```

---

## 8. 部署与分发

### 8.1 Web应用

**Docker部署**:

```dockerfile
FROM node:18-alpine
WORKDIR /app
COPY package*.json ./
RUN npm install
COPY . .
RUN npm run build
EXPOSE 3000
CMD ["npm", "start"]
```

### 8.2 桌面应用

**Electron打包**:

```javascript
// main.js
const { app, BrowserWindow } = require('electron');

function createWindow() {
  const win = new BrowserWindow({
    width: 1200,
    height: 800,
    webPreferences: {
      nodeIntegration: true
    }
  });
  win.loadURL('http://localhost:3000');
}

app.whenReady().then(createWindow);
```

### 8.3 CLI工具

```bash
# 安装
npm install -g lean4-semantics

# 使用
l4s parse file.l4s
l4s check file.l4s
l4s eval file.l4s
l4s visualize --type ast file.l4s
l4s verify --property "safety" file.l4s
```

---

## 9. 用户反馈与改进

### 9.1 遥测

**收集指标**:

- 功能使用频率
- 性能数据
- 错误报告
- 用户路径

### 9.2 反馈渠道

- GitHub Issues
- 用户论坛
- 问卷调查
- 用户访谈

---

## 10. 交叉引用

- [SEMANTIC_VISUALIZATION.md](./SEMANTIC_VISUALIZATION.md) - 可视化理论
- [IMPLEMENTATION_GUIDE.md](./IMPLEMENTATION_GUIDE.md) - 实现指南
- [COURSE_DESIGN.md](./COURSE_DESIGN.md) - 课程设计
- [VERIFICATION_TOOLS.md](./VERIFICATION_TOOLS.md) - 验证工具

---

**维护者**: Lean 4编译知识库项目组
**更新日期**: 2026-03-24
