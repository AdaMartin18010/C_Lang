---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
﻿# C语言行为型设计模式

## 目录

- [C语言行为型设计模式](#c语言行为型设计模式)
  - [目录](#目录)
  - [观察者模式](#观察者模式)
  - [策略模式](#策略模式)
  - [状态机模式](#状态机模式)
  - [命令模式](#命令模式)
  - [迭代器模式](#迭代器模式)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 观察者模式

```c
#include <stdlib.h>
#include <string.h>

typedef struct observer observer_t;
typedef struct subject subject_t;

struct observer {
    void (*update)(observer_t*, const char* event, void* data);
    subject_t* subject;
    observer_t* next;
};

struct subject {
    observer_t* observers;
    mtx_t lock;
};

void subject_attach(subject_t* s, observer_t* o) {
    mtx_lock(&s->lock);
    o->subject = s;
    o->next = s->observers;
    s->observers = o;
    mtx_unlock(&s->lock);
}

void subject_notify(subject_t* s, const char* event, void* data) {
    mtx_lock(&s->lock);
    observer_t* o = s->observers;
    while (o) {
        o->update(o, event, data);
        o = o->next;
    }
    mtx_unlock(&s->lock);
}
```

---

## 策略模式

```c
typedef struct {
    int (*execute)(void* strategy, int a, int b);
} strategy_ops_t;

typedef struct {
    strategy_ops_t* ops;
    void* data;
} context_t;

int add_execute(void* s, int a, int b) {
    (void)s;
    return a + b;
}

int context_execute(context_t* ctx, int a, int b) {
    return ctx->ops->execute(ctx->data, a, b);
}
```

---

## 状态机模式

```c
typedef void (*state_fn_t)(void* sm, int event);

typedef struct {
    state_fn_t current_state;
} state_machine_t;

void state_idle(state_machine_t* sm, int event) {
    if (event == 1) {
        printf("Starting...\n");
        sm->current_state = (state_fn_t)state_running;
    }
}

void state_running(state_machine_t* sm, int event) {
    if (event == 0) {
        printf("Stopping...\n");
        sm->current_state = (state_fn_t)state_idle;
    }
}

void sm_handle(state_machine_t* sm, int event) {
    sm->current_state(sm, event);
}
```

---

## 命令模式

```c
typedef struct {
    void (*execute)(void* cmd);
    void (*undo)(void* cmd);
    void (*destroy)(void* cmd);
} command_ops_t;

typedef struct {
    command_ops_t* ops;
} command_t;

/* 具体命令 */
typedef struct {
    command_ops_t ops;
    int* value;
    int new_val;
    int old_val;
} set_value_cmd_t;

void set_value_execute(void* cmd) {
    set_value_cmd_t* c = cmd;
    c->old_val = *c->value;
    *c->value = c->new_val;
}

void set_value_undo(void* cmd) {
    set_value_cmd_t* c = cmd;
    *c->value = c->old_val;
}

/* 命令队列 */
typedef struct {
    command_t** commands;
    int count;
    int capacity;
} command_queue_t;

void cmd_queue_push(command_queue_t* q, command_t* cmd) {
    if (q->count < q->capacity) {
        q->commands[q->count++] = cmd;
    }
}

void cmd_queue_execute_all(command_queue_t* q) {
    for (int i = 0; i < q->count; i++) {
        q->commands[i]->ops->execute(q->commands[i]);
    }
}
```

---

## 迭代器模式

```c
typedef struct {
    void* (*next)(void* iterator);
    bool (*has_next)(void* iterator);
    void (*destroy)(void* iterator);
} iterator_ops_t;

typedef struct {
    iterator_ops_t* ops;
    void* container;
    int current;
} iterator_t;

/* 数组迭代器 */
typedef struct {
    iterator_ops_t ops;
    void** array;
    int size;
    int current;
} array_iterator_t;

void* array_next(void* iter) {
    array_iterator_t* it = iter;
    if (it->current < it->size) {
        return it->array[it->current++];
    }
    return NULL;
}

bool array_has_next(void* iter) {
    array_iterator_t* it = iter;
    return it->current < it->size;
}

iterator_t* array_iterator_create(void** array, int size) {
    array_iterator_t* it = malloc(sizeof(array_iterator_t));
    static iterator_ops_t ops = {
        array_next, array_has_next, free
    };
    it->ops = &ops;
    it->array = array;
    it->size = size;
    it->current = 0;
    return (iterator_t*)it;
}
```

---

## 总结

行为型模式在C语言中使用函数指针实现多态行为，通过结构体包含实现组合。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
