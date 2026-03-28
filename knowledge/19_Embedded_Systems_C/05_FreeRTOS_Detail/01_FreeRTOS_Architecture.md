# FreeRTOS 架构深度解析

> **版本**: FreeRTOS V10.5+ / FreeRTOS Kernel V11.0+
> **许可证**: MIT (开源)
> **目标平台**: 35+ 处理器架构
> **最后更新**: 2026-03-29

---

## 1. 核心概念形式化定义

### 1.1 FreeRTOS 架构层次

```
┌─────────────────────────────────────────────────────────────────┐
│                 FreeRTOS 架构层次                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │  应用层 (Application Layer)                              │   │
│  │  ├── 用户任务 (User Tasks)                               │   │
│  │  ├── 中断服务程序 (ISRs)                                 │   │
│  │  └── 应用代码 (Application Code)                         │   │
│  └─────────────────────────────────────────────────────────┘   │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────────────┐     │
│  │  API层 (Kernel API)                                     │     │
│  │  ├── 任务管理 (Task Management)                         │     │
│  │  │   └── xTaskCreate, xTaskDelete, vTaskDelay...       │     │
│  │  ├── 队列/信号量 (Queues/Semaphores)                    │     │
│  │  │   └── xQueueSend, xSemaphoreTake...                  │     │
│  │  ├── 内存管理 (Memory Management)                       │     │
│  │  │   └── pvPortMalloc, vPortFree...                     │     │
│  │  ├── 定时器 (Software Timers)                           │     │
│  │  │   └── xTimerStart, xTimerStop...                     │     │
│  │  └── 中断管理 (Interrupt Management)                    │     │
│  │      └── portYIELD_FROM_ISR...                          │     │
│  └─────────────────────────────────────────────────────────┘     │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────────────┐     │
│  │  内核层 (Kernel Core)                                   │     │
│  │  ├── 调度器 (Scheduler)                                 │     │
│  │  │   ├── vTaskSwitchContext() - 上下文切换              │     │
│  │  │   ├── prvSelectHighestPriorityTask() - 任务选择     │     │
│  │  │   └── Tick计数管理                                   │     │
│  │  ├── 任务控制块 (TCB - Task Control Block)              │     │
│  │  │   └── 任务状态、优先级、栈指针等                     │     │
│  │  ├── 就绪列表 (Ready Lists)                             │     │
│  │  │   └── 按优先级组织的双向链表                         │     │
│  │  └── 阻塞列表 (Delayed Lists)                           │     │
│  │      └── 按唤醒时间排序的任务                           │     │
│  └─────────────────────────────────────────────────────────┘     │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────────────┐     │
│  │  移植层 (Port Layer) - 硬件相关                         │     │
│  │  ├── 上下文切换 (Context Switch)                        │     │
│  │  │   └── portSAVE_CONTEXT, portRESTORE_CONTEXT          │     │
│  │  ├── 中断控制 (Interrupt Control)                       │     │
│  │  │   └── portDISABLE_INTERRUPTS...                      │     │
│  │  ├── Tick定时器 (Tick Timer)                            │     │
│  │  │   └── 配置SysTick或定时器中断                        │     │
│  │  └── 内存屏障 (Memory Barriers)                         │     │
│  │      └── 多核/乱序执行支持                              │     │
│  └─────────────────────────────────────────────────────────┘     │
│                          │                                      │
│  ┌───────────────────────▼───────────────────────────────┐     │
│  │  硬件抽象层 (HAL/MCU)                                     │     │
│  │  └── 具体微控制器硬件 (ARM Cortex-M, RISC-V, etc.)       │     │
│  └─────────────────────────────────────────────────────────┘     │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 1.2 任务状态机形式化定义

**定义 1.1 (任务状态)**

```
TaskState ::= RUNNING | READY | BLOCKED | SUSPENDED | DELETED

状态转换函数:
  δ: TaskState × Event → TaskState

具体转换:
  δ(RUNNING, TimeSliceEnd) = READY          (时间片用完)
  δ(RUNNING, vTaskDelay) = BLOCKED          (主动阻塞)
  δ(RUNNING, vTaskSuspend) = SUSPENDED      (挂起)
  δ(RUNNING, vTaskDelete) = DELETED         (删除)

  δ(READY, vTaskSwitchContext) = RUNNING    (被调度)

  δ(BLOCKED, Timeout/ResourceAvailable) = READY  (唤醒)
  δ(BLOCKED, vTaskSuspend) = SUSPENDED      (挂起阻塞任务)

  δ(SUSPENDED, vTaskResume) = READY         (恢复)

