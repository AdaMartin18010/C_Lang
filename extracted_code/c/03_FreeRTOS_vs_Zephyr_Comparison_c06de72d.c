/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 646
 * Language: c
 * Block ID: c06de72d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 线程管理完整示例
 * 文件: zephyr_thread_example.c
 * 配置: prj.conf
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(thread_example, LOG_LEVEL_DBG);

/* 线程参数结构体 */
struct thread_params {
    uint32_t thread_id;
    uint32_t interval_ms;
    const char *name;
};

/* 线程栈大小定义 */
#define THREAD_STACK_SIZE       2048
#define THREAD_PRIORITY_HIGH    K_PRIO_PREEMPT(0)
#define THREAD_PRIORITY_NORMAL  K_PRIO_PREEMPT(5)
#define THREAD_PRIORITY_LOW     K_PRIO_PREEMPT(10)
#define THREAD_PRIORITY_COOP    K_PRIO_COOP(1)

/* 线程栈空间 - 必须使用K_THREAD_STACK_DEFINE */
K_THREAD_STACK_DEFINE(thread_stack_1, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_stack_2, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(thread_stack_3, THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(workq_stack, THREAD_STACK_SIZE);

/* 线程控制块 */
static struct k_thread thread_data[3];
static struct k_thread workq_thread;

/* 同步对象 */
static K_SEM_DEFINE(start_sem, 0, 1);
static K_EVENT_DEFINE(event_handle);
static K_MUTEX_DEFINE(resource_mutex);

/* 线程参数 */
static struct thread_params thread_params[3] = {
    {1, 100, "Periodic100"},
    {2, 500, "Periodic500"},
    {3, 0,   "EventDriven"}
};

/* 工作队列 */
static struct k_work_q my_work_q;

/*
 * 线程1: 周期性线程
 * 功能: 使用k_sleep实现精确周期
 */
static void periodic_thread(void *p1, void *p2, void *p3)
{
    struct thread_params *params = p1;
    uint32_t execution_count = 0;
    int64_t last_time, current_time;

    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    /* 等待启动信号 */
    k_sem_take(&start_sem, K_FOREVER);

    LOG_INF("[Thread %s] Started, ID=%u, Interval=%u ms",
            params->name, params->thread_id, params->interval_ms);

    last_time = k_uptime_get();

    while (1) {
        /* 精确周期睡眠 */
        k_sleep(K_MSEC(params->interval_ms));

        current_time = k_uptime_get();
        execution_count++;

        LOG_INF("[Thread %s] Execution #%u at time %lld ms (jitter=%lld ms)",
                params->name, execution_count, current_time,
                current_time - last_time - params->interval_ms);

        last_time = current_time;

        /* 模拟工作负载 */
        k_busy_wait(1000);  /* 1ms忙等待 */

        /* 条件挂起演示 */
        if (execution_count >= 10) {
            LOG_INF("[Thread %s] Reached max executions, aborting", params->name);
            k_thread_abort(k_current_get());
        }
    }
}

/*
 * 线程2: 事件驱动线程
 * 功能: 使用k_poll等待多个事件源
 */
static void event_driven_thread(void *p1, void *p2, void *p3)
{
    struct thread_params *params = p1;
    struct k_poll_event events[2];
    int rc;

    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[Thread %s] Event-driven thread started", params->name);

    /* 配置poll事件 */
    k_poll_event_init(&events[0], K_POLL_TYPE_SEM_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, &start_sem);
    k_poll_event_init(&events[1], K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                      K_POLL_MODE_NOTIFY_ONLY, NULL);

    while (1) {
        /* 等待多个事件源 */
        rc = k_poll(events, 1, K_MSEC(5000));

        if (rc == 0) {
            /* 处理事件 */
            if (events[0].state == K_POLL_STATE_SEM_AVAILABLE) {
                k_sem_take(&start_sem, K_NO_WAIT);
                LOG_INF("[Thread %s] Received semaphore event", params->name);
            }

            /* 重置事件状态 */
            events[0].state = K_POLL_STATE_NOT_READY;
        } else if (rc == -EAGAIN) {
            LOG_WRN("[Thread %s] Poll timeout", params->name);
        }
    }
}

/*
 * 线程3: 工作线程
 * 功能: 执行耗时后台任务
 */
static void worker_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[WorkerThread] Started");

    while (1) {
        /* 等待工作 */
        k_sleep(K_MSEC(100));

        /* 获取互斥锁保护共享资源 */
        if (k_mutex_lock(&resource_mutex, K_MSEC(100)) == 0) {
            LOG_DBG("[WorkerThread] Accessing shared resource");

            /* 访问共享资源 */
            k_busy_wait(5000);  /* 5ms工作 */

            k_mutex_unlock(&resource_mutex);
        }
    }
}

/*
 * 系统工作队列处理函数
 */
static void work_handler(struct k_work *work)
{
    ARG_UNUSED(work);
    LOG_INF("[WorkQueue] Processing deferred work");
}

static K_WORK_DEFINE(my_work, work_handler);

/*
 * 初始化线程
 * 功能: 系统初始化后退出
 */
static void init_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[InitThread] System initialization started");

    /* 执行初始化 */
    k_sleep(K_MSEC(100));
    LOG_INF("[InitThread] Hardware initialized");

    k_sleep(K_MSEC(100));
    LOG_INF("[InitThread] Software components initialized");

    /* 提交延迟工作 */
    k_work_submit(&my_work);

    /* 释放启动信号 */
    LOG_INF("[InitThread] Releasing start semaphore");
    k_sem_give(&start_sem);

    LOG_INF("[InitThread] Initialization complete");

    /* 线程自然退出 - 不需要显式abort */
}

