/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 1847
 * Language: c
 * Block ID: a7350aaf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 事件与通知示例
 * 文件: zephyr_event_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(event_example, LOG_LEVEL_DBG);

/* 事件定义 */
#define EVENT_WIFI_CONNECTED    BIT(0)
#define EVENT_WIFI_GOT_IP       BIT(1)
#define EVENT_MQTT_CONNECTED    BIT(2)
#define EVENT_SENSOR_READY      BIT(3)
#define EVENT_DATA_READY        BIT(4)

#define EVENT_NETWORK_READY     (EVENT_WIFI_CONNECTED | EVENT_WIFI_GOT_IP)
#define EVENT_ALL_READY         (EVENT_NETWORK_READY | EVENT_MQTT_CONNECTED)

/* 事件对象 */
static K_EVENT_DEFINE(system_events);

/* 轮询对象 */
static K_POLL_SIGNAL_DEFINE(wifi_signal);
static K_POLL_SIGNAL_DEFINE(mqtt_signal);

/*
 * WiFi管理线程
 */
static void wifi_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        LOG_INF("[WiFiThread] Connecting to WiFi...");
        k_sleep(K_SECONDS(2));

        /* WiFi连接成功 */
        k_event_post(&system_events, EVENT_WIFI_CONNECTED);
        LOG_INF("[WiFiThread] WiFi connected");

        /* 触发信号 */
        k_poll_signal_raise(&wifi_signal, 1);

        k_sleep(K_SECONDS(1));

        /* 获取IP地址 */
        k_event_post(&system_events, EVENT_WIFI_GOT_IP);
        LOG_INF("[WiFiThread] Got IP address");

        k_sleep(K_SECONDS(10));
    }
}

/*
 * MQTT客户端线程
 */
static void mqtt_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        LOG_INF("[MQTTThread] Waiting for network...");

        /* 等待网络就绪 - 使用事件 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_NETWORK_READY,
                                        false,           /* 不消费 */
                                        K_SECONDS(10));

        if ((events & EVENT_NETWORK_READY) == EVENT_NETWORK_READY) {
            LOG_INF("[MQTTThread] Network ready, connecting to MQTT broker...");
            k_sleep(K_SECONDS(1));

            /* MQTT连接成功 */
            k_event_post(&system_events, EVENT_MQTT_CONNECTED);
            LOG_INF("[MQTTThread] MQTT connected");

            k_poll_signal_raise(&mqtt_signal, 1);
        } else {
            LOG_WRN("[MQTTThread] Network timeout");
        }

        k_sleep(K_SECONDS(5));
    }
}

/*
 * 传感器线程
 */
static void sensor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        LOG_INF("[SensorThread] Initializing sensors...");
        k_sleep(K_MSEC(500));

        /* 传感器就绪 */
        k_event_post(&system_events, EVENT_SENSOR_READY);
        LOG_INF("[SensorThread] Sensors ready");

        /* 等待所有系统就绪 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_ALL_READY,
                                        false,
                                        K_FOREVER);

        if ((events & EVENT_ALL_READY) == EVENT_ALL_READY) {
            LOG_INF("[SensorThread] All systems ready, starting data collection");

            for (int i = 0; i < 5; i++) {
                k_sleep(K_SECONDS(1));
                LOG_INF("[SensorThread] Collected sample #%d", i + 1);

                /* 数据就绪事件 */
                k_event_post(&system_events, EVENT_DATA_READY);
            }
        }

        k_sleep(K_SECONDS(5));
    }
}

/*
 * 数据处理线程
 */
static void data_processing_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t data_count = 0;

    while (1) {
        /* 等待数据就绪 - 使用带超时的等待 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_DATA_READY,
                                        true,           /* 消费事件 */
                                        K_FOREVER);

        if (events & EVENT_DATA_READY) {
            data_count++;
            LOG_INF("[DataThread] Processing data packet #%u", data_count);
        }
    }
}

/*
 * 系统监控线程
 */
static void monitor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        k_sleep(K_SECONDS(3));

        /* 获取当前事件状态（不消费） */
        uint32_t events = k_event_test(&system_events, 0);

        LOG_INF("[Monitor] Event status:");
        LOG_INF("  WiFi Connected: %s",
                (events & EVENT_WIFI_CONNECTED) ? "YES" : "NO");
        LOG_INF("  Got IP: %s",
                (events & EVENT_WIFI_GOT_IP) ? "YES" : "NO");
        LOG_INF("  MQTT Connected: %s",
                (events & EVENT_MQTT_CONNECTED) ? "YES" : "NO");
        LOG_INF("  Sensor Ready: %s",
                (events & EVENT_SENSOR_READY) ? "YES" : "NO");
        LOG_INF("  Data Ready: %s",
                (events & EVENT_DATA_READY) ? "YES" : "NO");
    }
}

