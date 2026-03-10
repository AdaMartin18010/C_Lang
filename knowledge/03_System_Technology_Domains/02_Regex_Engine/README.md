# 02 Regex Engine - 正则表达式引擎

> **难度等级**: L4 | **预估学习时间**: 15-20小时 | **前置知识**: 有限自动机理论、C基础数据结构

---

## 技术概述

正则表达式引擎是实现模式匹配算法的核心组件，广泛应用于文本处理、日志分析、编译器词法分析、网络协议解析等场景。
本模块深入讲解两种经典的正则表达式实现方法：Thompson的NFA构造法和Pike的VM实现法。

### 核心概念

| 概念 | 说明 | 复杂度 |
|:-----|:-----|:------:|
| **Thompson NFA** | 将正则表达式转换为NFA，支持线性时间匹配 | O(n) 匹配时间 |
| **Pike VM** | 基于虚拟机的回溯模拟，支持捕获组和复杂特性 | O(n·m) 匹配时间 |
| **DFA转换** | NFA到DFA的子集构造，无回溯 | 构造O(2ⁿ)，匹配O(n) |

### 正则引擎分类

```text
正则引擎实现方式
├── DFA引擎 (确定性有限自动机)
│   ├── 特点: 无回溯，线性时间，不支持捕获组/反向引用
│   └── 代表: RE2, Go regexp, grep
│
├── NFA引擎 (非确定性有限自动机)
│   ├── Thompson NFA (并行模拟)
│   │   └── 特点: 线性时间，支持捕获组
│   ├── 回溯NFA (深度优先)
│   │   └── 特点: 支持所有特性，可能指数爆炸
│   └── Pike VM (虚拟机模拟)
│       └── 特点: 简洁实现，支持复杂特性
│
└── 混合引擎
    └── DFA快速路径 + NFA复杂特性回退
```

---

## 应用场景

### 1. 词法分析器 (Lexer)

编译器/解释器的第一阶段，将源代码转换为Token流。

```c
// 简单词法分析器示例
token_t lex(char* src) {
    if (match_regex(src, "[a-zA-Z_][a-zA-Z0-9_]*"))
        return TOKEN_IDENTIFIER;
    if (match_regex(src, "[0-9]+"))
        return TOKEN_NUMBER;
    // ...
}
```

### 2. 日志解析与提取

从海量日志中提取结构化信息，如IP地址、时间戳、错误码等。

### 3. 网络协议分析

HTTP报文解析、URL路由匹配、防火墙规则匹配等。

### 4. 数据验证

邮箱格式、电话号码、身份证号等结构化数据校验。

### 5. 代码分析与重构

静态分析工具中的模式匹配、代码格式化、重构建议。

---

## 文档列表

| 文件 | 主题 | 难度 | 核心内容 |
|:-----|:-----|:----:|:---------|
| [01_NFA_Implementation.md](./01_NFA_Implementation.md) | Thompson NFA实现 | L4 | ε-NFA构造、子集构造算法、并行状态模拟、线性时间匹配保证 |
| [02_Pike_VM.md](./02_Pike_VM.md) | Pike虚拟机 | L4 | 指令编码、线程调度、捕获组实现、回溯控制、递归模式支持 |

### 学习路径建议

```
NFA基础理论 → Thompson构造 → Pike VM → 优化技巧
     ↓              ↓           ↓          ↓
    3天            4天         5天        3天
```

---

## 参考开源项目

### DFA/NFA引擎

| 项目 | 语言 | 特点 | 链接 |
|:-----|:-----|:-----|:-----|
| **RE2** | C++ | Google的DFA正则引擎，保证线性时间 | <https://github.com/google/re2> |
| **Oniguruma** | C | Ruby使用的正则引擎，支持复杂特性 | <https://github.com/kkos/oniguruma> |
| **PCRE2** | C | Perl兼容正则表达式，功能最全面 | <https://github.com/PCRE2Project/pcre2> |
| **Go regexp** | Go | RE2风格，纯DFA实现 | <https://golang.org/pkg/regexp/> |
| **Hyperscan** | C++ | Intel的高性能正则匹配库，支持SIMD | <https://github.com/intel/hyperscan> |

### 教学/轻量级实现

| 项目 | 语言 | 特点 | 链接 |
|:-----|:-----|:-----|:-----|
| **slre** | C | 超级轻量级正则引擎，仅几百行代码 | <https://github.com/cesanta/slre> |
| **tre** | C | POSIX兼容，支持近似匹配 | <https://github.com/laurikari/tre> |
| **tiny-regex-c** | C | 极简实现，适合学习 | <https://github.com/kokke/tiny-regex-c> |
| **regex-c** | C | 基于Thompson NFA的教学实现 | <https://github.com/gedmarc/regex-c> |

### 相关工具

| 项目 | 说明 | 链接 |
|:-----|:-----|:-----|
| **RegexBuddy** | 正则表达式调试工具 | <https://www.regexbuddy.com/> |
| **regex101** | 在线正则测试工具 | <https://regex101.com/> |

---

## 技术架构图

