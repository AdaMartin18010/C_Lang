# 博弈语义


---

## 📑 目录

- [博弈语义](#博弈语义)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [博弈论语义学基础](#博弈论语义学基础)
    - [核心概念](#核心概念)
    - [简单类型博弈](#简单类型博弈)
    - [C 语言视角的博弈解释](#c-语言视角的博弈解释)
  - [过程交互模型](#过程交互模型)
    - [标记博弈 (Arena Games)](#标记博弈-arena-games)
    - [名称与指针的博弈](#名称与指针的博弈)
    - [并发过程的博弈](#并发过程的博弈)
  - [对话游戏](#对话游戏)
    - [合法博弈序列](#合法博弈序列)
    - [策略表示](#策略表示)
  - [线性逻辑与博弈](#线性逻辑与博弈)
    - [线性类型与资源](#线性类型与资源)
    - [C 语言中的线性类型模拟](#c-语言中的线性类型模拟)
    - [会话类型 (Session Types)](#会话类型-session-types)
  - [博弈语义在程序验证中的应用](#博弈语义在程序验证中的应用)
    - [规范即博弈](#规范即博弈)
    - [精炼检验](#精炼检验)
  - [延伸阅读](#延伸阅读)


---

## 概述

博弈语义 (Game Semantics) 是程序语义学的重要分支，通过博弈论模型来描述程序的计算过程。它将程序执行建模为两个参与者（Player 和 Opponent）之间的对话游戏，为理解程序行为、验证程序正确性提供了强大的数学基础。

---

## 博弈论语义学基础

### 核心概念

```
博弈语义的基本组成：

┌─────────────────────────────────────────────────────────────┐
│                      博弈模型                                │
├─────────────────────────────────────────────────────────────┤
│  参与者 (Players)                                           │
│  ├── Opponent (O): 环境/上下文，发起询问                     │
│  └── Player   (P): 程序/进程，提供响应                       │
│                                                             │
│  动作 (Moves)                                               │
│  ├── Question (Q): 询问，请求信息                            │
│  └── Answer   (A): 回答，提供结果                            │
│                                                             │
│  位置 (Positions)                                           │
│  └── 博弈的序列状态，形式为交替的动作序列                     │
│                                                             │
│  策略 (Strategies)                                          │
│  └── 给定位置下选择动作的映射函数                            │
└─────────────────────────────────────────────────────────────┘
```

### 简单类型博弈

```haskell
-- 类型构造的博弈解释（伪代码表示概念）

-- 基本类型：整数博弈
Game Int =
  O 询问 "Value?"
  P 回答  n : Integer

-- 函数类型：A → B 的博弈
Game (A -> B) =
  O 询问 "Apply to what?"
  P 在 Game A 中扮演 O 进行子博弈
  O 在 Game A 中回答
  P 在 Game B 中继续博弈

-- 乘积类型：A × B
Game (A, B) =
  O 选择 Left 或 Right
  若 Left:  继续 Game A
  若 Right: 继续 Game B

-- 和类型：A + B
Game (Either A B) =
  P 选择 Left 或 Right
  若 Left:  继续 Game A
  若 Right: 继续 Game B
```

### C 语言视角的博弈解释

```c
/*
 * C 程序中的博弈语义映射
 *
 * 函数调用 = 博弈启动
 * 参数传递 = Opponent 的询问
 * 返回值   = Player 的回答
 * 副作用   = 与环境的交互
 */

// 基本值博弈
int get_value(void)
{
    // O: "给我一个 int"
    return 42;  // P: "这里是 42"
}

// 函数类型博弈
int add(int x, int y)
{
    // O: "add 作用于什么?"
    // P: "给我第一个参数"
    // O: "x = 3"
    // P: "给我第二个参数"
    // O: "y = 5"
    return x + y;  // P: "结果是 8"
}

// 高阶函数博弈
int apply(int (*f)(int), int x)
{
    // O: "f 是什么函数?"
    // O: "x 是什么值?"
    return f(x);  // 启动子博弈
}

// 指针作为博弈引用
typedef struct {
    int (*query)(void);   // O 询问
    void (*command)(int); // P 执行
} Interface;

// 博弈中的命名位置
void interaction_example(Interface *iface)
{
    // O 询问当前状态
    int state = iface->query();  // P 回答

    // P 执行命令
    iface->command(state + 1);   // O 接受执行
}
```

---

## 过程交互模型

### 标记博弈 (Arena Games)

```
博弈舞台 (Arena) 结构：

┌──────────────────────────────────────────────┐
│                  根问题                       │
│              "计算结果是什么?"                │
│                      │                        │
│                      ▼                        │
│           ┌─────────┴─────────┐              │
│           ▼                   ▼              │
│      子问题 A             子问题 B            │
│    "参数 1 的值?"      "参数 2 的值?"         │
│           │                   │              │
│           ▼                   ▼              │
│        答案 A              答案 B             │
│           │                   │              │
│           └─────────┬─────────┘              │
│                     ▼                        │
│                  最终答案                     │
└──────────────────────────────────────────────┘
```

### 名称与指针的博弈

```c
/*
 * C 语言中的名称绑定与博弈位置
 */

#include <stdio.h>
#include <stdlib.h>

// 名称作为博弈的命名策略
typedef struct {
    int *value_ref;  // 对博弈位置的引用
} Name;

// 创建一个命名位置
Name create_name(int initial)
{
    Name n;
    n.value_ref = malloc(sizeof(int));
    *n.value_ref = initial;
    return n;
}

// 博弈中的询问动作 (Opponent)
int query(Name n)
{
    // O: "这个位置当前的值是什么?"
    return *n.value_ref;  // P 回答当前值
}

// 博弈中的更新动作 (Player)
void update(Name n, int new_value)
{
    // P: "我将这个位置更新为..."
    *n.value_ref = new_value;
    // O: 接受更新（继续博弈）
}

// 作用域作为博弈的可见性规则
void scoped_interaction(void)
{
    // 进入作用域 = 创建新的博弈子舞台
    Name x = create_name(0);

    // 在作用域内的交互
    int v = query(x);      // O 询问
    update(x, v + 1);      // P 更新

    // 离开作用域 = 博弈结束，资源释放
    free(x.value_ref);
}
```

### 并发过程的博弈

```c
/*
 * 并发程序的博弈语义
 *
 * 并行组合：多个博弈同时进行
 * 同步：博弈之间的握手点
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

// 通道作为博弈之间的连接
typedef struct {
    sem_t ready;    // P 发送信号
    sem_t ack;      // O 确认接收
    int value;      // 传递的数据
} Channel;

// 发送动作 (Player 移动)
void send(Channel *ch, int value)
{
    // P: "我有值要发送"
    ch->value = value;
    sem_post(&ch->ready);   // 通知就绪
    sem_wait(&ch->ack);     // 等待确认
}

// 接收动作 (Opponent 移动)
int receive(Channel *ch)
{
    // O: "等待接收值"
    sem_wait(&ch->ready);   // 等待发送者
    int v = ch->value;
    sem_post(&ch->ack);     // 确认接收
    return v;
}

// 生产者博弈
void *producer_game(void *arg)
{
    Channel *ch = arg;
    for (int i = 0; i < 5; i++) {
        printf("P: Sending %d\n", i);
        send(ch, i);  // Player 移动
    }
    return NULL;
}

// 消费者博弈
void *consumer_game(void *arg)
{
    Channel *ch = arg;
    for (int i = 0; i < 5; i++) {
        int v = receive(ch);  // Opponent 响应
        printf("O: Received %d\n", v);
    }
    return NULL;
}

// 并行组合运行
void parallel_composition(void)
{
    Channel ch;
    sem_init(&ch.ready, 0, 0);
    sem_init(&ch.ack, 0, 0);

    pthread_t prod, cons;
    pthread_create(&prod, NULL, producer_game, &ch);
    pthread_create(&cons, NULL, consumer_game, &ch);

    pthread_join(prod, NULL);
    pthread_join(cons, NULL);

    sem_destroy(&ch.ready);
    sem_destroy(&ch.ack);
}
```

---

## 对话游戏

### 合法博弈序列

```
合法序列的定义：

规则 1: 交替性
    博弈必须是 O 和 P 交替移动
    非法: O-O-P-P
    合法: O-P-O-P

规则 2: 可见性
    只能引用当前可见的开启问题

规则 3: 良好括号性
    问题和回答必须正确匹配

示例：函数复合的博弈

  O: q0        (询问整体结果)
  P: q1        (询问第一个参数)
  O: a1        (提供第一个参数)
  P: q2        (询问第二个参数)
  O: a2        (提供第二个参数)
  P: a0        (给出最终结果)

序列: q0 → q1 → a1 → q2 → a2 → a0 ✓
```

### 策略表示

```c
/*
 * 策略作为函数：位置 -> 移动
 */

#include <stdbool.h>
#include <stdlib.h>

// 博弈位置（简化表示）
typedef struct Position {
    int move_history[100];
    int length;
} Position;

// 移动类型
typedef enum { MOVE_QUESTION, MOVE_ANSWER } MoveType;

typedef struct Move {
    MoveType type;
    int label;
} Move;

// 策略：给定位置，选择移动
typedef Move (*Strategy)(const Position *pos);

// 常数策略：总是返回固定值
Move const_strategy(const Position *pos)
{
    (void)pos;  // 忽略历史
    return (Move){ .type = MOVE_ANSWER, .label = 42 };
}

// 复制策略：返回接收到的值
Move copy_strategy(const Position *pos)
{
    // 查找最后一个答案
    for (int i = pos->length - 1; i >= 0; i--) {
        // 假设偶数索引是答案
        if (i % 2 == 1) {
            return (Move){
                .type = MOVE_ANSWER,
                .label = pos->move_history[i]
            };
        }
    }
    return (Move){ .type = MOVE_ANSWER, .label = 0 };
}

// 顺序组合策略
Move compose_strategy(const Position *pos, Strategy s1, Strategy s2)
{
    // 根据位置历史决定调用哪个策略
    if (pos->length < 2) {
        return s1(pos);
    } else {
        return s2(pos);
    }
}

// 博弈执行引擎
void run_game(Strategy player, Strategy opponent, int max_moves)
{
    Position pos = { .length = 0 };
    bool player_turn = false;  // O 先开始

    while (pos.length < max_moves) {
        Move m;
        if (player_turn) {
            m = player(&pos);
            printf("P: %s %d\n",
                   m.type == MOVE_QUESTION ? "?" : "!", m.label);
        } else {
            m = opponent(&pos);
            printf("O: %s %d\n",
                   m.type == MOVE_QUESTION ? "?" : "!", m.label);
        }

        pos.move_history[pos.length++] = m.label;
        player_turn = !player_turn;

        // 终止条件：最终答案
        if (m.type == MOVE_ANSWER && pos.length > 2) break;
    }
}
```

---

## 线性逻辑与博弈

### 线性类型与资源

```
线性逻辑与博弈的对应：

线性逻辑          博弈语义
─────────────────────────────────────────
A ⊗ B            A 和 B 的并行博弈
A ⅋ B            A 和 B 的选择博弈
A ⊸ B            从 A 到 B 的策略
!A               可复制的 A 博弈
?A               可选择的 A 博弈
1                空博弈（终止）
⊥                立即失败

资源管理：
线性类型确保资源恰好使用一次
```

### C 语言中的线性类型模拟

```c
/*
 * 使用所有权类型模拟线性逻辑
 */

#include <stdio.h>
#include <stdlib.h>

// 线性资源：只能移动，不能复制
typedef struct {
    int data;
    bool consumed;
} Linear;

// 创建线性资源（所有权初始化）
Linear linear_new(int value)
{
    return (Linear){ .data = value, .consumed = false };
}

// 消费线性资源（所有权转移）
int linear_consume(Linear *res)
{
    if (res->consumed) {
        fprintf(stderr, "Error: Double consumption\n");
        abort();
    }
    res->consumed = true;
    return res->data;
}

// 线性函数：A ⊸ B
typedef Linear (*LinearFn)(Linear);

// 示例：消耗 int 返回加倍后的 int
Linear double_linear(Linear x)
{
    int v = linear_consume(&x);
    return linear_new(v * 2);
}

// 线性复合：(A ⊸ B) ⊸ (B ⊸ C) ⊸ (A ⊸ C)
Linear linear_compose(Linear x, LinearFn f, LinearFn g)
{
    Linear y = f(x);
    Linear z = g(y);
    return z;
}

// 使用示例
void linear_example(void)
{
    Linear x = linear_new(21);

    // 可以转移所有权
    Linear y = double_linear(x);
    // x 现在已消费，不能再使用

    int result = linear_consume(&y);
    printf("Result: %d\n", result);
    // y 现在已消费
}

// 文件句柄作为线性资源
typedef struct {
    FILE *handle;
    bool closed;
} LinearFile;

LinearFile linear_fopen(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) perror("fopen");
    return (LinearFile){ .handle = f, .closed = false };
}

void linear_fclose(LinearFile *lf)
{
    if (lf->closed) {
        fprintf(stderr, "Error: File already closed\n");
        return;
    }
    fclose(lf->handle);
    lf->closed = true;
}
```

### 会话类型 (Session Types)

```c
/*
 * 会话类型：描述通信协议的类型
 */

#include <stdbool.h>

// 会话类型构造器（类型级编程概念）

// ?T.S  : 接收 T，然后继续 S
// !T.S  : 发送 T，然后继续 S
// ⊕{l:S} : 选择分支
// &{l:S} : 提供选择
// end   : 终止

// 文件服务器协议示例：
// Server = &{ READ: ?String.!Content.end
//            , WRITE: ?String.!Bool.end
//            , CLOSE: end }

// 客户端必须选择其中一个分支

// C 语言中的类型编码（简化）

typedef enum { S_RECV, S_SEND, S_CHOICE, S_OFFER, S_END } SessionOp;

typedef struct SessionType {
    SessionOp op;
    union {
        struct { int type_tag; struct SessionType *cont; } action;
        struct { int num_branches; struct SessionType **branches; } choice;
    };
} SessionType;

// 类型安全的通道（概念性）
typedef struct {
    int fd;
    const SessionType *current_type;
} SessionChannel;

// 发送操作（检查类型）
bool session_send(SessionChannel *ch, const void *data, size_t len)
{
    if (ch->current_type->op != S_SEND) {
        fprintf(stderr, "Type error: expected send capability\n");
        return false;
    }
    // 执行发送...
    ch->current_type = ch->current_type->action.cont;
    return true;
}

// 接收操作（检查类型）
bool session_recv(SessionChannel *ch, void *buffer, size_t len)
{
    if (ch->current_type->op != S_RECV) {
        fprintf(stderr, "Type error: expected recv capability\n");
        return false;
    }
    // 执行接收...
    ch->current_type = ch->current_type->action.cont;
    return true;
}
```

---

## 博弈语义在程序验证中的应用

### 规范即博弈

```c
/*
 * 将程序规范表示为博弈约束
 */

#include <assert.h>
#include <stdbool.h>

// 前置条件：Opponent 的责任
// 后置条件：Player 的承诺

typedef struct {
    bool (*pre)(int args[], int argc);
    bool (*post)(int result, int args[], int argc);
} Contract;

// 契约检查包装
int with_contract(int (*f)(int[], int), int args[], int argc,
                   Contract c)
{
    // 检查前置条件（Opponent 必须满足）
    assert(c.pre(args, argc) && "Precondition violated");

    // 执行函数
    int result = f(args, argc);

    // 检查后置条件（Player 必须满足）
    assert(c.post(result, args, argc) && "Postcondition violated");

    return result;
}

// 示例：除法函数的契约
bool div_pre(int args[], int argc)
{
    (void)argc;
    return args[1] != 0;  // 除数不能为零
}

bool div_post(int result, int args[], int argc)
{
    (void)argc;
    return result * args[1] + (args[0] % args[1]) == args[0];
}

int safe_divide_impl(int args[], int argc)
{
    (void)argc;
    return args[0] / args[1];
}

int safe_divide(int a, int b)
{
    int args[] = {a, b};
    Contract c = { .pre = div_pre, .post = div_post };
    return with_contract(safe_divide_impl, args, 2, c);
}
```

### 精炼检验

```c
/*
 * 博弈语义中的精炼关系
 *
 * 策略 σ 精炼策略 τ 当：
 * - σ 的所有合法移动 τ 也能做
 * - σ 的响应比 τ 更具体或相等
 */

#include <stdbool.h>
#include <string.h>

// 抽象行为类型
typedef struct {
    int move_set[10];
    int num_moves;
} Behavior;

// 精炼检查
bool refines(const Behavior *spec, const Behavior *impl)
{
    // 实现的每个移动都必须在规范允许范围内
    for (int i = 0; i < impl->num_moves; i++) {
        bool found = false;
        for (int j = 0; j < spec->num_moves; j++) {
            if (impl->move_set[i] == spec->move_set[j]) {
                found = true;
                break;
            }
        }
        if (!found) return false;
    }
    return true;
}

// 前序关系：impl ≤ spec
bool implements(const Behavior *impl, const Behavior *spec)
{
    return refines(spec, impl);
}
```

---

## 延伸阅读

- [线性逻辑](../03_Linear_Logic/README.md)
- [余代数方法](../02_Coalgebraic_Methods/README.md)
- [进程代数](https://en.wikipedia.org/wiki/Process_calculus)
- [Game Semantics](https://en.wikipedia.org/wiki/Game_semantics)
- Abramsky, S., & McCusker, G. (1999). Game Semantics. In *Computational Logic*.
- Hyland, M., & Ong, L. (2000). On full abstraction for PCF.
