/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 81
 * Language: c
 * Block ID: aaa6e3f8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 汽车ECU实时任务模型
// 符合AUTOSAR OS和OSEK/VDX标准
// ============================================================================

#include <stdint.h>
#include <stdbool.h>

// 任务优先级定义 (OSEK/VDX标准)
// 数值越小优先级越高
typedef enum {
    PRIORITY_IDLE = 63,         // 最低优先级 - 后台任务
    PRIORITY_DIAGNOSTIC = 40,   // 诊断任务
    PRIORITY_LOGGING = 30,      // 日志记录
    PRIORITY_COMMUNICATION = 20,// 通信任务 (CAN/LIN)
    PRIORITY_CONTROL = 10,      // 控制算法
    PRIORITY_SENSORS = 5,       // 传感器采集
    PRIORITY_SAFETY = 1,        // 安全监控
    PRIORITY_ISR = 0            // 中断服务 (最高)
} TaskPriority;

// 任务类型
typedef enum {
    TASK_BASIC,     // 基本任务 - 单次执行完成
    TASK_EXTENDED   // 扩展任务 - 可等待事件
} TaskType;

// 任务调度策略
typedef enum {
    SCHEDULE_FULL,      // 完全抢占
    SCHEDULE_NON        // 非抢占
} TaskSchedule;

// 任务状态
typedef enum {
    TASK_RUNNING,       // 正在运行
    TASK_READY,         // 就绪等待
    TASK_WAITING,       // 等待事件
    TASK_SUSPENDED,     // 挂起
    TASK_TERMINATED     // 终止
} TaskState;

// 任务控制块
typedef struct {
    uint16_t task_id;           // 任务ID
    TaskPriority priority;      // 当前优先级
    TaskPriority base_priority; // 基础优先级
    TaskState state;            // 当前状态
    TaskType type;              // 任务类型

    // 时间参数 (单位: ms)
    uint16_t period;            // 周期 (0=非周期)
    uint16_t offset;            // 相位偏移
    uint16_t deadline;          // 相对截止期限
    uint16_t wcet;              // 最坏执行时间

    // 运行时统计
    uint32_t last_start_time;   // 上次启动时间
    uint32_t last_finish_time;  // 上次完成时间
    uint32_t execution_count;   // 执行次数
    uint32_t deadline_misses;   // 截止时间错过次数

    // 栈信息
    uint8_t *stack_base;
    uint16_t stack_size;

    // 任务入口
    void (*entry_point)(void);
} TaskControlBlock;

// 任务配置表 (设计时静态配置)
typedef struct {
    uint16_t task_id;
    TaskPriority priority;
    TaskType type;
    uint16_t period_ms;
    uint16_t offset_ms;
    uint16_t deadline_ms;
    uint16_t wcet_ms;
    uint16_t stack_size;
    void (*entry)(void);
} TaskConfig;

// ============================================================================
// 汽车ABS典型任务配置
// ============================================================================

const TaskConfig abs_task_configs[] = {
    // 任务ID, 优先级, 类型, 周期, 偏移, 截止期, WCET, 栈大小, 入口
    { 1, PRIORITY_ISR,      TASK_BASIC,   0,   0,   1,   0,   256,  NULL },  // 中断聚合
    { 2, PRIORITY_SAFETY,   TASK_BASIC,   5,   0,   5,   1,   512,  safety_monitor_task },
    { 3, PRIORITY_SENSORS,  TASK_BASIC,  10,   0,  10,   2,   512,  sensor_acquisition_task },
    { 4, PRIORITY_CONTROL,  TASK_BASIC,  10,   2,  10,   3,   1024, abs_control_task },
    { 5, PRIORITY_CONTROL,  TASK_BASIC,  20,   5,  20,   5,   512,  reference_speed_task },
    { 6, PRIORITY_COMMUNICATION, TASK_BASIC, 50, 10, 50, 3, 512,  can_tx_task },
    { 7, PRIORITY_DIAGNOSTIC, TASK_EXTENDED, 100, 0, 100, 10, 512, diagnostic_task },
    { 8, PRIORITY_LOGGING,  TASK_BASIC,  1000, 0, 1000, 5, 512,  data_logging_task },
};
#define NUM_TASKS (sizeof(abs_task_configs) / sizeof(TaskConfig))
