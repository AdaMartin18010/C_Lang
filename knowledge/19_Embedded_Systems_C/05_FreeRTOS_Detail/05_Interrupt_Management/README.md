# 嵌入式中断管理深度解析

> **核心挑战**: 延迟确定性、上下文保存、优先级管理
> **关键指标**: 中断延迟、上下文切换开销、抖动
> **架构类型**: NVIC (Cortex-M)、VIC (ARM7)、PLIC (RISC-V)
> **最后更新**: 2026-03-29

---

## 1. 中断架构形式化定义

### 1.1 Cortex-M NVIC架构

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                       Cortex-M NVIC架构                                       │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│  ┌─────────────────────────────────────────────────────────────────────┐   │
│  │                         处理器核心                                   │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                    执行状态                                  │   │   │
│  │  │  ┌──────────┐  ┌──────────┐  ┌──────────┐                 │   │   │
│  │  │  │ Thread   │  │ Handler  │  │  异常     │                 │   │   │
│  │  │  │ Mode     │←→│ Mode     │←→│ Return   │                 │   │   │
│  │  │  │ (PSP)    │  │ (MSP)    │  │          │                 │   │   │
│  │  │  └──────────┘  └──────────┘  └──────────┘                 │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  │                              │                                      │   │
│  │                              │ 异常/中断进入                        │   │
│  │                              ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │              嵌套向量中断控制器 (NVIC)                       │   │   │
│  │  │                                                             │   │   │
│  │  │  ┌─────────────────────────────────────────────────────┐   │   │   │
│  │  │  │              中断控制器核心                         │   │   │   │
│  │  │  │  ┌──────────┐  ┌──────────┐  ┌──────────┐         │   │   │   │
│  │  │  │  │ IRQ0     │  │ IRQ1     │  │ ...      │         │   │   │   │
│  │  │  │  │ Priority │  │ Priority │  │          │         │   │   │   │
│  │  │  │  │ Config   │  │ Config   │  │          │         │   │   │   │
│  │  │  │  └──────────┘  └──────────┘  └──────────┘         │   │   │   │
│  │  │  │                                                     │   │   │   │
│  │  │  │  ┌─────────────────────────────────────────────┐   │   │   │   │
│  │  │  │  │  优先级分组 (PRIGROUP)                      │   │   │   │   │
│  │  │  │  │  • 抢占优先级位数 (Preempt)                 │   │   │   │   │
│  │  │  │  │  • 子优先级位数 (Subpriority)               │   │   │   │   │
│  │  │  │  └─────────────────────────────────────────────┘   │   │   │   │
│  │  │  └─────────────────────────────────────────────────────┘   │   │   │
│  │  │                              │                              │   │   │
│  │  │                              │ 最高优先级 pending IRQ       │   │   │
│  │  │                              ▼                              │   │   │
│  │  │  ┌─────────────────────────────────────────────────────┐   │   │   │
│  │  │  │  向量表 (Vector Table)                              │   │   │   │
│  │  │  │  ┌─────────────────────────────────────────────┐   │   │   │   │
│  │  │  │  │  0x0000_0000: 初始MSP值                     │   │   │   │   │
│  │  │  │  │  0x0000_0004: 复位向量                      │   │   │   │   │
│  │  │  │  │  0x0000_0008: NMI                           │   │   │   │   │
│  │  │  │  │  0x0000_000C: HardFault                     │   │   │   │   │
│  │  │  │  │  ...                                        │   │   │   │   │
│  │  │  │  │  0x0000_0040: IRQ0 向量                     │   │   │   │   │
│  │  │  │  │  0x0000_0044: IRQ1 向量                     │   │   │   │   │
│  │  │  │  │  ...                                        │   │   │   │   │
│  │  │  │  └─────────────────────────────────────────────┘   │   │   │   │
│  │  │  └─────────────────────────────────────────────────────┘   │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  │                              │                                      │   │
│  │                              │ 向量取指                             │   │
│  │                              ▼                                      │   │
│  │  ┌─────────────────────────────────────────────────────────────┐   │   │
│  │  │                 ISR 执行                                    │   │   │
│  │  └─────────────────────────────────────────────────────────────┘   │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 1.2 中断延迟公式

**定义 1.1 (中断延迟)**

