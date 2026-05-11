# C 语言设计模式完全指南

> **难度**: L3-L5
> **核心目标**: 在没有类、继承、多态的C语言中实现可维护、可扩展的架构
> **前提**: 结构体、函数指针、指针运算、内存管理

---

## 1. 为什么C需要设计模式

C语言没有OOP支持，但工业级C代码（Linux Kernel、SQLite、Redis、Nginx）广泛使用架构模式：

```text
┌─────────────────────────────────────────────────────────────┐
│                    C 模式 = 数据结构 + 函数约定               │
├─────────────────────────────────────────────────────────────┤
│  没有类 → 结构体 + 相关函数前缀                               │
│  没有继承 → 结构体嵌套（组合）或手动 vtable                   │
│  没有多态 → 函数指针表（vtable）或 switch 分发                │
│  没有接口 → 函数指针结构体                                    │
│  没有泛型 → void* + 类型标签 或 宏模板                        │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 对象模拟模式 (Object Simulation)

### 2.1 封装：结构体 + 访问函数

```c
// ✅ 封装：隐藏实现细节，暴露接口
// file: vector.h
#ifndef VECTOR_H
#define VECTOR_H
#include <stddef.h>

typedef struct Vector Vector;  // 不透明类型（前置声明）

// 构造函数/析构函数
Vector *vector_create(size_t elem_size);
void    vector_destroy(Vector *v);

// 访问函数
size_t  vector_size(const Vector *v);
void   *vector_get(Vector *v, size_t index);
int     vector_push(Vector *v, const void *elem);

#endif
```

```c
// file: vector.c
#include "vector.h"
#include <stdlib.h>
#include <string.h>

struct Vector {
    char  *data;       // 字节数组（泛型存储）
    size_t elem_size;  // 元素大小
    size_t size;       // 当前元素数
    size_t capacity;   // 容量
};

Vector *vector_create(size_t elem_size) {
    Vector *v = malloc(sizeof(Vector));
    if (!v) return NULL;
    v->data = NULL;
    v->elem_size = elem_size;
    v->size = 0;
    v->capacity = 0;
    return v;
}

void vector_destroy(Vector *v) {
    if (v) {
        free(v->data);
        free(v);
    }
}

size_t vector_size(const Vector *v) {
    return v ? v->size : 0;
}

void *vector_get(Vector *v, size_t index) {
    if (!v || index >= v->size) return NULL;
    return v->data + index * v->elem_size;
}

int vector_push(Vector *v, const void *elem) {
    if (!v || !elem) return -1;
    if (v->size >= v->capacity) {
        size_t new_cap = v->capacity ? v->capacity * 2 : 4;
        char *new_data = realloc(v->data, new_cap * v->elem_size);
        if (!new_data) return -1;
        v->data = new_data;
        v->capacity = new_cap;
    }
    memcpy(v->data + v->size * v->elem_size, elem, v->elem_size);
    v->size++;
    return 0;
}
```

### 2.2 继承：结构体嵌套（组合）

```c
// 基类"模拟"
typedef struct {
    int x, y;
} Shape;

void shape_move(Shape *s, int dx, int dy) {
    s->x += dx;
    s->y += dy;
}

// 派生"模拟"：结构体嵌套
typedef struct {
    Shape base;    // 必须是第一个成员，才能安全转型
    int radius;
} Circle;

// 向上转型（安全，因为 Circle 以 Shape 开头）
Shape *circle_as_shape(Circle *c) {
    return (Shape *)c;
}

// 使用
Circle c = { {10, 20}, 5 };
shape_move(circle_as_shape(&c), 5, 5);
// c.base.x == 15, c.base.y == 25
```

### 2.3 多态：函数指针表（vtable）

```c
// 虚函数表
typedef struct ShapeVTable ShapeVTable;
struct ShapeVTable {
    double (*area)(const void *self);
    void   (*draw)(const void *self);
};

// 基类"对象"
typedef struct {
    const ShapeVTable *vtable;
    int x, y;
} Shape;

// 派生：Circle
typedef struct {
    Shape base;
    int radius;
} Circle;

static double circle_area(const void *self) {
    const Circle *c = self;
    return 3.14159 * c->radius * c->radius;
}

static void circle_draw(const void *self) {
    const Circle *c = self;
    printf("Circle at (%d,%d) r=%d\n", c->base.x, c->base.y, c->radius);
}

static const ShapeVTable circle_vtable = {
    .area = circle_area,
    .draw = circle_draw,
};

// 构造函数
Circle *circle_create(int x, int y, int r) {
    Circle *c = malloc(sizeof(Circle));
    c->base.vtable = &circle_vtable;
    c->base.x = x;
    c->base.y = y;
    c->radius = r;
    return c;
}

// 多态调用
double shape_area(const Shape *s) {
    return s->vtable->area(s);  // 动态分发
}

void shape_draw(const Shape *s) {
    s->vtable->draw(s);
}

