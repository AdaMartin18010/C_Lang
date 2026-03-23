# 工业级案例：嵌入式RTOS内核设计与验证

> **案例类型**: 嵌入式系统 / 实时内核
> **参考标准**: ISO 26262 (ASIL-D), IEC 61508
> **难度级别**: L5
> **完整周期**: 需求→设计→实现→验证→认证

---

## 一、项目背景与需求

### 1.1 应用场景

```
项目：汽车动力系统控制ECU

安全要求：
• ASIL-D (最高汽车安全完整性等级)
• 单点故障覆盖率 > 99%
• 潜伏故障诊断覆盖率 > 90%
• 故障响应时间 < 50μs

实时要求：
• 任务数量: 50-100个
• 周期任务: 1ms - 100ms
• 中断延迟: < 12个时钟周期 (@100MHz = 120ns)
• 上下文切换: < 500ns

资源约束：
• 内存: 2MB RAM, 4MB Flash
• CPU: ARM Cortex-R52 (双核锁步)
• 功耗: < 5W
• 温度: -40°C to +125°C
```

### 1.2 安全需求推导

```
HARA分析 (危害分析与风险评估):

危害事件: 发动机意外熄火
├── 严重程度 (S): S3 (严重伤害可能)
├── 暴露概率 (E): E4 (高概率)
├── 可控性 (C): C3 (难以控制)
└── ASIL等级: ASIL-D (最高)

安全目标分解:
┌─────────────────────────────────────────────────────────────┐
│  SG1: 确保控制算法正确执行 (ASIL-D)                          │
│  ├── SR1.1: 双核锁步监控 (D)                                │
│  ├── SR1.2: 内存ECC检测 (D)                                 │
│  └── SR1.3: 看门狗监控 (D)                                  │
├─────────────────────────────────────────────────────────────┤
│  SG2: 确保调度确定性 (ASIL-D)                                │
│  ├── SR2.1: 静态优先级调度 (D)                              │
│  ├── SR2.2: 最坏执行时间分析 (D)                            │
│  └── SR2.3: 任务监控与保护 (D)                              │
├─────────────────────────────────────────────────────────────┤
│  SG3: 确保故障检测与响应 (ASIL-D)                            │
│  ├── SR3.1: 硬件故障自检 (D)                                │
│  ├── SR3.2: 软件逻辑监控 (D)                                │
│  └── SR3.3: 安全状态转换 (D)                                │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、架构设计

### 2.1 内核架构

```
┌─────────────────────────────────────────────────────────────┐
│                     应用层 (ASIL-D)                          │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │ 控制算法  │ │ 故障处理  │ │ 诊断服务  │ │ 通信栈   │       │
│  └────┬─────┘ └────┬─────┘ └────┬─────┘ └────┬─────┘       │
├───────┴────────────┴────────────┴────────────┴──────────────┤
│                     内核服务层 (ASIL-D)                      │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │ 任务管理  │ │ 中断管理  │ │ 时间管理  │ │ 资源管理  │       │
│  │ 静态调度  │ │ 向量表    │ │ 高分辨率  │ │ 优先级 ceiling │    │
│  └────┬─────┘ └────┬─────┘ └────┬─────┘ └────┬─────┘       │
├───────┴────────────┴────────────┴────────────┴──────────────┤
│                     硬件抽象层 (ASIL-D)                      │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌──────────┐       │
│  │ 双核锁步  │ │ 内存保护  │ │ 时钟管理  │ │ 外设驱动  │       │
│  │ Lockstep │ │ MPU      │ │ 高精度    │ │ 安全监控  │       │
│  └──────────┘ └──────────┘ └──────────┘ └──────────┘       │
├─────────────────────────────────────────────────────────────┤
│                     硬件层                                   │
│           ARM Cortex-R52 + 专用安全监控硬件                  │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 调度算法设计

