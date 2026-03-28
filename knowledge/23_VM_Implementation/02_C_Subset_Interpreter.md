# C子集解释器实现

> **层级定位**: 23 VM Implementation / 解释器层
> **前置知识**: [栈式虚拟机](./01_Stack_Based_VM.md), [C到汇编：控制流](../22_Assembler_Implementation/04_C_to_Assembly_Control.md)
> **后续延伸**: [从图灵机到C](../24_Complete_Chain/01_From_Turing_to_C.md)
> **横向关联**: [C4编译器](https://github.com/rswier/c4), [编译器基础](../01_Core_Knowledge_System/15_Compiler_Basics/01_Lexical_Analysis.md)
> **预计学习时间**: 2周
> **实践验证**: [c4_interpreter.c](../../examples/interpreter/c4_interpreter.c)

---

## 核心洞察

```
解释器直接执行源代码或AST，无需编译到机器码。
它是理解语言实现的最直接方式。

通过实现C子集解释器，您将理解：
1. 词法分析：将字符流转换为Token
2. 语法分析：将Token流转换为AST
3. 递归下降解析：手工实现解析器的技术
4. 树遍历求值：执行AST的直接方式
5. 环境与作用域：变量查找的机制

C4编译器是一个仅4个函数、约500行C代码的
C子集编译器/解释器，是学习解释器的绝佳起点。
```

---

## 1. C4解释器架构

### 1.1 整体流程

```
C4解释器架构:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

源代码
   ↓
词法分析 (Lexer)
   ↓
Token流: [INT, MAIN, LPAREN, RPAREN, LBRACE, ...]
   ↓
语法分析 + 代码生成 (Parser)
   ↦ 使用递归下降解析
   ↦ 直接生成虚拟机字节码
   ↓
字节码/中间表示
   ↓
虚拟机执行 (VM)
   ↓
结果

C4的独特之处：
- 没有显式的AST构建
- 解析时直接生成字节码
- 极简设计：4个核心函数
```

### 1.2 C4支持的C子集

```
C4支持的语法:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

数据类型:
- int (32位整数，唯一支持的类型)
- 指针（通过int模拟）

表达式:
- 算术: +, -, *, /, %
- 关系: <, >, <=, >=, ==, !=
- 位运算: |, &, ^, <<, >>
- 逻辑: &&, ||
- 赋值: =, +=, -=, *=, /=
- 递增/递减: ++, --
- 条件: ? :

语句:
- 表达式语句
- if-else
- while
- return
- 代码块 {...}

声明:
- 全局变量
- 局部变量
- 函数（支持递归）

库函数（内建）:
- printf
- malloc
- free
- memset
- exit
- open, read, close

不支持:
- 结构体
- 数组（可通过指针模拟）
- for循环（可用while模拟）
- switch
- 多类型
```

### 1.3 核心数据结构

```c
// C4核心数据结构

// Token类型
enum {
    Num = 128, Fun, Sys, Glo, Loc, Id,
    Char, Else, Enum, If, Int, Return, Sizeof, While,
    Assign, Cond, Lor, Lan, Or, Xor, And, Eq, Ne, Lt, Gt, Le, Ge, Shl, Shr, Add, Sub, Mul, Div, Mod, Inc, Dec, Brak
};

// 词法单元
int token;              // 当前token类型
int token_val;          // token值（数字或字符串表索引）
char *src;              // 源代码指针

// 符号表
struct ident {
    int tk;             // 类型：Id, Fun, Sys, Glo, Loc
    int hash;           // 哈希值
    char *name;         // 名称
    int class;          // 存储类别
    int type;           // 数据类型
    int val;            // 值（变量地址或函数地址）
    int hclass;         // 用于查找隐藏的外部变量
    int htype;
    int hval;
    struct ident *next; // 哈希冲突链
} *id,  // 当前标识符
  *sym; // 符号表

// 生成的代码
int *e;                 // 代码段
int *le;                // 当前代码位置

// 数据段
char *data;             // 数据段（存储字符串和全局变量）
int *text;              // 代码段
int *stack;             // 栈

// 寄存器模拟
int pc;                 // 程序计数器
int sp;                 // 栈指针
int bp;                 // 基址指针
int ax;                 // 累加器
```

---

## 2. 词法分析实现

### 2.1 词法分析器

```c
// C4词法分析器核心

void next() {
    char *pp;

    while ((token = *src)) {
        ++src;

        // 标识符或关键字
        if ((token >= 'a' && token <= 'z') ||
            (token >= 'A' && token <= 'Z') ||
            token == '_') {
            pp = src - 1;
            // 计算哈希
            while ((*src >= 'a' && *src <= 'z') ||
                   (*src >= 'A' && *src <= 'Z') ||
                   (*src >= '0' && *src <= '9') ||
                   *src == '_')
                hash = hash * 147 + *src++;
            // 查找符号表
            // ...
            return;
        }

        // 数字
        if (token >= '0' && token <= '9') {
            token_val = token - '0';
            while (*src >= '0' && *src <= '9')
                token_val = token_val * 10 + *src++ - '0';
            token = Num;
            return;
        }

        // 字符串
        if (token == '"') {
            pp = data;
            while (*src != '"' && *src) {
                if ((token_val = *src++) == '\\') {
                    // 转义字符处理
                    if ((token_val = *src++) == 'n') token_val = '\n';
                }
                *data++ = token_val;
            }
            *data++ = 0;
            src++;
            token_val = (int)pp;  // 字符串地址
            token = Num;
            return;
        }

        // 注释
        if (token == '/') {
            if (*src == '/') {
                // 单行注释
                while (*src != 0 && *src != '\n') ++src;
            } else {
                token = Div;
                return;
            }
        }

        // 多字符运算符
        if (token == '=') { if (*src == '=') { src++; token = Eq; } else token = Assign; return; }
        if (token == '+') { if (*src == '+') { src++; token = Inc; } else token = Add; return; }
        if (token == '-') { if (*src == '-') { src++; token = Dec; } else token = Sub; return; }
        if (token == '!') { if (*src == '=') { src++; token = Ne; } return; }
        if (token == '<') { if (*src == '=') { src++; token = Le; } else if (*src == '<') { src++; token = Shl; } else token = Lt; return; }
        if (token == '>') { if (*src == '=') { src++; token = Ge; } else if (*src == '>') { src++; token = Shr; } else token = Gt; return; }
        if (token == '|') { if (*src == '|') { src++; token = Lor; } else token = Or; return; }
        if (token == '&') { if (*src == '&') { src++; token = Lan; } else token = And; return; }
        if (token == '^') { token = Xor; return; }
        if (token == '%') { token = Mod; return; }
        if (token == '*') { token = Mul; return; }
        if (token == '[') { token = Brak; return; }
        if (token == '?') { token = Cond; return; }

        // 单字符token直接返回（如'(', ')', '{', '}', ';', ',', ...）
        return;
    }
}
```

### 2.2 词法分析示例

```c
// 源代码
"int main() { return 42; }"

// Token序列
INT        // int
Id         // main (标识符)
LPAREN     // (
RPAREN     // )
LBRACE     // {
RETURN     // return
Num (42)   // 42
SEMICOLON  // ;
RBRACE     // }
```

---

## 3. 语法分析与递归下降

### 3.1 递归下降解析原理

```
递归下降解析:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

为每个语法规则写一个函数
函数相互递归调用

表达式语法（简化）：
    expr    = assign
    assign  = equality ("=" assign)?
    equality= relational (("=="|"!=") relational)*
    relational = add (("<"|">"|"<="|">=") add)*
    add     = mul (("+"|"-") mul)*
    mul     = unary (("*"|"/"|"%") unary)*
    unary   = ("+"|"-"|"*"|"&") unary | primary
    primary = num | id | "(" expr ")" | id "(" args ")"

对应C4的实现:
    expr()   -> parse expression with lowest precedence
    level = Assign -> level = Cond -> level = Lor -> ... -> level = Inc
    unary()  -> handle unary operators
    primary()-> handle primary expressions
```

### 3.2 表达式解析

```c
// C4表达式解析

void expr(int lev) {
    int t, *d;

    // 处理一元运算符
    if (!token) { printf("%d: unexpected eof\n", line); exit(-1); }
    else if (token == Num) { *++e = IMM; *++e = token_val; next(); ty = INT; }
    else if (token == '"') {
        // 字符串常量
        *++e = IMM; *++e = token_val; next();
        while (token == '"') next();
        data = (char *)((int)data + sizeof(int) & -sizeof(int));
        ty = PTR;
    }
    else if (token == Sizeof) {
        // sizeof运算符
        next(); if (token == Int) next(); else if (token == Char) next();
        *++e = IMM; *++e = (ty == Char) ? 1 : 4;
        ty = INT;
    }
    else if (token == Id) {
        // 标识符
        d = id; next();
        if (token == '(') {
            // 函数调用
            next();
            t = 0;
            while (token != ')') { expr(Assign); *++e = PUSH; ++t; if (token == ',') next(); }
            next();
            // 内建函数 vs 用户定义函数
            if (d[Class] == Sys) *++e = d[Val];
            else if (d[Class] == Fun) { *++e = JSR; *++e = d[Val]; }
            else { printf("%d: bad function call\n", line); exit(-1); }
            if (t) { *++e = ADJ; *++e = t; }
            ty = d[Type];
        }
        else if (d[Class] == Num) { *++e = IMM; *++e = d[Val]; ty = INT; }
        else {
            // 变量
            if (d[Class] == Loc) { *++e = LEA; *++e = loc - d[Val]; }
            else if (d[Class] == Glo) { *++e = IMM; *++e = d[Val]; }
            else { printf("%d: undefined variable\n", line); exit(-1); }
            *++e = ((ty = d[Type]) == Char) ? LC : LI;
        }
    }
    else if (token == '(') {
        // 括号表达式或类型转换
        next();
        if (token == Int || token == Char) {
            t = (token == Char) ? CHAR : INT; next();
            while (token == Mul) { next(); t = t + PTR; }
            if (token == ')') next(); else { printf("%d: bad cast\n", line); exit(-1); }
            expr(Inc);
            ty = t;
        }
        else {
            expr(Assign);
            if (token == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }
        }
    }
    else if (token == Mul) {
        // 解引用
        next(); expr(Inc);
        if (ty > INT) ty = ty - PTR; else { printf("%d: bad dereference\n", line); exit(-1); }
        *++e = (ty == CHAR) ? LC : LI;
    }
    else if (token == And) {
        // 取地址
        next(); expr(Inc);
        if (*e == LC || *e == LI) --e; else { printf("%d: bad address-of\n", line); exit(-1); }
        ty = ty + PTR;
    }
    else if (token == '!') { next(); expr(Inc); *++e = PUSH; *++e = IMM; *++e = 0; *++e = EQ; ty = INT; }
    else if (token == '~') { next(); expr(Inc); *++e = PUSH; *++e = IMM; *++e = -1; *++e = XOR; ty = INT; }
    else if (token == Add) { next(); expr(Inc); ty = INT; }
    else if (token == Sub) { next(); *++e = IMM; *++e = 0; expr(Inc); *++e = SUB; ty = INT; }
    else if (token == Inc || token == Dec) {
        // 前缀++/--
        t = token; next(); expr(Inc);
        *++e = PUSH; *++e = IMM; *++e = (ty > PTR) ? 4 : 1;
        *++e = (t == Inc) ? ADD : SUB;
        *++e = (ty == CHAR) ? SC : SI;
    }
    else { printf("%d: bad expression\n", line); exit(-1); }

    // 处理二元运算符（按优先级）
    while (token >= lev) {
        t = ty;
        if (token == Assign) {
            // 赋值
            next();
            *++e = PUSH; expr(Assign);
            if (t == ty) { *++e = (ty == CHAR) ? SC : SI; }
            else { printf("%d: bad assignment\n", line); exit(-1); }
        }
        else if (token == Cond) {
            // 三元运算符 ? :
            next(); *++e = JZ; d = ++e; expr(Assign);
            if (token == ':') next(); else { printf("%d: missing colon\n", line); exit(-1); }
            *d = (int)(e + 3); *++e = JMP; d = ++e; expr(Cond); *d = (int)(e + 1);
        }
        // ... 更多二元运算符
        else if (token == Add) {
            next(); *++e = PUSH; expr(Mul);
            *++e = ADD; ty = INT;
        }
        else if (token == Sub) {
            next(); *++e = PUSH; expr(Mul);
            *++e = SUB; ty = INT;
        }
        else if (token == Mul) {
            next(); *++e = PUSH; expr(Inc);
            *++e = MUL; ty = INT;
        }
        else if (token == Div) {
            next(); *++e = PUSH; expr(Inc);
            *++e = DIV; ty = INT;
        }
        // ... 其他运算符
    }
}
```

### 3.3 语句解析

```c
// C4语句解析

void stmt() {
    int *a, *b;

    if (token == If) {
        // if语句
        next();
        if (token == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);        // 解析条件
        if (token == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }

        *++e = JZ; b = ++e;  // 条件为假时跳转
        stmt();              // then分支

        if (token == Else) {
            *b = (int)(e + 3); // 回填then结束后的地址
            *++e = JMP; b = ++e;
            next();
            stmt();          // else分支
        }
        *b = (int)(e + 1);   // 回填跳转地址
    }
    else if (token == While) {
        // while语句
        next();
        a = e + 1;           // 循环开始地址
        if (token == '(') next(); else { printf("%d: open paren expected\n", line); exit(-1); }
        expr(Assign);        // 解析条件
        if (token == ')') next(); else { printf("%d: close paren expected\n", line); exit(-1); }

        *++e = JZ; b = ++e;  // 条件为假时跳出
        stmt();              // 循环体
        *++e = JMP; *++e = (int)a; // 跳回条件
        *b = (int)(e + 1);   // 回填跳出地址
    }
    else if (token == Return) {
        // return语句
        next();
        if (token != ';') expr(Assign);
        *++e = LEV;
        if (token == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    }
    else if (token == '{') {
        // 代码块
        next();
        while (token != '}') stmt();
        next();
    }
    else if (token == ';') {
        // 空语句
        next();
    }
    else {
        // 表达式语句
        expr(Assign);
        if (token == ';') next(); else { printf("%d: semicolon expected\n", line); exit(-1); }
    }
}
```

---

## 4. 虚拟机执行

### 4.1 C4的虚拟机指令集

```c
// C4虚拟机指令
enum { LEA, IMM, JMP, JSR, BZ, BNZ, ENT, ADJ, LEV, LI, LC, SI, SC,
       PUSH, OR, XOR, AND, EQ, NE, LT, GT, LE, GE, SHL, SHR, ADD, SUB, MUL, DIV, MOD,
       OPEN, READ, CLOS, PRTF, MALC, FREE, MSET, MCMP, EXIT };

// 指令说明:
// LEA  offset  - 加载局部变量地址 (bp + offset)
// IMM  value   - 加载立即数
// JMP  addr    - 无条件跳转
// JSR  addr    - 调用子程序
// BZ   addr    - 为零则跳转
// BNZ  addr    - 非零则跳转
// ENT  size    - 进入函数，分配栈帧
// ADJ  size    - 调整栈指针（清理参数）
// LEV          - 离开函数
// LI           - 加载整数
// LC           - 加载字符
// SI           - 存储整数
// SC           - 存储字符
// PUSH         - 将ax压栈
// OR/XOR/AND   - 位运算
// EQ/NE/LT...  - 比较运算
// SHL/SHR      - 移位
// ADD/SUB/MUL..- 算术运算
// OPEN/READ... - 系统调用
```

### 4.2 虚拟机执行循环

```c
// C4虚拟机执行
int run(int *pc) {
    int cycle = 0;

    while (1) {
        int op = *pc++;
        cycle++;

        // 调试输出
        // printf("%d> %.4s\n", cycle, &ops[op * 5]);

        if (op == IMM)       ax = *pc++;                     // 加载立即数
        else if (op == LC)   ax = *(char *)ax;               // 加载字符
        else if (op == LI)   ax = *(int *)ax;                // 加载整数
        else if (op == SC)   ax = *(char *)*sp++ = ax;       // 存储字符
        else if (op == SI)   *(int *)*sp++ = ax;             // 存储整数
        else if (op == PUSH) *--sp = ax;                     // 压栈
        else if (op == JMP)  pc = (int *)*pc;                // 跳转
        else if (op == JSR)  { *--sp = (int)(pc + 1); pc = (int *)*pc; } // 调用
        else if (op == BZ)   pc = ax ? pc + 1 : (int *)*pc;  // 为零跳转
        else if (op == BNZ)  pc = ax ? (int *)*pc : pc + 1;  // 非零跳转
        else if (op == ENT)  { *--sp = (int)bp; bp = sp; sp = sp - *pc++; } // 进入函数
        else if (op == ADJ)  sp = sp + *pc++;                // 调整栈
        else if (op == LEV)  { sp = bp; bp = (int *)*sp++; pc = (int *)*sp++; } // 离开函数
        else if (op == LEA)  ax = (int)(bp + *pc++);         // 加载局部变量地址

        else if (op == OR)  ax = *sp++ | ax;
        else if (op == XOR) ax = *sp++ ^ ax;
        else if (op == AND) ax = *sp++ & ax;
        else if (op == EQ)  ax = *sp++ == ax;
        else if (op == NE)  ax = *sp++ != ax;
        else if (op == LT)  ax = *sp++ < ax;
        else if (op == GT)  ax = *sp++ > ax;
        else if (op == LE)  ax = *sp++ <= ax;
        else if (op == GE)  ax = *sp++ >= ax;
        else if (op == SHL) ax = *sp++ << ax;
        else if (op == SHR) ax = *sp++ >> ax;
        else if (op == ADD) ax = *sp++ + ax;
        else if (op == SUB) ax = *sp++ - ax;
        else if (op == MUL) ax = *sp++ * ax;
        else if (op == DIV) ax = *sp++ / ax;
        else if (op == MOD) ax = *sp++ % ax;

        // 系统调用
        else if (op == OPEN) ax = open((char *)sp[1], *sp);
        else if (op == READ) ax = read(sp[2], (char *)sp[1], *sp);
        else if (op == CLOS) ax = close(*sp);
        else if (op == PRTF) { sp = sp + 5; ax = printf((char *)sp[-4], sp[-3], sp[-2], sp[-1]); }
        else if (op == MALC) ax = (int)malloc(*sp);
        else if (op == FREE) free((void *)*sp);
        else if (op == MSET) ax = (int)memset((char *)sp[2], sp[1], *sp);
        else if (op == MCMP) ax = memcmp((char *)sp[2], (char *)sp[1], *sp);
        else if (op == EXIT) { printf("exit(%d)\n", *sp); return *sp; }
        else { printf("unknown instruction: %d\n", op); return -1; }
    }
}
```

---

## 5. 完整示例

### 5.1 解释执行示例

```c
// C源代码
int factorial(int n) {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

int main() {
    printf("factorial(5) = %d\n", factorial(5));
    return 0;
}
```

```
执行过程:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

1. 词法分析生成Token
2. 语法分析生成字节码
3. VM执行字节码

factorial(5)的递归展开:
    factorial(5)
    → 5 * factorial(4)
    → 5 * (4 * factorial(3))
    → 5 * (4 * (3 * factorial(2)))
    → 5 * (4 * (3 * (2 * factorial(1))))
    → 5 * (4 * (3 * (2 * 1)))
    → 120
```

---

## 6. 学习检查点

### 6.1 自测问题

**基础理解：**

1. 什么是递归下降解析？为什么它适合手工实现？
2. C4为什么解析时不构建AST，而是直接生成字节码？
3. 解释C4中表达式解析的优先级处理机制。

**进阶分析：**
4. 如何在C4中添加for循环支持？需要修改哪些部分？
5. 比较C4的虚拟机与上一章的栈式VM的异同。
6. 解释C4中变量的作用域和生命周期是如何管理的。

**动手实践：**
7. 为C4添加`break`和`continue`语句支持。
8. 实现一个简单的AST解释器（与C4的直接代码生成对比）。

### 6.2 答案与解析

<details>
<summary>点击查看答案</summary>

**问题1：**
递归下降解析为每个语法规则写一个函数，函数相互递归调用。它适合手工实现因为结构清晰，易于理解和调试，且不需要生成解析表。

**问题2：**
C4直接生成字节码是为了简化设计。构建AST需要额外的数据结构和遍历代码，而直接生成代码是更高效的方式。

**问题3：**
C4使用`lev`参数表示当前优先级，expr(lev)只处理优先级>=lev的运算符。递归调用时传入更高优先级，自然实现优先级 climbing。

**问题4：**
需要：1) 在词法分析中添加FOR token；2) 在stmt()中添加for循环处理；3) 生成初始化、条件检查、循环体、增量更新的代码。

