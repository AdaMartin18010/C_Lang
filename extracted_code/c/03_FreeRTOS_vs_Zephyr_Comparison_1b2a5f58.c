/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 3272
 * Language: c
 * Block ID: 1b2a5f58
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 中断处理示例
 * 文件: zephyr_interrupt_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/timer/system_timer.h>

LOG_MODULE_REGISTER(interrupt_example, LOG_LEVEL_DBG);

/* 中断优先级 */
#define IRQ_PRIORITY_LOW        3
#define IRQ_PRIORITY_MEDIUM     2
#define IRQ_PRIORITY_HIGH       1
#define IRQ_PRIORITY_HIGHEST    0

/* 同步对象 */
static K_SEM_DEFINE(isr_sem, 0, 1);
static K_EVENT_DEFINE(isr_events);

/* 工作队列用于延迟处理 */
static K_WORK_DEFINE(deferred_work, NULL);
static struct k_work_q deferred_work_q;
K_THREAD_STACK_DEFINE(deferred_work_q_stack, 2048);

/* 直接中断工作项 */
struct deferred_work_item {
    struct k_work work;
    uint32_t data;
    uint32_t timestamp;
};

/* 统计 */
static atomic_t isr_count;
static atomic_t deferred_count;

/*
 * 直接ISR - 零延迟
 * 使用IRQ_CONNECT宏注册
 */
static void zero_latency_isr(const void *param)
{
    ARG_UNUSED(param);

    /* 最高优先级中断 - 不使用任何内核API */
    atomic_inc(&isr_count);

    /* 直接硬件操作 */
    /* 清除中断标志等 */
}

/*
 * 标准ISR - 使用内核API
 */
static void standard_isr(const void *param)
{
    ARG_UNUSED(param);

    /* 增加计数 */
    uint32_t count = atomic_inc(&isr_count);

    /* 释放信号量通知任务 */
    k_sem_give(&isr_sem);

    /* 设置事件 */
    k_event_post(&isr_events, BIT(0));

    LOG_DBG("[ISR] Standard ISR executed, count=%u", count);
}

/*
 * 延迟处理工作函数
 */
static void deferred_work_handler(struct k_work *work)
{
    struct deferred_work_item *item = CONTAINER_OF(work, struct deferred_work_item, work);

    atomic_inc(&deferred_count);

    LOG_INF("[DeferredWork] Processing data=%u, timestamp=%u, latency=%u ms",
            item->data, item->timestamp, k_uptime_get_32() - item->timestamp);

    /* 执行复杂处理 */
    k_sleep(K_MSEC(10));

    /* 释放工作项内存（如果是动态分配） */
    /* k_free(item); */
}

/*
 * 延迟处理ISR
 */
static void deferred_isr(const void *param)
{
    ARG_UNUSED(param);

    static struct deferred_work_item work_item;

    /* 填充工作项数据 */
    work_item.data = atomic_get(&isr_count);
    work_item.timestamp = k_uptime_get_32();

    /* 初始化工作 */
    k_work_init(&work_item.work, deferred_work_handler);

    /* 提交到工作队列 */
    k_work_submit_to_queue(&deferred_work_q, &work_item.work);

    LOG_DBG("[ISR] Deferred work submitted");
}

/*
 * 临界区演示
 */
static void critical_section_demo(void)
{
    /* 方法1: 锁定中断（禁用所有中断） */
    int key = irq_lock();
    {
        /* 临界区代码 */
        atomic_inc(&isr_count);
    }
    irq_unlock(key);

    /* 方法2: 使用互斥锁（仅任务级别） */
    /* k_mutex_lock(&mutex, K_FOREVER); */
    /* ... */
    /* k_mutex_unlock(&mutex); */

    /* 方法3: 使用自旋锁（SMP系统） */
    /* static struct k_spinlock my_lock; */
    /* k_spinlock_key_t key = k_spin_lock(&my_lock); */
    /* ... */
    /* k_spin_unlock(&my_lock, key); */
}

/*
 * 中断处理线程
 */
static void interrupt_handler_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[IntHandlerThread] Started");

    while (1) {
        /* 等待中断信号 */
        if (k_sem_take(&isr_sem, K_FOREVER) == 0) {
            uint32_t count = atomic_get(&isr_count);

            LOG_INF("[IntHandlerThread] ISR count=%u", count);

            /* 检查事件 */
            uint32_t events = k_event_test(&isr_events, BIT(0));
            if (events & BIT(0)) {
                LOG_DBG("[IntHandlerThread] Timer event detected");
                /* 消费事件 */
                k_event_clear(&isr_events, BIT(0));
            }
        }
    }
}