```c
/*
 * 混合调度策略：
 * • 表驱动调度 (TTS) - 硬实时任务
 * • 优先级调度 (EDF/ RMS) - 软实时任务
 * • 空闲任务 - 背景处理
 */

// 任务类型定义
typedef enum {
    TASK_TYPE_TTS,      // 表驱动 (时间触发)
    TASK_TYPE_EDF,      // 最早截止时间优先
    TASK_TYPE_IDLE      // 空闲任务
} task_type_t;

// 任务控制块 (简化)
typedef struct {
    uint32_t id;
    task_type_t type;
    uint32_t period_ms;
    uint32_t deadline_ms;
    uint32_t wcet_us;           // 最坏执行时间
    void (*entry)(void);
    uint32_t *stack_base;
    uint32_t stack_size;
    uint8_t priority;           // 静态优先级 (RMS)
    uint64_t next_release;      // 下次释放时间
    uint64_t absolute_deadline; // 绝对截止时间
} task_tcb_t;

// 调度决策函数
static task_tcb_t *schedule(void) {
    uint64_t now = get_highres_time();
    task_tcb_t *next_task = NULL;
    uint64_t earliest_deadline = UINT64_MAX;

    // 1. 检查表驱动任务 (TTS)
    for (int i = 0; i < num_tts_tasks; i++) {
        if (tts_table[i].release_time <= now &&
            tts_table[i].completed == 0) {
            return tts_table[i].task;  // TTS任务优先级最高
        }
    }

    // 2. EDF调度 (动态优先级)
    for (int i = 0; i < num_tasks; i++) {
        if (tasks[i].state == READY) {
            if (tasks[i].absolute_deadline < earliest_deadline) {
                earliest_deadline = tasks[i].absolute_deadline;
                next_task = &tasks[i];
            }
        }
    }

    // 3. 空闲任务
    if (next_task == NULL) {
        next_task = &idle_task;
    }

    return next_task;
}

// 上下文切换 (汇编实现，ARMv7-R)
__asm void context_switch(task_tcb_t *prev, task_tcb_t *next) {
    // 保存上下文
    PUSH    {R4-R11, LR}        // 保存被调用者保存寄存器
    STR     SP, [R0, #SP_OFFSET] // 保存SP到prev->stack_ptr

    // 恢复上下文
    LDR     SP, [R1, #SP_OFFSET] // 从next->stack_ptr恢复SP
    POP     {R4-R11, LR}        // 恢复寄存器
    BX      LR                  // 返回
}

// 调度时序保证
// 时钟频率: 100MHz
// 上下文切换: ~30时钟周期 = 300ns
// 调度决策: ~50时钟周期 = 500ns
// 总调度开销: < 1μs
```

### 2.3 内存保护设计

```c
/*
 * MPU (Memory Protection Unit) 配置
 * 任务隔离 + 栈溢出检测
 */

// MPU区域定义
typedef struct {
    uint32_t base_addr;
    uint32_t size;
    uint32_t attrs;     // 权限属性
} mpu_region_t;

// 任务内存布局
#define TASK_STACK_SIZE     0x4000      // 16KB 栈
#define TASK_PRIV_DATA_SIZE 0x1000      // 4KB 私有数据

// 每个任务的MPU配置
void mpu_configure_task(task_tcb_t *task) {
    mpu_region_t regions[4];

    // 区域0: 代码段 (只读执行)
    regions[0].base_addr = CODE_BASE;
    regions[0].size = CODE_SIZE;
    regions[0].attrs = MPU_ATTR_RO | MPU_ATTR_XN_CLEAR;

    // 区域1: 任务栈 (读写，禁止执行)
    regions[1].base_addr = task->stack_base - TASK_STACK_SIZE;
    regions[1].size = TASK_STACK_SIZE;
    regions[1].attrs = MPU_ATTR_RW | MPU_ATTR_XN;

    // 区域2: 任务私有数据
    regions[2].base_addr = task->priv_data_base;
    regions[2].size = TASK_PRIV_DATA_SIZE;
    regions[2].attrs = MPU_ATTR_RW | MPU_ATTR_XN;

    // 区域3: 共享数据 (通过权限控制)
    regions[3].base_addr = SHARED_DATA_BASE;
    regions[3].size = SHARED_DATA_SIZE;
    regions[3].attrs = task->shared_access_attrs;

    // 应用MPU配置
    mpu_load_regions(regions, 4);
    mpu_enable();
}

// 栈溢出检测 (通过MPU)
// 在栈底设置 guard page
void setup_stack_guard(task_tcb_t *task) {
    mpu_region_t guard = {
        .base_addr = task->stack_base - TASK_STACK_SIZE - 0x20,
        .size = 0x20,  // 32 bytes
        .attrs = MPU_ATTR_NO_ACCESS  // 不可访问
    };
    mpu_load_region(&guard, 4);  // 使用第5个区域
}

// MPU异常处理 (栈溢出检测)
void MemManage_Handler(void) {
    uint32_t mmfsr = get_MMFSR();  // 内存管理故障状态
    uint32_t mmar = get_MMAR();     // 故障地址

    if (mmfsr & MMFSR_IACCVIOL) {
        // 指令访问违规
        safety_panic(PANIC_INST_VIOLATION, mmar);
    }
    if (mmfsr & MMFSR_DACCVIOL) {
        // 数据访问违规
        if (is_stack_overflow(mmar)) {
            safety_panic(PANIC_STACK_OVERFLOW, mmar);
        } else {
            safety_panic(PANIC_DATA_VIOLATION, mmar);
        }
    }
}
```

