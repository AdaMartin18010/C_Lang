/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 1200
 * Language: c
 * Block ID: c6677cf0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file soft_plc_runtime.c
 * @brief 迷你软PLC运行时实现
 * @details 实现一个轻量级的软PLC运行时，支持扫描周期管理、
 *          变量表、I/O映射和简单的梯形图逻辑
 *
 * 编译: gcc -o soft_plc_runtime soft_plc_runtime.c -lpthread
 * 运行: sudo ./soft_plc_runtime
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/mman.h>

/* 版本信息 */
#define PLC_VERSION_MAJOR   1
#define PLC_VERSION_MINOR   0
#define PLC_VERSION_PATCH   0

/* PLC配置 */
#define PLC_MAX_VARIABLES   256     /* 最大变量数 */
#define PLC_MAX_PROGRAMS    8       /* 最大程序数 */
#define PLC_MAX_IO_POINTS   128     /* 最大I/O点数 */
#define PLC_SCAN_TIME_US    10000   /* 默认扫描周期: 10ms */

/* 变量类型 */
typedef enum {
    VAR_TYPE_BOOL = 0,      /* 布尔量 */
    VAR_TYPE_INT,           /* 整型 */
    VAR_TYPE_REAL,          /* 浮点 */
    VAR_TYPE_WORD,          /* 16位字 */
    VAR_TYPE_DWORD,         /* 32位双字 */
    VAR_TYPE_TIMER,         /* 定时器 */
    VAR_TYPE_COUNTER,       /* 计数器 */
    VAR_TYPE_MAX
} plc_var_type_t;

/* 变量区域 */
typedef enum {
    VAR_AREA_I = 0,         /* 输入区 */
    VAR_AREA_Q,             /* 输出区 */
    VAR_AREA_M,             /* 中间变量区 */
    VAR_AREA_DB,            /* 数据块区 */
    VAR_AREA_T,             /* 定时器区 */
    VAR_AREA_C,             /* 计数器区 */
    VAR_AREA_MAX
} plc_var_area_t;

/* 定时器结构 */
typedef struct {
    bool in;                /* 使能输入 */
    bool q;                 /* 输出 */
    uint32_t pt;            /* 预设时间 (ms) */
    uint32_t et;            /* 已计时间 (ms) */
    uint64_t start_time;    /* 启动时间戳 */
} plc_timer_t;

/* 计数器结构 */
typedef struct {
    bool cu;                /* 加计数输入 */
    bool cd;                /* 减计数输入 */
    bool r;                 /* 复位 */
    bool ld;                /* 装载 */
    int32_t pv;             /* 预设值 */
    int32_t cv;             /* 当前值 */
    bool q;                 /* 输出 */
    bool qu;                /* 加计数到达 */
    bool qd;                /* 减计数到达 */
    bool prev_cu;           /* 上次cu状态 */
    bool prev_cd;           /* 上次cd状态 */
} plc_counter_t;

/* 变量值联合体 */
typedef union {
    bool b;                 /* 布尔值 */
    int32_t i;              /* 整数值 */
    float f;                /* 浮点值 */
    uint16_t w;             /* 字值 */
    uint32_t dw;            /* 双字值 */
} plc_var_value_t;

/* 变量定义 */
typedef struct {
    char name[32];          /* 变量名 */
    plc_var_type_t type;    /* 变量类型 */
    plc_var_area_t area;    /* 变量区域 */
    uint16_t addr;          /* 区域地址 */
    plc_var_value_t value;  /* 当前值 */
    plc_var_value_t prev;   /* 上次值 */
    bool forced;            /* 强制标志 */
    plc_var_value_t force_val; /* 强制值 */
} plc_variable_t;

/* I/O点定义 */
typedef struct {
    plc_var_area_t area;    /* I/O区域 */
    uint16_t addr;          /* 地址 */
    bool value;             /* 当前值 */
    bool forced;            /* 强制标志 */
    uint8_t quality;        /* 质量码 (0=Good, 1=Bad, 2=Uncertain) */
    uint64_t timestamp;     /* 时间戳 */
} plc_io_point_t;

/* PLC程序 */
typedef struct {
    char name[32];          /* 程序名 */
    uint32_t cycle_count;   /* 执行计数 */
    uint32_t exec_time_us;  /* 执行时间 (微秒) */
    bool enabled;           /* 使能标志 */
    void (*init)(void);     /* 初始化函数 */
    void (*run)(void);      /* 运行函数 */
    void (*cleanup)(void);  /* 清理函数 */
} plc_program_t;

