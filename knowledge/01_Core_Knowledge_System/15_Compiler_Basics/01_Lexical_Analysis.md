# 词法分析基础

> **层级**: L3 (核心层)
> **目标**: 掌握词法分析器实现原理与手写实现
> **标准**: C23/C11
> **难度**: 高级

---

## 目录

- [词法分析基础](#词法分析基础)
  - [目录](#目录)
  - [1. Token 定义](#1-token-定义)
    - [1.1 词法单元类型](#11-词法单元类型)
    - [1.2 Token 属性](#12-token-属性)
    - [1.3 词法分析器接口](#13-词法分析器接口)
  - [2. 有限自动机](#2-有限自动机)
    - [2.1 确定性有限自动机 (DFA)](#21-确定性有限自动机-dfa)
    - [2.2 非确定性有限自动机 (NFA)](#22-非确定性有限自动机-nfa)
    - [2.3 子集构造算法](#23-子集构造算法)
    - [2.4 DFA 最小化](#24-dfa-最小化)
  - [3. 正则表达式](#3-正则表达式)
    - [3.1 语法定义](#31-语法定义)
    - [3.2 Thompson 构造法](#32-thompson-构造法)
    - [3.3 NFA 到 DFA 转换](#33-nfa-到-dfa-转换)
  - [4. 手写实现](#4-手写实现)
    - [4.1 整体架构](#41-整体架构)
    - [4.2 Token 结构定义](#42-token-结构定义)
    - [4.3 核心扫描函数](#43-核心扫描函数)
    - [4.4 关键字识别](#44-关键字识别)
    - [4.5 完整代码实现](#45-完整代码实现)
  - [5. 错误处理](#5-错误处理)
    - [5.1 错误类型](#51-错误类型)
    - [5.2 错误恢复策略](#52-错误恢复策略)
    - [5.3 错误报告机制](#53-错误报告机制)
  - [6. 深入理解](#6-深入理解)
    - [6.1 实现机制分析](#61-实现机制分析)
    - [6.2 性能优化策略](#62-性能优化策略)
  - [7. 思维表征](#7-思维表征)
    - [7.1 Token 类型决策树](#71-token-类型决策树)
    - [7.2 DFA vs NFA 对比矩阵](#72-dfa-vs-nfa-对比矩阵)
    - [7.3 词法分析流程图](#73-词法分析流程图)
  - [8. 层次映射](#8-层次映射)
    - [8.1 与语法分析的关系](#81-与语法分析的关系)
    - [8.2 与符号表的交互](#82-与符号表的交互)
  - [总结](#总结)
  - [参考](#参考)

---

## 1. Token 定义

### 1.1 词法单元类型

Token 是源程序的最小语义单元。C 语言词法单元可分为以下类别：

| 类别 | 示例 | 说明 |
|------|------|------|
| **关键字** | `int`, `if`, `while`, `return` | 保留标识符，共 32 个 (C11) |
| **标识符** | `main`, `count`, `_temp` | 以字母/下划线开头，后跟字母/数字/下划线 |
| **常量** | `42`, `3.14`, `"hello"`, `'a'` | 整型、浮点、字符串、字符常量 |
| **运算符** | `+`, `-`, `*`, `/`, `++`, `&&` | 算术、逻辑、位运算、赋值等 |
| **界符** | `{`, `}`, `(`, `)`, `;`, `,` | 分隔和组织代码结构 |
| **预处理** | `#include`, `#define` | 编译器指令 (通常单独处理) |

### 1.2 Token 属性

每个 Token 包含以下属性信息：

```c
typedef enum {
    TOKEN_KEYWORD,      // 关键字
    TOKEN_IDENTIFIER,   // 标识符
    TOKEN_INTEGER,      // 整数常量
    TOKEN_FLOAT,        // 浮点常量
    TOKEN_CHAR,         // 字符常量
    TOKEN_STRING,       // 字符串常量
    TOKEN_OPERATOR,     // 运算符
    TOKEN_DELIMITER,    // 界符
    TOKEN_EOF,          // 文件结束
    TOKEN_ERROR         // 错误
} TokenType;

typedef struct {
    TokenType type;     // Token 类型
    char *lexeme;       // 词素 (原始字符串)
    int line;           // 所在行号
    int column;         // 所在列号
    union {
        int int_val;        // 整数值
        double float_val;   // 浮点值
        char char_val;      // 字符值
        char *str_val;      // 字符串值
    } value;
} Token;
```

### 1.3 词法分析器接口

标准词法分析器对外暴露以下接口：

```c
// 初始化词法分析器，传入源代码
typedef struct Lexer Lexer;
Lexer* lexer_init(const char *source, const char *filename);

// 获取下一个 Token
Token lexer_next_token(Lexer *lexer);

// 预读 Token (不消耗)
Token lexer_peek_token(Lexer *lexer);

// 释放资源
void lexer_free(Lexer *lexer);

// 获取错误信息
const char* lexer_get_error(Lexer *lexer);
```

---

## 2. 有限自动机

### 2.1 确定性有限自动机 (DFA)

**形式化定义**：DFA 是一个五元组 $M = (Q, \Sigma, \delta, q_0, F)$

| 符号 | 含义 | 说明 |
|------|------|------|
| $Q$ | 有限状态集合 | $\{q_0, q_1, ..., q_n\}$ |
| $\Sigma$ | 输入字母表 | C 语言字符集 (ASCII/UTF-8) |
| $\delta$ | 转移函数 | $Q \times \Sigma \rightarrow Q$ |
| $q_0$ | 初始状态 | $q_0 \in Q$ |
| $F$ | 接受状态集 | $F \subseteq Q$ |

**DFA 特性**：

- 每个状态对每个输入字符有且仅有一个转移
- 无 $\varepsilon$ 转移 (空转移)
- 识别过程是确定性的

### 2.2 非确定性有限自动机 (NFA)

**形式化定义**：NFA 是一个五元组 $M = (Q, \Sigma, \delta, q_0, F)$

其中转移函数 $\delta: Q \times (\Sigma \cup \{\varepsilon\}) \rightarrow 2^Q$

**NFA 特性**：

- 允许同一输入对应多个转移
- 允许 $\varepsilon$ 转移 (不消耗输入)
- 识别过程需要回溯或并行模拟

### 2.3 子集构造算法

将 NFA 转换为等价的 DFA：

**算法**：子集构造法 (Subset Construction)

```
输入: NFA N = (Q_N, Σ, δ_N, q_0, F_N)
输出: DFA D = (Q_D, Σ, δ_D, {q_0}, F_D)

1. ε-closure(s) = 从状态 s 出发经 ε 转移可达的所有状态
2. 初始状态 D_0 = ε-closure(q_0)
3. Q_D = {D_0}
4. WorkList = {D_0}
5. while WorkList ≠ ∅:
6.     从 WorkList 取出一个状态 T
7.     for 每个输入符号 a ∈ Σ:
8.         U = ε-closure(∪_{t∈T} δ_N(t, a))
9.         if U ∉ Q_D:
10.            Q_D = Q_D ∪ {U}
11.            WorkList = WorkList ∪ {U}
12.        δ_D(T, a) = U
13. F_D = {S ∈ Q_D | S ∩ F_N ≠ ∅}
```

**复杂度分析**：

- 时间复杂度：$O(|Q_N|^2 \cdot |\Sigma| \cdot 2^{|Q_N|})$ 最坏情况
- 空间复杂度：$O(2^{|Q_N|})$ (DFA 状态数指数级增长)

### 2.4 DFA 最小化

**Hopcroft 算法**：将 DFA 状态划分为等价类

```
输入: DFA D = (Q, Σ, δ, q_0, F)
输出: 最小化 DFA D'

1. 初始划分 Π = {F, Q - F}  // 接受状态与非接受状态
2. while Π 发生变化:
3.     for 每个组 G ∈ Π:
4.         根据 δ(s, a) 所在组细分 G
5.         用细分后的组替换 G
6. 构造新 DFA，每个组作为一个状态
```

**复杂度**：$O(|Q| \log |Q|)$

---

## 3. 正则表达式

### 3.1 语法定义

**形式化语法** (巴科斯-瑙尔范式)：

$$
\begin{aligned}
R &\rightarrow R \mid R \quad \text{(并)} \\
  &\rightarrow R \cdot R \quad \text{(连接)} \\
  &\rightarrow R^* \quad \text{(Kleene 闭包)} \\
  &\rightarrow (R) \quad \text{(分组)} \\
  &\rightarrow a \quad \text{(字符)} \\
  &\rightarrow \varepsilon \quad \text{(空串)}
\end{aligned}
$$

**优先级** (从高到低)：

1. Kleene 闭包 `*`
2. 连接 (隐式)
3. 并 `|`

**C 语言标识符正则表达式**：

```regex
[a-zA-Z_][a-zA-Z0-9_]*
```

**C 语言整数常量正则表达式**：

```regex
(0[0-7]*)|(0[xX][0-9a-fA-F]+)|([1-9][0-9]*)
```

### 3.2 Thompson 构造法

将正则表达式转换为 NFA 的算法：

**基本构造规则**：

1. **空串 $\varepsilon$**：两个状态，一个 $\varepsilon$ 转移
2. **单字符 $a$**：两个状态，一个 $a$ 转移
3. **并 $r_1 | r_2$**：新的开始/结束状态，$\varepsilon$ 分支
4. **连接 $r_1 \cdot r_2$**：$r_1$ 的结束连接 $r_2$ 的开始
5. **闭包 $r^*$**：新的开始/结束，添加 $\varepsilon$ 回路

**构造示例**：`a|b*c`

```
      ε        a
    ┌──→(n1)───→(n2)──┐
(s)─┤                 ├─→(e)
    │   ε    ε   b    │
    └──→(n3)←──(n4)───┤
              |  ε    │
              └───────┤
              ε   c   │
            (n5)───→(n6)
```

### 3.3 NFA 到 DFA 转换

**示例**：将标识符正则表达式 `[a-zA-Z_][a-zA-Z0-9_]*` 转换为 DFA

```
NFA 构造:
    ε    [a-zA-Z_]       ε
→(s)──→(n1)───────→(n2)──→(f)
              ↑      │
              │ ε    │ [a-zA-Z0-9_]
              └──────┘

DFA 转换结果:
         [a-zA-Z_]
    ┌─────────────────┐
    ↓                 │
→(S0)──→(S1)←─────────┘
   初始   标识符状态
   状态   (接受状态)
```

---

## 4. 手写实现

### 4.1 整体架构

```
┌─────────────────────────────────────────┐
│           词法分析器架构                 │
├─────────────────────────────────────────┤
│  ┌─────────┐    ┌─────────┐   ┌──────┐ │
│  │ 输入缓冲 │───→│ 扫描器  │──→│ Token│ │
│  └─────────┘    └────┬────┘   └──────┘ │
│                      │                 │
│           ┌─────────┼─────────┐        │
│           ↓         ↓         ↓        │
│      ┌──────┐  ┌──────┐  ┌──────┐      │
│      │标识符│  │数字  │  │运算符│      │
│      │处理器│  │处理器│  │处理器│      │
│      └──────┘  └──────┘  └──────┘      │
└─────────────────────────────────────────┘
```

### 4.2 Token 结构定义

```c
#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>

// Token 类型枚举
typedef enum {
    // 关键字 (32个 C11 关键字)
    TOK_AUTO, TOK_BREAK, TOK_CASE, TOK_CHAR, TOK_CONST,
    TOK_CONTINUE, TOK_DEFAULT, TOK_DO, TOK_DOUBLE, TOK_ELSE,
    TOK_ENUM, TOK_EXTERN, TOK_FLOAT, TOK_FOR, TOK_GOTO,
    TOK_IF, TOK_INLINE, TOK_INT, TOK_LONG, TOK_REGISTER,
    TOK_RESTRICT, TOK_RETURN, TOK_SHORT, TOK_SIGNED, TOK_SIZEOF,
    TOK_STATIC, TOK_STRUCT, TOK_SWITCH, TOK_TYPEDEF, TOK_UNION,
    TOK_UNSIGNED, TOK_VOID, TOK_VOLATILE, TOK_WHILE, TOK_BOOL,    // _Bool
    TOK_COMPLEX, TOK_IMAGINARY,                                   // _Complex, _Imaginary

    // 标识符和常量
    TOK_IDENTIFIER,
    TOK_INTEGER_CONST,
    TOK_FLOAT_CONST,
    TOK_CHAR_CONST,
    TOK_STRING_LITERAL,

    // 运算符
    TOK_PLUS,           // +
    TOK_MINUS,          // -
    TOK_STAR,           // *
    TOK_SLASH,          // /
    TOK_PERCENT,        // %
    TOK_INCREMENT,      // ++
    TOK_DECREMENT,      // --
    TOK_ASSIGN,         // =
    TOK_PLUS_ASSIGN,    // +=
    TOK_MINUS_ASSIGN,   // -=
    TOK_MUL_ASSIGN,     // *=
    TOK_DIV_ASSIGN,     // /=
    TOK_MOD_ASSIGN,     // %=
    TOK_EQ,             // ==
    TOK_NEQ,            // !=
    TOK_LT,             // <
    TOK_GT,             // >
    TOK_LE,             // <=
    TOK_GE,             // >=
    TOK_AND,            // &&
    TOK_OR,             // ||
    TOK_NOT,            // !
    TOK_BIT_AND,        // &
    TOK_BIT_OR,         // |
    TOK_BIT_XOR,        // ^
    TOK_BIT_NOT,        // ~
    TOK_LSHIFT,         // <<
    TOK_RSHIFT,         // >>
    TOK_ARROW,          // ->
    TOK_DOT,            // .

    // 界符
    TOK_LPAREN,         // (
    TOK_RPAREN,         // )
    TOK_LBRACE,         // {
    TOK_RBRACE,         // }
    TOK_LBRACKET,       // [
    TOK_RBRACKET,       // ]
    TOK_SEMICOLON,      // ;
    TOK_COMMA,          // ,
    TOK_COLON,          // :
    TOK_QUESTION,       // ?
    TOK_HASH,           // #
    TOK_ELLIPSIS,       // ...

    // 特殊
    TOK_EOF,
    TOK_ERROR,
    TOK_COMMENT,

    TOKEN_COUNT
} TokenKind;

// Token 结构
typedef struct Token {
    TokenKind kind;
    const char *start;      // 词素起始位置
    size_t length;          // 词素长度
    int line;
    int column;
    union {
        long long int_val;
        double float_val;
        char char_val;
        struct {
            const char *data;
            size_t len;
        } str_val;
    } value;
} Token;

// 词法分析器状态
typedef struct Lexer {
    const char *source;     // 源代码
    const char *filename;   // 文件名
    const char *current;    // 当前扫描位置
    int line;               // 当前行
    int column;             // 当前列
    bool has_error;
    char error_msg[256];
} Lexer;

// 接口函数
Lexer* lexer_create(const char *source, const char *filename);
void lexer_destroy(Lexer *lexer);
Token lexer_next(Lexer *lexer);
Token lexer_peek(Lexer *lexer);
const char* token_kind_name(TokenKind kind);
void token_print(const Token *token);

#endif // LEXER_H
```

### 4.3 核心扫描函数

```c
#include "lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

// 关键字表
static const struct {
    const char *word;
    TokenKind kind;
} keywords[] = {
    {"auto", TOK_AUTO}, {"break", TOK_BREAK}, {"case", TOK_CASE},
    {"char", TOK_CHAR}, {"const", TOK_CONST}, {"continue", TOK_CONTINUE},
    {"default", TOK_DEFAULT}, {"do", TOK_DO}, {"double", TOK_DOUBLE},
    {"else", TOK_ELSE}, {"enum", TOK_ENUM}, {"extern", TOK_EXTERN},
    {"float", TOK_FLOAT}, {"for", TOK_FOR}, {"goto", TOK_GOTO},
    {"if", TOK_IF}, {"inline", TOK_INLINE}, {"int", TOK_INT},
    {"long", TOK_LONG}, {"register", TOK_REGISTER},
    {"restrict", TOK_RESTRICT}, {"return", TOK_RETURN},
    {"short", TOK_SHORT}, {"signed", TOK_SIGNED}, {"sizeof", TOK_SIZEOF},
    {"static", TOK_STATIC}, {"struct", TOK_STRUCT}, {"switch", TOK_SWITCH},
    {"typedef", TOK_TYPEDEF}, {"union", TOK_UNION},
    {"unsigned", TOK_UNSIGNED}, {"void", TOK_VOID},
    {"volatile", TOK_VOLATILE}, {"while", TOK_WHILE},
    {"_Bool", TOK_BOOL}, {"_Complex", TOK_COMPLEX},
    {"_Imaginary", TOK_IMAGINARY},
    {NULL, TOK_IDENTIFIER}
};

// 创建词法分析器
Lexer* lexer_create(const char *source, const char *filename) {
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) return NULL;

    lexer->source = source;
    lexer->filename = filename ? filename : "<input>";
    lexer->current = source;
    lexer->line = 1;
    lexer->column = 1;
    lexer->has_error = false;
    lexer->error_msg[0] = '\0';

    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    free(lexer);
}

// 辅助函数：检查是否到达文件末尾
static bool is_at_end(const Lexer *lexer) {
    return *lexer->current == '\0';
}

// 辅助函数：查看当前字符
static char peek(const Lexer *lexer) {
    return *lexer->current;
}

// 辅助函数：查看下一个字符
static char peek_next(const Lexer *lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->current[1];
}

// 辅助函数：消费当前字符
static char advance(Lexer *lexer) {
    char c = *lexer->current++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

// 辅助函数：匹配特定字符
static bool match(Lexer *lexer, char expected) {
    if (is_at_end(lexer)) return false;
    if (*lexer->current != expected) return false;
    advance(lexer);
    return true;
}

// 跳过空白字符和注释
static void skip_whitespace(Lexer *lexer) {
    while (true) {
        char c = peek(lexer);
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
            case '\f':
            case '\v':
                advance(lexer);
                break;
            case '\n':
                advance(lexer);
                break;
            case '/':
                if (peek_next(lexer) == '/') {
                    // 单行注释
                    while (peek(lexer) != '\n' && !is_at_end(lexer)) {
                        advance(lexer);
                    }
                } else if (peek_next(lexer) == '*') {
                    // 多行注释
                    advance(lexer);  // /
                    advance(lexer);  // *
                    while (!is_at_end(lexer)) {
                        if (peek(lexer) == '*' && peek_next(lexer) == '/') {
                            advance(lexer);  // *
                            advance(lexer);  // /
                            break;
                        }
                        advance(lexer);
                    }
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

// 创建 Token
static Token make_token(Lexer *lexer, TokenKind kind) {
    Token token;
    token.kind = kind;
    token.start = lexer->current;
    token.length = 0;
    token.line = lexer->line;
    token.column = lexer->column;
    return token;
}

// 从位置创建 Token
static Token make_token_at(Lexer *lexer, TokenKind kind, const char *start, size_t length) {
    Token token;
    token.kind = kind;
    token.start = start;
    token.length = length;
    token.line = lexer->line;
    token.column = lexer->column - (int)length;
    return token;
}

// 错误 Token
static Token error_token(Lexer *lexer, const char *msg) {
    Token token;
    token.kind = TOK_ERROR;
    token.start = msg;
    token.length = strlen(msg);
    token.line = lexer->line;
    token.column = lexer->column;
    lexer->has_error = true;
    strncpy(lexer->error_msg, msg, sizeof(lexer->error_msg) - 1);
    return token;
}
```

### 4.4 关键字识别

```c
// 检查关键字
static TokenKind check_keyword(const char *start, size_t length) {
    for (int i = 0; keywords[i].word != NULL; i++) {
        if (strlen(keywords[i].word) == length &&
            memcmp(keywords[i].word, start, length) == 0) {
            return keywords[i].kind;
        }
    }
    return TOK_IDENTIFIER;
}

// 扫描标识符或关键字
static Token identifier(Lexer *lexer) {
    const char *start = lexer->current - 1;  // 第一个字符已消费

    while (isalnum(peek(lexer)) || peek(lexer) == '_') {
        advance(lexer);
    }

    size_t length = lexer->current - start;
    TokenKind kind = check_keyword(start, length);

    return make_token_at(lexer, kind, start, length);
}

// 扫描数字 (整数或浮点数)
static Token number(Lexer *lexer) {
    const char *start = lexer->current - 1;
    bool is_float = false;
    bool is_hex = false;

    // 处理十六进制前缀
    if (*start == '0' && (peek(lexer) == 'x' || peek(lexer) == 'X')) {
        is_hex = true;
        advance(lexer);  // x 或 X
        while (isxdigit(peek(lexer))) {
            advance(lexer);
        }
    } else {
        // 十进制数字
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }

    // 检查小数点
    if (!is_hex && peek(lexer) == '.' && isdigit(peek_next(lexer))) {
        is_float = true;
        advance(lexer);  // .
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }

    // 检查指数部分
    if (!is_hex && (peek(lexer) == 'e' || peek(lexer) == 'E')) {
        is_float = true;
        advance(lexer);
        if (peek(lexer) == '+' || peek(lexer) == '-') {
            advance(lexer);
        }
        if (!isdigit(peek(lexer))) {
            return error_token(lexer, "浮点数指数部分必须有数字");
        }
        while (isdigit(peek(lexer))) {
            advance(lexer);
        }
    }

    // 检查浮点数后缀
    if (peek(lexer) == 'f' || peek(lexer) == 'F' ||
        peek(lexer) == 'l' || peek(lexer) == 'L') {
        is_float = true;
        advance(lexer);
    }

    // 检查整数后缀
    if (!is_float) {
        while (peek(lexer) == 'u' || peek(lexer) == 'U' ||
               peek(lexer) == 'l' || peek(lexer) == 'L') {
            advance(lexer);
        }
    }

    size_t length = lexer->current - start;
    Token token = make_token_at(lexer, is_float ? TOK_FLOAT_CONST : TOK_INTEGER_CONST, start, length);

    // 转换数值
    char *buf = (char*)malloc(length + 1);
    memcpy(buf, start, length);
    buf[length] = '\0';

    if (is_float) {
        token.value.float_val = strtod(buf, NULL);
    } else {
        if (is_hex) {
            token.value.int_val = strtoll(buf, NULL, 16);
        } else if (*start == '0' && length > 1) {
            token.value.int_val = strtoll(buf, NULL, 8);  // 八进制
        } else {
            token.value.int_val = strtoll(buf, NULL, 10);
        }
    }

    free(buf);
    return token;
}

// 扫描字符串
static Token string(Lexer *lexer) {
    const char *start = lexer->current;
    advance(lexer);  // 消费左双引号

    while (peek(lexer) != '"' && !is_at_end(lexer)) {
        if (peek(lexer) == '\n') {
            return error_token(lexer, "字符串不能跨行");
        }
        if (peek(lexer) == '\\') {
            advance(lexer);  // 消费转义字符
            if (is_at_end(lexer)) {
                return error_token(lexer, "不完整的转义序列");
            }
        }
        advance(lexer);
    }

    if (is_at_end(lexer)) {
        return error_token(lexer, "未终止的字符串");
    }

    advance(lexer);  // 消费右双引号

    Token token = make_token_at(lexer, TOK_STRING_LITERAL, start, lexer->current - start);
    token.value.str_val.data = start;
    token.value.str_val.len = lexer->current - start;
    return token;
}

// 扫描字符常量
static Token character(Lexer *lexer) {
    const char *start = lexer->current;
    advance(lexer);  // 消费左单引号

    char c;
    if (peek(lexer) == '\\') {
        advance(lexer);
        switch (peek(lexer)) {
            case 'n': c = '\n'; break;
            case 't': c = '\t'; break;
            case 'r': c = '\r'; break;
            case 'b': c = '\b'; break;
            case 'f': c = '\f'; break;
            case 'v': c = '\v'; break;
            case 'a': c = '\a'; break;
            case '\\': c = '\\'; break;
            case '\'': c = '\''; break;
            case '"': c = '"'; break;
            case '0': c = '\0'; break;
            case 'x':
            case 'X': {
                advance(lexer);
                int val = 0;
                while (isxdigit(peek(lexer))) {
                    char ch = advance(lexer);
                    val = val * 16 + (isdigit(ch) ? ch - '0' :
                                     (isupper(ch) ? ch - 'A' + 10 : ch - 'a' + 10));
                }
                c = (char)val;
                break;
            }
            default:
                c = peek(lexer);
        }
        advance(lexer);
    } else {
        c = advance(lexer);
    }

    if (peek(lexer) != '\'') {
        return error_token(lexer, "字符常量必须用单引号闭合");
    }
    advance(lexer);  // 消费右单引号

    Token token = make_token_at(lexer, TOK_CHAR_CONST, start, lexer->current - start);
    token.value.char_val = c;
    return token;
}
```

### 4.5 完整代码实现

```c
// 主扫描函数
Token lexer_next(Lexer *lexer) {
    skip_whitespace(lexer);

    if (is_at_end(lexer)) {
        return make_token(lexer, TOK_EOF);
    }

    const char *start = lexer->current;
    char c = advance(lexer);

    // 标识符或关键字
    if (isalpha(c) || c == '_') {
        lexer->current = start + 1;  // 调整位置
        return identifier(lexer);
    }

    // 数字
    if (isdigit(c)) {
        lexer->current = start + 1;
        return number(lexer);
    }

    // 字符串
    if (c == '"') {
        lexer->current = start;
        return string(lexer);
    }

    // 字符常量
    if (c == '\'') {
        lexer->current = start;
        return character(lexer);
    }

    // 运算符和界符
    switch (c) {
        // 单字符运算符
        case '(': return make_token_at(lexer, TOK_LPAREN, start, 1);
        case ')': return make_token_at(lexer, TOK_RPAREN, start, 1);
        case '{': return make_token_at(lexer, TOK_LBRACE, start, 1);
        case '}': return make_token_at(lexer, TOK_RBRACE, start, 1);
        case '[': return make_token_at(lexer, TOK_LBRACKET, start, 1);
        case ']': return make_token_at(lexer, TOK_RBRACKET, start, 1);
        case ';': return make_token_at(lexer, TOK_SEMICOLON, start, 1);
        case ',': return make_token_at(lexer, TOK_COMMA, start, 1);
        case ':': return make_token_at(lexer, TOK_COLON, start, 1);
        case '?': return make_token_at(lexer, TOK_QUESTION, start, 1);
        case '~': return make_token_at(lexer, TOK_BIT_NOT, start, 1);
        case '#': return make_token_at(lexer, TOK_HASH, start, 1);

        // 单字符或双字符运算符
        case '+':
            if (match(lexer, '+')) return make_token_at(lexer, TOK_INCREMENT, start, 2);
            if (match(lexer, '=')) return make_token_at(lexer, TOK_PLUS_ASSIGN, start, 2);
            return make_token_at(lexer, TOK_PLUS, start, 1);

        case '-':
            if (match(lexer, '-')) return make_token_at(lexer, TOK_DECREMENT, start, 2);
            if (match(lexer, '=')) return make_token_at(lexer, TOK_MINUS_ASSIGN, start, 2);
            if (match(lexer, '>')) return make_token_at(lexer, TOK_ARROW, start, 2);
            return make_token_at(lexer, TOK_MINUS, start, 1);

        case '*':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_MUL_ASSIGN, start, 2);
            return make_token_at(lexer, TOK_STAR, start, 1);

        case '/':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_DIV_ASSIGN, start, 2);
            return make_token_at(lexer, TOK_SLASH, start, 1);

        case '%':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_MOD_ASSIGN, start, 2);
            return make_token_at(lexer, TOK_PERCENT, start, 1);

        case '=':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_EQ, start, 2);
            return make_token_at(lexer, TOK_ASSIGN, start, 1);

        case '!':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_NEQ, start, 2);
            return make_token_at(lexer, TOK_NOT, start, 1);

        case '<':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_LE, start, 2);
            if (match(lexer, '<')) {
                if (match(lexer, '=')) {
                    return make_token_at(lexer, TOK_LSHIFT, start, 3);  // <<= (C23)
                }
                return make_token_at(lexer, TOK_LSHIFT, start, 2);
            }
            return make_token_at(lexer, TOK_LT, start, 1);

        case '>':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_GE, start, 2);
            if (match(lexer, '>')) {
                if (match(lexer, '=')) {
                    return make_token_at(lexer, TOK_RSHIFT, start, 3);  // >>= (C23)
                }
                return make_token_at(lexer, TOK_RSHIFT, start, 2);
            }
            return make_token_at(lexer, TOK_GT, start, 1);

        case '&':
            if (match(lexer, '&')) return make_token_at(lexer, TOK_AND, start, 2);
            if (match(lexer, '=')) return make_token_at(lexer, TOK_BIT_AND, start, 2);  // &=
            return make_token_at(lexer, TOK_BIT_AND, start, 1);

        case '|':
            if (match(lexer, '|')) return make_token_at(lexer, TOK_OR, start, 2);
            if (match(lexer, '=')) return make_token_at(lexer, TOK_BIT_OR, start, 2);  // |=
            return make_token_at(lexer, TOK_BIT_OR, start, 1);

        case '^':
            if (match(lexer, '=')) return make_token_at(lexer, TOK_BIT_XOR, start, 2);  // ^=
            return make_token_at(lexer, TOK_BIT_XOR, start, 1);

        case '.':
            if (match(lexer, '.')) {
                if (match(lexer, '.')) {
                    return make_token_at(lexer, TOK_ELLIPSIS, start, 3);
                }
                // 错误: 只有两点
                return error_token(lexer, "无效的运算符 '..'");
            }
            return make_token_at(lexer, TOK_DOT, start, 1);
    }

    return error_token(lexer, "未知的字符");
}

// Token 类型名称
static const char* token_names[] = {
    "AUTO", "BREAK", "CASE", "CHAR", "CONST",
    "CONTINUE", "DEFAULT", "DO", "DOUBLE", "ELSE",
    "ENUM", "EXTERN", "FLOAT", "FOR", "GOTO",
    "IF", "INLINE", "INT", "LONG", "REGISTER",
    "RESTRICT", "RETURN", "SHORT", "SIGNED", "SIZEOF",
    "STATIC", "STRUCT", "SWITCH", "TYPEDEF", "UNION",
    "UNSIGNED", "VOID", "VOLATILE", "WHILE",
    "BOOL", "COMPLEX", "IMAGINARY",
    "IDENTIFIER", "INTEGER_CONST", "FLOAT_CONST",
    "CHAR_CONST", "STRING_LITERAL",
    "PLUS", "MINUS", "STAR", "SLASH", "PERCENT",
    "INCREMENT", "DECREMENT", "ASSIGN",
    "PLUS_ASSIGN", "MINUS_ASSIGN", "MUL_ASSIGN", "DIV_ASSIGN", "MOD_ASSIGN",
    "EQ", "NEQ", "LT", "GT", "LE", "GE",
    "AND", "OR", "NOT", "BIT_AND", "BIT_OR", "BIT_XOR", "BIT_NOT",
    "LSHIFT", "RSHIFT", "ARROW", "DOT",
    "LPAREN", "RPAREN", "LBRACE", "RBRACE",
    "LBRACKET", "RBRACKET", "SEMICOLON", "COMMA",
    "COLON", "QUESTION", "HASH", "ELLIPSIS",
    "EOF", "ERROR", "COMMENT"
};

const char* token_kind_name(TokenKind kind) {
    if (kind >= 0 && kind < TOKEN_COUNT) {
        return token_names[kind];
    }
    return "UNKNOWN";
}

void token_print(const Token *token) {
    printf("Token(%s, line=%d, col=%d, ",
           token_kind_name(token->kind), token->line, token->column);

    if (token->length > 0 && token->start) {
        printf("lexeme=\"");
        for (size_t i = 0; i < token->length && i < 50; i++) {
            char c = token->start[i];
            if (c == '\n') printf("\\n");
            else if (c == '\t') printf("\\t");
            else if (c == '"') printf("\\\"");
            else if (c == '\\') printf("\\\\");
            else putchar(c);
        }
        if (token->length > 50) printf("...");
        printf("\")");
    }

    // 打印值
    switch (token->kind) {
        case TOK_INTEGER_CONST:
            printf(", val=%lld", token->value.int_val);
            break;
        case TOK_FLOAT_CONST:
            printf(", val=%g", token->value.float_val);
            break;
        case TOK_CHAR_CONST:
            printf(", val='%c'", token->value.char_val);
            break;
        default:
            break;
    }

    printf(")\n");
}

// 预读下一个 Token (不消耗)
Token lexer_peek(Lexer *lexer) {
    const char *saved_current = lexer->current;
    int saved_line = lexer->line;
    int saved_column = lexer->column;
    bool saved_error = lexer->has_error;

    Token token = lexer_next(lexer);

    lexer->current = saved_current;
    lexer->line = saved_line;
    lexer->column = saved_column;
    lexer->has_error = saved_error;

    return token;
}
```

**测试用例**：

```c
// test_lexer.c
#include "lexer.h"
#include <stdio.h>

int main() {
    const char *source =
        "int main() {\n"
        "    // 计算阶乘\n"
        "    int n = 5;\n"
        "    int result = 1;\n"
        "    for (int i = 1; i <= n; i++) {\n"
        "        result *= i;\n"
        "    }\n"
        "    return result;\n"
        "}\n";

    Lexer *lexer = lexer_create(source, "test.c");
    if (!lexer) {
        fprintf(stderr, "无法创建词法分析器\n");
        return 1;
    }

    printf("=== 词法分析测试 ===\n\n");

    Token token;
    int count = 0;
    do {
        token = lexer_next(lexer);
        token_print(&token);
        count++;
    } while (token.kind != TOK_EOF && token.kind != TOK_ERROR && count < 100);

    if (lexer->has_error) {
        printf("\n错误: %s\n", lexer->error_msg);
    }

    printf("\n总计 %d 个 Token\n", count);

    lexer_destroy(lexer);
    return 0;
}
```

---

## 5. 错误处理

### 5.1 错误类型

C 语言词法分析中常见的错误类型：

| 错误类型 | 示例 | 说明 |
|----------|------|------|
| **非法字符** | `@`, `$` | 不在 C 字符集中的符号 |
| **未终止字符串** | `"hello` | 缺少右双引号 |
| **未终止注释** | `/* ...` | 多行注释未闭合 |
| **非法转义** | `"\q"` | 无效的转义序列 |
| **数字格式错误** | `0xG1` | 十六进制含非法字符 |
| **字符过长** | `'ab'` | 字符常量含多个字符 |

### 5.2 错误恢复策略

**Panic Mode (紧急模式)恢复**：

```c
// 跳过错误字符直到同步点
static void synchronize(Lexer *lexer) {
    // 同步点：语句结束或块开始
    while (!is_at_end(lexer)) {
        switch (peek(lexer)) {
            case ';':
            case '{':
            case '}':
                return;
            default:
                advance(lexer);
        }
    }
}
```

**错误恢复原则**：

1. 报告错误时提供行号、列号和错误描述
2. 尝试从错误中恢复，继续分析
3. 避免级联错误 (同一错误导致的多个报告)

### 5.3 错误报告机制

```c
typedef struct {
    int line;
    int column;
    const char *message;
    const char *filename;
} LexerError;

// 格式化错误报告
void lexer_report_error(const LexerError *err) {
    fprintf(stderr, "%s:%d:%d: 词法错误: %s\n",
            err->filename, err->line, err->column, err->message);

    // 打印源代码上下文
    // ...
}
```

---

## 6. 深入理解

### 6.1 实现机制分析

**双缓冲区技术**：

```
┌─────────────────────────────────────────────────────┐
│  缓冲区1 (N字节)   │   缓冲区2 (N字节)   │   EOF    │
│  [当前扫描区域]    │   [预读区域]        │          │
└─────────────────────────────────────────────────────┘
         ↑
    lexeme_begin (当前词素起始)
         ↓
    forward (当前扫描位置)
```

双缓冲区解决了长词素跨边界的问题，保证任意 `lexeme` 可以完整载入内存。

**哨兵标记优化**：

在缓冲区末尾放置哨兵字符 (如 `EOF`)，避免每次检查边界：

```c
// 使用哨兵优化后
char c = *forward++;
if (c == EOF_MARKER) {
    // 处理缓冲区切换或文件结束
    c = handle_eof();
}
```

### 6.2 性能优化策略

| 优化策略 | 实现方式 | 效果 |
|----------|----------|------|
| **关键字哈希** | 完美哈希函数 | O(1) 关键字查找 |
| **状态机直接编码** | switch/goto 实现 | 减少函数调用开销 |
| **延迟数值转换** | Token 保存字符串 | 避免不必要的转换 |
| **预读优化** | 减少 peek 调用 | 降低内存访问次数 |
| **内存池** | 预分配 Token 内存 | 减少 malloc 开销 |

---

## 7. 思维表征

### 7.1 Token 类型决策树

```
                    输入字符
                       │
           ┌───────────┼───────────┬───────────┐
           ↓           ↓           ↓           ↓
        空白/注释    字母/_      数字       其他字符
           │           │           │           │
           ↓           ↓           ↓           ↓
        [跳过]    ┌─────────┐  ┌─────────┐   ┌─────────┐
                  │标识符/  │  │数字常量 │   │运算符/  │
                  │关键字?  │  │解析    │   │界符     │
                  └───┬─────┘  └────┬────┘   └────┬────┘
                      ↓             ↓              ↓
               ┌──────┴──────┐   ┌─┴─┐      ┌──────┴──────┐
               ↓             ↓   ↓   ↓      ↓             ↓
            查关键字表    标识符  整数 浮点  单字符界符  多字符运算符
               │             │               │             │
               ↓             ↓               ↓             ↓
            匹配成功      标识符Token      界符Token   查运算符表
               │
               ↓
           关键字Token
```

### 7.2 DFA vs NFA 对比矩阵

| 特性 | DFA | NFA |
|------|-----|-----|
| **状态转移** | 单值函数 $\delta: Q \times \Sigma \rightarrow Q$ | 多值函数 $\delta: Q \times \Sigma \rightarrow 2^Q$ |
| **$\varepsilon$ 转移** | 不允许 | 允许 |
| **识别过程** | 确定性，无回溯 | 非确定性，需回溯或并行 |
| **实现难度** | 简单，直接编码 | 复杂，需模拟执行 |
| **状态数量** | 可能指数级增长 | 线性于正则表达式大小 |
| **空间复杂度** | $O(2^{\|Q_N\|})$ | $O(\|Q_N\|)$ |
| **时间复杂度** | $O(\|w\|)$ | $O(\|Q_N\|^2 \cdot \|w\|)$ |
| **典型应用** | 手写词法分析器 | 正则表达式引擎内部 |

### 7.3 词法分析流程图

```mermaid
flowchart TD
    Start([开始]) --> Init[初始化词法分析器]
    Init --> Skip[跳过空白/注释]
    Skip --> Check{文件结束?}
    Check -->|是| End([返回 EOF])
    Check -->|否| First{首字符类型}

    First -->|字母/_| Id[扫描标识符]
    First -->|数字| Num[扫描数字]
    First -->|"| Str[扫描字符串]
    First -->|'| Char[扫描字符]
    First -->|运算符| Op[识别运算符]
    First -->|界符| Delim[识别界符]
    First -->|其他| Err[报告错误]

    Id --> Kw{关键字?}
    Kw -->|是| KwTok[返回关键字Token]
    Kw -->|否| IdTok[返回标识符Token]

    Num --> NumFmt{格式}
    NumFmt -->|整数| IntTok[返回整数Token]
    NumFmt -->|浮点| FloatTok[返回浮点Token]

    Str --> StrErr{字符串错误?}
    StrErr -->|是| StrErrTok[返回错误Token]
    StrErr -->|否| StrTok[返回字符串Token]

    Char --> CharTok[返回字符Token]
    Op --> OpTok[返回运算符Token]
    Delim --> DelimTok[返回界符Token]

    KwTok --> Out
    IdTok --> Out
    IntTok --> Out
    FloatTok --> Out
    StrTok --> Out
    CharTok --> Out
    OpTok --> Out
    DelimTok --> Out
    StrErrTok --> Out
    Err --> Sync[同步恢复]
    Sync --> Skip

    Out([返回Token])
```

---

## 8. 层次映射

### 8.1 与语法分析的关系

```
┌─────────────────────────────────────────────────────────────┐
│                     编译器前端架构                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────────┐      ┌──────────────────┐            │
│  │   词法分析器      │ ───→ │   语法分析器      │            │
│  │   (Lexical)      │      │   (Syntax)       │            │
│  │                  │      │                  │            │
│  │  输入: 字符流     │      │  输入: Token流    │            │
│  │  输出: Token流    │      │  输出: 语法树     │            │
│  │                  │      │                  │            │
│  │  职责:           │      │  职责:           │            │
│  │  - 识别词素      │      │  - 识别语法结构   │            │
│  │  - 分类Token     │      │  - 构建AST       │            │
│  │  - 删除空白/注释 │      │  - 语法错误检测   │            │
│  └──────────────────┘      └──────────────────┘            │
│           │                           │                     │
│           ↓                           ↓                     │
│      ┌──────────────────────────────────────┐              │
│      │           符号表 (Symbol Table)       │              │
│      │  - 存储标识符信息                      │              │
│      │  - 管理作用域                          │              │
│      │  - 类型信息                            │              │
│      └──────────────────────────────────────┘              │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

**接口契约**：

```c
// 词法分析器向语法分析器提供的接口
typedef struct {
    Token (*get_next_token)(void *lexer_state);
    Token (*peek_token)(void *lexer_state, int lookahead);
    const char* (*get_error)(void *lexer_state);
} LexerInterface;

// 语法分析器通过回调获取 Token
void parser_init(Parser *parser, LexerInterface *lexer_if, void *lexer_state);
```

### 8.2 与符号表的交互

**交互时机**：

1. **标识符识别时**：查询符号表，确认是类型名还是普通标识符
2. **常量存储时**：字符串常量、浮点常量存入常量表
3. **预处理宏**：宏定义需提前进入符号表

```c
// 标识符识别的符号表交互
typedef enum { ID_VAR, ID_FUNC, ID_TYPE, ID_ENUM_CONST } IdKind;

typedef struct Symbol {
    char *name;
    IdKind kind;
    Type *type;
    Scope *scope;
    // ... 其他属性
} Symbol;

static Token identifier_with_symbol_table(Lexer *lexer, SymbolTable *symtab) {
    Token token = identifier(lexer);

    // 查询符号表
    Symbol *sym = symbol_table_lookup(symtab, token.start, token.length);

    if (sym && sym->kind == ID_TYPE) {
        // 类型名在 C 语言中有特殊处理 (typedef)
        token.kind = TOK_TYPE_NAME;
    }

    // 保存符号引用
    token.value.sym_ref = sym;

    return token;
}
```

---

## 总结

词法分析是编译器前端的基础阶段，其核心任务是：

1. **识别词素**：从字符流中识别出最小的语义单元
2. **分类 Token**：将词素归类为关键字、标识符、常量等类型
3. **过滤无关内容**：删除空白、注释等不影响语义的字符
4. **错误检测**：发现并报告词法层面的错误

**关键要点**：

- 有限自动机是词法分析的理论基础，DFA 适合直接编码实现
- 正则表达式描述词法规则，Thompson 构造法实现正则到 NFA 的转换
- 手写词法分析器需关注性能优化：双缓冲区、完美哈希、状态机直接编码
- 词法分析与语法分析通过 Token 流解耦，通过符号表共享标识符信息

---

## 参考

1. Aho, A. V., et al. "Compilers: Principles, Techniques, and Tools" (龙书)
2. C11 Standard (ISO/IEC 9899:2011)
3. C23 Draft Standard (ISO/IEC 9899:2023)
4. Flex - The Fast Lexical Analyzer Generator
