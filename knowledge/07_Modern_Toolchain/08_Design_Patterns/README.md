---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# C 语言设计模式

> 在 C 语言中实现面向对象设计思想与可复用架构

---

## 目录

- [C 语言设计模式](#c-语言设计模式)
  - [目录](#目录)
  - [创建型模式](#创建型模式)
    - [单例模式 (Singleton)](#单例模式-singleton)
    - [工厂模式 (Factory)](#工厂模式-factory)
    - [构建器模式 (Builder)](#构建器模式-builder)
  - [结构型模式](#结构型模式)
    - [适配器模式 (Adapter)](#适配器模式-adapter)
    - [装饰器模式 (Decorator)](#装饰器模式-decorator)
    - [代理模式 (Proxy)](#代理模式-proxy)
  - [行为型模式](#行为型模式)
    - [观察者模式 (Observer)](#观察者模式-observer)
    - [策略模式 (Strategy)](#策略模式-strategy)
    - [状态机模式 (State Machine)](#状态机模式-state-machine)
  - [对象导向的 C 编程技巧](#对象导向的-c-编程技巧)
    - [封装与数据隐藏](#封装与数据隐藏)
    - [继承与组合](#继承与组合)
  - [回调与委托模式](#回调与委托模式)
    - [函数指针作为回调](#函数指针作为回调)
    - [带上下文的回调 (闭包模拟)](#带上下文的回调-闭包模拟)
  - [面向接口编程](#面向接口编程)
    - [接口定义与实现分离](#接口定义与实现分离)
    - [插件系统示例](#插件系统示例)
  - [总结](#总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)

---

## 创建型模式

### 单例模式 (Singleton)

单例模式确保一个类只有一个实例，并提供一个全局访问点。

```
┌─────────────────────────────────────────────────────────┐
│                     单例模式结构                         │
├─────────────────────────────────────────────────────────┤
│                                                         │
│   ┌──────────────┐          ┌──────────────────┐       │
│   │   Client     │─────────▶│    Singleton     │       │
│   └──────────────┘          ├──────────────────┤       │
│                             │ - instance: ptr  │       │
│                             │ - initialized    │       │
│                             ├──────────────────┤       │
│                             │ + getInstance()  │       │
│                             │ + operation()    │       │
│                             └──────────────────┘       │
│                                         ▲              │
│                                         │ 唯一实例      │
└─────────────────────────────────────────────────────────┘
```

```c
// singleton.h
#ifndef SINGLETON_H
#define SINGLETON_H

typedef struct {
    int data;
    char name[32];
} Singleton;

// 获取唯一实例
Singleton* singleton_get_instance(void);

// 实例操作
void singleton_set_data(Singleton* self, int value);
int singleton_get_data(const Singleton* self);

#endif
```

```c
// singleton.c
#include "singleton.h"
#include <string.h>

static Singleton instance;
static int initialized = 0;

Singleton* singleton_get_instance(void) {
    if (!initialized) {
        memset(&instance, 0, sizeof(instance));
        strcpy(instance.name, "Default");
        initialized = 1;
    }
    return &instance;
}

void singleton_set_data(Singleton* self, int value) {
    if (self == &instance) {
        self->data = value;
    }
}

int singleton_get_data(const Singleton* self) {
    return (self == &instance) ? self->data : 0;
}
```

**线程安全版本：**

```c
#include <pthread.h>

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

Singleton* singleton_get_instance_threadsafe(void) {
    if (!initialized) {
        pthread_mutex_lock(&mutex);
        // 双重检查锁定
        if (!initialized) {
            memset(&instance, 0, sizeof(instance));
            initialized = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
    return &instance;
}
```

---

### 工厂模式 (Factory)

工厂模式将对象创建逻辑与使用逻辑分离，支持运行时决定创建哪种对象。

```
┌─────────────────────────────────────────────────────────────────┐
│                        工厂模式结构                              │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌──────────────┐              ┌──────────────────────┐       │
│   │    Client    │─────────────▶│   ShapeFactory       │       │
│   └──────────────┘              ├──────────────────────┤       │
│                                 │ + create_shape(type) │       │
│                                 └──────────┬───────────┘       │
│                                            │                    │
│              ┌─────────────────────────────┼─────────────────┐ │
│              │                             │                 │ │
│              ▼                             ▼                 ▼ │
│   ┌──────────────────┐        ┌──────────────────┐  ┌─────────┐│
│   │    Circle        │        │    Rectangle     │  │ Triangle││
│   ├──────────────────┤        ├──────────────────┤  ├─────────┤│
│   │ - radius         │        │ - width, height  │  │ - base  ││
│   ├──────────────────┤        ├──────────────────┤  ├─────────┤│
│   │ + draw()         │        │ + draw()         │  │ + draw()││
│   │ + area()         │        │ + area()         │  │ + area()││
│   └──────────────────┘        └──────────────────┘  └─────────┘│
│              ▲                             ▲                 ▲ │
│              └─────────────────────────────┴─────────────────┘ │
│                           统一接口 (Shape)                       │
└─────────────────────────────────────────────────────────────────┘
```

```c
// shape.h - 抽象接口
#ifndef SHAPE_H
#define SHAPE_H

typedef struct Shape Shape;

typedef struct {
    void (*draw)(const Shape* self);
    double (*area)(const Shape* self);
    void (*destroy)(Shape* self);
} ShapeVtbl;

struct Shape {
    const ShapeVtbl* vtbl;
};

// 内联虚函数调用
static inline void shape_draw(const Shape* self) {
    self->vtbl->draw(self);
}

static inline double shape_area(const Shape* self) {
    return self->vtbl->area(self);
}

static inline void shape_destroy(Shape* self) {
    self->vtbl->destroy(self);
}

// 工厂函数
Shape* shape_create_circle(double radius);
Shape* shape_create_rectangle(double width, double height);

#endif
```

```c
// circle.c
#include "shape.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    Shape base;
    double radius;
} Circle;

static void circle_draw(const Shape* self) {
    const Circle* c = (const Circle*)self;
    printf("Circle: radius=%.2f\n", c->radius);
}

static double circle_area(const Shape* self) {
    const Circle* c = (const Circle*)self;
    return M_PI * c->radius * c->radius;
}

static void circle_destroy(Shape* self) {
    free(self);
}

static const ShapeVtbl circle_vtbl = {
    .draw = circle_draw,
    .area = circle_area,
    .destroy = circle_destroy
};

Shape* shape_create_circle(double radius) {
    Circle* c = malloc(sizeof(Circle));
    if (c) {
        c->base.vtbl = &circle_vtbl;
        c->radius = radius;
    }
    return (Shape*)c;
}
```

```c
// shape_factory.c - 使用示例
#include "shape.h"
#include <stdio.h>

typedef enum {
    SHAPE_CIRCLE,
    SHAPE_RECTANGLE
} ShapeType;

Shape* shape_factory_create(ShapeType type, ...) {
    // 简化的工厂函数
    switch (type) {
        case SHAPE_CIRCLE: {
            double radius = 5.0; // 实际应从可变参数获取
            return shape_create_circle(radius);
        }
        case SHAPE_RECTANGLE: {
            double w = 4.0, h = 6.0;
            return shape_create_rectangle(w, h);
        }
        default:
            return NULL;
    }
}

// 使用
void factory_example(void) {
    Shape* shapes[2];
    shapes[0] = shape_create_circle(5.0);
    shapes[1] = shape_create_rectangle(4.0, 6.0);

    for (int i = 0; i < 2; i++) {
        shape_draw(shapes[i]);
        printf("Area: %.2f\n", shape_area(shapes[i]));
        shape_destroy(shapes[i]);
    }
}
```

---

### 构建器模式 (Builder)

构建器模式用于创建复杂对象，允许分步骤构建，并通过链式调用提高可读性。

```c
// http_request.h
#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

typedef struct HttpRequest HttpRequest;

// 构建器接口
typedef struct {
    HttpRequest* (*set_method)(HttpRequest* self, const char* method);
    HttpRequest* (*set_url)(HttpRequest* self, const char* url);
    HttpRequest* (*set_header)(HttpRequest* self,
                               const char* key,
                               const char* value);
    HttpRequest* (*set_body)(HttpRequest* self, const char* body);
    HttpRequest* (*build)(HttpRequest* self);
} HttpRequestBuilder;

// 获取构建器
HttpRequestBuilder* http_request_builder(void);

// 请求操作
void http_request_execute(const HttpRequest* req);
void http_request_destroy(HttpRequest* req);

#endif
```

```c
// http_request.c
#include "http_request.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_HEADERS 16

typedef struct {
    char key[32];
    char value[128];
} Header;

struct HttpRequest {
    char method[16];
    char url[256];
    Header headers[MAX_HEADERS];
    int header_count;
    char* body;

    // 构建器方法
    HttpRequestBuilder builder;
};

static HttpRequest* builder_set_method(HttpRequest* self, const char* method) {
    strncpy(self->method, method, sizeof(self->method) - 1);
    return self;
}

static HttpRequest* builder_set_url(HttpRequest* self, const char* url) {
    strncpy(self->url, url, sizeof(self->url) - 1);
    return self;
}

static HttpRequest* builder_set_header(HttpRequest* self,
                                       const char* key,
                                       const char* value) {
    if (self->header_count < MAX_HEADERS) {
        Header* h = &self->headers[self->header_count++];
        strncpy(h->key, key, sizeof(h->key) - 1);
        strncpy(h->value, value, sizeof(h->value) - 1);
    }
    return self;
}

static HttpRequest* builder_set_body(HttpRequest* self, const char* body) {
    if (self->body) free(self->body);
    self->body = strdup(body);
    return self;
}

static HttpRequest* builder_build(HttpRequest* self) {
    return self;
}

HttpRequestBuilder* http_request_builder(void) {
    HttpRequest* req = calloc(1, sizeof(HttpRequest));
    if (!req) return NULL;

    strcpy(req->method, "GET");

    req->builder.set_method = builder_set_method;
    req->builder.set_url = builder_set_url;
    req->builder.set_header = builder_set_header;
    req->builder.set_body = builder_set_body;
    req->builder.build = builder_build;

    return &req->builder;
}

// 宏简化链式调用
#define BUILDER_CALL(builder, method, ...) \
    (builder)->method((HttpRequest*)(builder), ##__VA_ARGS__)

// 使用示例
void builder_example(void) {
    HttpRequestBuilder* b = http_request_builder();
    HttpRequest* req = BUILDER_CALL(
        b, set_method, "POST"
    )->set_url(
        (HttpRequest*)b, "https://api.example.com/data"
    )->set_header(
        (HttpRequest*)b, "Content-Type", "application/json"
    )->set_header(
        (HttpRequest*)b, "Authorization", "Bearer token123"
    )->set_body(
        (HttpRequest*)b, "{\"key\":\"value\"}"
    )->build((HttpRequest*)b);

    http_request_execute(req);
    http_request_destroy(req);
}
```

---

## 结构型模式

### 适配器模式 (Adapter)

适配器模式将不兼容的接口转换为客户端期望的接口。

```
┌─────────────────────────────────────────────────────────────────┐
│                       适配器模式结构                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌──────────────┐              ┌──────────────────────┐       │
│   │    Client    │─────────────▶│    Target Interface  │       │
│   │  expects     │              │    + request()       │       │
│   └──────────────┘              └──────────┬───────────┘       │
│                                            │                    │
│                                   ┌────────┴────────┐          │
│                                   │                 │          │
│                                   ▼                 ▼          │
│                          ┌──────────────┐    ┌──────────────┐  │
│                          │   Adapter    │    │  Adaptee     │  │
│                          ├──────────────┤    │  (legacy)    │  │
│                          │ - adaptee    │────▶│+ specific() │  │
│                          ├──────────────┤    ├──────────────┤  │
│                          │ + request()  │    │              │  │
│                          │   adaptee->  │    │              │  │
│                          │   specific() │    │              │  │
│                          └──────────────┘    └──────────────┘  │
└─────────────────────────────────────────────────────────────────┘
```

```c
// 旧的数据库接口 (Adaptee)
typedef struct {
    void (*connect_legacy)(const char* host);
    void (*query_legacy)(const char* sql);
} LegacyDatabase;

// 新的统一接口 (Target)
typedef struct {
    int (*connect)(void* self, const char* conn_str);
    int (*execute)(void* self, const char* sql);
    void (*close)(void* self);
} DatabaseInterface;

// 适配器实现
typedef struct {
    DatabaseInterface interface;
    LegacyDatabase* adaptee;
    char host[64];
} DatabaseAdapter;

static int adapter_connect(void* self, const char* conn_str) {
    DatabaseAdapter* adapter = self;
    // 解析 conn_str 提取 host
    adapter->adaptee->connect_legacy(conn_str);
    return 0;
}

static int adapter_execute(void* self, const char* sql) {
    DatabaseAdapter* adapter = self;
    adapter->adaptee->query_legacy(sql);
    return 0;
}

static void adapter_close(void* self) {
    (void)self;
    // 旧接口没有 close，可能什么都不做
}

DatabaseAdapter* database_adapter_create(LegacyDatabase* legacy) {
    DatabaseAdapter* a = malloc(sizeof(DatabaseAdapter));
    a->interface.connect = adapter_connect;
    a->interface.execute = adapter_execute;
    a->interface.close = adapter_close;
    a->adaptee = legacy;
    return a;
}
```

---

### 装饰器模式 (Decorator)

装饰器模式动态地为对象添加额外职责，比继承更灵活。

```c
// io_stream.h - 组件接口
typedef struct {
    int (*read)(void* self, char* buf, int size);
    int (*write)(void* self, const char* buf, int size);
    void (*close)(void* self);
} IOStream;

// 具体组件: 文件流
typedef struct {
    IOStream interface;
    FILE* file;
} FileStream;

// 装饰器基类
typedef struct {
    IOStream interface;
    IOStream* wrapped;
} StreamDecorator;

// 具体装饰器: 缓冲装饰器
typedef struct {
    StreamDecorator base;
    char buffer[4096];
    int buf_pos;
    int buf_size;
} BufferedStream;

static int buffered_read(void* self, char* buf, int size) {
    BufferedStream* bs = self;
    // 实现缓冲逻辑
    if (bs->buf_pos >= bs->buf_size) {
        // 缓冲区空，从底层读取
        bs->buf_size = bs->base.wrapped->read(bs->base.wrapped,
                                               bs->buffer,
                                               sizeof(bs->buffer));
        bs->buf_pos = 0;
        if (bs->buf_size <= 0) return bs->buf_size;
    }

    int to_copy = size < (bs->buf_size - bs->buf_pos) ?
                  size : (bs->buf_size - bs->buf_pos);
    memcpy(buf, bs->buffer + bs->buf_pos, to_copy);
    bs->buf_pos += to_copy;
    return to_copy;
}

// 具体装饰器: 压缩装饰器
typedef struct {
    StreamDecorator base;
    z_stream zstream;  // zlib
} CompressedStream;

static int compressed_write(void* self, const char* buf, int size) {
    CompressedStream* cs = self;
    // 压缩后写入底层流
    // ... zlib 压缩逻辑
    return cs->base.wrapped->write(cs->base.wrapped, buf, size);
}

// 组合使用示例
void decorator_example(void) {
    // 文件 -> 压缩 -> 缓冲
    FileStream* file = file_stream_open("data.bin");
    CompressedStream* compressed = compressed_stream_create((IOStream*)file);
    BufferedStream* buffered = buffered_stream_create((IOStream*)compressed);

    // 写入经过缓冲->压缩->文件的完整链
    char data[] = "Hello, World!";
    buffered->base.interface.write(buffered, data, sizeof(data));

    // 关闭时反向关闭
    buffered->base.interface.close(buffered);
}
```

---

### 代理模式 (Proxy)

代理模式为其他对象提供一种代理以控制对这个对象的访问。

```c
// 虚拟代理: 延迟加载大型资源
typedef struct {
    const char* (*get_image_data)(void* self);
    int (*get_width)(void* self);
    int (*get_height)(void* self);
} Image;

// 真实图像
typedef struct {
    Image interface;
    char* data;
    int width;
    int height;
} RealImage;

// 代理
typedef struct {
    Image interface;
    const char* filename;
    RealImage* real_image;  // 延迟加载
} ImageProxy;

static void ensure_loaded(ImageProxy* proxy) {
    if (!proxy->real_image) {
        proxy->real_image = real_image_load(proxy->filename);
    }
}

static const char* proxy_get_data(void* self) {
    ImageProxy* proxy = self;
    ensure_loaded(proxy);
    return proxy->real_image->interface.get_image_data(proxy->real_image);
}

static int proxy_get_width(void* self) {
    ImageProxy* proxy = self;
    // 元数据可以预先加载，不需要完整数据
    if (proxy->real_image) {
        return proxy->real_image->interface.get_width(proxy->real_image);
    }
    return image_metadata_get_width(proxy->filename);  // 只读元数据
}

// 使用场景: 图片列表，只有显示时才加载
void gallery_display(Image** images, int count, int visible_index) {
    for (int i = 0; i < count; i++) {
        if (i == visible_index) {
            // 这会触发实际加载
            printf("Displaying: %dx%d\n",
                   images[i]->get_width(images[i]),
                   images[i]->get_height(images[i]));
        }
        // 其他图片作为代理，不占用内存
    }
}
```

---

## 行为型模式

### 观察者模式 (Observer)

观察者模式定义对象间的一对多依赖，当一个对象状态改变时，所有依赖者自动收到通知。

```
┌─────────────────────────────────────────────────────────────────┐
│                       观察者模式结构                             │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│   ┌──────────────────┐         ┌──────────────────────┐        │
│   │   Subject        │◀────────│     Observer         │        │
│   ├──────────────────┤         ├──────────────────────┤        │
│   │ - observers[]    │         │ + update(data)       │        │
│   ├──────────────────┤         └──────────────────────┘        │
│   │ + attach(obs)    │                      ▲                   │
│   │ + detach(obs)    │                      │                   │
│   │ + notify()       │         ┌───────────┴───────────┐       │
│   └──────────────────┘         │                       │       │
│              │           ┌─────────────┐        ┌─────────────┐│
│              │           │  ObserverA  │        │  ObserverB  ││
│              │           │  + update() │        │  + update() ││
│              │           └─────────────┘        └─────────────┘│
│              │                                                  │
│              ▼                                                  │
│       ┌──────────────┐                                          │
│       │ notify()     │───▶ for each obs: obs->update(data)     │
│       └──────────────┘                                          │
└─────────────────────────────────────────────────────────────────┘
```

```c
// observer.h
#ifndef OBSERVER_H
#define OBSERVER_H

typedef struct Observer Observer;
typedef struct Subject Subject;

struct Observer {
    void (*update)(Observer* self, const char* event, void* data);
    void (*destroy)(Observer* self);
};

struct Subject {
    void (*attach)(Subject* self, Observer* observer);
    void (*detach)(Subject* self, Observer* observer);
    void (*notify)(Subject* self, const char* event, void* data);
};

// 创建具体主题
Subject* event_bus_create(void);

#endif
```

```c
// observer.c
#include "observer.h"
#include <stdlib.h>
#include <string.h>

#define MAX_OBSERVERS 32

typedef struct {
    Subject interface;
    Observer* observers[MAX_OBSERVERS];
    int count;
} EventBus;

static void event_bus_attach(Subject* self, Observer* observer) {
    EventBus* bus = (EventBus*)self;
    if (bus->count < MAX_OBSERVERS) {
        bus->observers[bus->count++] = observer;
    }
}

static void event_bus_detach(Subject* self, Observer* observer) {
    EventBus* bus = (EventBus*)self;
    for (int i = 0; i < bus->count; i++) {
        if (bus->observers[i] == observer) {
            // 移动后续元素
            memmove(&bus->observers[i], &bus->observers[i+1],
                   (bus->count - i - 1) * sizeof(Observer*));
            bus->count--;
            break;
        }
    }
}

static void event_bus_notify(Subject* self, const char* event, void* data) {
    EventBus* bus = (EventBus*)self;
    for (int i = 0; i < bus->count; i++) {
        bus->observers[i]->update(bus->observers[i], event, data);
    }
}

Subject* event_bus_create(void) {
    EventBus* bus = calloc(1, sizeof(EventBus));
    bus->interface.attach = event_bus_attach;
    bus->interface.detach = event_bus_detach;
    bus->interface.notify = event_bus_notify;
    return (Subject*)bus;
}
```

```c
// 使用示例: 温度监控系统
typedef struct {
    Observer interface;
    int threshold;
} TemperatureAlert;

static void alert_update(Observer* self, const char* event, void* data) {
    TemperatureAlert* alert = (TemperatureAlert*)self;
    if (strcmp(event, "temperature") == 0) {
        float temp = *(float*)data;
        if (temp > alert->threshold) {
            printf("ALERT: Temperature %.1f exceeds threshold %d!\n",
                   temp, alert->threshold);
        }
    }
}

void observer_example(void) {
    Subject* bus = event_bus_create();

    // 创建观察者
    TemperatureAlert high_temp_alert = {
        .interface.update = alert_update,
        .threshold = 80
    };

    // 订阅
    bus->attach(bus, (Observer*)&high_temp_alert);

    // 模拟温度变化
    float temp = 85.0f;
    bus->notify(bus, "temperature", &temp);

    // 输出: ALERT: Temperature 85.0 exceeds threshold 80!
}
```

---

### 策略模式 (Strategy)

策略模式定义算法族，分别封装起来，让它们可以互相替换。

```c
// 排序策略接口
typedef struct {
    void (*sort)(int* arr, int n);
    const char* name;
} SortStrategy;

// 具体策略: 快速排序
static void quick_sort_impl(int* arr, int left, int right) {
    if (left >= right) return;
    int pivot = arr[(left + right) / 2];
    int i = left, j = right;
    while (i <= j) {
        while (arr[i] < pivot) i++;
        while (arr[j] > pivot) j--;
        if (i <= j) {
            int tmp = arr[i]; arr[i] = arr[j]; arr[j] = tmp;
            i++; j--;
        }
    }
    quick_sort_impl(arr, left, j);
    quick_sort_impl(arr, i, right);
}

static void quick_sort(int* arr, int n) {
    quick_sort_impl(arr, 0, n - 1);
}

static const SortStrategy quick_sort_strategy = {
    .sort = quick_sort,
    .name = "QuickSort"
};

// 具体策略: 归并排序
static void merge_sort_impl(int* arr, int* temp, int left, int right) {
    if (left >= right) return;
    int mid = (left + right) / 2;
    merge_sort_impl(arr, temp, left, mid);
    merge_sort_impl(arr, temp, mid + 1, right);

    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        temp[k++] = (arr[i] <= arr[j]) ? arr[i++] : arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];

    for (i = left; i <= right; i++) arr[i] = temp[i];
}

static void merge_sort(int* arr, int n) {
    int* temp = malloc(n * sizeof(int));
    merge_sort_impl(arr, temp, 0, n - 1);
    free(temp);
}

static const SortStrategy merge_sort_strategy = {
    .sort = merge_sort,
    .name = "MergeSort"
};

// 上下文: 排序器
typedef struct {
    const SortStrategy* strategy;
} Sorter;

void sorter_set_strategy(Sorter* sorter, const SortStrategy* strategy) {
    sorter->strategy = strategy;
}

void sorter_execute(Sorter* sorter, int* arr, int n) {
    printf("Using %s...\n", sorter->strategy->name);
    sorter->strategy->sort(arr, n);
}

// 使用
void strategy_example(void) {
    Sorter sorter = { &quick_sort_strategy };
    int data[] = {5, 2, 8, 1, 9};

    // 使用快速排序
    sorter_execute(&sorter, data, 5);

    // 切换为归并排序
    sorter_set_strategy(&sorter, &merge_sort_strategy);
    sorter_execute(&sorter, data, 5);
}
```

---

### 状态机模式 (State Machine)

状态机模式允许对象在内部状态改变时改变它的行为。

```c
// 网络连接状态机
typedef struct Connection Connection;

// 状态接口
typedef struct {
    void (*open)(Connection* conn);
    void (*close)(Connection* conn);
    void (*send)(Connection* conn, const char* data);
    void (*receive)(Connection* conn);
    const char* name;
} ConnectionState;

struct Connection {
    const ConnectionState* state;
    int socket_fd;
    char buffer[1024];
};

// 状态切换宏
#define CHANGE_STATE(conn, new_state) do { \
    printf("[%s] -> [%s]\n", (conn)->state->name, (new_state).name); \
    (conn)->state = &(new_state); \
} while(0)

// 具体状态: 关闭
static void closed_open(Connection* conn);
static void closed_close(Connection* conn);
static void closed_send(Connection* conn, const char* data);
static void closed_receive(Connection* conn);

static const ConnectionState CLOSED_STATE = {
    .open = closed_open,
    .close = closed_close,
    .send = closed_send,
    .receive = closed_receive,
    .name = "CLOSED"
};

static void closed_open(Connection* conn) {
    // 模拟连接建立
    conn->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Socket created: %d\n", conn->socket_fd);
    extern const ConnectionState ESTABLISHED_STATE;
    CHANGE_STATE(conn, ESTABLISHED_STATE);
}

static void closed_close(Connection* conn) {
    printf("Already closed\n");
}

static void closed_send(Connection* conn, const char* data) {
    (void)conn; (void)data;
    printf("ERROR: Cannot send in CLOSED state\n");
}

static void closed_receive(Connection* conn) {
    (void)conn;
    printf("ERROR: Cannot receive in CLOSED state\n");
}

// 具体状态: 已建立
static void established_open(Connection* conn);
static void established_close(Connection* conn);
static void established_send(Connection* conn, const char* data);
static void established_receive(Connection* conn);

const ConnectionState ESTABLISHED_STATE = {
    .open = established_open,
    .close = established_close,
    .send = established_send,
    .receive = established_receive,
    .name = "ESTABLISHED"
};

static void established_open(Connection* conn) {
    (void)conn;
    printf("Already connected\n");
}

static void established_close(Connection* conn) {
    close(conn->socket_fd);
    conn->socket_fd = -1;
    CHANGE_STATE(conn, CLOSED_STATE);
}

static void established_send(Connection* conn, const char* data) {
    send(conn->socket_fd, data, strlen(data), 0);
    printf("Sent: %s\n", data);
}

static void established_receive(Connection* conn) {
    int n = recv(conn->socket_fd, conn->buffer, sizeof(conn->buffer) - 1, 0);
    if (n > 0) {
        conn->buffer[n] = '\0';
        printf("Received: %s\n", conn->buffer);
    }
}

// 使用
void state_machine_example(void) {
    Connection conn = { &CLOSED_STATE, -1, {0} };

    conn.state->send(&conn, "Hello");  // ERROR
    conn.state->open(&conn);            // CLOSED -> ESTABLISHED
    conn.state->send(&conn, "Hello");  // 发送成功
    conn.state->close(&conn);           // ESTABLISHED -> CLOSED
}
```

---

## 对象导向的 C 编程技巧

### 封装与数据隐藏

```c
// stack.h - 只暴露接口，隐藏实现
typedef struct Stack Stack;

Stack* stack_create(int capacity);
void stack_destroy(Stack* stack);
int stack_push(Stack* stack, int value);
int stack_pop(Stack* stack, int* value);
int stack_peek(const Stack* stack, int* value);
int stack_size(const Stack* stack);
int stack_is_empty(const Stack* stack);

// stack.c - 实现细节对其他文件不可见
#include "stack.h"
#include <stdlib.h>

struct Stack {
    int* data;
    int top;
    int capacity;
};

Stack* stack_create(int capacity) {
    Stack* s = malloc(sizeof(Stack));
    s->data = malloc(capacity * sizeof(int));
    s->top = -1;
    s->capacity = capacity;
    return s;
}
// ... 其他实现
```

### 继承与组合

```c
// 基类
typedef struct {
    int x, y;
    void (*draw)(void* self);
} Point;

// 派生类 (包含基类作为第一个成员)
typedef struct {
    Point base;  // 必须作为第一个成员
    int radius;
} Circle;

// 多态使用
void draw_any_point(Point* p) {
    p->draw(p);  // 虚函数调用
}

// 安全的向上转型
Circle* circle_from_point(Point* p) {
    return (Circle*)p;  // 因为 Point 是 Circle 的第一个成员
}
```

---

## 回调与委托模式

### 函数指针作为回调

```c
// 异步操作回调
typedef void (*SuccessCallback)(void* result, void* user_data);
typedef void (*ErrorCallback)(int error_code, const char* msg, void* user_data);

typedef struct {
    SuccessCallback on_success;
    ErrorCallback on_error;
    void* user_data;
} AsyncCallbacks;

void async_operation(const char* request, const AsyncCallbacks* cb);

// 使用示例
void on_data_received(void* result, void* user_data) {
    HttpResponse* resp = result;
    int request_id = *(int*)user_data;
    printf("Request %d completed: %s\n", request_id, resp->body);
}

void on_error(int code, const char* msg, void* user_data) {
    int request_id = *(int*)user_data;
    printf("Request %d failed [%d]: %s\n", request_id, code, msg);
}

void fetch_data(void) {
    static int request_id = 1;
    AsyncCallbacks cb = {
        .on_success = on_data_received,
        .on_error = on_error,
        .user_data = &request_id
    };
    async_operation("https://api.example.com/data", &cb);
    request_id++;
}
```

### 带上下文的回调 (闭包模拟)

```c
typedef struct {
    void (*func)(void* ctx);
    void* context;
    void (*free_context)(void* ctx);
} Closure;

Closure* closure_create(void (*func)(void*), void* ctx,
                        void (*free_ctx)(void*)) {
    Closure* c = malloc(sizeof(Closure));
    c->func = func;
    c->context = ctx;
    c->free_context = free_ctx;
    return c;
}

void closure_call(Closure* c) {
    c->func(c->context);
}

void closure_destroy(Closure* c) {
    if (c->free_context) {
        c->free_context(c->context);
    }
    free(c);
}

// 使用: 创建带状态的回调
void counter_increment(void* ctx) {
    int* count = ctx;
    (*count)++;
    printf("Count: %d\n", *count);
}

Closure* create_counter(void) {
    int* count = malloc(sizeof(int));
    *count = 0;
    return closure_create(counter_increment, count, free);
}
```

---

## 面向接口编程

### 接口定义与实现分离

```c
// storage_interface.h - 纯接口定义
#ifndef STORAGE_INTERFACE_H
#define STORAGE_INTERFACE_H

#include <stddef.h>

typedef struct Storage Storage;

typedef struct {
    int (*open)(Storage* self, const char* path);
    int (*read)(Storage* self, void* buf, size_t size);
    int (*write)(Storage* self, const void* buf, size_t size);
    int (*close)(Storage* self);
    void (*destroy)(Storage* self);
} StorageInterface;

struct Storage {
    const StorageInterface* vtable;
};

// 辅助内联函数
static inline int storage_open(Storage* s, const char* p) {
    return s->vtable->open(s, p);
}
static inline int storage_read(Storage* s, void* b, size_t n) {
    return s->vtable->read(s, b, n);
}
// ...

#endif
```

```c
// file_storage.c - 文件存储实现
#include "storage_interface.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    Storage base;
    FILE* fp;
} FileStorage;

static int file_open(Storage* self, const char* path) {
    FileStorage* fs = (FileStorage*)self;
    fs->fp = fopen(path, "r+");
    return fs->fp ? 0 : -1;
}

static int file_read(Storage* self, void* buf, size_t size) {
    FileStorage* fs = (FileStorage*)self;
    return fread(buf, 1, size, fs->fp);
}

// ... 其他实现

static const StorageInterface file_storage_vtable = {
    .open = file_open,
    .read = file_read,
    .write = file_write,
    .close = file_close,
    .destroy = file_destroy
};

Storage* file_storage_create(void) {
    FileStorage* fs = calloc(1, sizeof(FileStorage));
    fs->base.vtable = &file_storage_vtable;
    return (Storage*)fs;
}
```

```c
// memory_storage.c - 内存存储实现
typedef struct {
    Storage base;
    char* buffer;
    size_t size;
    size_t pos;
} MemoryStorage;

static int mem_write(Storage* self, const void* buf, size_t size) {
    MemoryStorage* ms = (MemoryStorage*)self;
    if (ms->pos + size > ms->size) return -1;
    memcpy(ms->buffer + ms->pos, buf, size);
    ms->pos += size;
    return size;
}
// ... 其他实现

Storage* memory_storage_create(void* buffer, size_t size) {
    MemoryStorage* ms = calloc(1, sizeof(MemoryStorage));
    ms->base.vtable = &memory_storage_vtable;
    ms->buffer = buffer;
    ms->size = size;
    return (Storage*)ms;
}
```

### 插件系统示例

```c
// plugin.h
typedef struct {
    const char* name;
    const char* version;
    int (*init)(void);
    void (*shutdown)(void);
    int (*execute)(const char* args);
} Plugin;

// 插件导出宏
#define PLUGIN_EXPORT __attribute__((visibility("default")))

#define DEFINE_PLUGIN(name, ver, init_fn, shutdown_fn, exec_fn) \
    PLUGIN_EXPORT const Plugin plugin_info = { \
        .name = name, \
        .version = ver, \
        .init = init_fn, \
        .shutdown = shutdown_fn, \
        .execute = exec_fn \
    }

// 插件加载器
typedef struct {
    void* handle;
    const Plugin* info;
} LoadedPlugin;

LoadedPlugin* plugin_load(const char* path);
void plugin_unload(LoadedPlugin* p);

// 使用统一接口调用任何插件
int plugin_execute(LoadedPlugin* p, const char* args) {
    return p->info->execute(args);
}
```

---

## 总结

C 语言虽然没有直接的面向对象语法支持，但通过以下技术可以实现类似的设计模式：

| 面向对象特性 | C 语言实现方式 |
|-------------|---------------|
| 封装 | 不透明结构体 + 访问函数 |
| 继承 | 结构体嵌套 (第一个成员) |
| 多态 | 虚函数表 (vtable) |
| 接口 | 纯函数指针结构体 |
| 构造函数 | 工厂函数 |
| 析构函数 | 显式 destroy 函数 |
| 泛型 | void* + 类型回调 |

设计模式的核心价值在于**解耦**和**复用**，C 语言通过结构化的方式同样可以实现这些目标。掌握这些模式有助于编写可维护、可扩展的高质量 C 代码。


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
