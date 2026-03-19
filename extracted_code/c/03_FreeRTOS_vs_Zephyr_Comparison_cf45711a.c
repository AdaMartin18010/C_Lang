/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 3618
 * Language: c
 * Block ID: cf45711a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS 内存管理示例
 * 文件: freertos_memory_example.c
 */

#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include <string.h>

/* 使用Heap_4方案（推荐） */
/* configTOTAL_HEAP_SIZE 在FreeRTOSConfig.h中定义 */

/* 内存统计结构 */
typedef struct {
    size_t total_heap;
    size_t free_heap;
    size_t minimum_free;
    size_t allocated;
    size_t peak_allocated;
} HeapStats_t;

/* 对象类型示例 */
typedef struct {
    uint32_t id;
    char name[32];
    uint32_t data[10];
} MyObject_t;

/*
 * 动态内存分配示例
 */
static void memory_allocation_demo(void)
{
    void *ptr1, *ptr2, *ptr3;
    MyObject_t *obj;

    /* 基本分配 */
    ptr1 = pvPortMalloc(100);
    if (ptr1 != NULL) {
        printf("[MemDemo] Allocated 100 bytes at %p\n", ptr1);
        memset(ptr1, 0, 100);
    }

    /* 对齐分配 */
    ptr2 = pvPortMalloc(64);
    if (ptr2 != NULL) {
        printf("[MemDemo] Allocated 64 bytes at %p (aligned to %zu)\n",
               ptr2, sizeof(void*));
    }

    /* 分配并清零 */
    ptr3 = pvPortCalloc(1, sizeof(MyObject_t));  /* Heap_5+ */
    if (ptr3 != NULL) {
        printf("[MemDemo] Allocated and zeroed object at %p\n", ptr3);
    }

    /* 分配对象 */
    obj = (MyObject_t *)pvPortMalloc(sizeof(MyObject_t));
    if (obj != NULL) {
        obj->id = 1;
        strncpy(obj->name, "TestObject", 31);
        printf("[MemDemo] Created object: id=%lu, name=%s\n",
               obj->id, obj->name);
    }

    /* 释放内存 */
    vPortFree(ptr1);
    vPortFree(ptr2);
    vPortFree(ptr3);
    vPortFree(obj);

    printf("[MemDemo] Memory freed\n");
}

/*
 * 静态内存分配示例
 */
#define MAX_STATIC_OBJECTS  10

static MyObject_t xStaticObjects[MAX_STATIC_OBJECTS];
static uint8_t xObjectUsed[MAX_STATIC_OBJECTS] = {0};

static MyObject_t* allocate_static_object(void)
{
    for (int i = 0; i < MAX_STATIC_OBJECTS; i++) {
        if (!xObjectUsed[i]) {
            xObjectUsed[i] = 1;
            memset(&xStaticObjects[i], 0, sizeof(MyObject_t));
            xStaticObjects[i].id = i;
            return &xStaticObjects[i];
        }
    }
    return NULL;  /* 无可用对象 */
}

static void free_static_object(MyObject_t *obj)
{
    if (obj >= xStaticObjects &&
        obj < xStaticObjects + MAX_STATIC_OBJECTS) {
        int index = obj - xStaticObjects;
        xObjectUsed[index] = 0;
    }
}

/*
 * 内存池实现（自定义）
 */
#define POOL_BLOCK_SIZE     64
#define POOL_NUM_BLOCKS     20

typedef struct {
    uint8_t blocks[POOL_NUM_BLOCKS][POOL_BLOCK_SIZE];
    uint8_t used[POOL_NUM_BLOCKS];
} MemoryPool_t;

static MemoryPool_t xMemoryPool;

static void* pool_allocate(void)
{
    taskENTER_CRITICAL();
    {
        for (int i = 0; i < POOL_NUM_BLOCKS; i++) {
            if (!xMemoryPool.used[i]) {
                xMemoryPool.used[i] = 1;
                taskEXIT_CRITICAL();
                return xMemoryPool.blocks[i];
            }
        }
    }
    taskEXIT_CRITICAL();
    return NULL;
}