/* PLC运行时状态 */
typedef enum {
    PLC_STATE_STOP = 0,     /* 停止 */
    PLC_STATE_STARTING,     /* 启动中 */
    PLC_STATE_RUNNING,      /* 运行中 */
    PLC_STATE_STOPPING,     /* 停止中 */
    PLC_STATE_ERROR         /* 错误 */
} plc_state_t;

/* PLC运行时上下文 */
typedef struct {
    /* 状态 */
    plc_state_t state;
    uint32_t scan_count;
    uint32_t scan_time_us;
    uint32_t last_scan_time_us;
    uint32_t min_scan_time_us;
    uint32_t max_scan_time_us;
    uint64_t total_scan_time_us;

    /* 变量表 */
    plc_variable_t variables[PLC_MAX_VARIABLES];
    uint16_t var_count;

    /* I/O表 */
    plc_io_point_t io_inputs[PLC_MAX_IO_POINTS];
    plc_io_point_t io_outputs[PLC_MAX_IO_POINTS];
    uint16_t input_count;
    uint16_t output_count;

    /* 定时器和计数器 */
    plc_timer_t timers[32];
    plc_counter_t counters[32];

    /* 程序表 */
    plc_program_t programs[PLC_MAX_PROGRAMS];
    uint8_t program_count;

    /* 运行时控制 */
    volatile bool running;
    pthread_t cycle_thread;
    pthread_mutex_t lock;
} plc_runtime_t;

/* 全局运行时实例 */
static plc_runtime_t g_plc = {0};
static volatile int g_app_running = 1;

/**
 * @brief 获取当前时间 (微秒)
 */
static inline uint64_t get_time_us(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000000ULL + ts.tv_nsec / 1000;
}

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    printf("\n[INFO] Stopping PLC runtime...\n");
    g_app_running = 0;
    g_plc.running = false;
}

/**
 * @brief 初始化PLC运行时
 */
static int plc_init(void)
{
    printf("[PLC] Initializing runtime...\n");

    memset(&g_plc, 0, sizeof(g_plc));
    g_plc.state = PLC_STATE_STOP;
    g_plc.scan_time_us = PLC_SCAN_TIME_US;
    g_plc.min_scan_time_us = 0xFFFFFFFF;

    pthread_mutex_init(&g_plc.lock, NULL);

    printf("[PLC] Runtime initialized\n");
    return 0;
}

/**
 * @brief 清理PLC运行时
 */
static void plc_cleanup(void)
{
    printf("[PLC] Cleaning up...\n");

    pthread_mutex_destroy(&g_plc.lock);

    printf("[PLC] Cleanup complete\n");
}

/**
 * @brief 创建变量
 */
static plc_variable_t *plc_create_variable(const char *name,
                                           plc_var_type_t type,
                                           plc_var_area_t area,
                                           uint16_t addr)
{
    if (g_plc.var_count >= PLC_MAX_VARIABLES) {
        return NULL;
    }

    plc_variable_t *var = &g_plc.variables[g_plc.var_count++];
    strncpy(var->name, name, sizeof(var->name) - 1);
    var->type = type;
    var->area = area;
    var->addr = addr;
    var->forced = false;
    memset(&var->value, 0, sizeof(var->value));
    memset(&var->prev, 0, sizeof(var->prev));

    return var;
}

/**
 * @brief 查找变量
 */
static plc_variable_t *plc_find_variable(const char *name)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        if (strcmp(g_plc.variables[i].name, name) == 0) {
            return &g_plc.variables[i];
        }
    }
    return NULL;
}

/**
 * @brief 获取变量值 (布尔)
 */
static bool plc_get_bool(plc_var_area_t area, uint16_t addr)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            if (var->forced) {
                return var->force_val.b;
            }
            return var->value.b;
        }
    }
    return false;
}

/**
 * @brief 设置变量值 (布尔)
 */
static void plc_set_bool(plc_var_area_t area, uint16_t addr, bool value)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            var->prev.b = var->value.b;
            var->value.b = value;
            return;
        }
    }
}

/**
 * @brief 获取变量值 (整型)
 */
static int32_t plc_get_int(plc_var_area_t area, uint16_t addr)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            if (var->forced) {
                return var->force_val.i;
            }
            return var->value.i;
        }
    }
    return 0;
}

/**
 * @brief 设置变量值 (整型)
 */
static void plc_set_int(plc_var_area_t area, uint16_t addr, int32_t value)
{
    for (uint16_t i = 0; i < g_plc.var_count; i++) {
        plc_variable_t *var = &g_plc.variables[i];
        if (var->area == area && var->addr == addr) {
            var->prev.i = var->value.i;
            var->value.i = value;
            return;
        }
    }
}