/*
 * 使用k_poll的多路等待
 */
static void poll_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct k_poll_event events[2];

    /* 初始化轮询事件 */
    k_poll_event_init(&events[0], K_POLL_TYPE_SIGNAL,
                      K_POLL_MODE_NOTIFY_ONLY, &wifi_signal);
    k_poll_event_init(&events[1], K_POLL_TYPE_SIGNAL,
                      K_POLL_MODE_NOTIFY_ONLY, &mqtt_signal);

    while (1) {
        LOG_INF("[PollThread] Waiting for signals...");

        /* 等待任一信号 */
        int rc = k_poll(events, 2, K_SECONDS(30));

        if (rc == 0) {
            if (events[0].state == K_POLL_STATE_SIGNALED) {
                LOG_INF("[PollThread] WiFi signal received");
                k_poll_signal_reset(&wifi_signal);
                events[0].state = K_POLL_STATE_NOT_READY;
            }

            if (events[1].state == K_POLL_STATE_SIGNALED) {
                LOG_INF("[PollThread] MQTT signal received");
                k_poll_signal_reset(&mqtt_signal);
                events[1].state = K_POLL_STATE_NOT_READY;
            }
        }
    }
}

/*
 * 屏障同步示例
 */
static K_SEM_DEFINE(barrier_sem, 0, 3);
static atomic_t barrier_count;

static void barrier_thread(void *p1, void *p2, void *p3)
{
    uint32_t task_id = (uint32_t)p1;

    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[BarrierThread%u] Started, doing initialization...", task_id);

    /* 模拟不同初始化时间 */
    k_sleep(K_MSEC(500 * task_id));

    LOG_INF("[BarrierThread%u] Initialization complete, waiting at barrier", task_id);

    /* 增加计数 */
    atomic_inc(&barrier_count);

    /* 等待所有线程到达 */
    if (atomic_get(&barrier_count) == 3) {
        /* 最后一个线程释放所有等待 */
        for (int i = 0; i < 3; i++) {
            k_sem_give(&barrier_sem);
        }
    }

    k_sem_take(&barrier_sem, K_FOREVER);

    LOG_INF("[BarrierThread%u] All threads reached barrier, continuing", task_id);
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(wifi_stack, 2048);
K_THREAD_STACK_DEFINE(mqtt_stack, 2048);
K_THREAD_STACK_DEFINE(sensor_stack, 2048);
K_THREAD_STACK_DEFINE(data_stack, 2048);
K_THREAD_STACK_DEFINE(monitor_stack, 2048);
K_THREAD_STACK_DEFINE(poll_stack, 2048);
K_THREAD_STACK_DEFINE(barrier_stack_1, 1024);
K_THREAD_STACK_DEFINE(barrier_stack_2, 1024);
K_THREAD_STACK_DEFINE(barrier_stack_3, 1024);

static struct k_thread wifi_thread_data;
static struct k_thread mqtt_thread_data;
static struct k_thread sensor_thread_data;
static struct k_thread data_thread_data;
static struct k_thread monitor_thread_data;
static struct k_thread poll_thread_data;
static struct k_thread barrier_thread_data[3];

int main(void)
{
    printk("\n=== Zephyr RTOS Event Demo ===\n\n");

    /* 创建线程 */
    k_thread_create(&wifi_thread_data, wifi_stack,
                    K_THREAD_STACK_SIZEOF(wifi_stack),
                    wifi_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);

    k_thread_create(&mqtt_thread_data, mqtt_stack,
                    K_THREAD_STACK_SIZEOF(mqtt_stack),
                    mqtt_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    k_thread_create(&sensor_thread_data, sensor_stack,
                    K_THREAD_STACK_SIZEOF(sensor_stack),
                    sensor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    k_thread_create(&data_thread_data, data_stack,
                    K_THREAD_STACK_SIZEOF(data_stack),
                    data_processing_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    k_thread_create(&monitor_thread_data, monitor_stack,
                    K_THREAD_STACK_SIZEOF(monitor_stack),
                    monitor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    k_thread_create(&poll_thread_data, poll_stack,
                    K_THREAD_STACK_SIZEOF(poll_stack),
                    poll_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    /* 创建屏障同步线程 */
    k_thread_create(&barrier_thread_data[0], barrier_stack_1,
                    K_THREAD_STACK_SIZEOF(barrier_stack_1),
                    barrier_thread, (void*)1, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    k_thread_create(&barrier_thread_data[1], barrier_stack_2,
                    K_THREAD_STACK_SIZEOF(barrier_stack_2),
                    barrier_thread, (void*)2, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    k_thread_create(&barrier_thread_data[2], barrier_stack_3,
                    K_THREAD_STACK_SIZEOF(barrier_stack_3),
                    barrier_thread, (void*)3, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(30));
        LOG_INF("[Main] System running...");
    }

    return 0;
}