状态约束:
  • 任意时刻只有一个任务处于RUNNING (单核)
  • BLOCKED任务必须有超时或等待条件
  • SUSPENDED任务不消耗CPU时间
```

### 1.3 任务状态转换图

```
                    ┌─────────────┐
                    │   创建任务   │
                    └──────┬──────┘
                           │
                           ▼
┌──────────────────────────────────────────────────────────────┐
│                      ┌─────────┐                             │
│           ┌─────────│  READY  │◄────────────────────┐       │
│           │         └────┬────┘                     │       │
│           │              │ vTaskSwitchContext()     │       │
│           │              ▼                          │       │
│           │         ┌─────────┐   时间片到/       │       │
│  vTaskResume()      │ RUNNING │   高优先级就绪     │       │
│           │         └────┬────┘───────────────────┘       │
│           │              │                                  │
│           │    ┌─────────┼─────────┬──────────┐            │
│           │    │         │         │          │            │
│           │    ▼         ▼         ▼          ▼            │
│           │ ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐       │
│           └─│BLOCKED│  │SUSPENDED│  │DELETED│  │       │
│             └──┬───┘  └──┬───┘  └──────┘            │
│                │         │                           │
│                │         │                           │
│         超时/   │   vTaskResume()                   │
│       资源可用   │                                   │
│                │                                    │
└──────────────────────────────────────────────────────────────┘
```

---

## 2. 任务调度算法形式化分析

### 2.1 调度策略矩阵

| 调度策略 | 配置宏 | 时间复杂度 | 适用场景 | 特点 |
|----------|--------|------------|----------|------|
| **抢占式** | configUSE_PREEMPTION=1 | O(1) | 实时应用 | 高优先级立即抢占 |
| **时间片** | configUSE_TIME_SLICING=1 | O(1) | 同优先级公平 | 轮流执行 |
| **合作式** | configUSE_PREEMPTION=0 | O(1) | 低功耗 | 任务主动让出 |

### 2.2 优先级调度定理

**定理 2.1 (最高优先级就绪任务先运行)**

```
定理: 在抢占式调度下，当前运行的任务总是最高优先级的就绪任务

证明:
  设就绪列表 R = {T₁, T₂, ..., Tₙ}，每个任务有优先级 P(Tᵢ)

  1. 调度器维护 pxCurrentTCB 指向当前运行任务
  2. 每次Tick中断或任务让出时调用 vTaskSwitchContext()
  3. 调度器选择: T_selected = argmax{P(T) | T ∈ ReadyList}
  4. 如果 P(T_selected) > P(pxCurrentTCB)，则发生上下文切换
  5. 因此，运行任务总是最高优先级就绪任务

证毕。
```

### 2.3 调度时机决策树

```
上下文切换触发时机
│
├── 系统Tick中断 (周期性)
│   ├── 检查时间片轮转 (同优先级任务)
│   └── 检查阻塞任务超时
│       └── 超时任务移到就绪列表 → 可能触发切换
│
├── 任务主动让出
│   ├── vTaskDelay() / vTaskDelayUntil()
│   │   └── 当前任务阻塞，调度下一个任务
│   ├── taskYIELD() / portYIELD()
│   │   └── 显式请求上下文切换
│   └── vTaskSuspend()
│       └── 挂起自己，调度下一个任务
│
├── 同步原语操作
│   ├── xQueueSend() / xQueueReceive()
│   │   └── 如果更高优先级任务被唤醒 → 切换
│   ├── xSemaphoreGive() / xSemaphoreTake()
│   │   └── 同上
│   └── xEventGroupSetBits()
│       └── 同上
│
└── 中断服务程序
    └── portYIELD_FROM_ISR(xHigherPriorityTaskWoken)
        └── 如果唤醒了更高优先级任务 → 切换