/**
 * @brief 处理定时器
 */
static void plc_process_timers(void)
{
    uint64_t now = get_time_us();

    for (int i = 0; i < 32; i++) {
        plc_timer_t *t = &g_plc.timers[i];

        if (t->in) {
            if (t->et < t->pt) {
                uint64_t elapsed = (now - t->start_time) / 1000;  /* ms */
                t->et = (uint32_t)elapsed;
                if (t->et >= t->pt) {
                    t->et = t->pt;
                    t->q = true;
                }
            }
        } else {
            t->q = false;
            t->et = 0;
            t->start_time = now;
        }
    }
}

/**
 * @brief 处理计数器
 */
static void plc_process_counters(void)
{
    for (int i = 0; i < 32; i++) {
        plc_counter_t *c = &g_plc.counters[i];

        /* 复位 */
        if (c->r) {
            c->cv = 0;
            c->qu = false;
            c->qd = false;
            c->prev_cu = false;
            c->prev_cd = false;
            continue;
        }

        /* 装载 */
        if (c->ld) {
            c->cv = c->pv;
        }

        /* 加计数 (上升沿) */
        if (c->cu && !c->prev_cu) {
            if (c->cv < 32767) {
                c->cv++;
            }
        }
        c->prev_cu = c->cu;

        /* 减计数 (上升沿) */
        if (c->cd && !c->prev_cd) {
            if (c->cv > -32768) {
                c->cv--;
            }
        }
        c->prev_cd = c->cd;

        /* 输出状态 */
        c->qu = (c->cv >= c->pv);
        c->qd = (c->cv <= 0);
        c->q = c->qu;
    }
}

/**
 * @brief 梯形图逻辑示例 - 电机启停控制
 * @details
 *   I0.0 - 启动按钮 (常开)
 *   I0.1 - 停止按钮 (常闭)
 *   M0.0 - 电机运行标志
 *   Q0.0 - 电机输出
 */
static void plc_program_motor_control(void)
{
    /* 读取输入 */
    bool start_btn = plc_get_bool(VAR_AREA_I, 0);   /* I0.0 */
    bool stop_btn = plc_get_bool(VAR_AREA_I, 1);    /* I0.1 */

    /* 自锁电路 */
    bool motor_run = plc_get_bool(VAR_AREA_M, 0);   /* M0.0 */

    /* 梯形图逻辑: M0.0 = (I0.0 OR M0.0) AND I0.1 */
    motor_run = (start_btn || motor_run) && stop_btn;

    /* 写入输出 */
    plc_set_bool(VAR_AREA_M, 0, motor_run);         /* M0.0 */
    plc_set_bool(VAR_AREA_Q, 0, motor_run);         /* Q0.0 */
}

/**
 * @brief 梯形图逻辑示例 - 顺序控制
 * @details 简单的三步骤顺序控制，使用定时器延时
 */
static void plc_program_sequential_control(void)
{
    static uint8_t step = 0;

    /* 使用定时器T0 */
    plc_timer_t *t0 = &g_plc.timers[0];

    switch (step) {
        case 0:  /* 等待启动 */
            if (plc_get_bool(VAR_AREA_I, 2)) {  /* I0.2 启动信号 */
                step = 1;
                t0->in = true;
                t0->pt = 2000;  /* 2秒延时 */
            }
            break;

        case 1:  /* 步骤1: 打开阀门1 */
            plc_set_bool(VAR_AREA_Q, 1, true);   /* Q0.1 阀门1 */
            if (t0->q) {  /* 延时到 */
                t0->in = false;
                step = 2;
                t0->in = true;
                t0->pt = 3000;  /* 3秒延时 */
            }
            break;

        case 2:  /* 步骤2: 打开阀门2，关闭阀门1 */
            plc_set_bool(VAR_AREA_Q, 1, false);  /* Q0.1 */
            plc_set_bool(VAR_AREA_Q, 2, true);   /* Q0.2 阀门2 */
            if (t0->q) {
                t0->in = false;
                step = 0;  /* 回到初始 */
                plc_set_bool(VAR_AREA_Q, 2, false);
            }
            break;
    }
}

/**
 * @brief 梯形图逻辑示例 - 模拟量处理
 * @details 读取模拟量输入，进行滤波和缩放后输出
 */
