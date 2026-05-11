# 编译器基础 (Compiler Basics)

> **层级定位**: 01 Core Knowledge System / 15 Compiler Basics
> **主题**: 从源代码到中间表示的完整前端链
> **难度级别**: L3 应用 → L4 分析
> **预估学习时间**: 20-30 小时

## 概述

编译器前端是将高级语言（C）转换为中间表示（IR）的核心阶段。理解前端的工作原理，是掌握代码生成、优化和调试技术的基础。本模块系统阐述从词法分析到语义分析再到 IR 生成的完整编译前端链，对齐 Stanford CS107 和 NAND2Tetris 编译器项目的教学体系。

---

## 核心阶段

```
源代码(.c) → [词法分析] → Token流 → [语法分析] → AST → [语义分析] → 符号表+类型检查 → [IR生成] → 中间表示
```

---

## 阶段一：词法分析 (Lexical Analysis)

词法分析器（Lexer/Scanner）将字符流转换为有意义的 Token 序列。

### 核心概念

- **Token**: 最小有意义的词法单元（关键字、标识符、运算符、字面量）
- **正则表达式**: 定义 Token 模式的工具
- **有限状态自动机 (DFA/NFA)**: 词法分析的理论基础
- **最长匹配原则**: `>>` 应识别为右移运算符而非两个 `>`

### C 语言 Token 分类

| 类别 | 示例 |
|:-----|:-----|
| 关键字 | `int`, `return`, `if`, `while`, `struct` |
| 标识符 | `main`, `printf`, `_count` |
| 常量 | `42`, `3.14`, `"hello"`, `'c'` |
| 运算符 | `+`, `-`, `*`, `/`, `==`, `->`, `++` |
| 分隔符 | `(`, `)`, `{`, `}`, `;`, `,` |

### 实战：手写词法分析器

```c
// 简化的 C 词法分析器核心逻辑
typedef enum {
    TOK_INT, TOK_RETURN, TOK_IF, TOK_IDENTIFIER,
    TOK_NUMBER, TOK_STRING, TOK_PLUS, TOK_EQ,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_SEMICOLON, TOK_EOF
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
    int line;
} Token;

// 状态机驱动的扫描
Token next_token(const char *source, int *pos) {
    // 跳过空白和注释
    // 识别关键字、标识符、数字、字符串
    // 处理运算符（注意 >>= 等多字符运算符）
}
```

### 工具实践

- **flex**: 工业级词法分析器生成器
- `flex -o lexer.c lexer.l`

---

## 阶段二：语法分析 (Syntax Analysis / Parsing)

语法分析器（Parser）将 Token 流转换为抽象语法树（AST）。

### 核心概念

- **上下文无关文法 (CFG)**: 定义语言语法结构的形式化工具
- **抽象语法树 (AST)**: 源代码的树形抽象表示
- **推导与归约**: 自顶向下 vs 自底向上
- **优先级与结合性**: `a + b * c` 的解析歧义消除

### C 语言语法核心

```
program     → declaration*
declaration → function_decl | var_decl | type_decl
function_decl → type identifier '(' params ')' compound_stmt
statement   → expr_stmt | compound_stmt | if_stmt | while_stmt | return_stmt
expression  → assignment
assignment  → equality ('=' assignment)?
equality    → comparison (('==' | '!=') comparison)*
comparison  → addition (('>' | '<' | '>=' | '<=') addition)*
addition    → multiplication (('+' | '-') multiplication)*
multiplication → unary (('*' | '/') unary)*
unary       → ('+' | '-' | '*' | '&') unary | primary
primary     → NUMBER | STRING | IDENTIFIER | '(' expression ')'
```

### 递归下降解析器

```c
// 递归下降解析器示例：解析表达式
Expr *parse_expression(Parser *p) {
    return parse_assignment(p);
}

Expr *parse_assignment(Parser *p) {
    Expr *left = parse_equality(p);
    if (match(p, TOKEN_EQUAL)) {
        Token op = previous(p);
        Expr *right = parse_assignment(p);  // 右结合
        return create_binary(left, op, right);
    }
    return left;
}
```

### 工具实践

- **bison/yacc**: 工业级语法分析器生成器
- `bison -d -o parser.c parser.y`

---

## 阶段三：语义分析 (Semantic Analysis)

语义分析器检查 AST 的语义正确性，构建符号表并执行类型检查。

### 核心概念

- **符号表 (Symbol Table)**: 标识符到其属性的映射（作用域链）
- **类型检查**: 确保操作数类型兼容
- **类型推导**: 推断表达式类型（C11 `_Generic`）
- **作用域分析**: 块作用域、文件作用域、函数原型作用域