```
中断延迟 = 同步延迟 + 压栈时间 + 向量取指时间 + ISR开始执行

同步延迟:
  等待当前指令完成的最长时间
  Cortex-M: 最长12个时钟周期 (LDM指令)

压栈时间:
  保存寄存器到堆栈
  Cortex-M: 12个寄存器 × 1周期 = 12周期
  (R0-R3, R12, LR, PC, xPSR)

向量取指:
  从向量表读取ISR地址
  通常1-2周期 (Flash等待状态影响)

总延迟:
  最小延迟 = 12周期 (同步) + 12周期 (压栈) + 2周期 (取指)
           = 26周期 @ 168MHz = 155ns

  典型延迟 = 36-48周期 @ 168MHz = 215-286ns
```

---

## 2. 中断优先级管理

### 2.1 优先级分组决策矩阵

```
Cortex-M优先级分组配置

NVIC支持 8-bit 优先级 (理论256级)
实际实现通常使用 4-bit (16级)

优先级分组 (PRIGROUP):
  定义抢占优先级和子优先级的位数分配

┌─────────────────────────────────────────────────────────────────────────────┐
│  分组    抢占优先级位数    子优先级位数    抢占级别    子级别    说明       │
├─────────────────────────────────────────────────────────────────────────────┤
│   0         0               4             1         16       无抢占        │
│   1         1               3             2          8       2级抢占       │
│   2         2               2             4          4       4级抢占(常用)  │
│   3         3               1             8          2       8级抢占       │
│   4         4               0            16          1       纯抢占        │
│  5-7      (保留)                                                          │
└─────────────────────────────────────────────────────────────────────────────┘

抢占优先级 vs 子优先级:
  - 抢占优先级: 可打断正在执行的同优先级或低优先级ISR
  - 子优先级: 仅在同抢占优先级内决定响应顺序，不打断

示例配置 (分组2):
  优先级值 = [抢占:2bit][子:2bit]

  优先级15 (0x0F): 抢占=3, 子=3 (最低)
  优先级14 (0x0E): 抢占=3, 子=2
  优先级12 (0x0C): 抢占=3, 子=0
  优先级11 (0x0B): 抢占=2, 子=3 (可打断抢占=3的ISR)
  ...
  优先级0  (0x00): 抢占=0, 子=0 (最高)
```

### 2.2 优先级反转问题

```
中断优先级反转场景

场景: 低优先级中断中访问共享资源，高优先级中断也需要该资源

时间线:
  t0 ──┬── 低优先级ISR_L开始执行
       │   获取互斥锁 M
       │
  t1 ──┼── 高优先级ISR_H准备执行 (pending)
       │   但由于优先级机制，不能打断ISR_L
       │   (不同抢占优先级可打断，同优先级需等待)
       │
  t2 ──┼── 中优先级中断ISR_M发生
       │   抢占优先级高于ISR_L
       │   打断ISR_L开始执行
       │
  t3 ──┼── ISR_M执行中...
       │   ISR_H继续pending等待
       │   ISR_L持有锁被阻塞
       │
       │   ★ 优先级反转: ISR_M (中) 延迟了 ISR_H (高)
       │
  t4 ──┼── ISR_M完成
       │   ISR_L恢复，完成临界区，释放锁
       │
  t5 ──┼── ISR_H终于可以执行
       │
  t6 ──┴── ISR_H完成

解决方案:
  1. 提升临界区代码优先级 (临时)
  2. 使用尾链机制减少延迟
  3. 最小化临界区时间
  4. 避免在ISR中使用锁 (推荐)
```

---

## 3. 上下文切换机制

### 3.1 异常入口/出口形式化定义

**定义 3.1 (异常处理)**