```mermaid
flowchart TB
    subgraph Input["输入"]
        PATTERN[正则表达式<br/>a(b|c)*]
        TEXT[目标文本<br/>"abcb"]
    end

    subgraph Compilation["编译阶段"]
        PARSE[语法分析<br/>构建AST]
        CONSTRUCT[NFA构造<br/>Thompson算法]
        OPT[优化<br/>ε-消除/DFA缓存]
    end

    subgraph Execution["执行阶段"]
        subgraph NFA_Engine["NFA引擎"]
            PARALLEL[并行状态模拟<br/>O(n)时间]
            CAPTURE[捕获组记录<br/>子匹配提取]
        end

        subgraph VM_Engine["Pike VM引擎"]
            BYTECODE[字节码生成<br/>特殊指令集]
            THREAD[线程调度<br/>回溯管理]
        end
    end

    subgraph Output["输出"]
        MATCH[匹配结果<br/>boolean]
        GROUPS[捕获组<br/>子匹配数组]
    end

    PATTERN --> PARSE --> CONSTRUCT
    CONSTRUCT --> NFA_Engine
    CONSTRUCT --> VM_Engine
    TEXT --> NFA_Engine --> MATCH
    TEXT --> VM_Engine --> GROUPS
```

---

## 核心算法速查

### Thompson NFA构造

```c
// 基本构造规则
// ε:  ε
// a:  a
// AB: A -ε-> B
// A|B: ε->A, ε->B, A->ε, B->ε
// A*: ε->A->ε, A内部ε循环

typedef struct State {
    int c;              // 字符或Split
    struct State *out;  // 主输出
    struct State *out1; // 次输出(用于Split)
    int lastlist;       // 用于状态标记
} State;

#define Split 256
#define Match 257

// Thompson构造伪代码
State* thompson_construct(AST* ast) {
    switch (ast->type) {
        case CHAR:
            return create_state(ast->c, NULL, NULL);
        case CONCAT:
            return concat(thompson_construct(ast->left),
                         thompson_construct(ast->right));
        case UNION:
            return union_state(thompson_construct(ast->left),
                              thompson_construct(ast->right));
        case STAR:
            return star(thompson_construct(ast->left));
    }
}
```

### Pike VM核心循环

```c
// Pike VM: 基于线程的NFA模拟
typedef struct Thread {
    Inst* pc;
    char* sp;
    SubMatch subs[MAX_SUB];  // 捕获组
} Thread;

bool pike_vm_match(Prog* prog, char* input) {
    ThreadList* curr = create_list(prog->len);
    ThreadList* next = create_list(prog->len);

    add_thread(curr, (Thread){prog->start, input, {0}}, input);

    for (char* sp = input; *sp; sp++) {
        for (int i = 0; i < curr->n; i++) {
            Thread t = curr->t[i];
            switch (t.pc->opcode) {
                case CHAR:
                    if (*t.sp == t.pc->c)
                        add_thread(next, (Thread){t.pc+1, t.sp+1, t.subs}, input);
                    break;
                case JMP:
                    add_thread(curr, (Thread){t.pc->x, t.sp, t.subs}, input);
                    break;
                case SPLIT:
                    add_thread(curr, (Thread){t.pc->x, t.sp, t.subs}, input);
                    add_thread(curr, (Thread){t.pc->y, t.sp, t.subs}, input);
                    break;
                case SAVE:
                    t.subs[t.pc->n] = t.sp;
                    add_thread(curr, (Thread){t.pc+1, t.sp, t.subs}, input);
                    break;
                case MATCH:
                    return true;
            }
        }
        swap(&curr, &next);
        clear(next);
    }
    return false;
}
```

---

## 性能对比

| 实现方式 | 构造时间 | 匹配时间 | 内存占用 | 捕获组 | 特性支持 |
|:---------|:--------:|:--------:|:--------:|:------:|:--------:|
| Thompson NFA | O(n) | O(m·n) | 中等 | 支持 | 基础 |
| DFA | O(2ⁿ) | O(m) | 高 | 不支持 | 基础 |
| Pike VM | O(n) | O(m·n²) | 低 | 支持 | 完整 |
| 回溯NFA | O(n) | O(2^m) | 低 | 支持 | 最全 |

> n = 正则长度, m = 文本长度

---

## 关联知识

| 目标 | 路径 |
|:-----|:-----|
| 返回上层 | [03_System_Technology_Domains](../README.md) |
| 核心基础 | [01_Core_Knowledge_System](../../01_Core_Knowledge_System/README.md) |
| 状态机理论 | [02_Formal_Semantics_and_Physics](../../02_Formal_Semantics_and_Physics/README.md) |
| 编译器前端 | [02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md) |

---

## 推荐学习资源

### 经典论文

- "Regular Expression Search Algorithm" - Ken Thompson (1968)
- "The Text Editor sam" - Rob Pike (1987)
- "Regular Expression Matching Can Be Simple And Fast" - Russ Cox (2007)
- "Regular Expression Matching in the Wild" - Russ Cox (2010)

### 在线资源

- Russ Cox的Regular Expression系列文章 (swtch.com/~rsc/regexp/)
- RE2文档和源码分析

---

> **最后更新**: 2026-03-10
>
> **维护者**: C语言知识库团队