static void plc_program_analog_processing(void)
{
    static float filtered_value = 0.0f;
    const float alpha = 0.1f;  /* 一阶滤波系数 */

    /* 读取模拟量输入 (0-4095 对应 0-10V) */
    int32_t raw_input = plc_get_int(VAR_AREA_I, 10);  /* IW10 */

    /* 转换为实际值 (0.0 - 10.0V) */
    float voltage = (float)raw_input * 10.0f / 4095.0f;

    /* 一阶低通滤波 */
    filtered_value = alpha * voltage + (1.0f - alpha) * filtered_value;

    /* 温度转换 (假设0-10V对应0-200°C) */
    float temperature = filtered_value * 20.0f;  /* 0-200°C */

    /* 写入输出 */
    plc_set_int(VAR_AREA_Q, 10, (int32_t)(temperature * 10));  /* QW10 */

    /* 超温报警 */
    plc_set_bool(VAR_AREA_Q, 4, temperature > 150.0f);  /* Q0.4 */
}

/**
 * @brief PLC扫描周期任务
 */
static void plc_scan_cycle(void)
{
    uint64_t start_time, end_time;

    pthread_mutex_lock(&g_plc.lock);

    start_time = get_time_us();
    g_plc.scan_count++;

    /* ========== 扫描周期开始 ========== */

    /* 1. 读取输入 */
    /* (实际项目中从硬件读取) */

    /* 2. 处理定时器和计数器 */
    plc_process_timers();
    plc_process_counters();

    /* 3. 执行用户程序 */
    plc_program_motor_control();
    plc_program_sequential_control();
    plc_program_analog_processing();

    /* 4. 写入输出 */
    /* (实际项目中写入硬件) */

    /* ========== 扫描周期结束 ========== */

    end_time = get_time_us();
    g_plc.last_scan_time_us = (uint32_t)(end_time - start_time);

    /* 更新统计 */
    if (g_plc.last_scan_time_us < g_plc.min_scan_time_us) {
        g_plc.min_scan_time_us = g_plc.last_scan_time_us;
    }
    if (g_plc.last_scan_time_us > g_plc.max_scan_time_us) {
        g_plc.max_scan_time_us = g_plc.last_scan_time_us;
    }
    g_plc.total_scan_time_us += g_plc.last_scan_time_us;

    pthread_mutex_unlock(&g_plc.lock);
}

/**
 * @brief PLC运行时线程
 */
static void *plc_cycle_thread(void *arg)
{
    (void)arg;
    struct timespec next_time;

    printf("[PLC] Cycle thread started\n");

    clock_gettime(CLOCK_MONOTONIC, &next_time);

    while (g_plc.running && g_app_running) {
        /* 执行扫描周期 */
        plc_scan_cycle();

        /* 等待下一个周期 */
        next_time.tv_nsec += g_plc.scan_time_us * 1000;
        while (next_time.tv_nsec >= 1000000000L) {
            next_time.tv_nsec -= 1000000000L;
            next_time.tv_sec++;
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_time, NULL);
    }

    printf("[PLC] Cycle thread stopped\n");
    return NULL;
}

/**
 * @brief 启动PLC
 */
static int plc_start(void)
{
    printf("[PLC] Starting runtime...\n");

    if (g_plc.state == PLC_STATE_RUNNING) {
        printf("[PLC] Already running\n");
        return 0;
    }

    g_plc.state = PLC_STATE_STARTING;
    g_plc.running = true;

    /* 创建扫描周期线程 */
    if (pthread_create(&g_plc.cycle_thread, NULL, plc_cycle_thread, NULL) != 0) {
        g_plc.state = PLC_STATE_ERROR;
        return -1;
    }

    /* 设置实时优先级 */
    struct sched_param param = {.sched_priority = 80};
    pthread_setschedparam(g_plc.cycle_thread, SCHED_FIFO, &param);

    g_plc.state = PLC_STATE_RUNNING;
    printf("[PLC] Runtime started\n");
    return 0;
}

/**
 * @brief 停止PLC
 */
static void plc_stop(void)
{
    printf("[PLC] Stopping runtime...\n");

    g_plc.state = PLC_STATE_STOPPING;
    g_plc.running = false;

    pthread_join(g_plc.cycle_thread, NULL);

    g_plc.state = PLC_STATE_STOP;
    printf("[PLC] Runtime stopped\n");
}

/**
 * @brief 打印PLC状态
 */