---

## 三、安全机制实现

### 3.1 双核锁步 (Lockstep)

```c
/*
 * 双核锁步实现：
 * • Core0: 主核，执行实际工作
 * • Core1: 从核，延迟2个时钟周期执行相同指令
 * • 比较器：每个周期比较输出
 */

// 锁步监控状态
typedef struct {
    volatile uint32_t core0_output;
    volatile uint32_t core1_output;
    volatile uint32_t compare_result;
    uint32_t mismatch_count;
    uint32_t last_mismatch_pc;
} lockstep_state_t;

// 硬件比较器中断
void Lockstep_Error_Handler(void) {
    // 立即停止输出
    disable_all_outputs();

    // 记录故障信息
    log_fault(FAULT_LOCKSTEP_MISMATCH,
              lockstep_state.last_mismatch_pc);

    // 进入安全状态
    enter_safe_state(SAFE_STATE_STOP);
}

// 软件自检 (周期性)
void lockstep_selftest(void) {
    // 注入已知故障，验证检测机制
    uint32_t test_pattern = 0xAAAAAAAA;

    // 临时解除锁步 (测试模式)
    enter_test_mode();

    // 故意制造不匹配
    set_core0_test_output(test_pattern);
    set_core1_test_output(~test_pattern);

    // 验证检测到故障
    if (!check_fault_detected()) {
        // 自检失败！锁步机制失效
        safety_panic(PANIC_LOCKSTEP_SELFTEST_FAIL);
    }

    // 恢复正常运行
    exit_test_mode();
}

// 延迟锁步比较时序
// Core0:  T0   T1   T2   T3   T4
//         ↓    ↓    ↓    ↓    ↓
// Core1:       T0   T1   T2   T3
//                    ↓
//                比较T0结果
```

### 3.2 E2E保护 (End-to-End)

```c
/*
 * 端到端保护：数据从传感器到执行器的完整性
 * 使用CRC + 序列号 + 时间戳
 */

typedef struct {
    uint32_t data;          // 实际数据
    uint32_t sequence;      // 序列号 (检测丢失/重复)
    uint32_t timestamp;     // 时间戳 (检测延迟)
    uint32_t crc;           // CRC32校验
} protected_data_t;

// 发送端保护
void e2e_protect(protected_data_t *msg, uint32_t data) {
    static uint32_t seq = 0;

    msg->data = data;
    msg->sequence = seq++;
    msg->timestamp = get_timestamp();

    // 计算CRC (数据 + 序列号 + 时间戳)
    msg->crc = crc32((uint8_t*)msg, sizeof(*msg) - sizeof(msg->crc));
}

// 接收端验证
e2e_status_t e2e_verify(protected_data_t *msg) {
    // 1. CRC验证
    uint32_t calc_crc = crc32((uint8_t*)msg, sizeof(*msg) - sizeof(msg->crc));
    if (calc_crc != msg->crc) {
        return E2E_CRC_ERROR;
    }

    // 2. 序列号检查
    static uint32_t expected_seq = 0;
    if (msg->sequence != expected_seq) {
        if (msg->sequence > expected_seq) {
            return E2E_LOST_MESSAGES;  // 丢失消息
        } else {
            return E2E_DUPLICATE;       // 重复消息
        }
    }
    expected_seq++;

    // 3. 时间戳检查 (检测延迟)
    uint32_t now = get_timestamp();
    uint32_t delay = now - msg->timestamp;
    if (delay > MAX_ALLOWED_DELAY) {
        return E2E_TIMEOUT;
    }

    return E2E_OK;
}

// 安全通信协议栈
// 传感器 -> E2E保护 -> CAN总线 -> E2E验证 -> 控制算法 -> E2E保护 -> 执行器
```

---

## 四、验证与认证

### 4.1 WCET分析

