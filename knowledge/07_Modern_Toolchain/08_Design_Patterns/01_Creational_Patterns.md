# C语言创建型设计模式

## 目录

- [C语言创建型设计模式](#c语言创建型设计模式)
  - [目录](#目录)
  - [单例模式](#单例模式)
    - [线程安全单例](#线程安全单例)
    - [模块模式](#模块模式)
  - [工厂模式](#工厂模式)
    - [简单工厂](#简单工厂)
    - [抽象工厂](#抽象工厂)
  - [建造者模式](#建造者模式)
    - [链式建造者](#链式建造者)
    - [分步建造者](#分步建造者)
  - [对象池模式](#对象池模式)
    - [固定大小对象池](#固定大小对象池)
    - [带初始化的对象池](#带初始化的对象池)
  - [原型模式](#原型模式)
  - [总结](#总结)
  - [参考](#参考)

---

## 单例模式

### 线程安全单例

```c
#include <stdatomic.h>
#include <stdlib.h>
#include <threads.h>

/* 懒汉式单例 - 双重检查锁定 */
typedef struct {
    int data;
    /* 其他成员 */
} singleton_t;

static _Atomic(singleton_t*) instance = NULL;
static mtx_t init_mutex;
static atomic_int initialized = 0;

singleton_t* get_singleton(void) {
    /* 第一次检查 (无锁) */
    singleton_t* inst = atomic_load_explicit(&instance, memory_order_acquire);
    if (inst != NULL) {
        return inst;
    }

    mtx_lock(&init_mutex);

    /* 第二次检查 (加锁) */
    inst = atomic_load_explicit(&instance, memory_order_relaxed);
    if (inst == NULL) {
        inst = malloc(sizeof(singleton_t));
        inst->data = 0;
        /* 初始化 */

        atomic_store_explicit(&instance, inst, memory_order_release);
    }

    mtx_unlock(&init_mutex);
    return inst;
}

/* C11 call_once 实现 */
static once_flag singleton_once = ONCE_FLAG_INIT;
static singleton_t* singleton_instance = NULL;

void init_singleton(void) {
    singleton_instance = malloc(sizeof(singleton_t));
    /* 初始化 */
}

singleton_t* get_singleton_once(void) {
    call_once(&singleton_once, init_singleton);
    return singleton_instance;
}
```

### 模块模式

```c
/* C语言推荐: 模块级别的单例 */
/* module.c */

static struct {
    int config_value;
    void* internal_state;
} module_state = {
    .config_value = 42,
    .internal_state = NULL
};

/* 不需要显式单例，静态变量本身就是 */
int module_get_config(void) {
    return module_state.config_value;
}

void module_set_config(int value) {
    module_state.config_value = value;
}
```

---

## 工厂模式

### 简单工厂

```c
/* 对象类型 */
typedef enum {
    TYPE_CIRCLE,
    TYPE_RECTANGLE,
    TYPE_TRIANGLE
} shape_type_t;

typedef struct shape shape_t;

struct shape {
    shape_type_t type;
    void (*draw)(shape_t*);
    void (*destroy)(shape_t*);
    float area;
};

/* 具体类型 */
typedef struct {
    shape_t base;
    float radius;
} circle_t;

void circle_draw(shape_t* s) {
    circle_t* c = (circle_t*)s;
    printf("Circle: r=%.2f\n", c->radius);
}

float circle_area(shape_t* s) {
    circle_t* c = (circle_t*)s;
    return 3.14159f * c->radius * c->radius;
}

/* 工厂函数 */
shape_t* shape_create(shape_type_t type, ...) {
    va_list args;
    va_start(args, type);

    shape_t* shape = NULL;

    switch (type) {
        case TYPE_CIRCLE: {
            circle_t* c = malloc(sizeof(circle_t));
            c->base.type = TYPE_CIRCLE;
            c->base.draw = circle_draw;
            c->radius = va_arg(args, double);
            c->base.area = circle_area((shape_t*)c);
            shape = (shape_t*)c;
            break;
        }
        /* 其他类型... */
    }

    va_end(args);
    return shape;
}

/* 使用 */
void example_factory(void) {
    shape_t* circle = shape_create(TYPE_CIRCLE, 5.0);
    circle->draw(circle);
    free(circle);
}
```

### 抽象工厂

```c
/* UI组件工厂 */

typedef struct button button_t;
typedef struct checkbox checkbox_t;

typedef struct {
    button_t* (*create_button)(void);
    checkbox_t* (*create_checkbox)(void);
} ui_factory_t;

/* Windows实现 */
typedef struct {
    void (*render)(void);
} win_button_t;

button_t* win_create_button(void) {
    win_button_t* btn = malloc(sizeof(win_button_t));
    btn->render = win_button_render;
    return (button_t*)btn;
}

ui_factory_t* create_windows_factory(void) {
    static ui_factory_t factory = {
        .create_button = win_create_button,
        .create_checkbox = win_create_checkbox
    };
    return &factory;
}

/* Linux实现 */
ui_factory_t* create_linux_factory(void) {
    static ui_factory_t factory = {
        .create_button = linux_create_button,
        .create_checkbox = linux_create_checkbox
    };
    return &factory;
}
```

---

## 建造者模式

### 链式建造者

```c
/* HTTP请求建造者 */

typedef struct {
    char method[16];
    char url[256];
    char headers[1024];
    char body[4096];
} http_request_t;

typedef struct {
    http_request_t* req;
} request_builder_t;

request_builder_t* builder_new(void) {
    request_builder_t* b = malloc(sizeof(request_builder_t));
    b->req = calloc(1, sizeof(http_request_t));
    strcpy(b->req->method, "GET");
    return b;
}

request_builder_t* builder_method(request_builder_t* b, const char* method) {
    strncpy(b->req->method, method, sizeof(b->req->method) - 1);
    return b;  /* 返回自身支持链式调用 */
}

request_builder_t* builder_url(request_builder_t* b, const char* url) {
    strncpy(b->req->url, url, sizeof(b->req->url) - 1);
    return b;
}

request_builder_t* builder_header(request_builder_t* b,
                                   const char* key,
                                   const char* value) {
    snprintf(b->req->headers + strlen(b->req->headers),
             sizeof(b->req->headers) - strlen(b->req->headers),
             "%s: %s\r\n", key, value);
    return b;
}

request_builder_t* builder_body(request_builder_t* b, const char* body) {
    strncpy(b->req->body, body, sizeof(b->req->body) - 1);
    return b;
}

http_request_t* builder_build(request_builder_t* b) {
    http_request_t* req = b->req;
    free(b);
    return req;
}

/* 使用 */
void example_builder(void) {
    http_request_t* req = builder_new()
        ->method("POST")
        ->url("https://api.example.com/users")
        ->header("Content-Type", "application/json")
        ->header("Authorization", "Bearer token")
        ->body("{\"name\":\"John\"}")
        ->build();
}
```

### 分步建造者

```c
/* 复杂对象建造 */

typedef struct {
    char name[64];
    int age;
    char email[128];
    char phone[32];
    char address[256];
} person_t;

typedef struct {
    person_t* person;
} person_builder_t;

typedef struct {
    person_builder_t* (*with_name)(person_builder_t*, const char*);
    person_builder_t* (*with_age)(person_builder_t*, int);
    person_t* (*build)(person_builder_t*);
} person_builder_ops_t;

person_builder_t* person_builder_new(void) {
    person_builder_t* b = malloc(sizeof(person_builder_t));
    b->person = calloc(1, sizeof(person_t));
    return b;
}

person_t* person_builder_build(person_builder_t* b) {
    person_t* p = b->person;
    free(b);
    return p;
}

person_builder_t* person_builder_name(person_builder_t* b, const char* name) {
    strncpy(b->person->name, name, sizeof(b->person->name) - 1);
    return b;
}

/* 可选步骤 */
person_builder_t* person_builder_optional_email(person_builder_t* b,
                                                  const char* email) {
    if (email) {
        strncpy(b->person->email, email, sizeof(b->person->email) - 1);
    }
    return b;
}
```

---

## 对象池模式

### 固定大小对象池

```c
#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    void* buffer;
    atomic_int next_free;
    int capacity;
    int item_size;
    atomic_int* free_list;  /* 空闲索引列表 */
} object_pool_t;

object_pool_t* pool_create(int capacity, int item_size) {
    object_pool_t* pool = malloc(sizeof(object_pool_t));
    pool->capacity = capacity;
    pool->item_size = item_size;
    pool->buffer = calloc(capacity, item_size);
    pool->free_list = malloc(sizeof(atomic_int) * capacity);

    /* 初始化空闲列表 */
    for (int i = 0; i < capacity; i++) {
        atomic_init(&pool->free_list[i], i);
    }
    atomic_init(&pool->next_free, 0);

    return pool;
}

void* pool_acquire(object_pool_t* pool) {
    int idx = atomic_fetch_add(&pool->next_free, 1);

    if (idx >= pool->capacity) {
        /* 池满 */
        atomic_fetch_sub(&pool->next_free, 1);
        return NULL;
    }

    return (char*)pool->buffer +
           atomic_load(&pool->free_list[idx]) * pool->item_size;
}

void pool_release(object_pool_t* pool, void* obj) {
    /* 计算索引 */
    int idx = ((char*)obj - (char*)pool->buffer) / pool->item_size;

    /* 归还到空闲列表 */
    int free_pos = atomic_fetch_sub(&pool->next_free, 1) - 1;
    if (free_pos >= 0) {
        atomic_store(&pool->free_list[free_pos], idx);
    }
}

/* 使用 */
void example_pool(void) {
    object_pool_t* pool = pool_create(100, sizeof(my_object_t));

    my_object_t* obj = pool_acquire(pool);
    /* 使用 */
    pool_release(pool, obj);
}
```

### 带初始化的对象池

```c
typedef struct {
    void* (*init)(void);    /* 初始化函数 */
    void (*reset)(void*);   /* 重置函数 */
} pool_ops_t;

typedef struct {
    void* objects;
    int capacity;
    int item_size;
    pool_ops_t ops;

    mtx_t lock;
    void** available;
    int available_count;
} init_pool_t;

init_pool_t* pool_create_init(int cap, int size, pool_ops_t* ops) {
    init_pool_t* pool = malloc(sizeof(init_pool_t));
    pool->capacity = cap;
    pool->item_size = size;
    pool->ops = *ops;

    pool->objects = malloc(cap * size);
    pool->available = malloc(sizeof(void*) * cap);
    pool->available_count = cap;

    /* 初始化所有对象 */
    for (int i = 0; i < cap; i++) {
        void* obj = (char*)pool->objects + i * size;
        if (ops->init) {
            ops->init(obj);
        }
        pool->available[i] = obj;
    }

    mtx_init(&pool->lock, mtx_plain);
    return pool;
}

void* pool_get(init_pool_t* pool) {
    mtx_lock(&pool->lock);

    if (pool->available_count == 0) {
        mtx_unlock(&pool->lock);
        return NULL;
    }

    void* obj = pool->available[--pool->available_count];
    mtx_unlock(&pool->lock);
    return obj;
}

void pool_put(init_pool_t* pool, void* obj) {
    if (pool->ops.reset) {
        pool->ops.reset(obj);
    }

    mtx_lock(&pool->lock);
    pool->available[pool->available_count++] = obj;
    mtx_unlock(&pool->lock);
}
```

---

## 原型模式

```c
/* 克隆模式 */

typedef struct {
    void* (*clone)(const void*);
} cloneable_t;

typedef struct document {
    cloneable_t base;
    char* title;
    char* content;
    struct document** sections;
    int section_count;
} document_t;

void* document_clone(const void* src) {
    const document_t* orig = src;
    document_t* copy = malloc(sizeof(document_t));

    /* 深拷贝 */
    copy->title = strdup(orig->title);
    copy->content = strdup(orig->content);
    copy->section_count = orig->section_count;
    copy->sections = malloc(sizeof(document_t*) * orig->section_count);

    for (int i = 0; i < orig->section_count; i++) {
        copy->sections[i] = document_clone(orig->sections[i]);
    }

    copy->base.clone = document_clone;
    return copy;
}

document_t* document_new(const char* title) {
    document_t* doc = calloc(1, sizeof(document_t));
    doc->title = strdup(title);
    doc->base.clone = document_clone;
    return doc;
}

/* 使用 */
void example_clone(void) {
    document_t* original = document_new("Original");
    /* 添加内容... */

    document_t* copy = original->base.clone(original);
    /* 修改副本不影响原稿 */
}
```

---

## 总结

C语言创建型模式要点:

1. **单例**: 使用C11原子操作或call_once保证线程安全
2. **工厂**: 函数指针实现多态创建
3. **建造者**: 返回自身指针支持链式调用
4. **对象池**: 原子操作或无锁队列管理空闲对象
5. **原型**: 函数指针实现克隆接口

---

## 参考

- [Design Patterns in C](https://github.com/rafael-santiago/c-patterns)
- [Object-Oriented Programming in C](https://www.state-machine.com/doc/AN_OOP_in_C.pdf)