static void plc_print_status(void)
{
    pthread_mutex_lock(&g_plc.lock);

    const char *state_str[] = {"STOP", "STARTING", "RUNNING", "STOPPING", "ERROR"};

    printf("\n");
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                PLC Runtime Status                        ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ State:          %s\n", state_str[g_plc.state]);
    printf("║ Scan Count:     %u\n", g_plc.scan_count);
    printf("║ Scan Time:      %u us (target: %u us)\n",
           g_plc.last_scan_time_us, g_plc.scan_time_us);
    printf("║ Min Scan Time:  %u us\n", g_plc.min_scan_time_us);
    printf("║ Max Scan Time:  %u us\n", g_plc.max_scan_time_us);
    if (g_plc.scan_count > 0) {
        printf("║ Avg Scan Time:  %llu us\n",
               g_plc.total_scan_time_us / g_plc.scan_count);
    }
    printf("║ Variables:      %u/%d\n", g_plc.var_count, PLC_MAX_VARIABLES);
    printf("╚══════════════════════════════════════════════════════════╝\n");

    pthread_mutex_unlock(&g_plc.lock);
}

/**
 * @brief 初始化演示变量
 */
static void init_demo_variables(void)
{
    /* 数字量输入 */
    plc_create_variable("StartButton", VAR_TYPE_BOOL, VAR_AREA_I, 0);
    plc_create_variable("StopButton", VAR_TYPE_BOOL, VAR_AREA_I, 1);
    plc_create_variable("SeqStart", VAR_TYPE_BOOL, VAR_AREA_I, 2);

    /* 数字量输出 */
    plc_create_variable("MotorRun", VAR_TYPE_BOOL, VAR_AREA_Q, 0);
    plc_create_variable("Valve1", VAR_TYPE_BOOL, VAR_AREA_Q, 1);
    plc_create_variable("Valve2", VAR_TYPE_BOOL, VAR_AREA_Q, 2);
    plc_create_variable("Alarm", VAR_TYPE_BOOL, VAR_AREA_Q, 4);

    /* 中间变量 */
    plc_create_variable("MotorState", VAR_TYPE_BOOL, VAR_AREA_M, 0);

    /* 模拟量 */
    plc_create_variable("RawAnalog", VAR_TYPE_INT, VAR_AREA_I, 10);
    plc_create_variable("Temperature", VAR_TYPE_INT, VAR_AREA_Q, 10);

    /* 初始化定时器 */
    g_plc.timers[0].pt = 0;
    g_plc.timers[0].et = 0;
    g_plc.timers[0].q = false;
    g_plc.timers[0].in = false;
    g_plc.timers[0].start_time = get_time_us();
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║       Mini Soft-PLC Runtime v%d.%d.%d                      ║\n",
           PLC_VERSION_MAJOR, PLC_VERSION_MINOR, PLC_VERSION_PATCH);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 锁定内存 */
    mlockall(MCL_CURRENT | MCL_FUTURE);

    /* 初始化PLC */
    if (plc_init() != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize PLC\n");
        return 1;
    }

    /* 初始化演示变量 */
    init_demo_variables();

    /* 启动PLC */
    if (plc_start() != 0) {
        fprintf(stderr, "[ERROR] Failed to start PLC\n");
        plc_cleanup();
        return 1;
    }

    printf("[INFO] PLC is running. Press Ctrl+C to stop.\n\n");

    /* 模拟输入变化 */
    uint32_t sim_counter = 0;
    while (g_app_running) {
        sleep(1);
        sim_counter++;

        pthread_mutex_lock(&g_plc.lock);

        /* 模拟启动按钮按下 (每10秒) */
        if (sim_counter % 10 == 0) {
            plc_set_bool(VAR_AREA_I, 0, true);
        } else if (sim_counter % 10 == 2) {
            plc_set_bool(VAR_AREA_I, 0, false);
        }

        /* 模拟停止按钮 (常闭) */
        plc_set_bool(VAR_AREA_I, 1, true);

        /* 模拟顺序启动信号 (每30秒) */
        if (sim_counter % 30 == 5) {
            plc_set_bool(VAR_AREA_I, 2, true);
        } else if (sim_counter % 30 == 6) {
            plc_set_bool(VAR_AREA_I, 2, false);
        }

        /* 模拟模拟量输入 (正弦波) */
        float angle = (float)sim_counter * 0.1f;
        int32_t analog_val = (int32_t)(2047.5f + 2047.5f * sin(angle));
        plc_set_int(VAR_AREA_I, 10, analog_val);

        pthread_mutex_unlock(&g_plc.lock);

        /* 每5秒打印状态 */
        if (sim_counter % 5 == 0) {
            plc_print_status();
        }
    }

    /* 停止PLC */
    plc_stop();

    /* 打印最终状态 */
    plc_print_status();

    /* 清理 */
    plc_cleanup();

    printf("\n[INFO] Program completed\n");

    return 0;
}
