/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 4387
 * Language: c
 * Block ID: 01c3b97a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file profiling.c
 * @brief 简单性能分析工具
 */

#include <stdint.h>

/* 使用DWT周期计数器 (Cortex-M3/M4) */
#define DWT_CTRL        (*(volatile uint32_t *)0xE0001000)
#define DWT_CYCCNT      (*(volatile uint32_t *)0xE0001004)
#define DEMCR           (*(volatile uint32_t *)0xE000EDFC)

/**
 * @brief 初始化周期计数器
 */
void Profiling_Init(void)
{
    /* 使能DWT跟踪 */
    DEMCR |= (1 << 24);
    /* 使能CYCCNT */
    DWT_CTRL |= (1 << 0);
}

/**
 * @brief 获取当前周期计数
 */
static inline uint32_t Profiling_GetCycles(void)
{
    return DWT_CYCCNT;
}

/**
 * @brief 测量函数执行时间
 */
typedef struct {
    uint32_t startCycles;
    uint32_t totalCycles;
    uint32_t callCount;
    uint32_t minCycles;
    uint32_t maxCycles;
    const char *name;
} Profile_Record;

#define MAX_PROFILE_RECORDS 16
static Profile_Record g_profileRecords[MAX_PROFILE_RECORDS];
static uint8_t g_profileCount = 0;

uint8_t Profiling_Register(const char *name)
{
    if (g_profileCount >= MAX_PROFILE_RECORDS) return 0xFF;

    uint8_t id = g_profileCount++;
    g_profileRecords[id].name = name;
    g_profileRecords[id].totalCycles = 0;
    g_profileRecords[id].callCount = 0;
    g_profileRecords[id].minCycles = 0xFFFFFFFF;
    g_profileRecords[id].maxCycles = 0;

    return id;
}

void Profiling_Start(uint8_t id)
{
    if (id < MAX_PROFILE_RECORDS) {
        g_profileRecords[id].startCycles = Profiling_GetCycles();
    }
}

void Profiling_Stop(uint8_t id)
{
    if (id >= MAX_PROFILE_RECORDS) return;

    uint32_t elapsed = Profiling_GetCycles() - g_profileRecords[id].startCycles;

    g_profileRecords[id].totalCycles += elapsed;
    g_profileRecords[id].callCount++;

    if (elapsed < g_profileRecords[id].minCycles) {
        g_profileRecords[id].minCycles = elapsed;
    }
    if (elapsed > g_profileRecords[id].maxCycles) {
        g_profileRecords[id].maxCycles = elapsed;
    }
}

void Profiling_Report(void (*printFunc)(const char *fmt, ...))
{
    printFunc("\r\n=== Performance Report ===\r\n");
    printFunc("%-16s %8s %8s %10s %10s\r\n",
              "Function", "Calls", "Avg(us)", "Min(us)", "Max(us)");

    for (uint8_t i = 0; i < g_profileCount; i++) {
        Profile_Record *r = &g_profileRecords[i];
        if (r->callCount > 0) {
            uint32_t avg = r->totalCycles / r->callCount;
            /* 假设72MHz，转换为微秒 */
            printFunc("%-16s %8d %8d %10d %10d\r\n",
                      r->name,
                      r->callCount,
                      avg / 72,
                      r->minCycles / 72,
                      r->maxCycles / 72);
        }
    }
}

/* 使用示例宏 */
#define PROFILE_BEGIN(id) Profiling_Start(id)
#define PROFILE_END(id)   Profiling_Stop(id)