**问题5：**
相同：都使用栈执行；不同：C4的VM指令更丰富，支持函数调用指令(ENT/LEV/JSR)，更贴近实际硬件。

</details>

---

## 7. 与知识库的深层连接

### 7.1 纵向连接

```
底层:
├── [栈式虚拟机](./01_Stack_Based_VM.md)
│   └── C4的VM基于此概念
│
└── [递归下降解析](../01_Core_Knowledge_System/15_Compiler_Basics/02_Syntactic_Analysis.md)
    └── 手工实现解析器的技术

当前层（解释器）:
└── 你在这里 → 直接执行源代码

上层:
└── [完整链条回顾](../24_Complete_Chain/01_From_Turing_to_C.md)
    └── 从理论到实现的完整视图
```

### 7.2 横向连接

| 相关文档 | 连接点 |
|:---------|:-------|
| [词法分析](../01_Core_Knowledge_System/15_Compiler_Basics/01_Lexical_Analysis.md) | Token生成的系统化方法 |
| [语法分析](../01_Core_Knowledge_System/15_Compiler_Basics/02_Syntactic_Analysis.md) | 解析算法的理论 |
| [编译器后端](../02_Formal_Semantics_and_Physics/11_Compiler_Internals/02_Backend_Code_Generation.md) | 代码生成技术 |

---

## 8. 延伸阅读

### 8.1 推荐资源

- **C4源代码**: <https://github.com/rswier/c4>
- **Crafting Interpreters**: 完整的解释器构建指南
- **Let's Build a Compiler**: 经典的编译器教程

### 8.2 实践项目

1. 扩展C4支持更多C语法（结构体、数组）
2. 为C4添加类型检查
3. 实现C4到RISC-V汇编的编译器后端
4. 添加垃圾回收机制

---

*文档创建: 2026-03-29*
*上次更新: 2026-03-29*
*状态: 核心框架完成*
