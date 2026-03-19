# C语言设计模式索引

> **类型**: 索引文档
> **位置**: `knowledge/06_Thinking_Representation/06_Index/03_Design_Patterns_Index.md`
> **描述**: C语言常用设计模式分类索引，包含创建型、结构型、行为型、并发型和嵌入式模式

---

## 索引目录

- [C语言设计模式索引](#c语言设计模式索引)
  - [索引目录](#索引目录)
  - [📑 目录](#-目录)
  - [创建型模式](#创建型模式)
    - [单例模式 (Singleton)](#单例模式-singleton)
    - [工厂模式 (Factory)](#工厂模式-factory)
    - [抽象工厂模式 (Abstract Factory)](#抽象工厂模式-abstract-factory)
    - [建造者模式 (Builder)](#建造者模式-builder)
    - [对象池模式 (Object Pool)](#对象池模式-object-pool)
  - [结构型模式](#结构型模式)
    - [适配器模式 (Adapter)](#适配器模式-adapter)
    - [桥接模式 (Bridge)](#桥接模式-bridge)
    - [组合模式 (Composite)](#组合模式-composite)
    - [装饰器模式 (Decorator)](#装饰器模式-decorator)
    - [外观模式 (Facade)](#外观模式-facade)
    - [享元模式 (Flyweight)](#享元模式-flyweight)
  - [行为型模式](#行为型模式)
    - [观察者模式 (Observer)](#观察者模式-observer)
    - [策略模式 (Strategy)](#策略模式-strategy)
    - [状态模式 (State)](#状态模式-state)
    - [命令模式 (Command)](#命令模式-command)
    - [迭代器模式 (Iterator)](#迭代器模式-iterator)
    - [模板方法模式 (Template Method)](#模板方法模式-template-method)
    - [责任链模式 (Chain of Responsibility)](#责任链模式-chain-of-responsibility)
    - [访问者模式 (Visitor)](#访问者模式-visitor)
  - [并发模式](#并发模式)
    - [读写锁模式 (Read-Write Lock)](#读写锁模式-read-write-lock)
    - [生产者-消费者模式 (Producer-Consumer)](#生产者-消费者模式-producer-consumer)
    - [线程池模式 (Thread Pool)](#线程池模式-thread-pool)
    - [屏障模式 (Barrier)](#屏障模式-barrier)
    - [信号量模式 (Semaphore)](#信号量模式-semaphore)
  - [嵌入式模式](#嵌入式模式)
    - [中断服务例程模式 (ISR)](#中断服务例程模式-isr)
    - [环形缓冲区模式 (Ring Buffer)](#环形缓冲区模式-ring-buffer)
    - [内存池模式 (Memory Pool)](#内存池模式-memory-pool)
    - [设备驱动模式 (Device Driver)](#设备驱动模式-device-driver)
    - [状态机模式 (State Machine)](#状态机模式-state-machine)
    - [看门狗模式 (Watchdog)](#看门狗模式-watchdog)
  - [模式选择指南](#模式选择指南)
    - [按问题类型选择](#按问题类型选择)
    - [模式复杂度分级](#模式复杂度分级)
  - [相关资源](#相关资源)

---


---

## 📑 目录

- [C语言设计模式索引](#c语言设计模式索引)
  - [索引目录](#索引目录)
  - [📑 目录](#-目录)
  - [创建型模式](#创建型模式)
    - [单例模式 (Singleton)](#单例模式-singleton)
    - [工厂模式 (Factory)](#工厂模式-factory)
    - [抽象工厂模式 (Abstract Factory)](#抽象工厂模式-abstract-factory)
    - [建造者模式 (Builder)](#建造者模式-builder)
    - [对象池模式 (Object Pool)](#对象池模式-object-pool)
  - [结构型模式](#结构型模式)
    - [适配器模式 (Adapter)](#适配器模式-adapter)
    - [桥接模式 (Bridge)](#桥接模式-bridge)
    - [组合模式 (Composite)](#组合模式-composite)
    - [装饰器模式 (Decorator)](#装饰器模式-decorator)
    - [外观模式 (Facade)](#外观模式-facade)
    - [享元模式 (Flyweight)](#享元模式-flyweight)
  - [行为型模式](#行为型模式)
    - [观察者模式 (Observer)](#观察者模式-observer)
    - [策略模式 (Strategy)](#策略模式-strategy)
    - [状态模式 (State)](#状态模式-state)
    - [命令模式 (Command)](#命令模式-command)
    - [迭代器模式 (Iterator)](#迭代器模式-iterator)
    - [模板方法模式 (Template Method)](#模板方法模式-template-method)
    - [责任链模式 (Chain of Responsibility)](#责任链模式-chain-of-responsibility)
    - [访问者模式 (Visitor)](#访问者模式-visitor)
  - [并发模式](#并发模式)
    - [读写锁模式 (Read-Write Lock)](#读写锁模式-read-write-lock)
    - [生产者-消费者模式 (Producer-Consumer)](#生产者-消费者模式-producer-consumer)
    - [线程池模式 (Thread Pool)](#线程池模式-thread-pool)
    - [屏障模式 (Barrier)](#屏障模式-barrier)
    - [信号量模式 (Semaphore)](#信号量模式-semaphore)
  - [嵌入式模式](#嵌入式模式)
    - [中断服务例程模式 (ISR)](#中断服务例程模式-isr)
    - [环形缓冲区模式 (Ring Buffer)](#环形缓冲区模式-ring-buffer)
    - [内存池模式 (Memory Pool)](#内存池模式-memory-pool)
    - [设备驱动模式 (Device Driver)](#设备驱动模式-device-driver)
    - [状态机模式 (State Machine)](#状态机模式-state-machine)
    - [看门狗模式 (Watchdog)](#看门狗模式-watchdog)
  - [模式选择指南](#模式选择指南)
    - [按问题类型选择](#按问题类型选择)
    - [模式复杂度分级](#模式复杂度分级)
  - [相关资源](#相关资源)


---

## 创建型模式

创建型模式关注对象的创建机制，提高代码灵活性和可重用性。

### 单例模式 (Singleton)

**描述**: 确保一个类只有一个实例，并提供一个全局访问点
**适用场景**: 全局配置管理器、日志记录器、数据库连接池
**实现要点**: 私有构造函数、静态实例指针、线程安全初始化
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/01_Creational_Patterns.md`

```c
// 核心实现
typedef struct {
    // 实例数据
} Singleton;

Singleton* singleton_get_instance(void);
void singleton_destroy(void);
```

---

### 工厂模式 (Factory)

**描述**: 定义创建对象的接口，由子类决定实例化哪个类
**适用场景**: 需要根据条件创建不同类型对象的场景
**实现要点**: 抽象产品接口、具体产品实现、工厂函数
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/01_Creational_Patterns.md`

```c
// 核心实现
typedef struct Product {
    void (*operation)(struct Product* self);
} Product;

Product* factory_create_product(const char* type);
```

---

### 抽象工厂模式 (Abstract Factory)

**描述**: 创建相关或依赖对象的家族，而无需明确指定具体类
**适用场景**: 跨平台UI组件、数据库访问层
**实现要点**: 抽象工厂接口、具体工厂实现、产品族
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/01_Creational_Patterns.md`

---

### 建造者模式 (Builder)

**描述**: 分步骤构建复杂对象，同样的构建过程可以创建不同的表示
**适用场景**: 复杂对象构建、配置对象组装
**实现要点**: 指挥者、抽象建造者、具体建造者
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/01_Creational_Patterns.md`

---

### 对象池模式 (Object Pool)

**描述**: 预先创建并维护一组可重用的对象，避免频繁创建销毁
**适用场景**: 数据库连接、线程池、内存池
**实现要点**: 池管理器、对象复用、生命周期管理
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/01_Creational_Patterns.md`

---

## 结构型模式

结构型模式关注类和对象的组合，形成更大的结构。

### 适配器模式 (Adapter)

**描述**: 将一个类的接口转换成客户希望的另一个接口
**适用场景**: 集成第三方库、接口兼容、遗留代码复用
**实现要点**: 目标接口、适配器、被适配者
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/02_Structural_Patterns.md`

```c
// 核心实现
typedef struct {
    TargetInterface target;
    Adaptee* adaptee;
} Adapter;

void adapter_request(Adapter* self);
```

---

### 桥接模式 (Bridge)

**描述**: 将抽象部分与实现部分分离，使它们可以独立变化
**适用场景**: 多维度变化、平台无关设计
**实现要点**: 抽象接口、实现接口、组合关系
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/02_Structural_Patterns.md`

---

### 组合模式 (Composite)

**描述**: 将对象组合成树形结构以表示"部分-整体"的层次结构
**适用场景**: 树形结构处理、文件系统、UI组件
**实现要点**: 组件接口、叶节点、复合节点
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/02_Structural_Patterns.md`

---

### 装饰器模式 (Decorator)

**描述**: 动态地给对象添加额外的职责
**适用场景**: 功能扩展、日志记录、性能监控
**实现要点**: 组件接口、具体组件、装饰器基类
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/02_Structural_Patterns.md`

---

### 外观模式 (Facade)

**描述**: 为子系统中的一组接口提供一个统一的高层接口
**适用场景**: 简化复杂系统接口、分层架构
**实现要点**: 外观类、子系统封装
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/02_Structural_Patterns.md`

---

### 享元模式 (Flyweight)

**描述**: 运用共享技术有效地支持大量细粒度的对象
**适用场景**: 大量相似对象、内存优化、缓存
**实现要点**: 享元工厂、内部状态、外部状态
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/02_Structural_Patterns.md`

---

## 行为型模式

行为型模式关注对象之间的通信和职责分配。

### 观察者模式 (Observer)

**描述**: 定义对象间的一对多依赖关系，当一个对象状态改变时，所有依赖者都会收到通知
**适用场景**: 事件处理、MVC模式、消息订阅
**实现要点**: 主题接口、观察者接口、通知机制
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

```c
// 核心实现
typedef struct Observer {
    void (*update)(struct Observer* self, void* data);
} Observer;

typedef struct Subject {
    Observer* observers[MAX_OBSERVERS];
    void (*attach)(struct Subject* self, Observer* obs);
    void (*notify)(struct Subject* self, void* data);
} Subject;
```

---

### 策略模式 (Strategy)

**描述**: 定义一系列算法，将它们封装起来，并且使它们可以互相替换
**适用场景**: 算法切换、支付方式选择、排序算法选择
**实现要点**: 策略接口、具体策略、上下文
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

### 状态模式 (State)

**描述**: 允许对象在其内部状态改变时改变它的行为
**适用场景**: 状态机实现、游戏角色状态、订单状态
**实现要点**: 状态接口、具体状态、上下文
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

### 命令模式 (Command)

**描述**: 将请求封装为对象，从而可以用不同的请求、队列或日志来参数化其他对象
**适用场景**: 撤销重做、任务队列、宏命令
**实现要点**: 命令接口、具体命令、调用者、接收者
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

### 迭代器模式 (Iterator)

**描述**: 提供一种方法顺序访问聚合对象中的各个元素
**适用场景**: 集合遍历、隐藏内部表示
**实现要点**: 迭代器接口、具体迭代器、聚合接口
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

### 模板方法模式 (Template Method)

**描述**: 定义算法的骨架，将某些步骤延迟到子类中实现
**适用场景**: 算法框架、流程控制、钩子方法
**实现要点**: 抽象类、模板方法、具体实现
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

### 责任链模式 (Chain of Responsibility)

**描述**: 使多个对象都有机会处理请求，从而避免请求的发送者和接收者之间的耦合
**适用场景**: 请求处理链、过滤器链、异常处理
**实现要点**: 处理者接口、链式结构、传递机制
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

### 访问者模式 (Visitor)

**描述**: 表示一个作用于某对象结构中的各元素的操作，可以在不改变各元素类的前提下定义新操作
**适用场景**: AST遍历、编译器、复杂对象结构操作
**实现要点**: 访问者接口、元素接口、双重分发
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/03_Behavioral_Patterns.md`

---

## 并发模式

并发模式关注多线程/多进程环境下的设计。

### 读写锁模式 (Read-Write Lock)

**描述**: 允许多个读操作同时进行，但写操作互斥
**适用场景**: 读多写少的场景、缓存系统
**实现要点**: 读计数、写锁、等待队列
**文档位置**: `knowledge/07_Modern_Toolchain/07_Concurrency_Parallelism/`

```c
// 核心实现
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t read_cond;
    pthread_cond_t write_cond;
    int readers;
    int writers_waiting;
    int writing;
} RWLock;
```

---

### 生产者-消费者模式 (Producer-Consumer)

**描述**: 解耦数据生产和消费，通过缓冲区平衡速度差异
**适用场景**: 任务队列、数据流处理、日志系统
**实现要点**: 共享缓冲区、同步机制、阻塞/非阻塞
**文档位置**: `knowledge/07_Modern_Toolchain/07_Concurrency_Parallelism/`

---

### 线程池模式 (Thread Pool)

**描述**: 维护一组工作线程，复用线程执行多个任务
**适用场景**: 高并发服务器、批量任务处理
**实现要点**: 任务队列、工作线程、线程同步
**文档位置**: `knowledge/07_Modern_Toolchain/07_Concurrency_Parallelism/05_Thread_Pool_Patterns.md`

---

### 屏障模式 (Barrier)

**描述**: 使一组线程到达屏障时被阻塞，直到所有线程都到达
**适用场景**: 并行计算、分阶段算法
**实现要点**: 计数器、互斥锁、条件变量
**文档位置**: `knowledge/07_Modern_Toolchain/07_Concurrency_Parallelism/`

---

### 信号量模式 (Semaphore)

**描述**: 控制同时访问特定资源的线程数量
**适用场景**: 资源池控制、限流、同步
**实现要点**: 计数信号量、等待队列、P/V操作
**文档位置**: `knowledge/07_Modern_Toolchain/07_Concurrency_Parallelism/`

---

## 嵌入式模式

嵌入式模式针对资源受限和硬件交互场景。

### 中断服务例程模式 (ISR)

**描述**: 响应硬件中断的快速处理机制
**适用场景**: 硬件事件响应、实时系统
**实现要点**: 短小精悍、快速响应、上下文保存
**文档位置**: `knowledge/01_Core_Knowledge_System/08_Application_Domains/02_Embedded_Systems.md`

```c
// 核心实现
void __attribute__((interrupt)) timer_isr(void) {
    // 保存上下文
    // 快速处理
    // 恢复上下文
}
```

---

### 环形缓冲区模式 (Ring Buffer)

**描述**: 固定大小的循环缓冲区，用于数据流缓冲
**适用场景**: 串口通信、DMA传输、音频处理
**实现要点**: 头尾指针、模运算、线程安全
**文档位置**: `knowledge/01_Core_Knowledge_System/08_Application_Domains/02_Embedded_Systems.md`

---

### 内存池模式 (Memory Pool)

**描述**: 预先分配固定大小的内存块，避免动态分配开销
**适用场景**: 实时系统、无碎片内存管理
**实现要点**: 固定块大小、空闲链表、快速分配
**文档位置**: `knowledge/07_Modern_Toolchain/08_Design_Patterns/01_Creational_Patterns.md`

---

### 设备驱动模式 (Device Driver)

**描述**: 封装硬件访问，提供统一接口
**适用场景**: 硬件抽象、驱动开发
**实现要点**: 寄存器映射、中断处理、电源管理
**文档位置**: `knowledge/01_Core_Knowledge_System/08_Application_Domains/02_Embedded_Systems.md`

---

### 状态机模式 (State Machine)

**描述**: 通过状态转换处理离散事件
**适用场景**: 协议解析、控制逻辑、嵌入式控制
**实现要点**: 状态表、事件处理、转换条件
**文档位置**: `knowledge/07_Modern_Toolchain/11_Workflow_StateMachines/01_State_Machine_Patterns.md`

---

### 看门狗模式 (Watchdog)

**描述**: 监控系统运行状态，异常时复位系统
**适用场景**: 系统可靠性、故障恢复
**实现要点**: 定时器、喂狗机制、超时处理
**文档位置**: `knowledge/01_Core_Knowledge_System/08_Application_Domains/02_Embedded_Systems.md`

---

## 模式选择指南

### 按问题类型选择

| 问题类型 | 推荐模式 |
|----------|----------|
| 需要唯一实例 | 单例模式 |
| 对象创建复杂 | 工厂/建造者模式 |
| 接口不兼容 | 适配器模式 |
| 需要动态扩展功能 | 装饰器模式 |
| 状态切换频繁 | 状态模式 |
| 需要事件通知 | 观察者模式 |
| 算法需要切换 | 策略模式 |
| 并发访问共享数据 | 读写锁模式 |
| 生产消费速率不匹配 | 生产者-消费者模式 |
| 资源受限环境 | 内存池/环形缓冲区模式 |

### 模式复杂度分级

| 级别 | 模式 |
|------|------|
| ⭐ | 单例、工厂、适配器、迭代器 |
| ⭐⭐ | 策略、状态、观察者、装饰器、外观 |
| ⭐⭐⭐ | 抽象工厂、桥接、组合、模板方法、命令 |
| ⭐⭐⭐⭐ | 访问者、责任链、享元 |
| ⭐⭐⭐⭐⭐ | 并发模式（需理解线程安全） |

---

## 相关资源

- [GoF设计模式](https://en.wikipedia.org/wiki/Design_Patterns)
- [C语言设计模式实践](../../07_Modern_Toolchain/08_Design_Patterns/README.md)
- [并发编程指南](../../07_Modern_Toolchain/07_Concurrency_Parallelism/README.md)
- [嵌入式系统设计](../../01_Core_Knowledge_System/08_Application_Domains/02_Embedded_Systems.md)

---

> **状态**: ✅ 已完成


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
