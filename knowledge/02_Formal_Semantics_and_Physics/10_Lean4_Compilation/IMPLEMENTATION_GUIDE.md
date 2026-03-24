# Lean 4语义实现指南

> **目标**: 提供从零开始实现类型检查器和语义分析器的完整工程指南
> **级别**: L6+++ (系统级元理论深度)
> **状态**: 符合 SEMANTIC_NOTATIONS.md
> **关联文档**: CASE_STUDY_TYPECHECKER.md, LEAN4_SEMANTICS_MAPPING.md, APPLIED_FUNCTIONAL.md

---

## 目录

- [Lean 4语义实现指南](#lean-4语义实现指南)
  - [目录](#目录)
  - [1. 引言](#1-引言)
    - [1.1 文档目标](#11-文档目标)
    - [1.2 前置知识](#12-前置知识)
    - [1.3 方法论](#13-方法论)
    - [1.4 符号约定](#14-符号约定)
  - [2. 项目设置](#2-项目设置)
    - [2.1 环境配置](#21-环境配置)
      - [安装Lean 4工具链](#安装lean-4工具链)
      - [VS Code配置](#vs-code配置)
    - [2.2 项目结构](#22-项目结构)
    - [2.3 依赖管理](#23-依赖管理)
      - [lakefile.toml配置](#lakefiletoml配置)
      - [构建命令](#构建命令)
    - [2.4 测试框架](#24-测试框架)
      - [使用Lean内置测试](#使用lean内置测试)
      - [示例测试用例](#示例测试用例)
  - [3. 词法分析](#3-词法分析)
    - [3.1 Token定义](#31-token定义)
    - [3.2 词法规则](#32-词法规则)
      - [词法分析器状态](#词法分析器状态)
      - [核心词法分析函数](#核心词法分析函数)
      - [关键字映射](#关键字映射)
    - [3.3 位置信息与错误报告](#33-位置信息与错误报告)
      - [完整的tokenize函数](#完整的tokenize函数)
  - [4. 语法分析](#4-语法分析)
    - [4.1 语法定义](#41-语法定义)
      - [抽象语法树(AST)定义](#抽象语法树ast定义)
    - [4.2 解析器实现](#42-解析器实现)
      - [解析器单子](#解析器单子)
      - [基本解析器组合子](#基本解析器组合子)
    - [4.3 AST定义与优先级处理](#43-ast定义与优先级处理)
      - [表达式解析(带优先级)](#表达式解析带优先级)
      - [类型与辅助解析](#类型与辅助解析)
  - [5. 类型检查器](#5-类型检查器)
    - [5.1 类型表示](#51-类型表示)
    - [5.2 上下文管理](#52-上下文管理)
    - [5.3 推导算法](#53-推导算法)
      - [合一算法](#合一算法)
      - [主类型推导算法](#主类型推导算法)
    - [5.4 错误处理](#54-错误处理)
  - [6. 操作语义实现](#6-操作语义实现)
    - [6.1 求值器](#61-求值器)
      - [大步操作语义求值器](#大步操作语义求值器)
    - [6.2 环境模型](#62-环境模型)
    - [6.3 闭包表示](#63-闭包表示)
    - [6.4 尾递归优化(TCO)](#64-尾递归优化tco)
  - [7. 验证与测试](#7-验证与测试)
    - [7.1 单元测试](#71-单元测试)
    - [7.2 属性测试](#72-属性测试)
    - [7.3 形式化验证](#73-形式化验证)
  - [8. 扩展功能](#8-扩展功能)
    - [8.1 多态类型](#81-多态类型)
    - [8.2 效应系统](#82-效应系统)
    - [8.3 模块系统](#83-模块系统)
    - [8.4 FFI](#84-ffi)
  - [9. 最佳实践](#9-最佳实践)
    - [9.1 代码组织](#91-代码组织)
    - [9.2 性能优化](#92-性能优化)
    - [9.3 错误处理](#93-错误处理)
    - [9.4 文档编写](#94-文档编写)
  - [10. 完整示例项目](#10-完整示例项目)
    - [10.1 项目概览](#101-项目概览)
    - [10.2 项目文件结构](#102-项目文件结构)
    - [10.3 核心代码实现](#103-核心代码实现)
      - [lakefile.toml](#lakefiletoml)
      - [MiniML/Basic.lean](#minimlbasiclean)
      - [MiniML/AST.lean](#minimlastlean)
      - [MiniML/Lexer.lean](#minimllexerlean)
      - [MiniML/Parser.lean](#minimlparserlean)
      - [MiniML/TypeChecker.lean](#minimltypecheckerlean)
      - [MiniML/Evaluator.lean](#minimlevaluatorlean)
      - [MiniML/REPL.lean](#minimlrepllean)
      - [Main.lean](#mainlean)
    - [10.4 测试套件](#104-测试套件)
    - [10.5 示例程序](#105-示例程序)
  - [11. 参考文献](#11-参考文献)
    - [11.1 核心文献](#111-核心文献)
    - [11.2 Lean 4相关](#112-lean-4相关)
    - [11.3 实现技术](#113-实现技术)
    - [11.4 在线资源](#114-在线资源)
  - [12. 交叉引用](#12-交叉引用)
    - [12.1 关联文档](#121-关联文档)
    - [12.2 前置知识](#122-前置知识)
    - [12.3 后续学习路径](#123-后续学习路径)
  - [附录: 代码片段索引](#附录-代码片段索引)

---

## 1. 引言

### 1.1 文档目标

本指南提供在Lean 4中实现完整编程语言语义工具的实践路径。从词法分析到类型检查器，从操作语义到形式化验证，覆盖构建可靠语言实现所需的所有核心组件。

### 1.2 前置知识

- Lean 4基础语法与类型系统
- 形式语义基本概念（操作语义、指称语义）
- 类型理论基础知识
- 函数式编程经验

### 1.3 方法论

本指南采用**渐进式构建**方法：

1. 从最简单的表达式语言开始
2. 逐步添加类型系统特性
3. 持续集成验证机制
4. 最终形成完整实现

### 1.4 符号约定

| 符号 | 含义 |
|:-----|:-----|
| `Γ ⊢ e : τ` | 在上下文Γ中，表达式e具有类型τ |
| `e ⇓ v` | 表达式e求值为值v |
| `e ⟶ e'` | 表达式e一步归约为e' |
| `⟦τ⟧` | 类型τ的指称 |

---

## 2. 项目设置

### 2.1 环境配置

#### 安装Lean 4工具链

```bash
# 安装elan (Lean版本管理器)
curl https://raw.githubusercontent.com/leanprover/elan/master/elan-init.sh -sSf | sh

# 安装最新稳定版Lean 4
elan toolchain install stable
elan default stable

# 验证安装
lean --version  # 输出: Lean (version 4.x.x, commit ...)
```

#### VS Code配置

安装以下扩展：

- `leanprover.lean4` - 官方Lean 4扩展
- 推荐配置 `.vscode/settings.json`:

```json
{
  "lean4.alwaysAskBuild": false,
  "lean4.autofocusOutput": false,
  "lean4.infoview.showExpectedType": true,
  "lean4.infoview.showGoalNames": true,
  "editor.semanticHighlighting.enabled": true
}
```

### 2.2 项目结构

标准语言实现项目布局：

```
MyLang/
├── lakefile.toml           # 项目配置
├── lean-toolchain          # Lean版本锁定
├── Main.lean              # 入口点
├── MyLang/
│   ├── Basic.lean         # 基础定义
│   ├── Lexer.lean         # 词法分析器
│   ├── Parser.lean        # 语法分析器
│   ├── AST.lean           # 抽象语法树
│   ├── Types.lean         # 类型定义
│   ├── TypeChecker.lean   # 类型检查器
│   ├── Evaluator.lean     # 求值器
│   ├── Environment.lean   # 环境管理
│   └── Utils.lean         # 工具函数
├── MyLang/Tests/
│   ├── LexerTests.lean    # 词法测试
│   ├── ParserTests.lean   # 语法测试
│   ├── TypeTests.lean     # 类型测试
│   └── EvalTests.lean     # 求值测试
└── examples/              # 示例程序
    ├── hello.ml
    ├── factorial.ml
    └── list_ops.ml
```

### 2.3 依赖管理

#### lakefile.toml配置

```toml
name = "MyLang"
version = "0.1.0"
defaultTargets = ["MyLang"]

[[lean_lib]]
name = "MyLang"

[[exe]]
name = "mylang"
root = "Main"

[dependencies]
# 标准库
std = { git = "https://github.com/leanprover/std4", rev = "..." }

# 可选: 测试框架
batteries = { git = "https://github.com/leanprover-community/batteries", rev = "..." }
```

#### 构建命令

```bash
# 构建项目
lake build

# 运行测试
lake test

# 构建可执行文件
lake exe mylang

# 清理构建产物
lake clean
```

### 2.4 测试框架

#### 使用Lean内置测试

```lean
-- MyLang/Tests/Basic.lean
namespace MyLang.Tests

def assertEq {α : Type} [BEq α] [ToString α] (actual expected : α) : IO Unit := do
  if actual != expected then
    throw <| IO.userError s!"Expected {expected}, got {actual}"

def testGroup (name : String) (tests : List (IO Unit)) : IO Unit := do
  IO.println s!"\n=== {name} ==="
  for test in tests do
    try
      test
      IO.println "✓ passed"
    catch e =>
      IO.println s!"✗ failed: {e}"

end MyLang.Tests
```

#### 示例测试用例

```lean
-- MyLang/Tests/LexerTests.lean
import MyLang.Tests.Basic
import MyLang.Lexer

open MyLang.Tests
open MyLang.Lexer

def lexerTests : List (IO Unit) := [
  (do assertEq (tokenize "42") [Token.number 42]),
  (do assertEq (tokenize "x") [Token.ident "x"]),
  (do assertEq (tokenize "+") [Token.plus]),
  (do assertEq (tokenize "let x = 1")
       [Token.let, Token.ident "x", Token.eq, Token.number 1])
]

def runLexerTests : IO Unit :=
  testGroup "Lexer Tests" lexerTests
```

---

## 3. 词法分析

### 3.1 Token定义

Token是词法分析的基本输出单元：

```lean
-- MyLang/Lexer.lean
namespace MyLang

/-- Token类型定义 -/
inductive Token : Type
  | ident    : String → Token      -- 标识符: x, foo, maxValue
  | number   : Int → Token         -- 整数: 42, -7
  | string   : String → Token      -- 字符串: "hello"
  | bool     : Bool → Token        -- 布尔: true, false
  | unit     : Token               -- ()

  -- 关键字
  | let      : Token
  | in       : Token
  | fun      : Token
  | if       : Token
  | then     : Token
  | else     : Token
  | match    : Token
  | with     : Token

  -- 运算符
  | plus     : Token               -- +
  | minus    : Token               -- -
  | star     : Token               -- *
  | slash    : Token               -- /
  | eq       : Token               -- =
  | eqeq     : Token               -- ==
  | neq      : Token               -- !=
  | lt       : Token               -- <
  | le       : Token               -- <=
  | gt       : Token               -- >
  | ge       : Token               -- >=
  | arrow    : Token               -- ->
  | colon    : Token               -- :
  | dcolon   : Token               -- ::

  -- 分隔符
  | lparen   : Token               -- (
  | rparen   : Token               -- )
  | lbrace   : Token               -- {
  | rbrace   : Token               -- }
  | lbracket : Token               -- [
  | rbracket : Token               -- ]
  | comma    : Token               -- ,
  | semicolon: Token               -- ;
  | dot      : Token               -- .
  | pipe     : Token               -- |

  -- 特殊
  | eof      : Token               -- 文件结束
  | error    : String → Token      -- 错误标记
  deriving Repr, BEq

end MyLang
```

### 3.2 词法规则

#### 词法分析器状态

```lean
-- MyLang/Lexer.lean (续)
namespace MyLang.Lexer

/-- 位置信息 -/
structure Position where
  line   : Nat
  column : Nat
  offset : Nat
  deriving Repr, BEq

def Position.initial : Position := ⟨1, 1, 0⟩

/-- 带位置的Token -/
structure PosToken where
  token : Token
  start : Position
  stop  : Position
  deriving Repr, BEq

/-- 词法分析器状态 -/
structure LexerState where
  input  : String
  pos    : Position
  tokens : List PosToken
  deriving Repr

def LexerState.init (input : String) : LexerState :=
  { input, pos := Position.initial, tokens := [] }

end MyLang.Lexer
```

#### 核心词法分析函数

```lean
-- MyLang/Lexer.lean (续)
namespace MyLang.Lexer

/-- 字符分类 -/
def isWhitespace (c : Char) : Bool :=
  c == ' ' || c == '\t' || c == '\n' || c == '\r'

def isDigit (c : Char) : Bool :=
  '0' ≤ c && c ≤ '9'

def isAlpha (c : Char) : Bool :=
  ('a' ≤ c && c ≤ 'z') || ('A' ≤ c && c ≤ 'Z')

def isIdentStart (c : Char) : Bool :=
  isAlpha c || c == '_'

def isIdentChar (c : Char) : Bool :=
  isIdentStart c || isDigit c

/-- 当前字符 -/
def LexerState.current (s : LexerState) : Option Char :=
  s.input.get? s.pos.offset

/-- 前进到下一个字符 -/
def LexerState.advance (s : LexerState) : LexerState :=
  match s.current with
  | none => s
  | some '\n' =>
    { s with
      pos := ⟨s.pos.line + 1, 1, s.pos.offset + 1⟩
    }
  | some _ =>
    { s with
      pos := ⟨s.pos.line, s.pos.column + 1, s.pos.offset + 1⟩
    }

/-- 跳过空白字符 -/
def skipWhitespace (s : LexerState) : LexerState :=
  match s.current with
  | some c => if isWhitespace c then skipWhitespace s.advance else s
  | none   => s

/-- 读取数字 -/
partial def readNumber (s : LexerState) (acc : String) : LexerState × Token :=
  match s.current with
  | some c =>
    if isDigit c then
      readNumber s.advance (acc.push c)
    else
      (s, Token.number (acc.toInt!))
  | none   => (s, Token.number (acc.toInt!))

/-- 读取标识符/关键字 -/
partial def readIdent (s : LexerState) (acc : String) : LexerState × Token :=
  match s.current with
  | some c =>
    if isIdentChar c then
      readIdent s.advance (acc.push c)
    else
      (s, matchKeyword acc)
  | none   => (s, matchKeyword acc)

end MyLang.Lexer
```

#### 关键字映射

```lean
-- MyLang/Lexer.lean (续)
namespace MyLang.Lexer

/-- 将字符串映射到关键字Token -/
def matchKeyword (s : String) : Token :=
  match s with
  | "let"    => Token.let
  | "in"     => Token.in
  | "fun"    => Token.fun
  | "if"     => Token.if
  | "then"   => Token.then
  | "else"   => Token.else
  | "match"  => Token.match
  | "with"   => Token.with
  | "true"   => Token.bool true
  | "false"  => Token.bool false
  | "unit"   => Token.unit
  | _        => Token.ident s

end MyLang.Lexer
```

### 3.3 位置信息与错误报告

```lean
-- MyLang/Lexer.lean (续)
namespace MyLang.Lexer

/-- 词法错误类型 -/
inductive LexError where
  | unexpectedChar : Char → Position → LexError
  | unclosedString : Position → LexError
  | invalidEscape  : String → Position → LexError
  | unexpectedEOF  : Position → LexError
  deriving Repr

def LexError.toString : LexError → String
  | .unexpectedChar c pos =>
    s!"Unexpected character '{c}' at {pos.line}:{pos.column}"
  | .unclosedString pos =>
    s!"Unclosed string literal at {pos.line}:{pos.column}"
  | .invalidEscape s pos =>
    s!"Invalid escape sequence '\{s}' at {pos.line}:{pos.column}"
  | .unexpectedEOF pos =>
    s!"Unexpected end of file at {pos.line}:{pos.column}"

/-- 词法分析结果 -/
structure LexResult where
  tokens : List PosToken
  errors : List LexError
  deriving Repr

end MyLang.Lexer
```

#### 完整的tokenize函数

```lean
-- MyLang/Lexer.lean (续)
namespace MyLang.Lexer

/-- 读取字符串字面量 (简化版) -/
partial def readString (s : LexerState) (acc : String) : LexerState × Token :=
  match s.current with
  | none => (s, Token.error "unclosed string")
  | some '"' => (s.advance, Token.string acc)
  | some c   => readString s.advance (acc.push c)

/-- 主tokenize函数 -/
partial def tokenizeAux (s : LexerState) (errors : List LexError)
    : LexerState × List LexError :=
  let s := skipWhitespace s
  let startPos := s.pos

  match s.current with
  | none => (s, errors)  -- 结束
  | some c =>
    -- 单字符Token
    let singleCharTokens : List (Char × Token) := [
      ('+', Token.plus), ('-', Token.minus), ('*', Token.star),
      ('/', Token.slash), ('=', Token.eq), ('<', Token.lt),
      ('>', Token.gt), ('(', Token.lparen), (')', Token.rparen),
      ('{', Token.lbrace), ('}', Token.rbrace), ('[', Token.lbracket),
      (']', Token.rbracket), (',', Token.comma), (';', Token.semicolon),
      ('.', Token.dot), ('|', Token.pipe)
    ]

    match singleCharTokens.find? (λ (c', _) => c' == c) with
    | some (_, tok) =>
      let s' := s.advance
      let posTok := { token := tok, start := startPos, stop := s'.pos : PosToken }
      tokenizeAux { s' with tokens := posTok :: s.tokens } errors
    | none =>
      -- 双字符Token
      match s.current, (s.input.get? (s.pos.offset + 1)) with
      | some '=', some '=' =>
        let s' := s.advance.advance
        let posTok := { token := Token.eqeq, start := startPos, stop := s'.pos : PosToken }
        tokenizeAux { s' with tokens := posTok :: s.tokens } errors
      | some ':', some ':' =>
        let s' := s.advance.advance
        let posTok := { token := Token.dcolon, start := startPos, stop := s'.pos : PosToken }
        tokenizeAux { s' with tokens := posTok :: s.tokens } errors
      | some '-', some '>' =>
        let s' := s.advance.advance
        let posTok := { token := Token.arrow, start := startPos, stop := s'.pos : PosToken }
        tokenizeAux { s' with tokens := posTok :: s.tokens } errors
      | _ =>
        -- 数字
        if isDigit c then
          let (s', tok) := readNumber s ""
          let posTok := { token := tok, start := startPos, stop := s'.pos : PosToken }
          tokenizeAux { s' with tokens := posTok :: s.tokens } errors
        -- 标识符/关键字
        else if isIdentStart c then
          let (s', tok) := readIdent s ""
          let posTok := { token := tok, start := startPos, stop := s'.pos : PosToken }
          tokenizeAux { s' with tokens := posTok :: s.tokens } errors
        -- 字符串
        else if c == '"' then
          let (s', tok) := readString s.advance ""
          let posTok := { token := tok, start := startPos, stop := s'.pos : PosToken }
          tokenizeAux { s' with tokens := posTok :: s.tokens } errors
        -- 错误
        else
          let err := LexError.unexpectedChar c s.pos
          tokenizeAux s.advance (err :: errors)

/-- 公开接口 -/
def tokenize (input : String) : LexResult :=
  let s := LexerState.init input
  let (s', errors) := tokenizeAux s []
  { tokens := s'.tokens.reverse, errors := errors.reverse }

end MyLang.Lexer
```

---

## 4. 语法分析

### 4.1 语法定义

#### 抽象语法树(AST)定义

```lean
-- MyLang/AST.lean
namespace MyLang.AST

/-- 类型 -/
inductive Ty : Type
  | int      : Ty
  | bool     : Ty
  | unit     : Ty
  | string   : Ty
  | arrow    : Ty → Ty → Ty      -- τ₁ → τ₂
  | list     : Ty → Ty           -- τ list
  | tuple    : List Ty → Ty      -- (τ₁, τ₂, ...)
  | var      : String → Ty       -- 类型变量(用于多态)
  deriving Repr, BEq, Inhabited

notation:60 τ₁ " ~> " τ₂ => Ty.arrow τ₁ τ₂

/-- 字面量 -/
inductive Literal : Type
  | intLit    : Int → Literal
  | boolLit   : Bool → Literal
  | stringLit : String → Literal
  | unitLit   : Literal
  deriving Repr, BEq, Inhabited

/-- 二元运算符 -/
inductive BinOp : Type
  | add | sub | mul | div
  | eq | ne | lt | le | gt | ge
  | and | or
  | cons  -- ::
  deriving Repr, BEq, Inhabited

/-- 一元运算符 -/
inductive UnOp : Type
  | neg | not
  deriving Repr, BEq, Inhabited

/-- 模式 -/
inductive Pattern : Type
  | pVar   : String → Pattern
  | pLit   : Literal → Pattern
  | pWild  : Pattern               -- _
  | pCons  : Pattern → Pattern → Pattern
  | pTuple : List Pattern → Pattern
  deriving Repr, BEq, Inhabited

/-- 表达式 -/
inductive Expr : Type
  | lit    : Literal → Expr
  | var    : String → Expr
  | binOp  : BinOp → Expr → Expr → Expr
  | unOp   : UnOp → Expr → Expr
  | app    : Expr → Expr → Expr
  | lambda : String → Option Ty → Expr → Expr
  | letIn  : String → Expr → Expr → Expr
  | letRec : String → Expr → Expr → Expr
  | ifThenElse : Expr → Expr → Expr → Expr
  | matchExpr  : Expr → List (Pattern × Expr) → Expr
  | tuple  : List Expr → Expr
  | list   : List Expr → Expr
  | ascribe : Expr → Ty → Expr    -- type annotation: e : τ
  deriving Repr, Inhabited

/-- 顶层声明 -/
inductive Decl : Type
  | defn   : String → List String → Option Ty → Expr → Decl
  | defRec : String → List String → Option Ty → Expr → Decl
  | expr   : Expr → Decl
  deriving Repr, Inhabited

/-- 程序 -/
def Program : Type := List Decl

end MyLang.AST
```

### 4.2 解析器实现

#### 解析器单子

```lean
-- MyLang/Parser.lean
import MyLang.AST
import MyLang.Lexer

namespace MyLang.Parser

open MyLang.AST
open MyLang.Lexer

/-- 解析错误 -/
structure ParseError where
  message : String
  pos     : Position
  deriving Repr

/-- 解析器状态 -/
structure ParserState where
  tokens : List PosToken
  pos    : Nat
  deriving Repr

def ParserState.current (s : ParserState) : Option PosToken :=
  s.tokens.get? s.pos

def ParserState.peek (s : ParserState) : Option Token :=
  s.current.map (·.token)

def ParserState.advance (s : ParserState) : ParserState :=
  { s with pos := s.pos + 1 }

/-- 解析器单子 -/
def Parser (α : Type) : Type :=
  ParserState → Except ParseError (α × ParserState)

instance : Monad Parser where
  pure a := λ s => .ok (a, s)
  bind m f := λ s =>
    match m s with
    | .ok (a, s') => f a s'
    | .error e    => .error e

instance : Alternative Parser where
  failure := λ s => .error { message := "parse failed", pos := ⟨0,0,0⟩ }
  orElse p q := λ s =>
    match p s with
    | .ok r => .ok r
    | .error _ => q () s

/-- 获取当前位置 -/
def getPos : Parser Position :=
  λ s => .ok ((s.current.map (·.start)).getD ⟨0,0,0⟩, s)

/-- 报错 -/
def fail (msg : String) : Parser α :=
  λ s =>
    let pos := (s.current.map (·.start)).getD ⟨0,0,0⟩
    .error { message := msg, pos }

end MyLang.Parser
```

#### 基本解析器组合子

```lean
-- MyLang/Parser.lean (续)
namespace MyLang.Parser

/-- 消耗特定Token -/
def expect (t : Token) : Parser Unit :=
  λ s =>
    match s.peek with
    | some t' =>
      if t == t' then
        .ok ((), s.advance)
      else
        let pos := (s.current.map (·.start)).getD ⟨0,0,0⟩
        .error { message := s!"Expected {repr t}, got {repr t'}", pos }
    | none =>
      .error { message := s!"Expected {repr t}, got EOF", pos := ⟨0,0,0⟩ }

/-- 条件解析 -/
def satisfy (pred : Token → Bool) (desc : String) : Parser Token :=
  λ s =>
    match s.peek with
    | some t =>
      if pred t then
        .ok (t, s.advance)
      else
        let pos := (s.current.map (·.start)).getD ⟨0,0,0⟩
        .error { message := s!"Expected {desc}, got {repr t}", pos }
    | none =>
      .error { message := s!"Expected {desc}, got EOF", pos := ⟨0,0,0⟩ }

/-- 可选解析 -/
def optional (p : Parser α) : Parser (Option α) :=
  λ s =>
    match p s with
    | .ok (a, s') => .ok (some a, s')
    | .error _    => .ok (none, s)

/-- 重复解析(0次或多次) -/
partial def many (p : Parser α) : Parser (List α) :=
  λ s =>
    match p s with
    | .error _    => .ok ([], s)
    | .ok (a, s') =>
      match many p s' with
      | .ok (as, s'') => .ok (a :: as, s'')
      | .error e      => .error e

/-- 重复解析(1次或多次) -/
def many1 (p : Parser α) : Parser (List α) := do
  let a ← p
  let as ← many p
  return a :: as

/-- 用分隔符分隔的列表 -/
partial def sepBy (p : Parser α) (sep : Parser β) : Parser (List α) :=
  λ s =>
    match p s with
    | .error _    => .ok ([], s)
    | .ok (a, s') =>
      let rec loop acc s :=
        match sep s with
        | .error _    => .ok (acc.reverse, s)
        | .ok (_, s') =>
          match p s' with
          | .error e    => .error e
          | .ok (a, s'') => loop (a :: acc) s''
      loop [a] s'

end MyLang.Parser
```

### 4.3 AST定义与优先级处理

#### 表达式解析(带优先级)

```lean
-- MyLang/Parser.lean (续)
namespace MyLang.Parser

mutual

/-- 解析表达式(入口点) -/
partial def parseExpr : Parser Expr :=
  parseLet

/-- 解析let表达式 -/
partial def parseLet : Parser Expr :=
  (do expect Token.let
      let x ← parseIdent
      expect Token.eq
      let e1 ← parseExpr
      expect Token.in
      let e2 ← parseExpr
      return Expr.letIn x e1 e2)
  <|> parseIf

/-- 解析if表达式 -/
partial def parseIf : Parser Expr :=
  (do expect Token.if
      let cond ← parseExpr
      expect Token.then
      let thenBranch ← parseExpr
      expect Token.else
      let elseBranch ← parseExpr
      return Expr.ifThenElse cond thenBranch elseBranch)
  <|> parseLambda

/-- 解析lambda -/
partial def parseLambda : Parser Expr :=
  (do expect Token.fun
      let x ← parseIdent
      let ty ← optional (do expect Token.colon; parseType)
      expect Token.arrow
      let body ← parseExpr
      return Expr.lambda x ty body)
  <|> parseOr

/-- 解析or (优先级: 最低) -/
partial def parseOr : Parser Expr :=
  parseLeftAssoc parseAnd (λ e1 e2 => Expr.binOp BinOp.or e1 e2) Token.ident "||"

/-- 解析and -/
partial def parseAnd : Parser Expr :=
  parseLeftAssoc parseComparison (λ e1 e2 => Expr.binOp BinOp.and e1 e2) Token.ident "&&"
  <|> parseComparison

/-- 解析比较 -/
partial def parseComparison : Parser Expr :=
  parseLeftAssoc parseCons parseCompOp Token.ident "cmp"
  <|> parseCons

partial def parseCompOp (e1 : Expr) : Parser Expr := do
  let op ← (expect Token.eqeq *> pure BinOp.eq)
       <|> (expect Token.neq *> pure BinOp.ne)
       <|> (expect Token.lt *> pure BinOp.lt)
       <|> (expect Token.le *> pure BinOp.le)
       <|> (expect Token.gt *> pure BinOp.gt)
       <|> (expect Token.ge *> pure BinOp.ge)
  let e2 ← parseCons
  return Expr.binOp op e1 e2

/-- 解析cons (::) -/
partial def parseCons : Parser Expr :=
  (do let e1 ← parseAdd
      expect Token.dcolon
      let e2 ← parseCons
      return Expr.binOp BinOp.cons e1 e2)
  <|> parseAdd

/-- 解析加减 -/
partial def parseAdd : Parser Expr :=
  parseLeftAssoc parseMul parseAddSubOp Token.ident "+-"

partial def parseAddSubOp (e1 : Expr) : Parser Expr := do
  let op ← (expect Token.plus *> pure BinOp.add)
       <|> (expect Token.minus *> pure BinOp.sub)
  let e2 ← parseMul
  return Expr.binOp op e1 e2

/-- 解析乘除 -/
partial def parseMul : Parser Expr :=
  parseLeftAssoc parseApp parseMulDivOp Token.ident "*/"

partial def parseMulDivOp (e1 : Expr) : Parser Expr := do
  let op ← (expect Token.star *> pure BinOp.mul)
       <|> (expect Token.slash *> pure BinOp.div)
  let e2 ← parseApp
  return Expr.binOp op e1 e2

/-- 解析函数应用 -/
partial def parseApp : Parser Expr := do
  let f ← parseAtomic
  let args ← many parseAtomic
  return args.foldl Expr.app f

/-- 解析原子表达式 -/
partial def parseAtomic : Parser Expr :=
  parseLiteral
  <|> parseVar
  <|> parseParens
  <|> parseList
  <|> parseTuple

/-- 辅助: 左结合二元运算 -/
partial def parseLeftAssoc (p : Parser Expr) (op : Expr → Expr → Expr)
    (tok : Parser Token) (desc : String) : Parser Expr := do
  let e1 ← p
  let rec loop e :=
    (do let _ ← tok
        let e2 ← p
        loop (op e e2))
    <|> pure e
  loop e1

end

end MyLang.Parser
```

#### 类型与辅助解析

```lean
-- MyLang/Parser.lean (续)
namespace MyLang.Parser

/-- 解析标识符 -/
def parseIdent : Parser String := do
  let t ← satisfy (λ t => match t with | Token.ident _ => true | _ => false) "identifier"
  match t with
  | Token.ident s => return s
  | _ => fail "expected identifier"

/-- 解析类型 -/
partial def parseType : Parser Ty :=
  parseArrowType

partial def parseArrowType : Parser Ty := do
  let t1 ← parseAtomicType
  (do expect Token.arrow
      let t2 ← parseArrowType
      return Ty.arrow t1 t2)
  <|> pure t1

partial def parseAtomicType : Parser Ty :=
  (do let s ← parseIdent
      match s with
      | "Int"    => return Ty.int
      | "Bool"   => return Ty.bool
      | "Unit"   => return Ty.unit
      | "String" => return Ty.string
      | _        => return Ty.var s)
  <|> (do expect Token.lparen
          let t ← parseType
          expect Token.rparen
          return t)

/-- 解析字面量 -/
def parseLiteral : Parser Expr := do
  let t ← satisfy (λ t => match t with
    | Token.number _ | Token.bool _ | Token.string _ | Token.unit => true
    | _ => false) "literal"
  match t with
  | Token.number n => return Expr.lit (Literal.intLit n)
  | Token.bool b   => return Expr.lit (Literal.boolLit b)
  | Token.string s => return Expr.lit (Literal.stringLit s)
  | Token.unit     => return Expr.lit Literal.unitLit
  | _ => fail "expected literal"

/-- 解析变量 -/
def parseVar : Parser Expr := do
  let s ← parseIdent
  return Expr.var s

/-- 解析括号表达式 -/
def parseParens : Parser Expr := do
  expect Token.lparen
  let e ← parseExpr
  expect Token.rparen
  return e

/-- 解析列表 -/
def parseList : Parser Expr := do
  expect Token.lbracket
  let es ← sepBy parseExpr (expect Token.comma)
  expect Token.rbracket
  return Expr.list es

/-- 解析元组 -/
def parseTuple : Parser Expr := do
  expect Token.lparen
  let e ← parseExpr
  expect Token.comma
  let es ← many1 (expect Token.comma *> parseExpr)
  expect Token.rparen
  return Expr.tuple (e :: es)

/-- 主解析函数 -/
def parse (input : String) : Except ParseError Program :=
  let tokens := (tokenize input).tokens
  let s := { tokens, pos := 0 }
  match parseProgram s with
  | .ok (prog, s') =>
    match s'.peek with
    | some t => .error { message := s!"Unexpected token {repr t}", pos := ⟨0,0,0⟩ }
    | none   => .ok prog
  | .error e => .error e

partial def parseProgram : Parser Program :=
  many parseDecl

partial def parseDecl : Parser Decl :=
  (do expect Token.let
      let rec? ← (optional (expect Token.ident *> pure ()))
      let name ← parseIdent
      let params ← many parseIdent
      expect Token.eq
      let body ← parseExpr
      return Decl.defn name params none body)
  <|> (do let e ← parseExpr
          return Decl.expr e)

end MyLang.Parser
```

---

## 5. 类型检查器

### 5.1 类型表示

```lean
-- MyLang/Types.lean
import MyLang.AST

namespace MyLang.Types

open MyLang.AST

/-- 类型方案(用于多态let) -/
structure Scheme where
  vars : List String  -- 泛型变量
  type : Ty
  deriving Repr, BEq

def Scheme.instantiate (s : Scheme) (freshVars : List Ty) : Ty :=
  if s.vars.length != freshVars.length then
    s.type
  else
    let subst := List.zip s.vars freshVars |>.foldr (λ (v, t) acc =>
      λ (t' : Ty) => match t' with
        | Ty.var n => if n == v then t else Ty.var n
        | _ => t') id
  subst s.type

/-- 自由类型变量 -/
def Ty.ftv : Ty → List String
  | Ty.var n    => [n]
  | Ty.arrow t1 t2 => t1.ftv ++ t2.ftv
  | Ty.list t   => t.ftv
  | Ty.tuple ts => ts.bind Ty.ftv
  | _           => []

def Scheme.ftv (s : Scheme) : List String :=
  s.type.ftv.filter (λ v => !(s.vars.contains v))

end MyLang.Types
```

### 5.2 上下文管理

```lean
-- MyLang/TypeChecker.lean
import MyLang.Types

namespace MyLang.TypeChecker

open MyLang.AST
open MyLang.Types

/-- 类型上下文 -/
def TypeContext := List (String × Scheme)
  deriving Repr, Inhabited

def TypeContext.empty : TypeContext := []

def TypeContext.extend (ctx : TypeContext) (x : String) (s : Scheme) : TypeContext :=
  (x, s) :: ctx

def TypeContext.lookup (ctx : TypeContext) (x : String) : Option Scheme :=
  ctx.lookup x

/-- 替换: 类型变量 -> 类型 -/
def Subst := List (String × Ty)
  deriving Repr

def Subst.empty : Subst := []

def Subst.apply (σ : Subst) (t : Ty) : Ty :=
  match t with
  | Ty.var n    => match σ.lookup n with | some t' => t' | none => Ty.var n
  | Ty.arrow t1 t2 => Ty.arrow (σ.apply t1) (σ.apply t2)
  | Ty.list t   => Ty.list (σ.apply t)
  | Ty.tuple ts => Ty.tuple (ts.map (Subst.apply σ))
  | _           => t

def Subst.compose (σ1 σ2 : Subst) : Subst :=
  let σ2' := σ2.map (λ (v, t) => (v, σ1.apply t))
  σ1 ++ σ2'

end MyLang.TypeChecker
```

### 5.3 推导算法

```lean
-- MyLang/TypeChecker.lean (续)
namespace MyLang.TypeChecker

/-- 类型变量生成器 -/
structure TypeGen where
  counter : Nat
  deriving Repr

def TypeGen.new : TypeGen := ⟨0⟩

def TypeGen.fresh (g : TypeGen) : (String × TypeGen) :=
  let name := s!"t{g.counter}"
  (name, ⟨g.counter + 1⟩)

def TypeGen.freshTy (g : TypeGen) : (Ty × TypeGen) :=
  let (name, g') := g.fresh
  (Ty.var name, g')

/-- 类型推导状态 -/
structure InferState where
  gen  : TypeGen
  subst : Subst
  deriving Repr

def InferState.init : InferState := ⟨TypeGen.new, []⟩

/-- 类型推导结果 -/
def InferResult := Except TypeError (Ty × Subst × TypeGen)
  deriving Repr

/-- 类型错误 -/
inductive TypeError where
  | unificationError : Ty → Ty → TypeError
  | undefinedVar     : String → TypeError
  | infiniteType     : String → Ty → TypeError
  | typeMismatch     : String → Ty → Ty → TypeError
  | ambiguousType    : Expr → TypeError
  deriving Repr

def TypeError.toString : TypeError → String
  | .unificationError t1 t2 =>
    s!"Cannot unify {repr t1} with {repr t2}"
  | .undefinedVar x =>
    s!"Undefined variable: {x}"
  | .infiniteType v t =>
    s!"Infinite type: {v} occurs in {repr t}"
  | .typeMismatch ctx expected actual =>
    s!"Type mismatch in {ctx}: expected {repr expected}, got {repr actual}"
  | .ambiguousType e =>
    s!"Ambiguous type for expression: {repr e}"

end MyLang.TypeChecker
```

#### 合一算法

```lean
-- MyLang/TypeChecker.lean (续)
namespace MyLang.TypeChecker

/-- 判断类型变量是否出现在类型中(occurs check) -/
def occursIn (v : String) : Ty → Bool
  | Ty.var n    => v == n
  | Ty.arrow t1 t2 => occursIn v t1 || occursIn v t2
  | Ty.list t   => occursIn v t
  | Ty.tuple ts => ts.any (occursIn v)
  | _           => false

/-- 合一算法 -/
def unify (t1 t2 : Ty) : Except TypeError Subst :=
  match t1, t2 with
  | Ty.int, Ty.int   => .ok []
  | Ty.bool, Ty.bool => .ok []
  | Ty.unit, Ty.unit => .ok []
  | Ty.string, Ty.string => .ok []
  | Ty.var n1, Ty.var n2 =>
    if n1 == n2 then .ok [] else .ok [(n1, Ty.var n2)]
  | Ty.var n, t | t, Ty.var n =>
    if occursIn n t then
      .error (TypeError.infiniteType n t)
    else
      .ok [(n, t)]
  | Ty.arrow a1 r1, Ty.arrow a2 r2 =>
    do
      let σ1 ← unify a1 a2
      let σ2 ← unify (σ1.apply r1) (σ1.apply r2)
      return σ2.compose σ1
  | Ty.list t1', Ty.list t2' =>
    unify t1' t2'
  | Ty.tuple ts1, Ty.tuple ts2 =>
    if ts1.length != ts2.length then
      .error (TypeError.unificationError t1 t2)
    else
      unifyMany ts1 ts2
  | _, _ =>
    .error (TypeError.unificationError t1 t2)
where
  unifyMany : List Ty → List Ty → Except TypeError Subst
    | [], [] => .ok []
    | t1 :: ts1, t2 :: ts2 => do
      let σ1 ← unify t1 t2
      let σ2 ← unifyMany (ts1.map (σ1.apply)) (ts2.map (σ1.apply))
      return σ2.compose σ1
    | _, _ => .error (TypeError.unificationError t1 t2)

end MyLang.TypeChecker
```

#### 主类型推导算法

```lean
-- MyLang/TypeChecker.lean (续)
namespace MyLang.TypeChecker

mutual

/-- 泛化: 将类型转换为类型方案 -/
partial def generalize (ctx : TypeContext) (t : Ty) : Scheme :=
  let ctxVars := ctx.bind (λ (_, s) => s.ftv)
  let typeVars := t.ftv
  let genVars := typeVars.filter (λ v => !ctxVars.contains v)
  { vars := genVars, type := t }

/-- 实例化: 将类型方案转换为类型 -/
partial def instantiate (g : TypeGen) (s : Scheme) : (Ty × TypeGen) :=
  let (freshVars, g') := genFresh g s.vars.length []
  (applySubst freshVars s.type, g')
where
  genFresh g 0 acc := (acc.reverse, g)
  genFresh g (n+1) acc :=
    let (t, g') := g.freshTy
    genFresh g' n (t :: acc)

  applySubst freshVars ty :=
    let subst := List.zip s.vars freshVars
    subst.foldr (λ (v, t) t' =>
      match t' with
      | Ty.var n => if n == v then t else Ty.var n
      | _ => t') ty

/-- 主类型推导 -/
partial def infer (ctx : TypeContext) (e : Expr) (g : TypeGen)
    : Except TypeError (Ty × Subst × TypeGen) :=
  match e with
  | Expr.lit l => match l with
    | Literal.intLit _    => .ok (Ty.int, [], g)
    | Literal.boolLit _   => .ok (Ty.bool, [], g)
    | Literal.stringLit _ => .ok (Ty.string, [], g)
    | Literal.unitLit     => .ok (Ty.unit, [], g)

  | Expr.var x =>
    match ctx.lookup x with
    | some scheme =>
      let (t, g') := instantiate g scheme
      .ok (t, [], g')
    | none => .error (TypeError.undefinedVar x)

  | Expr.lambda x tyAnnot body => do
    let (argTy, g1) := match tyAnnot with
      | some t => (t, g)
      | none   => g.freshTy
    let ctx' := ctx.extend x { vars := [], type := argTy }
    let (bodyTy, σ1, g2) ← infer ctx' body g1
    let funTy := Ty.arrow (σ1.apply argTy) bodyTy
    .ok (funTy, σ1, g2)

  | Expr.app f arg => do
    let (funTy, σ1, g1) ← infer ctx f g
    let (argTy, σ2, g2) ← infer (applyCtx σ1 ctx) arg g1
    let (retTy, g3) := g2.freshTy
    let σ3 ← unify (σ2.apply funTy) (Ty.arrow argTy retTy)
    let finalSubst := σ3.compose (σ2.compose σ1)
    .ok (σ3.apply retTy, finalSubst, g3)

  | Expr.letIn x e1 e2 => do
    let (t1, σ1, g1) ← infer ctx e1 g
    let t1' := σ1.apply t1
    let scheme := generalize (applyCtx σ1 ctx) t1'
    let ctx' := (applyCtx σ1 ctx).extend x scheme
    let (t2, σ2, g2) ← infer ctx' e2 g1
    .ok (t2, σ2.compose σ1, g2)

  | Expr.binOp op e1 e2 => do
    let (t1, σ1, g1) ← infer ctx e1 g
    let (t2, σ2, g2) ← infer (applyCtx σ1 ctx) arg g1
    let (resultTy, opSubst) ← inferBinOp op (σ2.apply t1) t2
    let finalSubst := opSubst.compose (σ2.compose σ1)
    .ok (resultTy, finalSubst, g2)

  | Expr.ifThenElse cond thenBranch elseBranch => do
    let (tCond, σ1, g1) ← infer ctx cond g
    let σ1' ← unify tCond Ty.bool
    let ctx' := applyCtx (σ1'.compose σ1) ctx
    let (tThen, σ2, g2) ← infer ctx' thenBranch g1
    let (tElse, σ3, g3) ← infer (applyCtx σ2 ctx') elseBranch g2
    let σ4 ← unify (σ3.apply tThen) tElse
    let finalTy := σ4.apply tElse
    let finalSubst := σ4.compose (σ3.compose (σ2.compose (σ1'.compose σ1)))
    .ok (finalTy, finalSubst, g3)

  | _ => .error (TypeError.ambiguousType e)

where
  applyCtx σ ctx := ctx.map (λ (x, s) => (x, { s with type := σ.apply s.type }))

  inferBinOp : BinOp → Ty → Ty → Except TypeError (Ty × Subst)
    | BinOp.add, Ty.int, Ty.int   => .ok (Ty.int, [])
    | BinOp.sub, Ty.int, Ty.int   => .ok (Ty.int, [])
    | BinOp.mul, Ty.int, Ty.int   => .ok (Ty.int, [])
    | BinOp.div, Ty.int, Ty.int   => .ok (Ty.int, [])
    | BinOp.eq, t1, t2            => do
      let σ ← unify t1 t2
      .ok (Ty.bool, σ)
    | BinOp.lt, Ty.int, Ty.int    => .ok (Ty.bool, [])
    | BinOp.and, Ty.bool, Ty.bool => .ok (Ty.bool, [])
    | BinOp.or, Ty.bool, Ty.bool  => .ok (Ty.bool, [])
    | op, t1, t2                  =>
      .error (TypeError.typeMismatch (repr op) t1 t2)

end

end MyLang.TypeChecker
```

### 5.4 错误处理

```lean
-- MyLang/TypeChecker.lean (续)
namespace MyLang.TypeChecker

/-- 带位置的类型错误 -/
structure TypeErrorWithPos where
  error : TypeError
  pos   : Option Position
  deriving Repr

/-- 类型检查结果 -/
structure TypeCheckResult where
  type   : Option Ty
  errors : List TypeErrorWithPos
  deriving Repr

/-- 顶层类型检查 -/
def typeCheck (ctx : TypeContext) (e : Expr) : TypeCheckResult :=
  match infer ctx e TypeGen.new with
  | .ok (t, σ, _) =>
    { type := some (σ.apply t), errors := [] }
  | .error e =>
    { type := none, errors := [{ error := e, pos := none }] }

/-- 类型检查声明 -/
def typeCheckDecl (ctx : TypeContext) (d : Decl)
    : (TypeContext × List TypeErrorWithPos) :=
  match d with
  | Decl.defn name params retTy body =>
    -- 创建带参数类型的上下文
    let paramTypes := params.map (λ _ => Ty.var "_")  -- 简化处理
    let ctxWithParams := params.foldl (λ ctx p =>
      ctx.extend p { vars := [], type := Ty.var "_" }) ctx

    match infer ctxWithParams body TypeGen.new with
    | .ok (t, _, _) =>
      let scheme := generalize ctx t
      (ctx.extend name scheme, [])
    | .error e =>
      (ctx, [{ error := e, pos := none }])

  | _ => (ctx, [])

end MyLang.TypeChecker
```


---

## 6. 操作语义实现

### 6.1 求值器

```lean
-- MyLang/Evaluator.lean
import MyLang.AST

namespace MyLang.Evaluator

open MyLang.AST

/-- 运行时值 -/
inductive Value where
  | vint    : Int → Value
  | vbool   : Bool → Value
  | vstring : String → Value
  | vunit   : Value
  | vclosure : Env → String → Expr → Value
  | vrecClosure : Env → String → String → Expr → Value  -- 递归闭包
  | vlist   : List Value → Value
  | vtuple  : List Value → Value
  deriving Repr, Inhabited

/-- 环境 -/
def Env := List (String × Value)
  deriving Repr, Inhabited

def Env.empty : Env := []

def Env.extend (env : Env) (x : String) (v : Value) : Env :=
  (x, v) :: env

def Env.lookup (env : Env) (x : String) : Option Value :=
  env.lookup x

def Env.update (env : Env) (x : String) (v : Value) : Env :=
  (x, v) :: env.filter (λ (y, _) => y != x)

/-- 求值错误 -/
inductive EvalError where
  | unboundVar      : String → EvalError
  | typeError       : String → Value → EvalError
  | divisionByZero  : EvalError
  | nonExhaustive   : EvalError
  | notAFunction    : Value → EvalError
  deriving Repr

def EvalError.toString : EvalError → String
  | .unboundVar x    => s!"Unbound variable: {x}"
  | .typeError op v  => s!"Type error in {op}: {repr v}"
  | .divisionByZero  => "Division by zero"
  | .nonExhaustive   => "Non-exhaustive pattern match"
  | .notAFunction v  => s!"Not a function: {repr v}"

end MyLang.Evaluator
```

#### 大步操作语义求值器

```lean
-- MyLang/Evaluator.lean (续)
namespace MyLang.Evaluator

mutual

/-- 求值表达式 -/
partial def eval (env : Env) (e : Expr) : Except EvalError Value :=
  match e with
  | Expr.lit l => evalLiteral l
  | Expr.var x =>
    match env.lookup x with
    | some v => .ok v
    | none   => .error (EvalError.unboundVar x)

  | Expr.lambda x _ body =>
    .ok (Value.vclosure env x body)

  | Expr.app f arg => do
    let vf ← eval env f
    match vf with
    | Value.vclosure env' x body =>
      let varg ← eval env arg
      eval (env'.extend x varg) body
    | Value.vrecClosure env' f x body =>
      let varg ← eval env arg
      let env'' := env'.extend f (Value.vrecClosure env' f x body)
      eval (env''.extend x varg) body
    | _ => .error (EvalError.notAFunction vf)

  | Expr.letIn x e1 e2 => do
    let v1 ← eval env e1
    eval (env.extend x v1) e2

  | Expr.letRec f (Expr.lambda x _ body) e2 => do
    let recVal := Value.vrecClosure env f x body
    eval (env.extend f recVal) e2

  | Expr.binOp op e1 e2 => do
    let v1 ← eval env e1
    let v2 ← eval env e2
    evalBinOp op v1 v2

  | Expr.unOp op e => do
    let v ← eval env e
    evalUnOp op v

  | Expr.ifThenElse cond t f => do
    let vcond ← eval env cond
    match vcond with
    | Value.vbool true  => eval env t
    | Value.vbool false => eval env f
    | _ => .error (EvalError.typeError "if condition" vcond)

  | Expr.matchExpr e cases => do
    let v ← eval env e
    evalMatch env v cases

  | Expr.tuple es => do
    let vs ← es.mapM (eval env)
    .ok (Value.vtuple vs)

  | Expr.list es => do
    let vs ← es.mapM (eval env)
    .ok (Value.vlist vs)

  | _ => .error (EvalError.typeError "expression" (Value.vunit))

partial def evalLiteral : Literal → Except EvalError Value
  | Literal.intLit n    => .ok (Value.vint n)
  | Literal.boolLit b   => .ok (Value.vbool b)
  | Literal.stringLit s => .ok (Value.vstring s)
  | Literal.unitLit     => .ok Value.vunit

partial def evalBinOp : BinOp → Value → Value → Except EvalError Value
  | BinOp.add, Value.vint n1, Value.vint n2 => .ok (Value.vint (n1 + n2))
  | BinOp.sub, Value.vint n1, Value.vint n2 => .ok (Value.vint (n1 - n2))
  | BinOp.mul, Value.vint n1, Value.vint n2 => .ok (Value.vint (n1 * n2))
  | BinOp.div, Value.vint n1, Value.vint n2 =>
    if n2 == 0 then .error EvalError.divisionByZero
    else .ok (Value.vint (n1 / n2))
  | BinOp.eq, v1, v2 => .ok (Value.vbool (beqValue v1 v2))
  | BinOp.lt, Value.vint n1, Value.vint n2 => .ok (Value.vbool (n1 < n2))
  | BinOp.le, Value.vint n1, Value.vint n2 => .ok (Value.vbool (n1 ≤ n2))
  | BinOp.gt, Value.vint n1, Value.vint n2 => .ok (Value.vbool (n1 > n2))
  | BinOp.ge, Value.vint n1, Value.vint n2 => .ok (Value.vbool (n1 ≥ n2))
  | BinOp.and, Value.vbool b1, Value.vbool b2 => .ok (Value.vbool (b1 && b2))
  | BinOp.or, Value.vbool b1, Value.vbool b2 => .ok (Value.vbool (b1 || b2))
  | BinOp.cons, v, Value.vlist vs => .ok (Value.vlist (v :: vs))
  | op, v1, v2 => .error (EvalError.typeError (repr op) v1)

partial def evalUnOp : UnOp → Value → Except EvalError Value
  | UnOp.neg, Value.vint n => .ok (Value.vint (-n))
  | UnOp.not, Value.vbool b => .ok (Value.vbool (!b))
  | op, v => .error (EvalError.typeError (repr op) v)

partial def evalMatch (env : Env) (v : Value) (cases : List (Pattern × Expr))
    : Except EvalError Value :=
  match cases with
  | [] => .error EvalError.nonExhaustive
  | (p, e) :: rest =>
    match matchPattern p v with
    | some bindings => eval (bindings.foldl (λ env' (x, v') => env'.extend x v') env) e
    | none => evalMatch env v rest

partial def matchPattern : Pattern → Value → Option (List (String × Value))
  | Pattern.pWild, _ => some []
  | Pattern.pVar x, v => some [(x, v)]
  | Pattern.pLit (Literal.intLit n1), Value.vint n2 =>
    if n1 == n2 then some [] else none
  | Pattern.pLit (Literal.boolLit b1), Value.vbool b2 =>
    if b1 == b2 then some [] else none
  | Pattern.pLit Literal.unitLit, Value.vunit => some []
  | Pattern.pCons p1 p2, Value.vlist (v :: vs) =>
    match matchPattern p1 v with
    | some b1 =>
      match matchPattern p2 (Value.vlist vs) with
      | some b2 => some (b1 ++ b2)
      | none    => none
    | none => none
  | Pattern.pTuple ps, Value.vtuple vs =>
    if ps.length != vs.length then none
    else matchPatterns ps vs
  | _, _ => none
where
  matchPatterns : List Pattern → List Value → Option (List (String × Value))
    | [], [] => some []
    | p :: ps, v :: vs => do
      let b1 ← matchPattern p v
      let b2 ← matchPatterns ps vs
      return b1 ++ b2
    | _, _ => none

partial def beqValue : Value → Value → Bool
  | Value.vint n1, Value.vint n2 => n1 == n2
  | Value.vbool b1, Value.vbool b2 => b1 == b2
  | Value.vstring s1, Value.vstring s2 => s1 == s2
  | Value.vunit, Value.vunit => true
  | Value.vlist vs1, Value.vlist vs2 =>
    vs1.length == vs2.length && (vs1.zip vs2).all (λ (v1, v2) => beqValue v1 v2)
  | Value.vtuple vs1, Value.vtuple vs2 =>
    vs1.length == vs2.length && (vs1.zip vs2).all (λ (v1, v2) => beqValue v1 v2)
  | _, _ => false

end

end MyLang.Evaluator
```

### 6.2 环境模型

```lean
-- MyLang/Environment.lean
import MyLang.Evaluator

namespace MyLang.Environment

open MyLang.Evaluator

/-- 词法作用域环境 -/
structure LexicalEnv where
  bindings : List (String × Value)
  parent   : Option LexicalEnv
  deriving Repr

def LexicalEnv.empty : LexicalEnv := ⟨[], none⟩

def LexicalEnv.extend (env : LexicalEnv) (x : String) (v : Value) : LexicalEnv :=
  { env with bindings := (x, v) :: env.bindings }

def LexicalEnv.lookup (env : LexicalEnv) (x : String) : Option Value :=
  match env.bindings.lookup x with
  | some v => some v
  | none   => env.parent.bind (·.lookup x)

/-- 动态作用域环境(较少使用) -/
structure DynamicEnv where
  stack : List (List (String × Value))
  deriving Repr

def DynamicEnv.empty : DynamicEnv := ⟨[[]]⟩

def DynamicEnv.pushFrame (env : DynamicEnv) : DynamicEnv :=
  { env with stack := [] :: env.stack }

def DynamicEnv.popFrame (env : DynamicEnv) : DynamicEnv :=
  match env.stack with
  | [] | [_] => env
  | _ :: rest => { env with stack := rest }

def DynamicEnv.define (env : DynamicEnv) (x : String) (v : Value) : DynamicEnv :=
  match env.stack with
  | [] => env
  | frame :: rest => { env with stack := ((x, v) :: frame) :: rest }

def DynamicEnv.lookup (env : DynamicEnv) (x : String) : Option Value :=
  env.stack.findSome? (λ frame => frame.lookup x)

end MyLang.Environment
```

### 6.3 闭包表示

```lean
-- MyLang/Evaluator.lean (续)
namespace MyLang.Evaluator

/-- 闭包优化表示 -/
structure Closure where
  code    : Expr          -- 函数体
  param   : String        -- 参数名
  env     : Env           -- 捕获的环境
  isRec   : Bool          -- 是否递归
  recName : Option String -- 递归函数名
  deriving Repr

def Closure.apply (c : Closure) (arg : Value) : Except EvalError Value :=
  let env' := c.env.extend c.param arg
  if c.isRec then
    match c.recName with
    | some f => eval (env'.extend f (Value.vclosure c.env c.param c.code)) c.code
    | none   => eval env' c.code
  else
    eval env' c.code

/-- 将表达式转换为优化闭包 -/
def mkClosure (env : Env) (param : String) (body : Expr) (isRec := false)
    (recName := Option.none) : Closure :=
  { code := body, param, env, isRec, recName }

/-- 值转换 -/
def Value.toClosure : Value → Option Closure
  | Value.vclosure env x body =>
    some { code := body, param := x, env, isRec := false, recName := none }
  | Value.vrecClosure env f x body =>
    some { code := body, param := x, env, isRec := true, recName := some f }
  | _ => none

end MyLang.Evaluator
```

### 6.4 尾递归优化(TCO)

```lean
-- MyLang/Evaluator.lean (续)
namespace MyLang.Evaluator

/-- 尾调用标记 -/
inductive TailStatus where
  | notTail    : TailStatus
  | isTail     : TailStatus
  | tailCall   : Env → Expr → TailStatus  -- 尾调用待执行
  deriving Repr

/-- 支持TCO的求值器 -/
partial def evalTCO (env : Env) (e : Expr) : Except EvalError Value :=
  evalTail env e TailStatus.notTail
where
  evalTail (env : Env) (e : Expr) (status : TailStatus) : Except EvalError Value :=
    match e with
    | Expr.lit l => evalLiteral l
    | Expr.var x =>
      match env.lookup x with
      | some v => .ok v
      | none   => .error (EvalError.unboundVar x)

    | Expr.lambda x _ body =>
      .ok (Value.vclosure env x body)

    | Expr.app f arg =>
      -- 标记为尾调用
      evalTail env (Expr.letIn "__arg" arg (Expr.app f (Expr.var "__arg")))
        (match status with | TailStatus.isTail => TailStatus.isTail | _ => TailStatus.notTail)

    | Expr.letIn x e1 e2 => do
      let v1 ← evalTail env e1 TailStatus.notTail
      evalTail (env.extend x v1) e2 status  -- 传递尾状态

    | Expr.letRec f (Expr.lambda x _ body) e2 =>
      let recVal := Value.vrecClosure env f x body
      evalTail (env.extend f recVal) e2 status

    | Expr.ifThenElse cond t f => do
      let vcond ← evalTail env cond TailStatus.notTail
      match vcond with
      | Value.vbool true  => evalTail env t status  -- 传递尾状态
      | Value.vbool false => evalTail env f status  -- 传递尾状态
      | _ => .error (EvalError.typeError "if condition" vcond)

    | _ => eval env e  -- 非尾调用使用普通求值

/-- 检测尾位置 -/
def isTailPosition (parent : Expr) (child : Expr) : Bool :=
  match parent with
  | Expr.lambda _ _ body => body == child
  | Expr.letIn _ _ e2    => e2 == child
  | Expr.letRec _ _ e2   => e2 == child
  | Expr.ifThenElse _ t f => t == child || f == child
  | _ => false

end MyLang.Evaluator
```

---

## 7. 验证与测试

### 7.1 单元测试

```lean
-- MyLang/Tests/UnitTests.lean
import MyLang.Tests.Basic
import MyLang.Lexer
import MyLang.Parser
import MyLang.TypeChecker
import MyLang.Evaluator

namespace MyLang.Tests

open MyLang.Lexer
open MyLang.Parser
open MyLang.AST
open MyLang.TypeChecker
open MyLang.Evaluator

/-- 词法测试 -/
def lexerTests : List (String × List Token) := [
  ("42", [Token.number 42, Token.eof]),
  ("x + y", [Token.ident "x", Token.plus, Token.ident "y", Token.eof]),
  ("let x = 1", [Token.let, Token.ident "x", Token.eq, Token.number 1, Token.eof]),
  ("fun x -> x + 1", [Token.fun, Token.ident "x", Token.arrow,
                       Token.ident "x", Token.plus, Token.number 1, Token.eof]),
  ("if true then 1 else 0", [Token.if_, Token.bool true, Token.then,
                              Token.number 1, Token.else_, Token.number 0, Token.eof])
]

def runLexerTests : IO Unit := do
  IO.println "=== Lexer Tests ==="
  for (input, expected) in lexerTests do
    let result := tokenize input
    let tokens := result.tokens.map (·.token)
    if tokens == expected then
      IO.println s!"✓ '{input}'"
    else
      IO.println s!"✗ '{input}': Expected {repr expected}, got {repr tokens}"

/-- 解析测试 -/
def parserTests : List (String × String) := [
  ("42", "Expr.lit (Literal.intLit 42)"),
  ("x", "Expr.var \"x\""),
  ("1 + 2", "binOp"),
  ("fun x -> x", "lambda"),
  ("let x = 1 in x + 2", "letIn")
]

def runParserTests : IO Unit := do
  IO.println "\n=== Parser Tests ==="
  for (input, desc) in parserTests do
    match parse input with
    | .ok _ => IO.println s!"✓ '{input}' → {desc}"
    | .error e => IO.println s!"✗ '{input}': {e.message}"

/-- 类型检查测试 -/
def typeCheckTests : List (String × Ty) := [
  ("42", Ty.int),
  ("true", Ty.bool),
  ("1 + 2", Ty.int),
  ("fun x -> x + 1", Ty.arrow Ty.int Ty.int),
  ("let x = 1 in x + 2", Ty.int)
]

def runTypeCheckTests : IO Unit := do
  IO.println "\n=== Type Check Tests ==="
  for (input, expected) in typeCheckTests do
    match parse input with
    | .ok [Decl.expr e] =>
      match infer [] e TypeGen.new with
      | .ok (t, σ, _) =>
        let actual := σ.apply t
        if actual == expected then
          IO.println s!"✓ '{input}' : {repr actual}"
        else
          IO.println s!"✗ '{input}': Expected {repr expected}, got {repr actual}"
      | .error e => IO.println s!"✗ '{input}': {e.toString}"
    | _ => IO.println s!"✗ '{input}': Parse error"

/-- 求值测试 -/
def evalTests : List (String × Value) := [
  ("42", Value.vint 42),
  ("1 + 2", Value.vint 3),
  ("fun x -> x + 1", Value.vclosure [] "x" (Expr.binOp BinOp.add (Expr.var "x") (Expr.lit (Literal.intLit 1)))),
  ("(fun x -> x + 1) 5", Value.vint 6)
]

def runEvalTests : IO Unit := do
  IO.println "\n=== Eval Tests ==="
  for (input, expected) in evalTests do
    match parse input with
    | .ok [Decl.expr e] =>
      match eval [] e with
      | .ok v =>
        if beqValue v expected then
          IO.println s!"✓ '{input}' ⇒ {repr v}"
        else
          IO.println s!"✗ '{input}': Expected {repr expected}, got {repr v}"
      | .error e => IO.println s!"✗ '{input}': {e.toString}"
    | _ => IO.println s!"✗ '{input}': Parse error"

/-- 运行所有单元测试 -/
def runAllUnitTests : IO Unit := do
  runLexerTests
  runParserTests
  runTypeCheckTests
  runEvalTests
  IO.println "\n=== All Tests Complete ==="

end MyLang.Tests
```

### 7.2 属性测试

```lean
-- MyLang/Tests/PropertyTests.lean
import MyLang.Tests.Basic
import MyLang.TypeChecker
import MyLang.Evaluator

namespace MyLang.Tests

open MyLang.AST
open MyLang.TypeChecker
open MyLang.Evaluator

/-- 简单表达式生成器(简化版) -/
inductive SimpleExpr where
  | eInt  : Int → SimpleExpr
  | eBool : Bool → SimpleExpr
  | eAdd  : SimpleExpr → SimpleExpr → SimpleExpr
  | eMul  : SimpleExpr → SimpleExpr → SimpleExpr
  deriving Repr

def SimpleExpr.toExpr : SimpleExpr → Expr
  | .eInt n  => Expr.lit (Literal.intLit n)
  | .eBool b => Expr.lit (Literal.boolLit b)
  | .eAdd e1 e2 => Expr.binOp BinOp.add (toExpr e1) (toExpr e2)
  | .eMul e1 e2 => Expr.binOp BinOp.mul (toExpr e1) (toExpr e2)

/-- 类型安全属性: 良类型表达式求值不失败 -/
def typeSafetyProperty (e : Expr) : Bool :=
  match infer [] e TypeGen.new with
  | .ok _ =>
    match eval [] e with
    | .ok _ => true
    | .error _ => false  -- 求值失败但类型检查通过(违反类型安全)
  | .error _ => true     -- 类型检查失败，不测试

/-- 求值确定性属性 -/
def evalDeterminismProperty (e : Expr) : Bool :=
  match eval [] e, eval [] e with
  | .ok v1, .ok v2 => beqValue v1 v2
  | .error e1, .error e2 => e1.toString == e2.toString
  | _, _ => false

/-- 类型保持属性 -/
def preservationProperty (e : Expr) : Bool :=
  match infer [] e TypeGen.new with
  | .ok (t, _, _) =>
    match eval [] e with
    | .ok v =>
      -- 简化的类型检查: 值应该与表达式类型一致
      match t, v with
      | Ty.int, Value.vint _ => true
      | Ty.bool, Value.vbool _ => true
      | Ty.string, Value.vstring _ => true
      | Ty.unit, Value.vunit => true
      | Ty.arrow _ _, Value.vclosure _ _ _ => true
      | _, _ => false
    | .error _ => true  -- 不求值时不测试
  | .error _ => true

/-- 运行属性测试(使用预定义用例) -/
def runPropertyTests : IO Unit := do
  IO.println "=== Property Tests ==="

  let testExprs : List Expr := [
    Expr.lit (Literal.intLit 42),
    Expr.binOp BinOp.add (Expr.lit (Literal.intLit 1)) (Expr.lit (Literal.intLit 2)),
    Expr.binOp BinOp.mul (Expr.lit (Literal.intLit 3)) (Expr.lit (Literal.intLit 4)),
    Expr.lit (Literal.boolLit true),
    Expr.ifThenElse (Expr.lit (Literal.boolLit true))
      (Expr.lit (Literal.intLit 1))
      (Expr.lit (Literal.intLit 0))
  ]

  let mut passed := 0
  let mut failed := 0

  for e in testExprs do
    if typeSafetyProperty e then
      passed := passed + 1
    else
      failed := failed + 1
      IO.println s!"Type safety failed for: {repr e}"

    if evalDeterminismProperty e then
      passed := passed + 1
    else
      failed := failed + 1
      IO.println s!"Eval determinism failed for: {repr e}"

    if preservationProperty e then
      passed := passed + 1
    else
      failed := failed + 1
      IO.println s!"Preservation failed for: {repr e}"

  IO.println s!"\nResults: {passed} passed, {failed} failed"

end MyLang.Tests
```

### 7.3 形式化验证

```lean
-- MyLang/Verification.lean
import MyLang.TypeChecker
import MyLang.Evaluator

namespace MyLang.Verification

open MyLang.AST
open MyLang.TypeChecker
open MyLang.Evaluator

/-- 值类型对应关系 -/
inductive ValueHasType : Value → Ty → Prop where
  | vInt   : ∀ n, ValueHasType (Value.vint n) Ty.int
  | vBool  : ∀ b, ValueHasType (Value.vbool b) Ty.bool
  | vString: ∀ s, ValueHasType (Value.vstring s) Ty.string
  | vUnit  : ValueHasType Value.vunit Ty.unit
  | vList  : ∀ vs t, (∀ v ∈ vs, ValueHasType v t) →
               ValueHasType (Value.vlist vs) (Ty.list t)
  | vTuple : ∀ vs ts, List.Forall₂ ValueHasType vs ts →
               ValueHasType (Value.vtuple vs) (Ty.tuple ts)
  | vClosure: ∀ env x body t1 t2,
                (∀ v, ValueHasType v t1 →
                 ∀ env',
                 ValueHasType (Value.vclosure env' x body) (Ty.arrow t1 t2))

/-- 环境类型对应 -/
def EnvHasType (env : Env) (ctx : TypeContext) : Prop :=
  ∀ x v, env.lookup x = some v →
  ∃ s, ctx.lookup x = some s ∧ ValueHasType v s.type

/-- 保持性定理陈述 -/
theorem preservation : ∀ e v env ctx t,
  infer ctx e TypeGen.new = Except.ok (t, [], _) →
  eval env e = Except.ok v →
  ValueHasType v t := by
  intro e v env ctx t h1 h2
  -- 实际证明需要归纳法
  sorry

/-- 进展性定理陈述 -/
theorem progress : ∀ e t,
  infer [] e TypeGen.new = Except.ok (t, [], _) →
  (∃ v, eval [] e = Except.ok v) ∨
  (∃ e', step e e') := by
  intro e t h
  -- 实际证明
  sorry

/-- 求值关系(小步语义) -/
inductive Step : Expr → Expr → Prop where
  | sLit   : ∀ l, Step (Expr.lit l) (Expr.lit l)
  | sAdd   : ∀ n1 n2, Step (Expr.binOp BinOp.add (Expr.lit (Literal.intLit n1))
                                     (Expr.lit (Literal.intLit n2)))
                           (Expr.lit (Literal.intLit (n1 + n2)))
  | sApp   : ∀ x body arg v,
               Step (Expr.app (Expr.lambda x none body) (Expr.lit arg))
                    (subst x (Expr.lit arg) body)
where
  subst : String → Expr → Expr → Expr
    | x, e, Expr.var y => if x == y then e else Expr.var y
    | x, e, Expr.lambda y ty body =>
        if x == y then Expr.lambda y ty body
        else Expr.lambda y ty (subst x e body)
    | x, e, Expr.app f arg => Expr.app (subst x e f) (subst x e arg)
    | _, _, e => e

/-- 求值多步关系 -/
inductive Steps : Expr → Expr → Prop where
  | refl : ∀ e, Steps e e
  | trans: ∀ e1 e2 e3, Step e1 e2 → Steps e2 e3 → Steps e1 e3

/-- Church-Rosser定理陈述 -/
theorem churchRosser : ∀ e e1 e2,
  Steps e e1 → Steps e e2 →
  ∃ e', Steps e1 e' ∧ Steps e2 e' := by
  intro e e1 e2 h1 h2
  -- 实际证明
  sorry

end MyLang.Verification
```

---

## 8. 扩展功能

### 8.1 多态类型

```lean
-- MyLang/Extensions/Polymorphism.lean
import MyLang.AST
import MyLang.Types

namespace MyLang.Extensions

open MyLang.AST
open MyLang.Types

/-- 显式多态类型 -/
inductive PolyTy where
  | mono    : Ty → PolyTy
  | forall_ : List String → Ty → PolyTy  -- ∀α.τ
  deriving Repr, BEq

/-- 多态表达式扩展 -/
inductive PolyExpr where
  | base    : Expr → PolyExpr
  | tyLambda: String → PolyExpr → PolyExpr  -- Λα.e
  | tyApp   : PolyExpr → Ty → PolyExpr      -- e[τ]
  | pack    : Ty → PolyExpr → PolyTy → PolyExpr  -- pack τ, e as ∃α.τ'
  | unpack  : String → String → PolyExpr → PolyExpr → PolyExpr  -- unpack α, x = e1 in e2
  deriving Repr

/-- 系统F类型检查 -/
partial def inferPoly (ctx : TypeContext) (e : PolyExpr) (g : TypeGen)
    : Except TypeError (PolyTy × Subst × TypeGen) :=
  match e with
  | PolyExpr.base e' => do
    let (t, σ, g') ← infer ctx e' g
    return (PolyTy.mono t, σ, g')

  | PolyExpr.tyLambda α e' => do
    let ctx' := ctx.filter (λ (_, s) => !(s.vars.contains α))
    let (pt, σ, g') ← inferPoly ctx' e' g
    match pt with
    | PolyTy.mono t =>
      if t.ftv.contains α then
        return (PolyTy.forall_ [α] t, σ, g')
      else
        return (PolyTy.mono t, σ, g')
    | PolyTy.forall_ αs t =>
      return (PolyTy.forall_ (α :: αs) t, σ, g')

  | PolyExpr.tyApp e' τ => do
    let (pt, σ, g') ← inferPoly ctx e' g
    match pt with
    | PolyTy.mono (Ty.arrow t1 t2) => do
      let σ' ← unify t1 τ
      return (PolyTy.mono (σ'.apply t2), σ'.compose σ, g')
    | PolyTy.forall_ (α :: αs) t =>
      let t' := instantiateType α τ t
      return (PolyTy.forall_ αs t', σ, g')
    | _ => .error (TypeError.typeMismatch "type application" Ty.unit Ty.unit)
where
  instantiateType (α : String) (τ : Ty) : Ty → Ty
    | Ty.var β => if α == β then τ else Ty.var β
    | Ty.arrow t1 t2 => Ty.arrow (instantiateType α τ t1) (instantiateType α τ t2)
    | Ty.list t => Ty.list (instantiateType α τ t)
    | Ty.tuple ts => Ty.tuple (ts.map (instantiateType α τ))
    | t => t

end MyLang.Extensions
```

### 8.2 效应系统

```lean
-- MyLang/Extensions/Effects.lean
import MyLang.AST

namespace MyLang.Extensions

open MyLang.AST

/-- 效应标签 -/
inductive Effect where
  | io       : Effect
  | exception: Effect
  | state    : Effect
  | nondet   : Effect  -- 非确定性
  | console  : Effect
  | file     : Effect
  | network  : Effect
  deriving Repr, BEq, Inhabited

/-- 带效应的类型 -/
structure EffectTy where
  ty     : Ty
  effects: List Effect
  deriving Repr, BEq

/-- 效应操作 -/
inductive EffectOp where
  | print    : Expr → EffectOp
  | read     : EffectOp
  | throw    : Expr → EffectOp
  | catch    : Expr → (String → Expr) → EffectOp
  | get      : EffectOp
  | put      : Expr → EffectOp
  | random   : EffectOp
  deriving Repr

/-- 效应检查 -/
def effectCheck : EffectOp → List Effect
  | EffectOp.print _    => [Effect.io, Effect.console]
  | EffectOp.read       => [Effect.io, Effect.console]
  | EffectOp.throw _    => [Effect.exception]
  | EffectOp.catch _ _  => [Effect.exception]
  | EffectOp.get        => [Effect.state]
  | EffectOp.put _      => [Effect.state]
  | EffectOp.random     => [Effect.nondet]

/-- 效应处理程序类型 -/
structure EffectHandler where
  ret   : String → Expr      -- return clause
  ops   : List (String × (List String) × Expr)  -- operation clauses
  deriving Repr

/-- 代数效应风格的处理 -/
inductive EffExpr where
  | pure   : Expr → EffExpr
  | op     : EffectOp → (Value → EffExpr) → EffExpr  -- 操作调用 + 续延
  | handle : EffExpr → EffectHandler → EffExpr
  deriving Repr

end MyLang.Extensions
```

### 8.3 模块系统

```lean
-- MyLang/Extensions/Modules.lean
import MyLang.AST

namespace MyLang.Extensions

open MyLang.AST

/-- 模块签名 -/
structure Signature where
  name    : String
  types   : List (String × Ty)
  values  : List (String × Ty)
  modules : List Signature
  deriving Repr

/-- 模块结构 -/
structure Module where
  name       : String
  imports    : List String  -- 导入的模块名
  exports    : List String  -- 导出的名称
  typeDefs   : List (String × Ty)
  values     : List (String × Expr)
  submodules : List Module
  deriving Repr

/-- 模块环境 -/
def ModuleEnv := List (String × Module)

def ModuleEnv.lookup (env : ModuleEnv) (name : String) : Option Module :=
  env.lookup name

/-- 模块类型检查 -/
structure ModuleCheckResult where
  signature : Signature
  errors    : List String
  deriving Repr

def checkModule (env : ModuleEnv) (m : Module) : ModuleCheckResult :=
  -- 检查导入
  let importErrors := m.imports.filterMap (λ imp =>
    if env.lookup imp |>.isSome then none
    else some s!"Unknown module: {imp}")

  -- 检查导出存在
  let allNames := m.typeDefs.map Prod.fst ++ m.values.map Prod.fst
  let exportErrors := m.exports.filterMap (λ exp =>
    if allNames.contains exp then none
    else some s!"Export not found: {exp}")

  -- 构建签名
  let sig : Signature := {
    name := m.name,
    types := m.typeDefs,
    values := m.exports.filterMap (λ exp =>
      m.values.lookup exp |>.map (λ _ => (exp, Ty.unit))),  -- 简化
    modules := []
  }

  { signature := sig, errors := importErrors ++ exportErrors }

/-- 模块组合 -/
def composeModules (m1 m2 : Module) : Module := {
  name := s!"{m1.name}.{m2.name}",
  imports := m1.imports ++ m2.imports,
  exports := m1.exports ++ m2.exports,
  typeDefs := m1.typeDefs ++ m2.typeDefs,
  values := m1.values ++ m2.values,
  submodules := m1.submodules ++ [m2] ++ m2.submodules
}

end MyLang.Extensions
```

### 8.4 FFI

```lean
-- MyLang/Extensions/FFI.lean
import MyLang.AST
import MyLang.Evaluator

namespace MyLang.Extensions

open MyLang.AST
open MyLang.Evaluator

/-- FFI类型映射 -/
inductive FFITy where
  | cInt     : FFITy
  | cBool    : FFITy
  | cString  : FFITy
  | cVoid    : FFITy
  | cPtr     : FFITy → FFITy
  | cFun     : List FFITy → FFITy → FFITy
  deriving Repr, BEq

/-- FFI声明 -/
structure FFIDecl where
  name      : String
  cName     : String
  argTypes  : List FFITy
  retType   : FFITy
  lib       : Option String  -- 库名
  deriving Repr

/-- 外部函数绑定 -/
inductive External where
  | cFun     : FFIDecl → External
  | leanFun  : FFIDecl → External
  | callback : (List Value → Except EvalError Value) → External
  deriving Repr

/-- 值与C类型转换 -/
def valueToC : Value → FFITy → Option String
  | Value.vint n,    FFITy.cInt    => some (toString n)
  | Value.vbool true, FFITy.cBool  => some "1"
  | Value.vbool false, FFITy.cBool => some "0"
  | Value.vstring s, FFITy.cString => some s!"\"{s}\""
  | _, _ => none

def cToValue : String → FFITy → Option Value
  | s, FFITy.cInt    => s.toInt?.map Value.vint
  | "0", FFITy.cBool => some (Value.vbool false)
  | "1", FFITy.cBool => some (Value.vbool true)
  | s, FFITy.cString => some (Value.vstring s)
  | _, _ => none

/-- FFI调用表达式 -/
structure FFICall where
  decl : FFIDecl
  args : List Expr
  deriving Repr

/-- 安全的FFI包装 -/
def safeFFICall (decl : FFIDecl) (args : List Value) : Except EvalError Value :=
  -- 检查参数数量
  if args.length != decl.argTypes.length then
    .error (EvalError.typeError "FFI call" (Value.vstring "arity mismatch"))
  else
    -- 转换参数
    let cArgs := (args.zip decl.argTypes).filterMap (λ (v, t) => valueToC v t)
    if cArgs.length != args.length then
      .error (EvalError.typeError "FFI call" (Value.vstring "type conversion failed"))
    else
      -- 模拟调用
      .ok (Value.vunit)  -- 实际实现需要调用外部函数

end MyLang.Extensions
```

---

## 9. 最佳实践

### 9.1 代码组织

```lean
-- MyLang/Utils/Organization.lean

namespace MyLang.Utils

/-- 模块组织原则 -/
structure ModuleGuidelines where
  /-- 每个模块专注于单一职责 -/
  singleResponsibility : Prop

  /-- 模块依赖形成有向无环图 -/
  noCircularDeps : Prop

  /-- 公开接口明确标记 -/
  explicitExports : Prop

/-- 推荐的项目结构 -/
def recommendedProjectStructure : String := ""
  MyLang/
  ├── Core/           -- 核心定义(最小依赖)
  │   ├── AST.lean
  │   └── Basic.lean
  ├── Frontend/       -- 前端(词法/语法)
  │   ├── Lexer.lean
  │   └── Parser.lean
  ├── Analysis/       -- 分析阶段
  │   ├── TypeChecker.lean
  │   └── Optimizer.lean
  ├── Backend/        -- 后端(求值/代码生成)
  │   ├── Evaluator.lean
  │   └── Compiler.lean
  ├── Utils/          -- 工具函数
  │   ├── Errors.lean
  │   └── PrettyPrint.lean
  └── Tests/          -- 测试
      ├── UnitTests.lean
      └── PropertyTests.lean

/-- 命名约定 -/
inductive NamingConvention where
  | types      : NamingConvention  -- PascalCase: Expr, TypeChecker
  | functions  : NamingConvention  -- camelCase: parseExpr, inferType
  | constants  : NamingConvention  -- UPPER_SNAKE: MAX_DEPTH
  | modules    : NamingConvention  -- PascalCase: Lexer.lean
  | locals     : NamingConvention  -- snake_case: local_var

/-- 导出控制 -/
structure ExportPolicy where
  /-- 默认私有，显式公开 -/
  privateByDefault : Bool

  /-- 使用export关键字组织公开API -/
  useExportKeyword : Bool

  /-- 公开API文档化 -/
  documentPublicAPI : Bool

end MyLang.Utils
```

### 9.2 性能优化

```lean
-- MyLang/Utils/Optimization.lean
import MyLang.AST

namespace MyLang.Utils

open MyLang.AST

/-- 常量折叠 -/
def constantFold : Expr → Expr
  | Expr.binOp op (Expr.lit l1) (Expr.lit l2) =>
    match evaluateLiteralOp op l1 l2 with
    | some l => Expr.lit l
    | none   => Expr.binOp op (Expr.lit l1) (Expr.lit l2)
  | Expr.binOp op e1 e2 =>
    let e1' := constantFold e1
    let e2' := constantFold e2
    match e1', e2' with
    | Expr.lit l1, Expr.lit l2 =>
      match evaluateLiteralOp op l1 l2 with
      | some l => Expr.lit l
      | none   => Expr.binOp op e1' e2'
    | _, _ => Expr.binOp op e1' e2'
  | Expr.letIn x e1 e2 =>
    let e1' := constantFold e1
    match e1' with
    | Expr.lit _ => constantFold (subst x e1' e2)  -- 内联常量
    | _ => Expr.letIn x e1' (constantFold e2)
  | e => e
where
  evaluateLiteralOp : BinOp → Literal → Literal → Option Literal
    | BinOp.add, Literal.intLit n1, Literal.intLit n2 => some (Literal.intLit (n1 + n2))
    | BinOp.sub, Literal.intLit n1, Literal.intLit n2 => some (Literal.intLit (n1 - n2))
    | BinOp.mul, Literal.intLit n1, Literal.intLit n2 => some (Literal.intLit (n1 * n2))
    | _, _, _ => none

  subst : String → Expr → Expr → Expr
    | x, e, Expr.var y => if x == y then e else Expr.var y
    | x, e, Expr.lambda y ty body =>
      if x == y then Expr.lambda y ty body
      else Expr.lambda y ty (subst x e body)
    | x, e, Expr.app f arg => Expr.app (subst x e f) (subst x e arg)
    | x, e, Expr.letIn y e1 e2 =>
      if x == y then Expr.letIn y (subst x e e1) e2
      else Expr.letIn y (subst x e e1) (subst x e e2)
    | _, _, e => e

/-- 尾递归优化检测 -/
def detectTailRecursion (f : String) : Expr → Bool
  | Expr.lambda _ _ body => isTailCall f body
  | _ => false
where
  isTailCall f : Expr → Bool
    | Expr.app (Expr.var g) _ => f == g
    | Expr.letIn _ _ e2 => isTailCall f e2
    | Expr.ifThenElse _ t e => isTailCall f t || isTailCall f e
    | _ => false

/-- 严格性分析 -/
def strictnessAnalysis : Expr → List String
  | Expr.lambda x _ body =>
    let used := collectVars body
    if used.contains x then [x] else []
  | _ => []
where
  collectVars : Expr → List String
    | Expr.var x => [x]
    | Expr.app f arg => collectVars f ++ collectVars arg
    | Expr.lambda x _ body => collectVars body |>.filter (· != x)
    | Expr.letIn x e1 e2 => collectVars e1 ++ (collectVars e2 |>.filter (· != x))
    | Expr.binOp _ e1 e2 => collectVars e1 ++ collectVars e2
    | _ => []

/-- 内联决策 -/
def shouldInline (body : Expr) (callSites : Nat) : Bool :=
  let size := exprSize body
  size ≤ 10 || (size ≤ 50 && callSites == 1)
where
  exprSize : Expr → Nat
    | Expr.lit _ => 1
    | Expr.var _ => 1
    | Expr.lambda _ _ body => 1 + exprSize body
    | Expr.app f arg => 1 + exprSize f + exprSize arg
    | Expr.letIn _ e1 e2 => 1 + exprSize e1 + exprSize e2
    | Expr.binOp _ e1 e2 => 1 + exprSize e1 + exprSize e2
    | _ => 1

end MyLang.Utils
```

### 9.3 错误处理

```lean
-- MyLang/Utils/Errors.lean

namespace MyLang.Utils

/-- 统一的错误类型 -/
inductive LangError where
  | lexError   : Position → String → LangError
  | parseError : Position → String → LangError
  | typeError  : Option Position → String → LangError
  | evalError  : String → LangError
  | ioError    : String → LangError
  | verifyError: String → LangError
  deriving Repr

def LangError.toString : LangError → String
  | .lexError pos msg =>
    s!"Lexical error at {pos.line}:{pos.column}: {msg}"
  | .parseError pos msg =>
    s!"Parse error at {pos.line}:{pos.column}: {msg}"
  | .typeError (some pos) msg =>
    s!"Type error at {pos.line}:{pos.column}: {msg}"
  | .typeError none msg =>
    s!"Type error: {msg}"
  | .evalError msg =>
    s!"Runtime error: {msg}"
  | .ioError msg =>
    s!"IO error: {msg}"
  | .verifyError msg =>
    s!"Verification error: {msg}"

/-- 错误累积 -/
structure ErrorContext where
  errors : List LangError
  maxErrors : Nat := 10
  deriving Repr

def ErrorContext.empty : ErrorContext := { errors := [] }

def ErrorContext.add (ctx : ErrorContext) (e : LangError) : ErrorContext :=
  if ctx.errors.length >= ctx.maxErrors then
    ctx
  else
    { ctx with errors := ctx.errors ++ [e] }

def ErrorContext.hasErrors (ctx : ErrorContext) : Bool :=
  !ctx.errors.isEmpty

/-- 错误恢复策略 -/
inductive RecoveryStrategy where
  | failFast    : RecoveryStrategy  -- 第一个错误停止
  | collectAll  : RecoveryStrategy  -- 收集所有错误
  | panicMode   : RecoveryStrategy  -- 跳过到同步点
  deriving Repr

/-- 带恢复的错误处理 -/
def withRecovery (strategy : RecoveryStrategy) (f : α → Except LangError β) (x : α)
    : (List β × List LangError) :=
  match strategy with
  | RecoveryStrategy.failFast =>
    match f x with
    | .ok v    => ([v], [])
    | .error e => ([], [e])
  | _ => ([], [])  -- 简化实现

end MyLang.Utils
```

### 9.4 文档编写

```lean
    -- MyLang/Utils/Documentation.lean

    namespace MyLang.Utils

    /-- 文档标准 -/
    structure DocStandard where
      /-- 模块头注释 -/
      moduleHeader : String → String

      /-- 函数文档 -/
      functionDoc  : String → List String → String → String

      /-- 类型文档 -/
      typeDoc      : String → String → String

    /-- 示例文档模板 -/
    def docTemplate : String := ""
      /-!
      # 模块名称

      模块简短描述。

      ## 主要功能

      - 功能1
      - 功能2

      ## 使用示例

      ```lean
      -- 示例代码
      ```

      ## 实现说明

        重要实现细节。
        -/

        /--
        函数简短描述。

        参数:

      - `param1`: 参数1描述
      - `param2`: 参数2描述

        返回:
        返回值描述

        示例:

        ```lean
        function arg1 arg2 -- 结果
        ```

        -/
        def function (param1 : Type) (param2 : Type) : ReturnType := ...

      /-- 文档生成配置 -/
      structure DocGenConfig where
        outputDir    : String
        includeTests : Bool
        includePrivates : Bool
        style        : DocStyle

      default

      inductive DocStyle where
        | markdown : DocStyle
        | html     : DocStyle
        | latex    : DocStyle

      end MyLang.Utils

```

---

## 10. 完整示例项目

### 10.1 项目概览

本节提供一个完整的微型函数式语言实现——**MiniML**。MiniML支持：

- 整数、布尔、字符串、单位类型
- 函数定义与应用
- let绑定与递归
- 条件表达式
- 简单模式匹配
- 列表操作

### 10.2 项目文件结构

```

MiniML/
├── lakefile.toml
├── lean-toolchain
├── Main.lean
├── MiniML/
│   ├── Basic.lean
│   ├── AST.lean
│   ├── Lexer.lean
│   ├── Parser.lean
│   ├── TypeChecker.lean
│   ├── Evaluator.lean
│   └── REPL.lean
└── MiniML/Tests/
    └── AllTests.lean

```

### 10.3 核心代码实现

#### lakefile.toml

```toml
name = "MiniML"
version = "0.1.0"
defaultTargets = ["MiniML"]

[[lean_lib]]
name = "MiniML"

[[exe]]
name = "miniml"
root = "Main"
```

#### MiniML/Basic.lean

```lean
/-
MiniML - 微型函数式语言
核心基础定义
-/

namespace MiniML

/-- 错误处理 -/
inductive Result (α : Type) (ε : Type) where
  | ok    : α → Result α ε
  | error : ε → Result α ε
  deriving Repr

instance : Monad (Result · ε) where
  pure := Result.ok
  bind m f := match m with
    | .ok a    => f a
    | .error e => .error e

def Result.map (f : α → β) : Result α ε → Result β ε
  | .ok a    => .ok (f a)
  | .error e => .error e

def Result.get! [Inhabited α] : Result α ε → α
  | .ok a    => a
  | .error _ => default

end MiniML
```

#### MiniML/AST.lean

```lean
/-
MiniML抽象语法树定义
-/

import MiniML.Basic

namespace MiniML.AST

/-- 源位置 -/
structure SourcePos where
  line   : Nat
  column : Nat
  deriving Repr, BEq

def SourcePos.initial : SourcePos := ⟨1, 1⟩

/-- 带位置的节点 -/
structure Pos (α : Type) where
  value : α
  start : SourcePos
  stop  : SourcePos
  deriving Repr, BEq, Inhabited

/-- 类型 -/
inductive Type where
  | TInt    : Type
  | TBool   : Type
  | TString : Type
  | TUnit   : Type
  | TArrow  : Type → Type → Type
  | TList   : Type → Type
  | TTuple  : List Type → Type
  | TVar    : String → Type  -- 类型变量
  deriving Repr, BEq, Inhabited

notation:50 t1 " => " t2 => Type.TArrow t1 t2

/-- 字面量 -/
inductive Literal where
  | LInt    : Int → Literal
  | LBool   : Bool → Literal
  | LString : String → Literal
  | LUnit   : Literal
  deriving Repr, BEq, Inhabited

/-- 二元运算符 -/
inductive BinOp where
  | Add | Sub | Mul | Div | Mod
  | Eq | Ne | Lt | Le | Gt | Ge
  | And | Or
  | Cons
  deriving Repr, BEq, Inhabited

/-- 表达式 -/
inductive Expr where
  | ELiteral : Literal → Expr
  | EVar     : String → Expr
  | EBinOp   : BinOp → Expr → Expr → Expr
  | EApp     : Expr → Expr → Expr
  | ELam     : String → Option Type → Expr → Expr
  | ELet     : String → Expr → Expr → Expr
  | ELetRec  : String → Expr → Expr → Expr
  | EIf      : Expr → Expr → Expr → Expr
  | ETuple   : List Expr → Expr
  | EList    : List Expr → Expr
  | EAnnot   : Expr → Type → Expr  -- 类型标注
  deriving Repr, Inhabited

/-- 顶层声明 -/
inductive Decl where
  | DLet    : String → Expr → Decl
  | DLetRec : String → Expr → Decl
  | DExpr   : Expr → Decl
  deriving Repr, Inhabited

def Program : Type := List (Pos Decl)

end MiniML.AST
```

#### MiniML/Lexer.lean

```lean
/-
MiniML词法分析器
-/

import MiniML.AST

namespace MiniML.Lexer

open MiniML.AST

/-- Token类型 -/
inductive Token where
  | TokInt    : Int → Token
  | TokString : String → Token
  | TokIdent  : String → Token
  | TokTrue   : Token
  | TokFalse  : Token
  | TokUnit   : Token
  | TokLet    : Token
  | TokRec    : Token
  | TokIn     : Token
  | TokFun    : Token
  | TokIf     : Token
  | TokThen   : Token
  | TokElse   : Token
  | TokMatch  : Token
  | TokWith   : Token
  | TokPlus   : Token
  | TokMinus  : Token
  | TokStar   : Token
  | TokSlash  : Token
  | TokEq     : Token
  | TokArrow  : Token
  | TokLParen : Token
  | TokRParen : Token
  | TokLBrack : Token
  | TokRBrack : Token
  | TokComma  : Token
  | TokSemicolon : Token
  | TokColon  : Token
  | TokCons   : Token  -- ::
  | TokEOF    : Token
  deriving Repr, BEq

/-- 词法状态 -/
structure LexState where
  input  : String
  pos    : Nat
  line   : Nat
  col    : Nat
  deriving Repr

def LexState.init (s : String) : LexState :=
  { input := s, pos := 0, line := 1, col := 1 }

def LexState.current (s : LexState) : Option Char :=
  s.input.get? s.pos

def LexState.advance (s : LexState) : LexState :=
  match s.current with
  | none => s
  | some '\n' => { s with pos := s.pos + 1, line := s.line + 1, col := 1 }
  | some _  => { s with pos := s.pos + 1, col := s.col + 1 }

def LexState.toPos (s : LexState) : SourcePos :=
  ⟨s.line, s.col⟩

/-- 带位置的Token -/
structure TokenPos where
  token : Token
  start : SourcePos
  stop  : SourcePos
  deriving Repr, BEq

/-- 词法错误 -/
structure LexError where
  msg : String
  pos : SourcePos
  deriving Repr

/-- 跳过空白 -/
partial def skipWS (s : LexState) : LexState :=
  match s.current with
  | some c => if c.isWhitespace then skipWS s.advance else s
  | none   => s

/-- 读取整数 -/
partial def readInt (s : LexState) (acc : String) : LexState × Int :=
  match s.current with
  | some c =>
    if c.isDigit then
      readInt s.advance (acc.push c)
    else
      (s, acc.toInt!)
  | none   => (s, acc.toInt!)

/-- 读取标识符 -/
partial def readIdent (s : LexState) (acc : String) : LexState × String :=
  match s.current with
  | some c =>
    if c.isAlphanum || c == '_' then
      readIdent s.advance (acc.push c)
    else
      (s, acc)
  | none   => (s, acc)

/-- 读取字符串 -/
partial def readString (s : LexState) (acc : String) : LexState × String :=
  match s.current with
  | none => (s, acc)  -- 未闭合
  | some '"' => (s.advance, acc)
  | some c   => readString s.advance (acc.push c)

/-- 识别关键字 -/
def keyword (s : String) : Token :=
  match s with
  | "let"    => Token.TokLet
  | "rec"    => Token.TokRec
  | "in"     => Token.TokIn
  | "fun"    => Token.TokFun
  | "if"     => Token.TokIf
  | "then"   => Token.TokThen
  | "else"   => Token.TokElse
  | "match"  => Token.TokMatch
  | "with"   => Token.TokWith
  | "true"   => Token.TokTrue
  | "false"  => Token.TokFalse
  | "unit"   => Token.TokUnit
  | "Int"    => Token.TokIdent s
  | "Bool"   => Token.TokIdent s
  | "String" => Token.TokIdent s
  | _        => Token.TokIdent s

/-- 主词法分析 -/
partial def lexAux (s : LexState) (acc : List TokenPos)
    : List TokenPos × List LexError :=
  let s := skipWS s
  let startPos := s.toPos

  match s.current with
  | none => (acc.reverse, [])
  | some c =>
    let (tok, s') :=
      if c.isDigit then
        let (s', n) := readInt s ""
        (Token.TokInt n, s')
      else if c.isAlpha then
        let (s', id) := readIdent s ""
        (keyword id, s')
      else if c == '"' then
        let (s', str) := readString s.advance ""
        (Token.TokString str, s')
      else
        match c with
        | '+'  => (Token.TokPlus, s.advance)
        | '-'  =>
          if (s.input.get? (s.pos + 1)) == some '>' then
            (Token.TokArrow, s.advance.advance)
          else
            (Token.TokMinus, s.advance)
        | '*'  => (Token.TokStar, s.advance)
        | '/'  => (Token.TokSlash, s.advance)
        | '='  => (Token.TokEq, s.advance)
        | '('  => (Token.TokLParen, s.advance)
        | ')'  => (Token.TokRParen, s.advance)
        | '['  => (Token.TokLBrack, s.advance)
        | ']'  => (Token.TokRBrack, s.advance)
        | ','  => (Token.TokComma, s.advance)
        | ';'  => (Token.TokSemicolon, s.advance)
        | ':'  =>
          if (s.input.get? (s.pos + 1)) == some ':' then
            (Token.TokCons, s.advance.advance)
          else
            (Token.TokColon, s.advance)
        | _    => (Token.TokEOF, s.advance)  -- 忽略未知字符

    let stopPos := s'.toPos
    let tokPos := { token := tok, start := startPos, stop := stopPos }
    lexAux s' (tokPos :: acc)

/-- 词法分析入口 -/
def lex (input : String) : List TokenPos × List LexError :=
  lexAux (LexState.init input) []

def tokensOnly (input : String) : List Token :=
  (lex input).1.map (·.token)

end MiniML.Lexer
```

#### MiniML/Parser.lean

```lean
/-
MiniML语法分析器
-/

import MiniML.Lexer

namespace MiniML.Parser

open MiniML.AST
open MiniML.Lexer

/-- 解析错误 -/
structure ParseError where
  msg  : String
  pos  : SourcePos
  deriving Repr

/-- 解析器状态 -/
structure ParseState where
  tokens : List TokenPos
  pos    : Nat
  deriving Repr

def ParseState.current (s : ParseState) : Option TokenPos :=
  s.tokens.get? s.pos

def ParseState.peek (s : ParseState) : Option Token :=
  s.current.map (·.token)

def ParseState.advance (s : ParseState) : ParseState :=
  { s with pos := s.pos + 1 }

def ParseState.pos (s : ParseState) : SourcePos :=
  match s.current with
  | some t => t.start
  | none   => ⟨0, 0⟩

/-- 解析器单子 -/
def Parser (α : Type) := ParseState → Result (α × ParseState) ParseError

instance : Monad Parser where
  pure a := λ s => .ok (a, s)
  bind m f := λ s =>
    match m s with
    | .ok (a, s') => f a s'
    | .error e    => .error e

instance : Alternative Parser where
  failure := λ s => .error { msg := "parse failed", pos := s.pos }
  orElse p q := λ s =>
    match p s with
    | .ok r => .ok r
    | .error _ => q () s

/-- 基本组合子 -/
def expect (t : Token) : Parser Unit :=
  λ s =>
    match s.peek with
    | some t' =>
      if t == t' then .ok ((), s.advance)
      else .error { msg := s!"Expected {repr t}", pos := s.pos }
    | none => .error { msg := s!"Expected {repr t}, got EOF", pos := s.pos }

def satisfy (p : Token → Bool) (desc : String) : Parser Token :=
  λ s =>
    match s.peek with
    | some t =>
      if p t then .ok (t, s.advance)
      else .error { msg := s!"Expected {desc}, got {repr t}", pos := s.pos }
    | none => .error { msg := s!"Expected {desc}, got EOF", pos := s.pos }

def optional (p : Parser α) : Parser (Option α) :=
  λ s =>
    match p s with
    | .ok (a, s') => .ok (some a, s')
    | .error _    => .ok (none, s)

partial def many (p : Parser α) : Parser (List α) :=
  λ s =>
    match p s with
    | .error _    => .ok ([], s)
    | .ok (a, s') =>
      match many p s' with
      | .ok (as, s'') => .ok (a :: as, s'')
      | .error e      => .error e

partial def sepBy (p : Parser α) (sep : Token) : Parser (List α) :=
  λ s =>
    match p s with
    | .error _    => .ok ([], s)
    | .ok (a, s') =>
      let rec loop acc s :=
        match s.peek with
        | some t =>
          if t == sep then
            match p s.advance with
            | .ok (a', s'') => loop (a' :: acc) s''
            | .error _      => .ok (acc.reverse, s)
          else
            .ok (acc.reverse, s)
        | none => .ok (acc.reverse, s)
      loop [a] s'

/-- 解析标识符 -/
def parseIdent : Parser String := do
  let t ← satisfy (λ t => match t with | Token.TokIdent _ => true | _ => false) "identifier"
  match t with
  | Token.TokIdent s => return s
  | _ => failure

/-- 解析类型 -/
mutual
  partial def parseType : Parser Type :=
    parseArrowType

  partial def parseArrowType : Parser Type := do
    let t ← parseAtomicType
    (do expect Token.TokArrow
        let t2 ← parseArrowType
        return Type.TArrow t t2)
    <|> pure t

  partial def parseAtomicType : Parser Type := do
    let t ← satisfy (λ _ => true) "type"
    match t with
    | Token.TokIdent "Int"    => return Type.TInt
    | Token.TokIdent "Bool"   => return Type.TBool
    | Token.TokIdent "String" => return Type.TString
    | Token.TokIdent "Unit"   => return Type.TUnit
    | Token.TokIdent s        => return Type.TVar s
    | Token.TokLParen =>
      let t ← parseType
      expect Token.TokRParen
      return t
    | _ => failure
end

/-- 解析表达式 -/
mutual
  partial def parseExpr : Parser Expr :=
    parseLet

  partial def parseLet : Parser Expr := do
    (do expect Token.TokLet
        let isRec ← (optional (expect Token.TokRec) |>.map (λ _ => true))
        let name ← parseIdent
        expect Token.TokEq
        let e1 ← parseExpr
        expect Token.TokIn
        let e2 ← parseExpr
        if isRec.getD false then
          return Expr.ELetRec name e1 e2
        else
          return Expr.ELet name e1 e2)
    <|> parseIf

  partial def parseIf : Parser Expr := do
    (do expect Token.TokIf
        let cond ← parseExpr
        expect Token.TokThen
        let t ← parseExpr
        expect Token.TokElse
        let f ← parseExpr
        return Expr.EIf cond t f)
    <|> parseLam

  partial def parseLam : Parser Expr := do
    (do expect Token.TokFun
        let x ← parseIdent
        let ty ← optional (do expect Token.TokColon; parseType)
        expect Token.TokArrow
        let body ← parseExpr
        return Expr.ELam x ty body)
    <|> parseOr

  partial def parseOr : Parser Expr :=
    parseLeftAssoc parseAnd BinOp.Or Token.TokIdent "||"

  partial def parseAnd : Parser Expr :=
    parseLeftAssoc parseComp BinOp.And Token.TokIdent "&&"

  partial def parseComp : Parser Expr := do
    let e ← parseCons
    (do let op ← (expect Token.TokEq *> pure BinOp.Eq)
             <|> (expect Token.TokIdent "<" *> pure BinOp.Lt)
             <|> (expect Token.TokIdent ">" *> pure BinOp.Gt)
        let e2 ← parseCons
        return Expr.EBinOp op e e2)
    <|> pure e

  partial def parseCons : Parser Expr := do
    let e ← parseAdd
    (do expect Token.TokCons
        let e2 ← parseCons
        return Expr.EBinOp BinOp.Cons e e2)
    <|> pure e

  partial def parseAdd : Parser Expr :=
    parseLeftAssoc parseMul BinOp.Add Token.TokPlus

  partial def parseMul : Parser Expr :=
    parseLeftAssoc parseApp BinOp.Mul Token.TokStar

  partial def parseApp : Parser Expr := do
    let f ← parseAtomic
    let args ← many parseAtomic
    return args.foldl Expr.EApp f

  partial def parseAtomic : Parser Expr :=
    parseLiteral
    <|> parseVar
    <|> parseParens
    <|> parseList
    <|> parseTuple

  partial def parseLiteral : Parser Expr := do
    let t ← satisfy (λ _ => true) "literal"
    match t with
    | Token.TokInt n    => return Expr.ELiteral (Literal.LInt n)
    | Token.TokTrue     => return Expr.ELiteral (Literal.LBool true)
    | Token.TokFalse    => return Expr.ELiteral (Literal.LBool false)
    | Token.TokUnit     => return Expr.ELiteral Literal.LUnit
    | Token.TokString s => return Expr.ELiteral (Literal.LString s)
    | _ => failure

  partial def parseVar : Parser Expr := do
    let s ← parseIdent
    return Expr.EVar s

  partial def parseParens : Parser Expr := do
    expect Token.TokLParen
    let e ← parseExpr
    expect Token.TokRParen
    return e

  partial def parseList : Parser Expr := do
    expect Token.TokLBrack
    let es ← sepBy parseExpr Token.TokComma
    expect Token.TokRBrack
    return Expr.EList es

  partial def parseTuple : Parser Expr := do
    expect Token.TokLParen
    let e ← parseExpr
    expect Token.TokComma
    let es ← sepBy parseExpr Token.TokComma
    expect Token.TokRParen
    return Expr.ETuple (e :: es)

  partial def parseLeftAssoc (p : Parser Expr) (op : BinOp) (tok : Token)
      : Parser Expr := do
    let e1 ← p
    let rec loop e :=
      (do expect tok
          let e2 ← p
          loop (Expr.EBinOp op e e2))
      <|> pure e
    loop e1
end

/-- 解析声明 -/
def parseDecl : Parser Decl :=
  (do expect Token.TokLet
      let isRec ← (optional (expect Token.TokRec) |>.map (λ _ => true))
      let name ← parseIdent
      expect Token.TokEq
      let e ← parseExpr
      if isRec.getD false then
        return Decl.DLetRec name e
      else
        return Decl.DLet name e)
  <|> (do let e ← parseExpr
          return Decl.DExpr e)

/-- 解析程序 -/
def parseProgram (s : ParseState) : Result Program ParseError :=
  match many parseDecl s with
  | .ok (decls, s') =>
    let mkPos d := { value := d, start := ⟨0,0⟩, stop := ⟨0,0⟩ }
    .ok (decls.map mkPos, s')
  | .error e => .error e

/-- 解析入口 -/
def parse (input : String) : Result Program ParseError :=
  let (tokens, _) := lex input
  let s := { tokens := tokens, pos := 0 }
  match parseProgram s with
  | .ok (prog, s') =>
    match s'.peek with
    | some Token.TokEOF => .ok prog
    | some t => .error { msg := s!"Unexpected {repr t}", pos := s'.pos }
    | none   => .ok prog
  | .error e => .error e

end MiniML.Parser
```

#### MiniML/TypeChecker.lean

```lean
/-
MiniML类型检查器
-/

import MiniML.Parser

namespace MiniML.TypeChecker

open MiniML.AST
open MiniML.Parser

/-- 类型方案 -/
structure Scheme where
  vars : List String
  type : Type
  deriving Repr

def Scheme.instantiate (s : Scheme) (freshVars : List Type) : Type :=
  if s.vars.length != freshVars.length then s.type
  else
    let subst := List.zip s.vars freshVars |>.foldr (λ (v, t) acc t' =>
      match t' with
      | Type.TVar n => if n == v then t else Type.TVar n
      | _ => t') id
    subst s.type

/-- 类型上下文 -/
def TypeCtx := List (String × Scheme)

def TypeCtx.empty : TypeCtx := []
def TypeCtx.extend (ctx : TypeCtx) (x : String) (s : Scheme) : TypeCtx :=
  (x, s) :: ctx
def TypeCtx.lookup (ctx : TypeCtx) (x : String) : Option Scheme :=
  ctx.lookup x

/-- 替换 -/
def Subst := List (String × Type)

def Subst.empty : Subst := []
def Subst.apply (σ : Subst) : Type → Type
  | Type.TVar n    => match σ.lookup n with | some t => t | none => Type.TVar n
  | Type.TArrow t1 t2 => Type.TArrow (σ.apply t1) (σ.apply t2)
  | Type.TList t   => Type.TList (σ.apply t)
  | Type.TTuple ts => Type.TTuple (ts.map σ.apply)
  | t              => t

def Subst.compose (σ1 σ2 : Subst) : Subst :=
  let σ2' := σ2.map (λ (v, t) => (v, σ1.apply t))
  σ1 ++ σ2'

/-- 类型变量生成 -/
structure TypeGen where
  counter : Nat
  deriving Repr

def TypeGen.new : TypeGen := ⟨0⟩
def TypeGen.fresh (g : TypeGen) : (Type × TypeGen) :=
  let name := s!"t{g.counter}"
  (Type.TVar name, ⟨g.counter + 1⟩)

/-- 类型错误 -/
inductive TypeError where
  | Unify      : Type → Type → TypeError
  | Occurs     : String → Type → TypeError
  | Undefined  : String → TypeError
  | Mismatch   : String → Type → Type → TypeError
  deriving Repr

def TypeError.toString : TypeError → String
  | .Unify t1 t2   => s!"Cannot unify {repr t1} with {repr t2}"
  | .Occurs v t    => s!"Occurs check failed: {v} in {repr t}"
  | .Undefined x   => s!"Undefined variable: {x}"
  | .Mismatch ctx e a => s!"Type mismatch in {ctx}: expected {repr e}, got {repr a}"

/-- 合一 -/
def occurs (v : String) : Type → Bool
  | Type.TVar n    => v == n
  | Type.TArrow t1 t2 => occurs v t1 || occurs v t2
  | Type.TList t   => occurs v t
  | Type.TTuple ts => ts.any (occurs v)
  | _              => false

def unify (t1 t2 : Type) : Result Subst TypeError :=
  match t1, t2 with
  | Type.TInt, Type.TInt     => .ok []
  | Type.TBool, Type.TBool   => .ok []
  | Type.TString, Type.TString => .ok []
  | Type.TUnit, Type.TUnit   => .ok []
  | Type.TVar n1, Type.TVar n2 =>
    if n1 == n2 then .ok [] else .ok [(n1, Type.TVar n2)]
  | Type.TVar n, t | t, Type.TVar n =>
    if occurs n t then .error (TypeError.Occurs n t)
    else .ok [(n, t)]
  | Type.TArrow a1 r1, Type.TArrow a2 r2 =>
    do let σ1 ← unify a1 a2
       let σ2 ← unify (σ1.apply r1) (σ1.apply r2)
       .ok (σ2.compose σ1)
  | Type.TList t1, Type.TList t2 =>
    unify t1 t2
  | Type.TTuple ts1, Type.TTuple ts2 =>
    if ts1.length != ts2.length then
      .error (TypeError.Unify t1 t2)
    else unifyMany ts1 ts2
  | _, _ => .error (TypeError.Unify t1 t2)
where
  unifyMany : List Type → List Type → Result Subst TypeError
    | [], [] => .ok []
    | t1 :: ts1, t2 :: ts2 => do
      let σ1 ← unify t1 t2
      let σ2 ← unifyMany (ts1.map (σ1.apply)) (ts2.map (σ1.apply))
      .ok (σ2.compose σ1)
    | _, _ => .error (TypeError.Unify t1 t2)

/-- 泛化与实例化 -/
def generalize (ctx : TypeCtx) (t : Type) : Scheme :=
  let ctxVars := ctx.bind (λ (_, s) => s.type.ftv)
  let typeVars := t.ftv
  let genVars := typeVars.filter (λ v => !ctxVars.contains v)
  { vars := genVars, type := t }
where
  Type.ftv : Type → List String
    | Type.TVar n    => [n]
    | Type.TArrow t1 t2 => t1.ftv ++ t2.ftv
    | Type.TList t   => t.ftv
    | Type.TTuple ts => ts.bind Type.ftv
    | _              => []

def instantiate (g : TypeGen) (s : Scheme) : (Type × TypeGen) :=
  let rec genFresh g 0 acc := (acc.reverse, g)
      | genFresh g (n+1) acc :=
        let (t, g') := g.fresh
        genFresh g' n (t :: acc)
  let (freshVars, g') := genFresh g s.vars.length []
  let applySubst ty :=
    let subst := List.zip s.vars freshVars
    subst.foldr (λ (v, t) t' =>
      match t' with
      | Type.TVar n => if n == v then t else Type.TVar n
      | _ => t') ty
  (applySubst s.type, g')

/-- 主类型推导 -/
mutual
  partial def infer (ctx : TypeCtx) (e : Expr) (g : TypeGen)
      : Result (Type × Subst × TypeGen) TypeError :=
    match e with
    | Expr.ELiteral l =>
      match l with
      | Literal.LInt _    => .ok (Type.TInt, [], g)
      | Literal.LBool _   => .ok (Type.TBool, [], g)
      | Literal.LString _ => .ok (Type.TString, [], g)
      | Literal.LUnit     => .ok (Type.TUnit, [], g)

    | Expr.EVar x =>
      match ctx.lookup x with
      | some scheme =>
        let (t, g') := instantiate g scheme
        .ok (t, [], g')
      | none => .error (TypeError.Undefined x)

    | Expr.ELam x tyAnnot body => do
      let (argTy, g1) := match tyAnnot with
        | some t => (t, g)
        | none   => g.fresh
      let ctx' := ctx.extend x { vars := [], type := argTy }
      let (bodyTy, σ1, g2) ← infer ctx' body g1
      let funTy := Type.TArrow (σ1.apply argTy) bodyTy
      .ok (funTy, σ1, g2)

    | Expr.EApp f arg => do
      let (funTy, σ1, g1) ← infer ctx f g
      let (argTy, σ2, g2) ← infer (applyCtx σ1 ctx) arg g1
      let (retTy, g3) := g2.fresh
      let σ3 ← unify (σ2.apply funTy) (Type.TArrow argTy retTy)
      let finalSubst := σ3.compose (σ2.compose σ1)
      .ok (σ3.apply retTy, finalSubst, g3)

    | Expr.ELet x e1 e2 => do
      let (t1, σ1, g1) ← infer ctx e1 g
      let t1' := σ1.apply t1
      let scheme := generalize (applyCtx σ1 ctx) t1'
      let ctx' := (applyCtx σ1 ctx).extend x scheme
      let (t2, σ2, g2) ← infer ctx' e2 g1
      .ok (t2, σ2.compose σ1, g2)

    | Expr.ELetRec f e1 e2 => do
      let (funTy, g1) := g.fresh
      let ctx' := ctx.extend f { vars := [], type := funTy }
      let (t1, σ1, g2) ← infer ctx' e1 g1
      let σ1' ← unify (σ1.apply funTy) t1
      let scheme := generalize (applyCtx σ1' ctx) (σ1'.apply funTy)
      let ctx'' := (applyCtx σ1' ctx).extend f scheme
      let (t2, σ2, g3) ← infer ctx'' e2 g2
      .ok (t2, σ2.compose σ1', g3)

    | Expr.EIf cond t f => do
      let (tCond, σ1, g1) ← infer ctx cond g
      let σ1' ← unify tCond Type.TBool
      let ctx' := applyCtx (σ1'.compose σ1) ctx
      let (tThen, σ2, g2) ← infer ctx' t g1
      let (tElse, σ3, g3) ← infer (applyCtx σ2 ctx') f g2
      let σ4 ← unify (σ3.apply tThen) tElse
      let finalTy := σ4.apply tElse
      let finalSubst := σ4.compose (σ3.compose (σ2.compose (σ1'.compose σ1)))
      .ok (finalTy, finalSubst, g3)

    | Expr.EBinOp op e1 e2 => do
      let (t1, σ1, g1) ← infer ctx e1 g
      let (t2, σ2, g2) ← infer (applyCtx σ1 ctx) e2 g1
      let (resultTy, opSubst) ← inferBinOp op (σ2.apply t1) t2
      let finalSubst := opSubst.compose (σ2.compose σ1)
      .ok (resultTy, finalSubst, g2)

    | _ => .error (TypeError.Mismatch "expression" Type.TUnit Type.TUnit)

  where
    applyCtx σ ctx := ctx.map (λ (x, s) => (x, { s with type := σ.apply s.type }))

    inferBinOp : BinOp → Type → Type → Result (Type × Subst) TypeError
      | BinOp.Add, Type.TInt, Type.TInt   => .ok (Type.TInt, [])
      | BinOp.Sub, Type.TInt, Type.TInt   => .ok (Type.TInt, [])
      | BinOp.Mul, Type.TInt, Type.TInt   => .ok (Type.TInt, [])
      | BinOp.Div, Type.TInt, Type.TInt   => .ok (Type.TInt, [])
      | BinOp.Mod, Type.TInt, Type.TInt   => .ok (Type.TInt, [])
      | BinOp.Eq, t1, t2            => do
        let σ ← unify t1 t2
        .ok (Type.TBool, σ)
      | BinOp.Lt, Type.TInt, Type.TInt    => .ok (Type.TBool, [])
      | BinOp.Le, Type.TInt, Type.TInt    => .ok (Type.TBool, [])
      | BinOp.And, Type.TBool, Type.TBool => .ok (Type.TBool, [])
      | BinOp.Or, Type.TBool, Type.TBool  => .ok (Type.TBool, [])
      | BinOp.Cons, t1, Type.TList t2     => do
        let σ ← unify t1 t2
        .ok (Type.TList (σ.apply t2), σ)
      | op, t1, t2 =>
        .error (TypeError.Mismatch (repr op) Type.TUnit Type.TUnit)
end

/-- 类型检查顶层 -/
def typeCheck (ctx : TypeCtx) (e : Expr) : Result Type TypeError :=
  match infer ctx e TypeGen.new with
  | .ok (t, σ, _) => .ok (σ.apply t)
  | .error e => .error e

end MiniML.TypeChecker
```

#### MiniML/Evaluator.lean

```lean
/-
MiniML求值器
-/

import MiniML.TypeChecker

namespace MiniML.Evaluator

open MiniML.AST
open MiniML.TypeChecker

/-- 运行时值 -/
inductive Value where
  | VInt    : Int → Value
  | VBool   : Bool → Value
  | VString : String → Value
  | VUnit   : Value
  | VList   : List Value → Value
  | VTuple  : List Value → Value
  | VClosure: Env → String → Expr → Value
  | VRecClosure : Env → String → String → Expr → Value
  deriving Repr, Inhabited

/-- 环境 -/
def Env := List (String × Value)

def Env.empty : Env := []
def Env.extend (env : Env) (x : String) (v : Value) : Env :=
  (x, v) :: env
def Env.lookup (env : Env) (x : String) : Option Value :=
  env.lookup x

/-- 求值错误 -/
inductive EvalError where
  | Unbound      : String → EvalError
  | TypeError    : String → Value → EvalError
  | DivByZero    : EvalError
  | NonExhaustive: EvalError
  deriving Repr

def EvalError.toString : EvalError → String
  | .Unbound x      => s!"Unbound variable: {x}"
  | .TypeError op v => s!"Type error in {op}: {repr v}"
  | .DivByZero      => "Division by zero"
  | .NonExhaustive  => "Non-exhaustive match"

/-- 求值 -/
mutual
  partial def eval (env : Env) (e : Expr) : Result Value EvalError :=
    match e with
    | Expr.ELiteral l => evalLiteral l
    | Expr.EVar x =>
      match env.lookup x with
      | some v => .ok v
      | none   => .error (EvalError.Unbound x)

    | Expr.ELam x _ body =>
      .ok (Value.VClosure env x body)

    | Expr.EApp f arg => do
      let vf ← eval env f
      match vf with
      | Value.VClosure env' x body =>
        let varg ← eval env arg
        eval (env'.extend x varg) body
      | Value.VRecClosure env' f' x body =>
        let varg ← eval env arg
        let env'' := env'.extend f' (Value.VRecClosure env' f' x body)
        eval (env''.extend x varg) body
      | _ => .error (EvalError.TypeError "application" vf)

    | Expr.ELet x e1 e2 => do
      let v1 ← eval env e1
      eval (env.extend x v1) e2

    | Expr.ELetRec f (Expr.ELam x _ body) e2 =>
      let recVal := Value.VRecClosure env f x body
      eval (env.extend f recVal) e2
    | Expr.ELetRec _ _ _ =>
      .error (EvalError.TypeError "let rec" (Value.VUnit))

    | Expr.EIf cond t f => do
      let vcond ← eval env cond
      match vcond with
      | Value.VBool true  => eval env t
      | Value.VBool false => eval env f
      | _ => .error (EvalError.TypeError "if" vcond)

    | Expr.EBinOp op e1 e2 => do
      let v1 ← eval env e1
      let v2 ← eval env e2
      evalBinOp op v1 v2

    | Expr.EList es => do
      let vs ← es.mapM (eval env)
      .ok (Value.VList vs)

    | Expr.ETuple es => do
      let vs ← es.mapM (eval env)
      .ok (Value.VTuple vs)

    | _ => .ok Value.VUnit

  partial def evalLiteral : Literal → Result Value EvalError
    | Literal.LInt n    => .ok (Value.VInt n)
    | Literal.LBool b   => .ok (Value.VBool b)
    | Literal.LString s => .ok (Value.VString s)
    | Literal.LUnit     => .ok Value.VUnit

  partial def evalBinOp : BinOp → Value → Value → Result Value EvalError
    | BinOp.Add, Value.VInt n1, Value.VInt n2 => .ok (Value.VInt (n1 + n2))
    | BinOp.Sub, Value.VInt n1, Value.VInt n2 => .ok (Value.VInt (n1 - n2))
    | BinOp.Mul, Value.VInt n1, Value.VInt n2 => .ok (Value.VInt (n1 * n2))
    | BinOp.Div, Value.VInt n1, Value.VInt n2 =>
      if n2 == 0 then .error EvalError.DivByZero
      else .ok (Value.VInt (n1 / n2))
    | BinOp.Mod, Value.VInt n1, Value.VInt n2 =>
      if n2 == 0 then .error EvalError.DivByZero
      else .ok (Value.VInt (n1 % n2))
    | BinOp.Eq, v1, v2 => .ok (Value.VBool (beqValue v1 v2))
    | BinOp.Lt, Value.VInt n1, Value.VInt n2 => .ok (Value.VBool (n1 < n2))
    | BinOp.Le, Value.VInt n1, Value.VInt n2 => .ok (Value.VBool (n1 ≤ n2))
    | BinOp.And, Value.VBool b1, Value.VBool b2 => .ok (Value.VBool (b1 && b2))
    | BinOp.Or, Value.VBool b1, Value.VBool b2 => .ok (Value.VBool (b1 || b2))
    | BinOp.Cons, v, Value.VList vs => .ok (Value.VList (v :: vs))
    | op, v1, v2 => .error (EvalError.TypeError (repr op) v1)

  partial def beqValue : Value → Value → Bool
    | Value.VInt n1, Value.VInt n2 => n1 == n2
    | Value.VBool b1, Value.VBool b2 => b1 == b2
    | Value.VString s1, Value.VString s2 => s1 == s2
    | Value.VUnit, Value.VUnit => true
    | Value.VList vs1, Value.VList vs2 =>
      vs1.length == vs2.length && (vs1.zip vs2).all (λ (v1, v2) => beqValue v1 v2)
    | Value.VTuple vs1, Value.VTuple vs2 =>
      vs1.length == vs2.length && (vs1.zip vs2).all (λ (v1, v2) => beqValue v1 v2)
    | _, _ => false
end

end MiniML.Evaluator
```

#### MiniML/REPL.lean

```lean
/-
MiniML REPL
-/

import MiniML.Evaluator

namespace MiniML.REPL

open MiniML.AST
open MiniML.Parser
open MiniML.TypeChecker
open MiniML.Evaluator

/-- 解析并执行 -/
def runExpr (input : String) : IO Unit := do
  match parse input with
  | .error e =>
    IO.println s!"Parse error: {e.msg}"
  | .ok prog =>
    for { value := decl, .. } in prog do
      match decl with
      | Decl.DExpr e =>
        -- 类型检查
        match typeCheck [] e with
        | .error te =>
          IO.println s!"Type error: {te.toString}"
        | .ok ty =>
          IO.println s!"Type: {repr ty}"
          -- 求值
          match eval [] e with
          | .error ee =>
            IO.println s!"Runtime error: {ee.toString}"
          | .ok v =>
            IO.println s!"Value: {repr v}"

      | Decl.DLet x e =>
        match typeCheck [] e with
        | .error te =>
          IO.println s!"Type error in let {x}: {te.toString}"
        | .ok ty =>
          IO.println s!"val {x} : {repr ty}"
          match eval [] e with
          | .ok v =>
            IO.println s!"{repr v}"
          | .error ee =>
            IO.println s!"Runtime error: {ee.toString}"

      | _ => IO.println "Declaration not yet supported"

/-- 简单REPL -/
partial def repl : IO Unit := do
  IO.print "MiniML> "
  let input ← IO.getLine
  if input.trim == ":quit" then
    IO.println "Goodbye!"
  else if input.trim == ":help" then
    IO.println "Commands: :quit, :help"
    repl
  else
    runExpr input
    repl

end MiniML.REPL
```

#### Main.lean

```lean
import MiniML.REPL

open MiniML.REPL

def main : IO Unit := do
  IO.println "MiniML - A Minimal Functional Language"
  IO.println "Type :help for help, :quit to exit"
  IO.println ""
  repl
```

### 10.4 测试套件

```lean
-- MiniML/Tests/AllTests.lean
import MiniML.Parser
import MiniML.TypeChecker
import MiniML.Evaluator

namespace MiniML.Tests

open MiniML.AST
open MiniML.Parser
open MiniML.TypeChecker
open MiniML.Evaluator

def runTests : IO Unit := do
  IO.println "=== MiniML Test Suite ==="

  -- 测试用例
  let tests : List (String × Option Type × Option Value) := [
    ("42", some Type.TInt, some (Value.VInt 42)),
    ("true", some Type.TBool, some (Value.VBool true)),
    ("1 + 2", some Type.TInt, some (Value.VInt 3)),
    ("fun x -> x + 1",
      some (Type.TArrow Type.TInt Type.TInt),
      some (Value.VClosure [] "x" (Expr.EBinOp BinOp.Add (Expr.EVar "x") (Expr.ELiteral (Literal.LInt 1))))),
    ("(fun x -> x + 1) 5", some Type.TInt, some (Value.VInt 6)),
    ("let x = 10 in x + 5", some Type.TInt, some (Value.VInt 15)),
    ("if true then 1 else 0", some Type.TInt, some (Value.VInt 1)),
    ("[1, 2, 3]", some (Type.TList Type.TInt), some (Value.VList [Value.VInt 1, Value.VInt 2, Value.VInt 3])),
    ("1 :: [2, 3]", some (Type.TList Type.TInt), some (Value.VList [Value.VInt 1, Value.VInt 2, Value.VInt 3]))
  ]

  let mut passed := 0
  let mut failed := 0

  for (input, expectedTy, expectedVal) in tests do
    IO.println s!"\nTest: {input}"

    match parse input with
    | .ok [{ value := Decl.DExpr e, .. }] =>
      -- 类型检查
      match expectedTy with
      | some ty =>
        match typeCheck [] e with
        | .ok t =>
          if t == ty then
            IO.println s!"  ✓ Type: {repr t}"
            passed := passed + 1
          else
            IO.println s!"  ✗ Type mismatch: expected {repr ty}, got {repr t}"
            failed := failed + 1
        | .error te =>
          IO.println s!"  ✗ Type error: {te.toString}"
          failed := failed + 1
      | none => IO.println "  - Type check skipped"

      -- 求值
      match expectedVal with
      | some val =>
        match eval [] e with
        | .ok v =>
          if beqValue v val then
            IO.println s!"  ✓ Value: {repr v}"
            passed := passed + 1
          else
            IO.println s!"  ✗ Value mismatch: expected {repr val}, got {repr v}"
            failed := failed + 1
        | .error ee =>
          IO.println s!"  ✗ Runtime error: {ee.toString}"
          failed := failed + 1
      | none => IO.println "  - Eval skipped"

    | _ =>
      IO.println "  ✗ Parse error"
      failed := failed + 1

  IO.println s!"\n=== Results: {passed} passed, {failed} failed ==="

end MiniML.Tests
```

### 10.5 示例程序

```
-- examples/factorial.ml
let rec factorial = fun n ->
  if n == 0 then 1 else n * factorial (n - 1)
in
factorial 5
-- Result: 120

-- examples/map.ml
let rec map = fun f -> fun xs ->
  if xs == [] then []
  else f (head xs) :: map f (tail xs)
in
map (fun x -> x + 1) [1, 2, 3]
-- Result: [2, 3, 4]

-- examples/fib.ml
let rec fib = fun n ->
  if n <= 1 then n
  else fib (n - 1) + fib (n - 2)
in
fib 10
-- Result: 55
```

---

## 11. 参考文献

### 11.1 核心文献

1. **Pierce, B.C.** (2002). *Types and Programming Languages*. MIT Press.
   - 类型系统理论的权威参考

2. **Winskel, G.** (1993). *The Formal Semantics of Programming Languages*. MIT Press.
   - 形式语义基础教材

3. **Harper, R.** (2016). *Practical Foundations for Programming Languages* (2nd ed.). Cambridge University Press.
   - 现代PL理论综合指南

4. **Nipkow, T., & Klein, G.** (2014). *Concrete Semantics*. Springer.
   - 使用Isabelle的形式语义与类型系统验证

### 11.2 Lean 4相关

1. **de Moura, L., & Ullrich, S.** (2021). *The Lean 4 Theorem Prover*.
   - Lean 4设计与实现论文

2. **Avigad, J., et al.** *Theorem Proving in Lean 4*. 官方教程.
   - <https://lean-lang.org/theorem_proving_in_lean4/>

3. **Ullrich, S., & de Moura, L.** (2019). *Counting Immutable Beans*.
   - Lean 4引用计数内存管理

### 11.3 实现技术

1. **Appel, A.W.** (1998). *Modern Compiler Implementation in ML*. Cambridge.
   - ML语言编译器实现经典

2. **Jones, S.P., et al.** (1993). *The Glasgow Haskell Compiler*.
   - GHC设计与实现技术

3. **Leroy, X.** (2009). *A formally verified compiler back-end*.
    - CompCert验证编译器

### 11.4 在线资源

- Lean 4官方文档: <https://lean-lang.org/>
- Lean 4 GitHub: <https://github.com/leanprover/lean4>
- Mathlib4: <https://github.com/leanprover-community/mathlib4>
- Zulip社区: <https://leanprover.zulipchat.com/>

---

## 12. 交叉引用

### 12.1 关联文档

| 文档 | 路径 | 关系 |
|:-----|:-----|:-----|
| **CASE_STUDY_TYPECHECKER.md** | `10_Lean4_Compilation/CASE_STUDY_TYPECHECKER.md` | 类型检查器验证案例研究，包含保持性、进展性证明 |
| **LEAN4_SEMANTICS_MAPPING.md** | `10_Lean4_Compilation/LEAN4_SEMANTICS_MAPPING.md` | 理论语义到Lean 4实现的完整映射 |
| **APPLIED_FUNCTIONAL.md** | `10_Lean4_Compilation/APPLIED_FUNCTIONAL.md` | 函数式语言应用语义框架 |
| **CASE_STUDY_UNIFICATION.md** | `10_Lean4_Compilation/CASE_STUDY_UNIFICATION.md` | 合一算法深入分析 |
| **CASE_STUDY_LEAN_KERNEL.md** | `10_Lean4_Compilation/CASE_STUDY_LEAN_KERNEL.md` | Lean内核实现分析 |

### 12.2 前置知识

本文档假设读者已掌握以下知识：

- Lean 4基础语法（定义、归纳类型、模式匹配）
- 类型理论基础（简单类型λ演算）
- 操作语义基本概念
- 基本的编译原理知识

### 12.3 后续学习路径

```
本指南 → CASE_STUDY_TYPECHECKER.md (形式化验证)
      → LEAN4_SEMANTICS_MAPPING.md (理论映射)
      → 实现优化编译器
      → 扩展依赖类型支持
      → 形式化验证完整实现
```

---

## 附录: 代码片段索引

| # | 片段描述 | 代码行数 | 位置 |
|:-:|:---------|:-------:|:-----|
| 1 | Token类型定义 | ~60 | 第3章 |
| 2 | 词法分析器状态 | ~30 | 第3章 |
| 3 | 词法分析核心 | ~100 | 第3章 |
| 4 | AST类型定义 | ~70 | 第4章 |
| 5 | 解析器单子 | ~40 | 第4章 |
| 6 | 解析器组合子 | ~60 | 第4章 |
| 7 | 表达式解析 | ~120 | 第4章 |
| 8 | 类型表示 | ~40 | 第5章 |
| 9 | 上下文与替换 | ~50 | 第5章 |
| 10 | 合一算法 | ~60 | 第5章 |
| 11 | 类型推导 | ~150 | 第5章 |
| 12 | 求值器定义 | ~80 | 第6章 |
| 13 | 大步语义求值 | ~150 | 第6章 |
| 14 | 闭包与TCO | ~80 | 第6章 |
| 15 | 单元测试框架 | ~100 | 第7章 |
| 16 | 属性测试 | ~80 | 第7章 |
| 17 | 形式化验证框架 | ~60 | 第7章 |
| 18 | 多态类型扩展 | ~70 | 第8章 |
| 19 | 效应系统 | ~60 | 第8章 |
| 20 | 性能优化 | ~80 | 第9章 |
| 21 | MiniML完整实现 | ~500 | 第10章 |

---

*文档版本: 1.0*
*最后更新: 2026-03-24*
*状态: 完整实现指南*
