/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 823
 * Language: c
 * Block ID: 1786c224
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// FreeRTOS高级配置与优化
// 针对嵌入式系统的实时性调优
// ============================================================

// FreeRTOSConfig.h 关键配置
#define configUSE_PREEMPTION                    1
#define configUSE_TIME_SLICING                  1
#define configUSE_TICKLESS_IDLE                 1  // 低功耗关键
#define configCPU_CLOCK_HZ                      160000000
#define configTICK_RATE_HZ                      1000  // 1ms tick
#define configMAX_PRIORITIES                    32
#define configMINIMAL_STACK_SIZE                1024
#define configTOTAL_HEAP_SIZE                   (64 * 1024)

// 互斥量配置 (优先级继承防止优先级反转)
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             1
#define configUSE_COUNTING_SEMAPHORES           1

// 运行时统计
#define configGENERATE_RUN_TIME_STATS           1
#define configUSE_TRACE_FACILITY                1

// 任务通知 (比信号量更高效)
#define configUSE_TASK_NOTIFICATIONS            1

// ========== 任务优先级设计 ==========
// 优先级数字越大，优先级越高

#define PRIO_ISR_HANDLER        6   // 最高，中断底半部
#define PRIO_MOTOR_CONTROL      5   // 电机控制，必须实时
#define PRIO_SENSOR_READ        4   // 传感器读取
#define PRIO_COMMUNICATION      3   // 通信处理
#define PRIO_DATA_LOGGING       2   // 数据记录
#define PRIO_UI                 1   // 用户界面
#define PRIO_IDLE               0   // 空闲任务

// ========== 零拷贝队列 ==========
// 大块数据传输，避免复制

typedef struct {
    void *data_ptr;      // 指向实际数据的指针
    size_t length;       // 数据长度
    void (*free_fn)(void*);  // 释放函数
} ZeroCopyBuffer;

QueueHandle_t zero_copy_queue;

void zero_copy_init(void) {
    zero_copy_queue = xQueueCreate(10, sizeof(ZeroCopyBuffer));
}

// 生产者
void producer_send_data(uint8_t *data, size_t len) {
    ZeroCopyBuffer buf = {
        .data_ptr = data,
        .length = len,
        .free_fn = free,
    };

    // 发送指针，不复制数据
    if (xQueueSend(zero_copy_queue, &buf, portMAX_DELAY) != pdPASS) {
        // 发送失败，释放内存
        free(data);
    }
}

// 消费者
void consumer_process(void) {
    ZeroCopyBuffer buf;
    if (xQueueReceive(zero_copy_queue, &buf, portMAX_DELAY) == pdPASS) {
        // 处理数据
        process_buffer(buf.data_ptr, buf.length);

        // 释放内存
        if (buf.free_fn) {
            buf.free_fn(buf.data_ptr);
        }
    }
}

// ========== 内存池管理 ==========
// 避免动态分配碎片

#define BLOCK_SIZE      256
#define BLOCK_COUNT     32

static uint8_t memory_pool[BLOCK_COUNT][BLOCK_SIZE];
static uint8_t pool_used[BLOCK_COUNT];
static SemaphoreHandle_t pool_mutex;

void memory_pool_init(void) {
    memset(pool_used, 0, sizeof(pool_used));
    pool_mutex = xSemaphoreCreateMutex();
}

void *pool_alloc(void) {
    xSemaphoreTake(pool_mutex, portMAX_DELAY);

    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (!pool_used[i]) {
            pool_used[i] = 1;
            xSemaphoreGive(pool_mutex);
            return memory_pool[i];
        }
    }

    xSemaphoreGive(pool_mutex);
    return NULL;  // 无可用块
}

void pool_free(void *ptr) {
    if (ptr == NULL) return;

    xSemaphoreTake(pool_mutex, portMAX_DELAY);

    int idx = ((uint8_t*)ptr - (uint8_t*)memory_pool) / BLOCK_SIZE;
    if (idx >= 0 && idx < BLOCK_COUNT) {
        pool_used[idx] = 0;
    }

    xSemaphoreGive(pool_mutex);
}

// ========== Tickless Idle配置 ==========
// 低功耗关键：空闲时停止tick

extern void vApplicationSleep(TickType_t xExpectedIdleTime);

void vApplicationSleep(TickType_t xExpectedIdleTime) {
    // 计算可以睡眠的时间 (us)
    uint64_t sleep_us = xExpectedIdleTime * 1000;

    // 配置RTC唤醒
    esp_sleep_enable_timer_wakeup(sleep_us);

    // 进入light sleep
    esp_light_sleep_start();
}