```
异常入口 (Entry Sequence):

1. 压栈 (由硬件自动完成):
   SP = SP - 32
   Mem[SP+28] = R0
   Mem[SP+24] = R1
   Mem[SP+20] = R2
   Mem[SP+16] = R3
   Mem[SP+12] = R12
   Mem[SP+8]  = LR (返回地址)
   Mem[SP+4]  = PC (返回地址+1)
   Mem[SP]    = xPSR

2. 向量取指:
   PC = VectorTable[ExceptionNumber × 4]

3. 状态更新:
   LR = EXC_RETURN (指示使用哪个堆栈)
   IPSR = ExceptionNumber
   PSR = 0 (清除EPSR)

异常出口 (Exit Sequence):

1. 触发退出 (软件):
   BX LR (LR包含EXC_RETURN)
   或
   LDMIA SP!, {..., PC}^ (带^表示恢复PSR)

2. 出栈 (由硬件完成):
   从SP位置恢复R0-R3, R12, LR, PC, xPSR
   SP = SP + 32

3. 恢复执行:
   PC = 恢复的PC值
   继续被中断的代码

EXC_RETURN值:
  0xFFFFFFF1: 返回Handler模式，使用MSP
  0xFFFFFFF9: 返回Thread模式，使用MSP
  0xFFFFFFFD: 返回Thread模式，使用PSP
```

### 3.2 上下文保存决策树

```
ISR上下文保存策略
│
├── 仅使用压栈寄存器?
│   ├── 是 (R0-R3, R12, LR, PC, xPSR已保存)
│   │   └── 无需额外压栈
│   │       └── 最快 (12周期)
│   │
│   └── 否 (使用其他寄存器)
│       ├── 使用R4-R11 (被调用者保存)
│       │   └── 手动压栈
│       │       PUSH {R4-R11, LR}
│       │       (额外10周期)
│       │
│       └── 使用FPU寄存器 (如果FPU使能)
│           └── 惰性压栈 (Lazy Stacking)
│               └── 默认只保存FPU上下文标志
│               └── 实际使用FPU时才保存寄存器
│
├── 嵌套中断?
│   ├── 是
│   │   └── 使用MSP (Main Stack)
│   │       └── 所有异常共享MSP
│   │
│   └── 否
│       └── 根据LR选择
│           └── Thread模式使用PSP
│
└── 使用RTOS?
    ├── 是
    │   └── PendSV处理上下文切换
    │       └── 保存完整任务上下文
    │       └── 16个寄存器 + FPU (如果使能)
    │
    └── 否
        └── 裸机ISR
            └── 最小上下文保存
```

---

## 4. 中断与RTOS集成

### 4.1 FreeRTOS中断安全API

```c
/* FreeRTOS中断安全API形式化定义 */

/* 从ISR调用时使用的API后缀: FromISR */

/* 队列操作 */
BaseType_t xQueueSendFromISR(
    QueueHandle_t xQueue,
    const void *pvItemToQueue,
    BaseType_t *pxHigherPriorityTaskWoken
);

BaseType_t xQueueReceiveFromISR(
    QueueHandle_t xQueue,
    void *pvBuffer,
    BaseType_t *pxHigherPriorityTaskWoken
);

/* 信号量操作 */
BaseType_t xSemaphoreGiveFromISR(
    SemaphoreHandle_t xSemaphore,
    BaseType_t *pxHigherPriorityTaskWoken
);

BaseType_t xSemaphoreTakeFromISR(
    SemaphoreHandle_t xSemaphore,
    BaseType_t *pxHigherPriorityTaskWoken
);

/* 任务通知 */
BaseType_t xTaskNotifyFromISR(
    TaskHandle_t xTaskToNotify,
    uint32_t ulValue,
    eNotifyAction eAction,
    BaseType_t *pxHigherPriorityTaskWoken
);

/* 上下文切换触发 */
void portYIELD_FROM_ISR(BaseType_t xHigherPriorityTaskWoken);

/* pxHigherPriorityTaskWoken机制:
 *
 * 初始值: pdFALSE
 *
 * 如果API操作导致更高优先级任务就绪:
 *   *pxHigherPriorityTaskWoken = pdTRUE
 *
 * ISR退出前检查:
 *   if (*pxHigherPriorityTaskWoken == pdTRUE):
 *       触发上下文切换
 *
 * 延迟到ISR末尾切换的原因:
 *   - 避免多次不必要的切换
 *   - 批量处理完成后再决策
 */

/* 完整ISR示例 */
void TIM_IRQHandler(void) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    uint32_t captured_value;

    /* 读取捕获值 */
    captured_value = TIM2->CCR1;

    /* 发送到队列 (中断安全) */
    xQueueSendFromISR(
        xCaptureQueue,
        &captured_value,
        &xHigherPriorityTaskWoken
    );

    /* 清除中断标志 */
    TIM2->SR &= ~TIM_SR_CC1IF;

    /* 上下文切换 (如果需要) */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}
```