// 使用
Circle *c = circle_create(0, 0, 10);
Shape *s = (Shape *)c;  // 向上转型
printf("area = %.2f\n", shape_area(s));  // 调用 Circle::area
shape_draw(s);                           // 调用 Circle::draw
```

---

## 3. 有限状态机 (Finite State Machine)

FSM 是C语言中最常用的模式（协议解析、UI状态、解析器）。

### 3.1 函数指针表实现

```c
// TCP 连接状态机示例
typedef enum {
    STATE_CLOSED,
    STATE_LISTEN,
    STATE_SYN_SENT,
    STATE_SYN_RECEIVED,
    STATE_ESTABLISHED,
    STATE_FIN_WAIT_1,
    STATE_FIN_WAIT_2,
    STATE_CLOSE_WAIT,
    STATE_CLOSING,
    STATE_LAST_ACK,
    STATE_TIME_WAIT,
    STATE_COUNT
} TcpState;

typedef enum {
    EVT_OPEN,
    EVT_SYN,
    EVT_SYN_ACK,
    EVT_ACK,
    EVT_FIN,
    EVT_CLOSE,
    EVT_TIMEOUT,
    EVT_COUNT
} TcpEvent;

// 状态函数签名
typedef TcpState (*StateHandler)(TcpState current, TcpEvent evt, void *ctx);

// 状态表：state_table[state][event] = handler
static StateHandler state_table[STATE_COUNT][EVT_COUNT];

// 各状态处理函数
static TcpState state_closed(TcpState s, TcpEvent evt, void *ctx) {
    (void)s; (void)ctx;
    switch (evt) {
        case EVT_OPEN:  return STATE_LISTEN;
        case EVT_SYN:   return STATE_SYN_SENT;
        default:        return STATE_CLOSED;  // 忽略无效事件
    }
}

static TcpState state_established(TcpState s, TcpEvent evt, void *ctx) {
    (void)s; (void)ctx;
    switch (evt) {
        case EVT_FIN:   return STATE_CLOSE_WAIT;
        case EVT_CLOSE: return STATE_FIN_WAIT_1;
        default:        return STATE_ESTABLISHED;
    }
}

// 初始化状态表
void fsm_init(void) {
    for (int s = 0; s < STATE_COUNT; s++)
        for (int e = 0; e < EVT_COUNT; e++)
            state_table[s][e] = NULL;  // 默认：无效转换

    state_table[STATE_CLOSED][EVT_OPEN] = state_closed;
    state_table[STATE_CLOSED][EVT_SYN]  = state_closed;
    state_table[STATE_ESTABLISHED][EVT_FIN]   = state_established;
    state_table[STATE_ESTABLISHED][EVT_CLOSE] = state_established;
    // ... 填充其他状态
}

// 状态机驱动
TcpState fsm_step(TcpState current, TcpEvent evt, void *ctx) {
    StateHandler handler = state_table[current][evt];
    if (handler) {
        return handler(current, evt, ctx);
    }
    // 无效转换：记录日志或报错
    return current;  // 保持在当前状态
}
```

### 3.2 状态模式（State Pattern）

```c
// 更灵活的状态模式：每个状态是一个"对象"
typedef struct State State;
struct State {
    const char *name;
    void (*enter)(void *ctx);
    void (*exit)(void *ctx);
    State *(*handle)(void *ctx, int event);
};

// 具体状态
static State *state_idle_handle(void *ctx, int event) {
    (void)ctx;
    if (event == EVT_START) {
        return &state_running;  // 转换到运行态
    }
    return &state_idle;  // 保持
}

static State state_idle = {
    .name = "IDLE",
    .enter = NULL,
    .exit = NULL,
    .handle = state_idle_handle,
};

// 状态机上下文
typedef struct {
    State *current;
} StateMachine;

void sm_step(StateMachine *sm, int event) {
    State *next = sm->current->handle(sm, event);
    if (next != sm->current) {
        if (sm->current->exit) sm->current->exit(sm);
        sm->current = next;
        if (sm->current->enter) sm->current->enter(sm);
    }
}
```

---

## 4. 回调与观察者模式 (Observer)

### 4.1 简单回调

```c
// 异步操作完成通知
typedef void (*CompletionCallback)(void *user_data, int result);

void async_read_file(const char *path, CompletionCallback cb, void *user_data);

// 使用
void on_file_loaded(void *user_data, int result) {
    AppState *app = user_data;
    if (result == 0) {
        app->status = FILE_LOADED;
    } else {
        app->status = FILE_ERROR;
    }
}

async_read_file("config.ini", on_file_loaded, &app_state);
```

### 4.2 多订阅者观察者

```c
#define MAX_OBSERVERS 16

typedef void (*EventHandler)(void *observer, int event_type, const void *event_data);

typedef struct {
    void *observer;
    EventHandler handler;
} ObserverEntry;

typedef struct {
    ObserverEntry observers[MAX_OBSERVERS];
    int count;
} Subject;

void subject_init(Subject *s) {
    s->count = 0;
}

int subject_subscribe(Subject *s, void *observer, EventHandler handler) {
    if (s->count >= MAX_OBSERVERS) return -1;
    s->observers[s->count].observer = observer;
    s->observers[s->count].handler = handler;
    s->count++;
    return 0;
}

