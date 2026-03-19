/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 2694
 * Language: c
 * Block ID: 53c85ddf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 中断安全编程指南 - 避免竞态条件和数据损坏
 * ============================================================================ */

#include <stdint.h>
#include <stdbool.h>

/* 关键概念:
 * 1. 中断可能随时发生
 * 2. 主循环和中断共享数据时需要保护
 * 3. 某些操作是原子的，某些不是
 */

/* ============================================================================
 * 原子操作
 * ============================================================================ */

/* 8位和16位操作通常是原子的 (在8/16位MCU上) */
/* 32位操作在32位MCU上是原子的 */

volatile uint8_t  atomic_byte;   /* 读写是原子的 */
volatile uint16_t atomic_half;   /* 16位MCU上是原子的 */
volatile uint32_t atomic_word;   /* 32位MCU上是原子的 */

/* 非原子操作示例 - 需要保护 */
typedef struct {
    uint8_t status;
    uint16_t count;
} sensor_data_t;

volatile sensor_data_t shared_data;  /* 读写都不是原子的! */

/* ============================================================================
 * 临界区保护 (Cortex-M)
 * ============================================================================ */

/* 进入临界区 */
#define ENTER_CRITICAL()    uint32_t primask = __get_PRIMASK(); \
                            __disable_irq()

/* 退出临界区 */
#define EXIT_CRITICAL()     __set_PRIMASK(primask)

/* 临界区使用示例 */
void update_shared_data(uint8_t status, uint16_t count)
{
    ENTER_CRITICAL();
    /* 以下操作是原子的 - 中断被禁用 */
    shared_data.status = status;
    shared_data.count = count;
    EXIT_CRITICAL();
}

/* 读取共享数据 */
void read_shared_data(uint8_t *status, uint16_t *count)
{
    ENTER_CRITICAL();
    *status = shared_data.status;
    *count = shared_data.count;
    EXIT_CRITICAL();
}

/* ============================================================================
 * 无锁队列 - 单生产者单消费者
 * ============================================================================ */

/* 当只有一个生产者和一个消费者时，不需要临界区保护 */

typedef struct {
    uint8_t buffer[64];
    volatile uint8_t head;   /* 生产者写入 */
    volatile uint8_t tail;   /* 消费者读取 */
} lockless_queue_t;

lockless_queue_t irq_queue;

/* 生产者在中断中调用 */
bool irq_queue_put(uint8_t data)
{
    uint8_t next_head = (irq_queue.head + 1) % sizeof(irq_queue.buffer);

    /* 检查队列满 - 原子读取tail */
    if (next_head == irq_queue.tail) {
        return false;  /* 满 */
    }

    irq_queue.buffer[irq_queue.head] = data;
    irq_queue.head = next_head;
    return true;
}

/* 消费者在主循环中调用 */
bool irq_queue_get(uint8_t *data)
{
    /* 检查队列空 - 原子读取head */
    if (irq_queue.head == irq_queue.tail) {
        return false;  /* 空 */
    }

    *data = irq_queue.buffer[irq_queue.tail];
    irq_queue.tail = (irq_queue.tail + 1) % sizeof(irq_queue.buffer);
    return true;
}

/* ============================================================================
 * 双缓冲技术
 * ============================================================================ */

typedef struct {
    uint8_t buffer_a[32];
    uint8_t buffer_b[32];
    volatile uint8_t *volatile active_buffer;    /* ISR写入 */
    volatile uint8_t *volatile processing_buffer; /* 主循环处理 */
    volatile bool data_ready;
} double_buffer_t;

double_buffer_t dbuf = {
    .active_buffer = dbuf.buffer_a,
    .processing_buffer = NULL,
    .data_ready = false
};

/* 中断服务程序 - 填充缓冲区 */
void ISR_data_received(void)
{
    static uint8_t pos = 0;
    uint8_t data = UART->DR;

    if (!dbuf.data_ready && dbuf.active_buffer) {
        dbuf.active_buffer[pos++] = data;

        if (pos >= 32 || data == '\n') {
            /* 缓冲区满或行结束 - 交换 */
            dbuf.processing_buffer = dbuf.active_buffer;
            dbuf.active_buffer = (dbuf.active_buffer == dbuf.buffer_a)
                                 ? dbuf.buffer_b : dbuf.buffer_a;
            dbuf.data_ready = true;
            pos = 0;
        }
    }
}

/* 主循环 - 处理缓冲区 */
void process_buffered_data(void)
{
    if (dbuf.data_ready) {
        /* 处理 dbuf.processing_buffer */
        process_data(dbuf.processing_buffer, 32);

        /* 清除标志 */
        dbuf.data_ready = false;
    }
}

/* ============================================================================
 * 中断安全的标志操作
 * ============================================================================ */

#define ATOMIC_SET(ptr, val)      __atomic_store_n(ptr, val, __ATOMIC_SEQ_CST)
#define ATOMIC_GET(ptr)           __atomic_load_n(ptr, __ATOMIC_SEQ_CST)
#define ATOMIC_OR(ptr, val)       __atomic_fetch_or(ptr, val, __ATOMIC_SEQ_CST)
#define ATOMIC_AND(ptr, val)      __atomic_fetch_and(ptr, val, __ATOMIC_SEQ_CST)

volatile uint32_t event_flags = 0;

#define EVENT_UART_RX     (1 << 0)
#define EVENT_TIMER       (1 << 1)
#define EVENT_ADC_DONE    (1 << 2)

/* 中断中设置标志 */
void UART_IRQHandler(void)
{
    ATOMIC_OR(&event_flags, EVENT_UART_RX);
}

void TIM_IRQHandler(void)
{
    ATOMIC_OR(&event_flags, EVENT_TIMER);
}

/* 主循环处理标志 */
void event_loop(void)
{
    uint32_t flags = ATOMIC_GET(&event_flags);

    if (flags & EVENT_UART_RX) {
        ATOMIC_AND(&event_flags, ~EVENT_UART_RX);
        handle_uart_rx();
    }

    if (flags & EVENT_TIMER) {
        ATOMIC_AND(&event_flags, ~EVENT_TIMER);
        handle_timer();
    }
}