```

---

## 3. 任务控制块 (TCB) 数据结构

### 3.1 TCB 形式化定义

```c
/* 任务控制块核心结构 */
typedef struct tskTaskControlBlock {
    /* 堆栈相关 */
    volatile StackType_t *pxTopOfStack;     /* 栈顶指针 */
    StackType_t *pxStack;                    /* 栈基地址 */
    configSTACK_DEPTH_TYPE usStackDepth;     /* 栈深度 */

    /* 状态链表节点 */
    ListItem_t xStateListItem;               /* 状态列表项 */
    ListItem_t xEventListItem;               /* 事件列表项 */

    /* 优先级 */
    UBaseType_t uxPriority;                  /* 当前优先级 */
    UBaseType_t uxBasePriority;              /* 基础优先级 (用于互斥量优先级继承) */

    /* 运行时统计 */
    uint32_t ulRunTimeCounter;               /* 运行时间计数 */

    /* 任务名称 */
    char pcTaskName[configMAX_TASK_NAME_LEN];

    /* 其他字段 (根据配置) */
    #if (configUSE_MUTEXES == 1)
        UBaseType_t uxMutexesHeld;           /* 持有的互斥量数量 */
    #endif

    #if (configUSE_TASK_NOTIFICATIONS == 1)
        volatile uint32_t ulNotifiedValue;   /* 通知值 */
        volatile uint8_t ucNotifyState;      /* 通知状态 */
    #endif

} tskTCB;

/* 简化版TCB内存布局:
 * ┌──────────────────────┐
 * │ pxTopOfStack         │ ─┐
 * │ pxStack              │  │
 * │ usStackDepth         │  │
 * │ ...                  │  │  固定部分
 * │ pcTaskName           │ ─┘
 * ├──────────────────────┤
 * │ xStateListItem       │ ─┐
 * │ xEventListItem       │  │  链表节点 (双向链表)
 * │                      │ ─┘
 * └──────────────────────┘
 */
```

### 3.2 就绪列表组织

```
┌─────────────────────────────────────────────────────────────────┐
│              FreeRTOS 就绪列表组织 (按优先级)                    │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  定义:                                                          │
│  pxReadyTasksLists[configMAX_PRIORITIES]                        │
│                                                                 │
│  每个优先级一个列表:                                            │
│  ┌──────────┬──────────────────────────────────────────┐       │
│  │ 优先级   │ 就绪任务列表                              │       │
│  ├──────────┼──────────────────────────────────────────┤       │
│  │  31      │ T5 → T8 (同优先级，循环执行)              │       │
│  │  30      │ T3                                        │       │
│  │  ...     │ (空)                                      │       │
│  │   2      │ T1 → T2                                   │       │
│  │   1      │ IDLE任务 (最低优先级，始终存在)           │       │
│  │   0      │ (保留/未使用)                             │       │
│  └──────────┴──────────────────────────────────────────┘       │
│                                                                 │
│  调度器查找最高优先级就绪任务:                                   │
│  1. 使用 uxTopReadyPriority 记录最高就绪优先级                  │
│  2. 直接访问 pxReadyTasksLists[uxTopReadyPriority]              │
│  3. 时间复杂度: O(1)                                            │
│                                                                 │
│  优化: 某些架构使用硬件CLZ指令加速查找                           │
│  uxTopReadyPriority = 31 - CLZ(uxReadyPrioritiesBitmask);       │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 4. 上下文切换形式化分析

### 4.1 上下文切换触发点

```c
/* 上下文切换核心逻辑 */
void vTaskSwitchContext(void) {
    /* 1. 检查调度器是否挂起 */
    if (uxSchedulerSuspended != (UBaseType_t)pdFALSE) {
        xYieldPending = pdTRUE;
        return;
    }

    /* 2. 查找最高优先级就绪任务 */
    taskSELECT_HIGHEST_PRIORITY_TASK();

    /* 3. 如果与当前任务不同，执行上下文切换 */
    if (pxCurrentTCB != pxPreviousTCB) {
        traceTASK_SWITCHED_OUT();
        /* 实际切换在退出临界区或中断返回时执行 */
    }
}

/* 选择最高优先级任务的宏 */
#define taskSELECT_HIGHEST_PRIORITY_TASK() \
{ \
    UBaseType_t uxTopPriority = uxTopReadyPriority; \
    listGET_OWNER_OF_NEXT_ENTRY(pxCurrentTCB, \
        &(pxReadyTasksLists[uxTopPriority])); \
}
```

### 4.2 上下文保存/恢复