```
WCET (Worst-Case Execution Time)分析流程：

┌─────────────────────────────────────────────────────────────┐
│  1. 代码分析                                                │
│     • 控制流图 (CFG)构建                                    │
│     • 循环边界确定                                          │
│     • 递归消除                                              │
├─────────────────────────────────────────────────────────────┤
│  2. 微架构建模                                              │
│     • 指令时序                                              │
│     • 缓存行为 (AI = n/(n+m) 分析)                          │
│     • 流水线效应                                            │
│     • 分支预测                                              │
├─────────────────────────────────────────────────────────────┤
│  3. 路径分析                                                │
│     • IPET (隐式路径枚举技术)                               │
│     • 整数线性规划 (ILP)求解                                │
│     • 流事实约束                                            │
├─────────────────────────────────────────────────────────────┤
│  4. 验证                                                    │
│     • 测量值 ≤ WCET估计                                     │
│     • 安全余量评估                                          │
│     • 定期重测                                              │
└─────────────────────────────────────────────────────────────┘

分析工具链：
• Bound-T: 商业WCET分析器
• aiT (AbsInt): 工业标准
• Chronos: 学术研究
• 自定义LLVM pass (轻量级)
```

### 4.2 MC/DC覆盖率

```
MC/DC (Modified Condition/Decision Coverage):
ASIL-D要求的最高覆盖率级别

要求：
• 每个条件的独立影响必须被测试
• 对于n个条件，最少需要n+1个测试用例

示例：
  if (A && B && C) { action(); }

  真值表：
  ┌───┬───┬───┬──────┐
  │ A │ B │ C │ 结果 │
  ├───┼───┼───┼──────┤
  │ T │ T │ T │  T   │
  │ T │ T │ F │  F   │  <- C独立影响
  │ T │ F │ T │  F   │  <- B独立影响
  │ F │ T │ T │  F   │  <- A独立影响
  └───┴───┴───┴──────┘

测试用例：1, 2, 3, 4 覆盖所有条件独立影响

自动化MC/DC：
• Cantata: 商业工具
• VectorCAST
• 自定义LLVM覆盖率插桩
```

### 4.3 形式化验证

```c
/*
 * 关键算法的形式化验证 (使用Frama-C)
 */

/*@
    requires \valid(p) && \valid(q);
    requires *p <= INT_MAX - *q;
    assigns \nothing;
    ensures \result == *p + *q;
    behavior overflow:
        assumes *p > INT_MAX - *q;
        ensures \result == INT_MAX;
        assigns errno;
    complete behaviors;
    disjoint behaviors;
*/
int safe_add(int *p, int *q) {
    if (*p > INT_MAX - *q) {
        errno = ERANGE;
        return INT_MAX;
    }
    return *p + *q;
}

// 调度算法的形式化验证
/*@
    predicate is_schedulable{L}(task_t tasks[], integer n) =
        \forall integer i; 0 <= i < n ==>
            tasks[i].wcet <= tasks[i].deadline;

    lemma util_bound:
        \forall task_t tasks[], integer n;
        is_rms_schedule(tasks, n) ==>
        utilization(tasks, n) <= n * (pow(2, 1.0/n) - 1);
*/

// 证明RMS调度在最优利用率下的可调度性
```

---

## 五、生产数据与经验

### 5.1 性能实测

| 指标 | 目标 | 实测 | 余量 |
|:-----|:-----|:-----|:-----|
| 中断延迟 | < 1μs | 450ns | 55% |
| 上下文切换 | < 1μs | 320ns | 68% |
| 调度周期 | 1ms | 0.998ms | 99.8% |
| WCET/Deadline | < 80% | 72% | 8% |

### 5.2 故障统计 (运行2年)

```
故障类型分布：
• 硬件瞬态故障：12次 (ECC纠正)
• 看门狗复位：2次 (软件超时)
• 安全状态进入：1次 (外部EMI干扰)
• 严重故障：0次

安全机制有效性：
• 双核锁步：检测到3次不匹配 (全部安全处理)
• MPU保护：阻止2次越界访问
• E2E保护：检测到5次通信异常
• 时间监控：触发1次截止时间违约
```

---

## 关联导航

- [实时系统设计](../03_System_Technology_Domains/03_Embedded_Systems/04_Real_Time_Systems.md)
- [进程调度算法](../03_System_Technology_Domains/16_OS_Kernel/02_Process_Scheduling_Deep.md)
- [形式化验证](../02_Formal_Semantics_and_Physics/13_Formal_Methods/01_Correctness_by_Construction.md)