/*
 * 多事件中断处理线程
 */
static void multi_event_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct k_poll_event events[3];

    /* 初始化poll事件 */
    k_poll_event_init(&events[0], K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &isr_sem);
    k_poll_event_init(&events[1], K_POLL_TYPE_SIGNAL,
                      K_POLL_MODE_NOTIFY_ONLY, NULL);
    k_poll_event_init(&events[2], K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, NULL);

    LOG_INF("[MultiEventThread] Started");

    while (1) {
        /* 等待多个中断源 */
        int rc = k_poll(events, 3, K_FOREVER);

        if (rc == 0) {
            /* 处理信号量事件 */
            if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
                k_sem_take(&isr_sem, K_NO_WAIT);
                LOG_INF("[MultiEventThread] Semaphore event");
                events[0].state = K_POLL_STATE_NOT_READY;
            }

            /* 处理信号事件 */
            if (events[1].state == K_POLL_STATE_SIGNALED) {
                LOG_INF("[MultiEventThread] Signal event");
                events[1].state = K_POLL_STATE_NOT_READY;
            }
        }
    }
}

/*
 * GPIO中断示例（使用设备树）
 */
#define SW0_NODE    DT_ALIAS(sw0)

#if DT_NODE_HAS_STATUS(SW0_NODE, okay)
static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET_OR(SW0_NODE, gpios, {0});
static struct gpio_callback button_cb_data;

static void button_pressed(const struct device *dev, struct gpio_callback *cb,
                           uint32_t pins)
{
    ARG_UNUSED(dev);
    ARG_UNUSED(cb);
    ARG_UNUSED(pins);

    LOG_INF("[GPIO_ISR] Button pressed!");

    /* 释放信号量通知主任务 */
    k_sem_give(&isr_sem);
}
#endif

/*
 * 动态中断连接示例
 */
static void dynamic_isr_install_demo(void)
{
    /* Zephyr 2.6+ 支持动态中断连接 */

    /* IRQ_CONNECT编译时静态连接（推荐） */
    /* IRQ_CONNECT(TIM3_IRQn, IRQ_PRIORITY_MEDIUM, standard_isr, NULL, 0); */

    /* 使能中断 */
    /* irq_enable(TIM3_IRQn); */
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(int_handler_stack, 2048);
K_THREAD_STACK_DEFINE(multi_event_stack, 2048);

static struct k_thread int_handler_thread_data;
static struct k_thread multi_event_thread_data;

/*
 * 主函数
 */
int main(void)
{
    printk("\n=== Zephyr RTOS Interrupt Handling Demo ===\n\n");

    /* 初始化延迟工作队列 */
    k_work_q_start(&deferred_work_q, deferred_work_q_stack,
                   K_THREAD_STACK_SIZEOF(deferred_work_q_stack),
                   K_PRIO_PREEMPT(3));

    /* 创建中断处理线程 */
    k_thread_create(&int_handler_thread_data, int_handler_stack,
                    K_THREAD_STACK_SIZEOF(int_handler_stack),
                    interrupt_handler_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);

    k_thread_create(&multi_event_thread_data, multi_event_stack,
                    K_THREAD_STACK_SIZEOF(multi_event_stack),
                    multi_event_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);

    /* GPIO中断设置（如果硬件支持） */
    #if DT_NODE_HAS_STATUS(SW0_NODE, okay)
    if (!device_is_ready(button.port)) {
        LOG_ERR("[Main] Button device not ready");
    } else {
        int ret = gpio_pin_configure_dt(&button, GPIO_INPUT);
        if (ret != 0) {
            LOG_ERR("[Main] Failed to configure button");
        } else {
            ret = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
            if (ret != 0) {
                LOG_ERR("[Main] Failed to configure button interrupt");
            } else {
                gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
                gpio_add_callback(button.port, &button_cb_data);
                LOG_INF("[Main] Button interrupt configured");
            }
        }
    }
    #endif

    LOG_INF("[Main] System initialized");

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(5));

        LOG_INF("[Main] Stats: isr_count=%u, deferred_count=%u",
                atomic_get(&isr_count), atomic_get(&deferred_count));
    }

    return 0;
}
