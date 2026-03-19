/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 3917
 * Language: c
 * Block ID: 108d0c32
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 内存管理示例
 * 文件: zephyr_memory_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/mem_stats.h>

LOG_MODULE_REGISTER(memory_example, LOG_LEVEL_DBG);

/* 堆内存池大小 - 在prj.conf中定义 */
/* CONFIG_HEAP_MEM_POOL_SIZE=8192 */

/* 对象结构 */
struct my_object {
    uint32_t id;
    char name[32];
    uint32_t data[10];
};

/* 静态对象池 */
#define MAX_STATIC_OBJECTS  10
static struct my_object static_objects[MAX_STATIC_OBJECTS];
static atomic_t object_used[MAX_STATIC_OBJECTS];

/* 内存slab - 固定大小对象 */
K_MEM_SLAB_DEFINE(my_slab, sizeof(struct my_object), 20, 4);

/* 内存池 - 可变大小对象 */
K_MEM_POOL_DEFINE(my_pool, 64, 1024, 4, 4);

/* 静态堆 - 独立内存区域 */
static uint8_t static_heap[16 * 1024];
STRUCT_SECTION_ITERABLE(k_heap, my_heap);

/*
 * 动态内存分配示例
 */
static void memory_allocation_demo(void)
{
    void *ptr1, *ptr2, *ptr3;
    struct my_object *obj;

    /* 基本分配 */
    ptr1 = k_malloc(100);
    if (ptr1 != NULL) {
        LOG_INF("[MemDemo] Allocated 100 bytes at %p", ptr1);
        memset(ptr1, 0, 100);
    }

    /* 对齐分配 */
    ptr2 = k_aligned_alloc(8, 128);
    if (ptr2 != NULL) {
        LOG_INF("[MemDemo] Allocated 128 bytes aligned to 8 at %p", ptr2);
    }

    /* 分配并清零 */
    ptr3 = k_calloc(1, sizeof(struct my_object));
    if (ptr3 != NULL) {
        LOG_INF("[MemDemo] Allocated and zeroed object at %p", ptr3);
    }

    /* 分配对象 */
    obj = (struct my_object *)k_malloc(sizeof(struct my_object));
    if (obj != NULL) {
        obj->id = 1;
        strncpy(obj->name, "TestObject", 31);
        LOG_INF("[MemDemo] Created object: id=%u, name=%s", obj->id, obj->name);
    }

    /* 释放内存 */
    k_free(ptr1);
    k_free(ptr2);
    k_free(ptr3);
    k_free(obj);

    LOG_INF("[MemDemo] Memory freed");
}

/*
 * 静态对象分配
 */
static struct my_object* allocate_static_object(void)
{
    for (int i = 0; i < MAX_STATIC_OBJECTS; i++) {
        uint32_t expected = 0;
        if (atomic_cas(&object_used[i], &expected, 1)) {
            memset(&static_objects[i], 0, sizeof(struct my_object));
            static_objects[i].id = i;
            return &static_objects[i];
        }
    }
    return NULL;
}

static void free_static_object(struct my_object *obj)
{
    if (obj >= static_objects && obj < static_objects + MAX_STATIC_OBJECTS) {
        int index = obj - static_objects;
        atomic_set(&object_used[index], 0);
    }
}

/*
 * 内存slab使用
 */
static void slab_demo(void)
{
    struct my_object *obj;

    /* 从slab分配 */
    int ret = k_mem_slab_alloc(&my_slab, (void **)&obj, K_NO_WAIT);
    if (ret == 0) {
        obj->id = 100;
        strncpy(obj->name, "SlabObject", 31);
        LOG_INF("[SlabDemo] Allocated from slab: %s", obj->name);

        /* 使用完毕释放 */
        k_mem_slab_free(&my_slab, (void *)obj);
        LOG_INF("[SlabDemo] Freed to slab");
    }

    /* 查询slab状态 */
    LOG_INF("[SlabDemo] Free blocks: %u", k_mem_slab_num_free_get(&my_slab));
}

/*
 * 内存池使用
 */
static void memory_pool_demo(void)
{
    struct k_mem_block block;

    /* 从池中分配 */
    int ret = k_mem_pool_alloc(&my_pool, &block, 256, K_NO_WAIT);
    if (ret == 0) {
        LOG_INF("[PoolDemo] Allocated 256 bytes from pool at %p", block.data);

        /* 使用内存 */
        memset(block.data, 0xAA, 256);

        /* 释放 */
        k_mem_pool_free(&block);
        LOG_INF("[PoolDemo] Freed to pool");
    }
}

/*
 * 自定义堆使用
 */
static void custom_heap_demo(void)
{
    /* 初始化自定义堆 */
    k_heap_init(&my_heap, static_heap, sizeof(static_heap));

    /* 从自定义堆分配 */
    void *ptr = k_heap_alloc(&my_heap, 512, K_NO_WAIT);
    if (ptr != NULL) {
        LOG_INF("[HeapDemo] Allocated 512 bytes from custom heap at %p", ptr);

        /* 使用 */
        memset(ptr, 0, 512);

        /* 释放 */
        k_heap_free(&my_heap, ptr);
        LOG_INF("[HeapDemo] Freed to custom heap");
    }
}

/*
 * 内存统计
 */
static void print_memory_stats(void)
{
    /* 全局堆统计 */
    LOG_INF("\n[MemStats] Memory Statistics:");
    LOG_INF("  Free heap: %zu bytes", k_free_get());

    /* Slab统计 */
    LOG_INF("  Slab free blocks: %u", k_mem_slab_num_free_get(&my_slab));
    LOG_INF("  Slab used blocks: %u", k_mem_slab_num_used_get(&my_slab));
}

/*
 * 内存管理线程
 */
static void memory_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    void *allocations[10];
    int alloc_count = 0;

    LOG_INF("[MemoryThread] Started");

    while (1) {
        /* 模拟分配和释放 */
        if (alloc_count < 10) {
            allocations[alloc_count] = k_malloc(100 + (alloc_count * 10));
            if (allocations[alloc_count] != NULL) {
                LOG_INF("[MemoryThread] Allocated block %d at %p",
                        alloc_count, allocations[alloc_count]);
                alloc_count++;
            }
        } else {
            /* 释放所有 */
            for (int i = 0; i < alloc_count; i++) {
                k_free(allocations[i]);
            }
            LOG_INF("[MemoryThread] Freed all allocations");
            alloc_count = 0;

            print_memory_stats();
        }

        k_sleep(K_SECONDS(1));
    }
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(memory_thread_stack, 2048);
static struct k_thread memory_thread_data;

int main(void)
{
    printk("\n=== Zephyr RTOS Memory Management Demo ===\n\n");

    /* 打印初始堆状态 */
    LOG_INF("[Main] Initial free heap: %zu bytes", k_free_get());

    /* 内存分配演示 */
    memory_allocation_demo();

    /* 静态分配演示 */
    struct my_object *obj = allocate_static_object();
    if (obj != NULL) {
        strncpy(obj->name, "StaticObject", 31);
        LOG_INF("[Main] Static object: %s", obj->name);
        free_static_object(obj);
    }

    /* Slab演示 */
    slab_demo();

    /* 内存池演示 */
    memory_pool_demo();

    /* 自定义堆演示 */
    custom_heap_demo();

    /* 打印统计 */
    print_memory_stats();

    /* 创建内存管理线程 */
    k_thread_create(&memory_thread_data, memory_thread_stack,
                    K_THREAD_STACK_SIZEOF(memory_thread_stack),
                    memory_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(10));
    }

    return 0;
}