static void pool_free(void *ptr)
{
    if (ptr == NULL) return;

    taskENTER_CRITICAL();
    {
        for (int i = 0; i < POOL_NUM_BLOCKS; i++) {
            if (xMemoryPool.blocks[i] == ptr) {
                xMemoryPool.used[i] = 0;
                taskEXIT_CRITICAL();
                return;
            }
        }
    }
    taskEXIT_CRITICAL();
}

/*
 * 内存统计
 */
static void print_heap_stats(void)
{
    HeapStats_t stats;

    vPortGetHeapStats((HeapStats_t *)&stats);

    printf("\n[HeapStats] Memory Statistics:\n");
    printf("  Total heap: %zu bytes\n", stats.total_heap);
    printf("  Free heap: %zu bytes\n", stats.free_heap);
    printf("  Minimum free: %zu bytes\n", stats.minimum_free);
    printf("  Largest free block: %zu bytes\n",
           xPortGetMinimumEverFreeHeapSize());
    printf("  Free heap size: %zu bytes\n", xPortGetFreeHeapSize());
}

/*
 * 栈溢出检测钩子
 */
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("[ERROR] Stack overflow detected in task: %s\n", pcTaskName);
    /* 记录或复位系统 */
    for (;;);
}

/*
 * 内存分配失败钩子
 */
void vApplicationMallocFailedHook(void)
{
    printf("[ERROR] Memory allocation failed\n");
    print_heap_stats();
    for (;;);
}

/*
 * 任务栈大小配置
 */
#define TASK_STACK_SIZE     512

static StaticTask_t xTaskTCB;
static StackType_t xTaskStack[TASK_STACK_SIZE];

static void vMemoryTask(void *pvParameters)
{
    (void)pvParameters;
    void *allocations[10];
    int alloc_count = 0;

    printf("[MemoryTask] Started\n");

    for (;;) {
        /* 模拟内存分配和释放 */
        if (alloc_count < 10) {
            allocations[alloc_count] = pvPortMalloc(100 + (alloc_count * 10));
            if (allocations[alloc_count] != NULL) {
                printf("[MemoryTask] Allocated block %d at %p\n",
                       alloc_count, allocations[alloc_count]);
                alloc_count++;
            }
        } else {
            /* 释放所有 */
            for (int i = 0; i < alloc_count; i++) {
                vPortFree(allocations[i]);
            }
            printf("[MemoryTask] Freed all allocations\n");
            alloc_count = 0;

            print_heap_stats();
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/*
 * 使用heap_5的多区域内存
 */
#ifdef USE_HEAP_5
static uint8_t ucHeap2[16 * 1024];  /* 额外16KB堆 */

static void setup_multi_region_heap(void)
{
    HeapRegion_t xHeapRegions[] = {
        { (uint8_t *)ucHeap2, sizeof(ucHeap2) },
        { NULL, 0 }  /* 终止标记 */
    };

    vPortDefineHeapRegions(xHeapRegions);
}
#endif

int main(void)
{
    printf("\n=== FreeRTOS Memory Management Demo ===\n\n");

    /* 打印初始堆状态 */
    printf("[Main] Initial heap size: %zu bytes\n", xPortGetFreeHeapSize());

    /* 内存分配演示 */
    memory_allocation_demo();

    /* 静态分配演示 */
    MyObject_t *obj = allocate_static_object();
    if (obj != NULL) {
        strncpy(obj->name, "StaticObject", 31);
        printf("[Main] Static object: %s\n", obj->name);
        free_static_object(obj);
    }

    /* 内存池演示 */
    void *pool_ptr = pool_allocate();
    if (pool_ptr != NULL) {
        printf("[Main] Pool allocation successful\n");
        pool_free(pool_ptr);
    }

    /* 打印堆统计 */
    print_heap_stats();

    /* 创建内存管理任务 */
    TaskHandle_t xTask = xTaskCreateStatic(
        vMemoryTask,
        "MemTask",
        TASK_STACK_SIZE,
        NULL,
        1,
        xTaskStack,
        &xTaskTCB
    );

    if (xTask == NULL) {
        printf("[Main] Failed to create task\n");
        return 1;
    }

    vTaskStartScheduler();

    return 0;
}