### 4.2 中断安全临界区

```c
/* FreeRTOS临界区管理 */

/* 任务级临界区 (使用调度器) */
taskENTER_CRITICAL();
/* 临界区代码 - 禁止任务切换 */
taskEXIT_CRITICAL();

/* 中断级临界区 (使用BASEPRI) */
taskENTER_CRITICAL_FROM_ISR();
/* 临界区代码 - 屏蔽低于阈值的中断 */
taskEXIT_CRITICAL_FROM_ISR(xSavedInterruptStatus);

/* BASEPRI机制详解 (Cortex-M):
 *
 * BASEPRI寄存器控制可响应的最小优先级
 * 值越大优先级越低
 *
 * 示例: configMAX_SYSCALL_INTERRUPT_PRIORITY = 5
 *       BASEPRI = 5
 *       优先级0-4的中断可响应 (高于阈值)
 *       优先级5-15的中断被屏蔽
 *
 * 安全调用API的中断优先级要求:
 *   中断优先级数值 ≥ configMAX_SYSCALL_INTERRUPT_PRIORITY
 *   (即优先级数值更大，实际优先级更低)
 */

/* 临界区嵌套计数 */
static UBaseType_t uxCriticalNesting = 0xaaaaaaaa;

void vPortEnterCritical(void) {
    /* 禁用中断 */
    portDISABLE_INTERRUPTS();

    /* 递增嵌套计数 */
    uxCriticalNesting++;
}

void vPortExitCritical(void) {
    configASSERT(uxCriticalNesting);

    /* 递减嵌套计数 */
    uxCriticalNesting--;

    /* 只在最外层启用中断 */
    if (uxCriticalNesting == 0) {
        portENABLE_INTERRUPTS();
    }
}
```

---

## 5. 性能优化策略

### 5.1 中断延迟优化决策矩阵

| 优化技术 | 实现复杂度 | 延迟改善 | 适用场景 |
|----------|-----------|----------|----------|
| **尾链 (Tail-chaining)** | 硬件自动 | 12周期 | 连续异常 |
| **延迟压栈 (Late-arriving)** | 硬件自动 | 可变 | 高优先级迟到 |
| **惰性FPU保存** | 配置 | 避免17周期 | FPU未使用 |
| **向量表重定位到SRAM** | 简单 | 减少等待状态 | Flash慢 |
| **优先级分组优化** | 简单 | 减少抢占冲突 | 多中断源 |
| **零等待Flash** | 硬件 | 减少取指延迟 | 速度敏感 |

### 5.2 ISR设计最佳实践

```
ISR设计决策树
│
├── ISR复杂度?
│   ├── 简单 (寄存器读写)
│   │   └── 全部在ISR中处理
│   │       └── 最小延迟
│   │
│   └── 复杂 (数据处理)
│       └── 两阶段处理
│           ├── 阶段1: ISR (关键部分)
│           │   ├── 读取/清除标志
│           │   ├── 缓冲原始数据
│           │   └── 通知任务
│           │
│           └── 阶段2: Task (非关键部分)
│               ├── 数据处理
│               ├── 算法计算
│               └── 结果输出
│
├── 数据量?
│   ├── 小 (<4字节)
│   │   └── 任务通知
│   │       └── 最快通知机制
│   │
│   ├── 中 (4-8字节)
│   │   └── 队列
│   │       └── 自带同步
│   │
│   └── 大 (>8字节)
│       └── 环形缓冲区 + 信号量
│           └── 减少队列拷贝开销
│
└── 实时性要求?
    ├── 硬实时 (<μs响应)
    │   └── 全部ISR处理
    │       └── 无调度延迟
    │
    └── 软实时 (允许ms级延迟)
        └── 延迟到任务处理
            └── 更好的系统响应
```

---

## 6. 参考资源

- **ARM Cortex-M4 TRM**: NVIC and Interrupt Handling
- **FreeRTOS Documentation**: Interrupt Management
- **Cortex-M3/M4 Definitive Guide**: Interrupt Architecture
- **PM0214**: STM32F4 Reference Manual - NVIC
