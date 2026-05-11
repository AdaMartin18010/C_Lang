# 实时任务调度器（工业实战项目）

RMS（Rate Monotonic Scheduling）和 EDF（Earliest Deadline First）调度算法的离散时间模拟器，用于嵌入式实时系统可调度性分析。

## 特性

- **RMS 调度**：周期越短优先级越高，固定优先级抢占式
- **EDF 调度**：截止期最近的任务优先，动态优先级
- **可调度性分析**：利用率充分条件测试
- **离散模拟**：逐时间片模拟任务执行
- **甘特图输出**：文本可视化调度结果

## 调度算法对比

| 特性 | RMS | EDF |
|------|-----|-----|
| 优先级 | 固定（周期决定） | 动态（截止期决定） |
| 抢占 | 是 | 是 |
| 利用率上限 | `n*(2^(1/n)-1)` ≤ 69.3% | 100% |
| 实现复杂度 | 低 | 中 |
| 适用场景 | 硬实时，固定周期任务 | 混合周期，高利用率 |

## 文件结构

| 文件 | 说明 |
|------|------|
| `rt_scheduler.h` | 任务、调度策略、模拟 API |
| `rt_scheduler.c` | RMS/EDF 实现 + 可调度性测试 |
| `test_scheduler.c` | 测试：可调度/不可调度/高利用率 |

## 编译运行

```bash
make
./test_scheduler
```

## 使用示例

```c
Task set[MAX_TASKS];
size_t count = 0;
task_set_add(set, &count, 10, 3);  /* 周期10, 执行时间3 */
task_set_add(set, &count, 15, 4);  /* 周期15, 执行时间4 */

/* 可调度性测试 */
bool ok = rms_schedulability_test(set, count);

/* 模拟 */
ScheduleEvent timeline[MAX_TIME];
int misses = schedule_simulate(set, count, SCHED_EDF, timeline, MAX_TIME);
print_gantt(timeline, 60, set, count);
```

## 数学基础

**RMS 利用率充分条件**：

```
U = Σ(Ci/Ti) ≤ n*(2^(1/n) - 1)
```

**EDF 利用率充分条件**：

```
U = Σ(Ci/Ti) ≤ 1
```

其中 `Ci` = 最坏执行时间，`Ti` = 周期，`n` = 任务数。

> 注意：这是充分条件，不是必要条件。即使不满足，任务集仍可能可调度。