/*
 * 线程监控回调
 * 功能: 当线程启动/终止时调用
 */
static void thread_monitor_callback(const struct k_thread *thread,
                                     enum k_thread_state state)
{
    const char *state_str;

    switch (state) {
        case K_THREAD_DEAD:
            state_str = "DEAD";
            break;
        case K_THREAD_SUSPENDED:
            state_str = "SUSPENDED";
            break;
        case K_THREAD_READY:
            state_str = "READY";
            break;
        case K_THREAD_RUNNING:
            state_str = "RUNNING";
            break;
        case K_THREAD_PENDING:
            state_str = "PENDING";
            break;
        default:
            state_str = "UNKNOWN";
            break;
    }

    LOG_DBG("[Monitor] Thread %p state changed to %s", thread, state_str);
}

/*
 * 主函数
 */
int main(void)
{
    printk("\n=== Zephyr RTOS Thread Management Demo ===\n\n");

    /* 创建初始化线程 - 最高优先级 */
    k_thread_create(&thread_data[0], init_thread_stack, THREAD_STACK_SIZE,
                    init_thread, NULL, NULL, NULL,
                    K_HIGHEST_THREAD_PRIO, 0, K_NO_WAIT);

    /* 设置线程名称 */
    k_thread_name_set(&thread_data[0], "init_thread");

    /* 创建周期线程1 */
    k_thread_create(&thread_data[0], thread_stack_1, THREAD_STACK_SIZE,
                    periodic_thread, &thread_params[0], NULL, NULL,
                    THREAD_PRIORITY_HIGH, 0, K_FOREVER);
    k_thread_name_set(&thread_data[0], "periodic_100");

    /* 创建周期线程2 */
    k_thread_create(&thread_data[1], thread_stack_2, THREAD_STACK_SIZE,
                    periodic_thread, &thread_params[1], NULL, NULL,
                    THREAD_PRIORITY_NORMAL, 0, K_FOREVER);
    k_thread_name_set(&thread_data[1], "periodic_500");

    /* 创建事件驱动线程 */
    k_thread_create(&thread_data[2], thread_stack_3, THREAD_STACK_SIZE,
                    event_driven_thread, &thread_params[2], NULL, NULL,
                    THREAD_PRIORITY_LOW, 0, K_FOREVER);
    k_thread_name_set(&thread_data[2], "event_driven");

    /* 启动工作队列 */
    k_work_q_start(&my_work_q, workq_stack,
                   K_THREAD_STACK_SIZEOF(workq_stack),
                   THREAD_PRIORITY_COOP);

    /* 启动创建的线程 */
    k_thread_start(&thread_data[0]);
    k_thread_start(&thread_data[1]);
    k_thread_start(&thread_data[2]);

    LOG_INF("[Main] All threads created, entering main loop");

    /* 主线程保持运行 */
    while (1) {
        k_sleep(K_SECONDS(1));

        /* 打印线程统计信息 */
        for (int i = 0; i < 3; i++) {
            struct k_thread *thread = &thread_data[i];
            LOG_DBG("[Main] Thread %d state: %d", i,
                    thread->base.thread_state);
        }
    }

    return 0;
}