### C 语言语义检查要点

| 检查项 | 说明 | 示例 |
|:-------|:-----|:-----|
| 类型兼容 | 操作数类型是否兼容 | `int + double` ✓, `struct A + int` ✗ |
| 隐式转换 | 整型提升和常规算术转换 | `char` → `int` → `double` |
| 左值检查 | 赋值目标必须是可修改左值 | `5 = x` ✗ |
| 函数签名 | 参数数量和类型匹配 | `foo(1, 2)` vs `void foo(int)` ✗ |
| 未定义标识符 | 使用前先声明 | `x = 5;` 未声明 ✗ |
| 控制流检查 | `return` 类型匹配、所有路径返回 | `int f() { if(x) return 1; }` ✗ |

### 符号表实现

```c
typedef struct Symbol {
    char *name;
    Type *type;
    int scope_level;
    StorageClass storage;
    struct Symbol *next;
} Symbol;

typedef struct SymbolTable {
    Symbol *buckets[HASH_SIZE];
    struct SymbolTable *parent;  // 外层作用域链
} SymbolTable;
```

---

## 阶段四：中间表示生成 (IR Generation)

将 AST 转换为中间表示（IR），为后端优化和代码生成做准备。

### 主流 IR 形式

| IR 类型 | 代表 | 特点 |
|:--------|:-----|:-----|
| **三地址码 (TAC)** | GCC GIMPLE | 每条指令最多三个操作数，类似汇编 |
| **SSA 形式** | LLVM IR | 静态单赋值，每个变量只赋值一次 |
| **图 IR** | Sea of Nodes | 控制流和数据流混合图 |
| **栈机代码** | JVM Bytecode | 基于栈的操作，适合解释执行 |

### 三地址码示例

```c
// C 源代码
int sum(int a, int b) {
    int c = a + b;
    return c * 2;
}

// 三地址码 (TAC)
t1 = a + b
c = t1
t2 = c * 2
return t2
```

### LLVM IR 示例

```llvm
; LLVM IR
define i32 @sum(i32 %a, i32 %b) {
entry:
  %add = add i32 %a, %b
  %mul = mul i32 %add, 2
  ret i32 %mul
}
```

---

## 完整编译前端架构

```
┌─────────────────────────────────────────┐
│           Source Code (.c)               │
└─────────────────┬───────────────────────┘
                  ▼
┌─────────────────────────────────────────┐
│  Lexer (词法分析)                        │
│  • 正则表达式匹配                        │
│  • 关键字/标识符/常量分类                │
│  • 错误恢复（跳过非法字符）              │
└─────────────────┬───────────────────────┘
                  ▼ Token Stream
┌─────────────────────────────────────────┐
│  Parser (语法分析)                       │
│  • 递归下降 / LR 分析                    │
│  • 优先级/结合性处理                     │
│  • 构建 AST                             │
└─────────────────┬───────────────────────┘
                  ▼ AST
┌─────────────────────────────────────────┐
│  Semantic Analyzer (语义分析)            │
│  • 符号表构建（作用域链）                │
│  • 类型检查与推导                        │
│  • 隐式转换插入                          │
└─────────────────┬───────────────────────┘
                  ▼ Typed AST + Symbol Table
┌─────────────────────────────────────────┐
│  IR Generator (中间代码生成)             │
│  • AST → 三地址码 / SSA                  │
│  • 控制流图 (CFG) 构建                   │
└─────────────────┬───────────────────────┘
                  ▼ Intermediate Representation
┌─────────────────────────────────────────┐
│  Optimizer (优化器) → 见 12_Compiler_Optimization │
└─────────────────────────────────────────┘
```

---

## 子文档索引

| 文件 | 主题 | 难度 |
|:-----|:-----|:-----|
| [01_Lexical_Analysis.md](01_Lexical_Analysis.md) | 词法分析：正则表达式、DFA、flex | L3 |
| 02_Syntactic_Analysis.md | 语法分析：递归下降、LR、bison | L4 |

## 权威参考

- **NAND2Tetris Ch.10-11**: 编译器前端（Jack→VM）
- **Stanford CS107**: C 编译器项目（完整前端实现）
- **Dragon Book** (Aho, Sethi, Ullman): 编译原理经典教材
- **Engineering a Compiler** (Cooper & Torczon): 现代编译器工程
- **LLVM Kaleidoscope Tutorial**: LLVM IR 生成入门

## 待补充内容

- [ ] 语义分析完整实现指南
- [ ] IR 生成器设计与实现
- [ ] 错误报告与恢复机制
- [ ] 预处理器的词法分析特殊性 (`#define`, `#include`)

---

*最后更新: 2026-05-10*