```
┌─────────────────────────────────────────────────────────────────┐
│              上下文切换过程 (ARM Cortex-M为例)                   │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  上下文 = CPU寄存器状态 + 栈状态                                │
│                                                                 │
│  保存上下文 (任务A → 任务B):                                    │
│  1. 触发PendSV异常 (最低优先级，确保其他中断完成)               │
│  2. 硬件自动保存: R0-R3, R12, LR, PC, xPSR                    │
│  3. 软件保存: R4-R11 (通过PendSV_Handler)                      │
│  4. 保存栈指针: pxCurrentTCB->pxTopOfStack = PSP              │
│  5. 更新当前TCB指针: pxCurrentTCB = pxNextTCB                 │
│  6. 恢复栈指针: PSP = pxCurrentTCB->pxTopOfStack              │
│  7. 软件恢复: R4-R11                                          │
│  8. 异常返回，硬件恢复R0-R3, R12, LR, PC, xPSR                │
│  9. 任务B从保存的PC处继续执行                                  │
│                                                                 │
│  时间开销: 约 12-20 个时钟周期 (取决于架构)                    │
│  内存开销: 每个任务独立的栈空间 (通常几百到几千字节)           │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 5. 内存管理策略

### 5.1 堆管理算法对比矩阵

| 算法 | 文件 | 碎片化 | 确定性 | 适用场景 |
|------|------|--------|--------|----------|
| **Heap_1** | heap_1.c | 无 | O(1) | 只分配不释放 |
| **Heap_2** | heap_2.c | 严重 | O(n) | 不推荐 (遗留) |
| **Heap_3** | heap_3.c | 依赖libc | 依赖libc | 使用标准库malloc |
| **Heap_4** | heap_4.c | 轻度 | O(n) | 推荐 (最佳合并) |
| **Heap_5** | heap_5.c | 轻度 | O(n) | 跨多个内存区域 |

### 5.2 Heap_4 算法分析

```
┌─────────────────────────────────────────────────────────────────┐
│              Heap_4 首次适应 + 内存合并算法                      │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  内存块结构:                                                    │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │ BlockSize_t xBlockSize (包含分配标志位)                  │   │
│  │ void *pxNextFreeBlock (指向下一个空闲块)                 │   │
│  │ uint8_t data[...] (用户数据或空闲空间)                   │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                                 │
│  分配流程:                                                      │
│  1. 遍历空闲链表，找第一个足够大的块 (首次适应)                 │
│  2. 如果块大小 > 请求大小 + 最小块大小，分割块                  │
│  3. 标记块为已分配 (设置最高位)                                 │
│  4. 返回数据区指针                                              │
│                                                                 │
│  释放流程:                                                      │
│  1. 标记块为空闲 (清除最高位)                                   │
│  2. 检查物理相邻块是否空闲                                      │
│  3. 如果相邻块空闲，合并成一个大块                              │
│  4. 插入空闲链表 (按地址排序，便于后续合并)                     │
│                                                                 │
│  合并示例:                                                      │
│  分配前: [空闲:100B] → [已分配:50B] → [空闲:100B]             │
│  释放后: [空闲:250B] (三块合并)                                │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

---

## 6. 配置决策树

```
FreeRTOS配置决策
│
├── 调度策略选择
│   ├── 需要实时响应? → configUSE_PREEMPTION=1
│   └── 低功耗优先? → configUSE_PREEMPTION=0 (合作式)
│
├── 时间片轮转
│   ├── 同优先级任务多? → configUSE_TIME_SLICING=1
│   └── 任务数少? → configUSE_TIME_SLICING=0
│
├── 内存管理
│   ├── 只分配不释放? → Heap_1
│   ├── 通用场景? → Heap_4
│   └── 多块内存? → Heap_5
│
├── 内核特性
│   ├── 需要互斥量? → configUSE_MUTEXES=1
│   ├── 需要递归互斥量? → configUSE_RECURSIVE_MUTEXES=1
│   ├── 需要计数信号量? → configUSE_COUNTING_SEMAPHORES=1
│   ├── 需要软件定时器? → configUSE_TIMERS=1
│   └── 需要任务通知? → configUSE_TASK_NOTIFICATIONS=1
│
└── 调试辅助
    ├── 运行时统计? → configGENERATE_RUN_TIME_STATS=1
    └── 跟踪调试? → configUSE_TRACE_FACILITY=1
```

---

## 7. 参考资源

- **FreeRTOS Reference Manual**: 官方参考手册
- **Mastering the FreeRTOS Real Time Kernel**: 官方书籍
- **FreeRTOS Source Code**: 源码是最好的文档
- **ARM Cortex-M3/M4 Port**: 移植层实现参考