void subject_notify(Subject *s, int event_type, const void *event_data) {
    for (int i = 0; i < s->count; i++) {
        s->observers[i].handler(s->observers[i].observer, event_type, event_data);
    }
}

// 使用
void on_temperature_change(void *observer, int event, const void *data) {
    Thermostat *t = observer;
    float temp = *(const float *)data;
    if (temp > t->threshold) {
        thermostat_cool(t);
    }
}

Subject temp_sensor;
subject_init(&temp_sensor);
subject_subscribe(&temp_sensor, &living_room_thermostat, on_temperature_change);
subject_subscribe(&temp_sensor, &bedroom_thermostat, on_temperature_change);
```

---

## 5. 策略模式 (Strategy)

```c
// 排序策略接口
typedef int (*CompareFunc)(const void *a, const void *b);
typedef void (*SortStrategy)(void *arr, size_t n, size_t elem_size, CompareFunc cmp);

// 具体策略
void bubble_sort(void *arr, size_t n, size_t elem_size, CompareFunc cmp);
void quick_sort(void *arr, size_t n, size_t elem_size, CompareFunc cmp);
void heap_sort(void *arr, size_t n, size_t elem_size, CompareFunc cmp);

// 上下文
typedef struct {
    SortStrategy strategy;
    CompareFunc compare;
} Sorter;

void sorter_init(Sorter *s, SortStrategy strat, CompareFunc cmp) {
    s->strategy = strat;
    s->compare = cmp;
}

void sorter_sort(Sorter *s, void *arr, size_t n, size_t elem_size) {
    s->strategy(arr, n, elem_size, s->compare);
}

// 使用
Sorter s;
sorter_init(&s, quick_sort, int_compare);
sorter_sort(&s, numbers, count, sizeof(int));
```

---

## 6. 迭代器模式 (Iterator)

```c
// 泛型迭代器接口
typedef struct Iterator Iterator;
struct Iterator {
    void *ctx;
    void *(*next)(Iterator *it);
    int   (*has_next)(const Iterator *it);
    void  (*reset)(Iterator *it);
    void  (*destroy)(Iterator *it);
};

// 链表迭代器实现
typedef struct Node { void *data; struct Node *next; } Node;

typedef struct {
    Node *head;
    Node *current;
} ListIterCtx;

static void *list_next(Iterator *it) {
    ListIterCtx *ctx = it->ctx;
    if (!ctx->current) return NULL;
    void *data = ctx->current->data;
    ctx->current = ctx->current->next;
    return data;
}

static int list_has_next(const Iterator *it) {
    ListIterCtx *ctx = it->ctx;
    return ctx->current != NULL;
}

static void list_reset(Iterator *it) {
    ListIterCtx *ctx = it->ctx;
    ctx->current = ctx->head;
}

Iterator *list_iterator_create(Node *head) {
    Iterator *it = malloc(sizeof(Iterator));
    ListIterCtx *ctx = malloc(sizeof(ListIterCtx));
    ctx->head = head;
    ctx->current = head;
    it->ctx = ctx;
    it->next = list_next;
    it->has_next = list_has_next;
    it->reset = list_reset;
    it->destroy = /* ... */;
    return it;
}

// 使用
Iterator *it = list_iterator_create(list_head);
while (it->has_next(it)) {
    int *val = it->next(it);
    printf("%d\n", *val);
}
it->destroy(it);
```

---

## 7. 工厂模式 (Factory)

```c
// 对象类型标签
typedef enum { OBJ_CIRCLE, OBJ_RECTANGLE, OBJ_TRIANGLE } ShapeType;

// 工厂函数
typedef struct { void *data; void (*draw)(void*); double (*area)(void*); } GenericShape;

GenericShape *shape_create(ShapeType type, ...) {
    GenericShape *s = malloc(sizeof(GenericShape));
    va_list args;
    va_start(args, type);

    switch (type) {
        case OBJ_CIRCLE: {
            int r = va_arg(args, int);
            Circle *c = circle_create(0, 0, r);
            s->data = c;
            s->draw = (void (*)(void*))circle_draw;
            s->area = (double (*)(void*))circle_area;
            break;
        }
        case OBJ_RECTANGLE: {
            int w = va_arg(args, int);
            int h = va_arg(args, int);
            Rectangle *r = rect_create(w, h);
            s->data = r;
            s->draw = (void (*)(void*))rect_draw;
            s->area = (double (*)(void*))rect_area;
            break;
        }
        // ...
    }
    va_end(args);
    return s;
}

// 使用
GenericShape *s1 = shape_create(OBJ_CIRCLE, 10);
GenericShape *s2 = shape_create(OBJ_RECTANGLE, 20, 30);
```

---

## 8. 检查清单

- [ ] 使用不透明指针（opaque pointer）隐藏实现细节
- [ ] 结构体第一个成员作为"基类"实现继承
- [ ] 函数指针表作为 vtable 实现多态
- [ ] 状态机用函数指针表或状态对象实现
- [ ] 回调总是携带 user_data 上下文参数
- [ ] 迭代器接口统一不同数据结构的遍历

---

> **最后更新**: 2026-05-11
> **参考**: Expert C Programming Ch.4, Linux Kernel Design Patterns, GObject
