/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 5569
 * Language: c
 * Block ID: 3b7efe24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS 温度监控系统
 * 文件: zephyr_temp_monitor.c
 * 功能: 采集温度、显示、报警、网络上报
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/net/net_ip.h>
#include <zephyr/net/socket.h>
#include <zephyr/random/rand32.h>

LOG_MODULE_REGISTER(temp_monitor, LOG_LEVEL_DBG);

/* 配置 */
#define TEMP_SAMPLE_PERIOD_MS       1000
#define ALARM_HIGH_THRESHOLD        35.0f
#define ALARM_LOW_THRESHOLD         10.0f
#define MAX_SENSORS                 4

/* 事件定义 */
#define EVENT_SENSOR_OK         BIT(0)
#define EVENT_ALARM_HIGH        BIT(1)
#define EVENT_ALARM_LOW         BIT(2)
#define EVENT_NETWORK_OK        BIT(3)
#define EVENT_DISPLAY_UPDATE    BIT(4)

/* 数据结构 */
struct temp_reading {
    uint8_t sensor_id;
    float temperature;
    uint32_t timestamp;
    uint8_t status;
};

struct temp_stats {
    uint32_t total_readings;
    uint32_t alarm_count;
    float avg_temp;
    float max_temp;
    float min_temp;
};

/* 全局对象 */
K_MSGQ_DEFINE(temp_msgq, sizeof(struct temp_reading), 10, 4);
K_EVENT_DEFINE(system_events);
K_MUTEX_DEFINE(stats_mutex);

/* 定时器 */
static struct k_timer sample_timer;

/* 统计 */
static struct temp_stats stats = {0};
static float current_temps[MAX_SENSORS] = {0};

/*
 * 模拟硬件读取
 */
static float read_sensor_hw(uint8_t sensor_id)
{
    /* 模拟传感器读取 */
    float base = 25.0f;
    float noise = (float)(sys_rand32_get() % 100) / 10.0f - 5.0f;
    float drift = (float)(k_uptime_get_32() % 1000) / 100.0f;
    return base + noise + drift - (sensor_id * 2);
}

/*
 * 采样线程
 */
static void sample_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint8_t current_sensor = 0;

    LOG_INF("[SampleThread] Started");

    while (1) {
        /* 读取所有传感器 */
        for (current_sensor = 0; current_sensor < MAX_SENSORS; current_sensor++) {
            struct temp_reading reading;
            reading.sensor_id = current_sensor;
            reading.temperature = read_sensor_hw(current_sensor);
            reading.timestamp = k_uptime_get_32();
            reading.status = 0;

            /* 验证读数 */
            if (reading.temperature > -40.0f && reading.temperature < 85.0f) {
                reading.status |= EVENT_SENSOR_OK;
            }

            /* 发送到消息队列 */
            int ret = k_msgq_put(&temp_msgq, &reading, K_MSEC(100));
            if (ret != 0) {
                LOG_WRN("[SampleThread] Queue full, dropping reading");
            }

            current_temps[current_sensor] = reading.temperature;
        }

        k_sleep(K_MSEC(TEMP_SAMPLE_PERIOD_MS));
    }
}

/*
 * 处理线程
 */
static void process_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct temp_reading reading;

    LOG_INF("[ProcessThread] Started");

    while (1) {
        /* 等待读数 */
        int ret = k_msgq_get(&temp_msgq, &reading, K_FOREVER);

        if (ret == 0) {
            uint32_t events = 0;

            /* 更新统计 */
            k_mutex_lock(&stats_mutex, K_FOREVER);
            stats.total_readings++;

            /* 计算统计 */
            float sum = 0;
            for (int i = 0; i < MAX_SENSORS; i++) {
                sum += current_temps[i];
            }
            stats.avg_temp = sum / MAX_SENSORS;

            /* 更新最大/最小 */
            if (reading.temperature > stats.max_temp) {
                stats.max_temp = reading.temperature;
            }
            if (reading.temperature < stats.min_temp || stats.min_temp == 0) {
                stats.min_temp = reading.temperature;
            }

            k_mutex_unlock(&stats_mutex);

            /* 检查告警 */
            if (reading.temperature > ALARM_HIGH_THRESHOLD) {
                events |= EVENT_ALARM_HIGH;
                stats.alarm_count++;
                LOG_INF("[ProcessThread] HIGH ALARM: Sensor %u = %.2f C",
                        reading.sensor_id, reading.temperature);
            }

            if (reading.temperature < ALARM_LOW_THRESHOLD) {
                events |= EVENT_ALARM_LOW;
                stats.alarm_count++;
                LOG_INF("[ProcessThread] LOW ALARM: Sensor %u = %.2f C",
                        reading.sensor_id, reading.temperature);
            }

            /* 设置事件 */
            if (events != 0) {
                k_event_post(&system_events, events);
            }

            /* 触发显示更新 */
            k_event_post(&system_events, EVENT_DISPLAY_UPDATE);
        }
    }
}

/*
 * 显示线程
 */
static void display_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[DisplayThread] Started");

    while (1) {
        /* 等待显示更新事件 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_DISPLAY_UPDATE,
                                        true,  /* 清除 */
                                        K_SECONDS(5));

        if (events & EVENT_DISPLAY_UPDATE) {
            printk("\n========== Temperature Monitor ==========\n");

            /* 显示当前读数 */
            for (int i = 0; i < MAX_SENSORS; i++) {
                printk("  Sensor %d: %.2f C\n", i, current_temps[i]);
            }

            /* 显示统计 */
            k_mutex_lock(&stats_mutex, K_FOREVER);
            printk("\n  Statistics:\n");
            printk("    Total readings: %u\n", stats.total_readings);
            printk("    Alarm count: %u\n", stats.alarm_count);
            printk("    Average: %.2f C\n", stats.avg_temp);
            printk("    Max: %.2f C, Min: %.2f C\n",
                   stats.max_temp, stats.min_temp);
            k_mutex_unlock(&stats_mutex);

            printk("========================================\n");
        }
    }
}

/*
 * 告警线程
 */
static void alarm_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[AlarmThread] Started");

    while (1) {
        /* 等待告警事件 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_ALARM_HIGH | EVENT_ALARM_LOW,
                                        true,  /* 清除 */
                                        K_FOREVER);

        if (events & EVENT_ALARM_HIGH) {
            /* 高温告警处理 */
            LOG_INF("[AlarmThread] *** HIGH TEMPERATURE ALARM ***");
        }

        if (events & EVENT_ALARM_LOW) {
            /* 低温告警处理 */
            LOG_INF("[AlarmThread] *** LOW TEMPERATURE ALARM ***");
        }
    }
}

/*
 * 网络线程（模拟）
 */
static void network_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[NetworkThread] Started");

    /* 模拟网络连接 */
    k_sleep(K_SECONDS(2));
    k_event_post(&system_events, EVENT_NETWORK_OK);
    LOG_INF("[NetworkThread] Network connected");

    while (1) {
        /* 等待网络就绪 */
        uint32_t events = k_event_wait(&system_events,
                                        EVENT_NETWORK_OK,
                                        false,  /* 不清除 */
                                        K_FOREVER);

        if (events & EVENT_NETWORK_OK) {
            /* 准备上报数据 */
            char json_buffer[256];
            float avg;

            k_mutex_lock(&stats_mutex, K_FOREVER);
            avg = stats.avg_temp;
            k_mutex_unlock(&stats_mutex);

            snprintf(json_buffer, sizeof(json_buffer),
                "{\"sensors\":[%.2f,%.2f,%.2f,%.2f],\"avg\":%.2f}",
                current_temps[0], current_temps[1],
                current_temps[2], current_temps[3], avg);

            LOG_INF("[NetworkThread] Uploading: %s", json_buffer);

            /* 实际网络发送在这里 */
            k_sleep(K_MSEC(100));
        }

        k_sleep(K_SECONDS(5));
    }
}

/*
 * 定时器回调
 */
static void sample_timer_callback(struct k_timer *timer)
{
    ARG_UNUSED(timer);
    LOG_DBG("[Timer] Sample period elapsed");
}

/* 线程栈定义 */
K_THREAD_STACK_DEFINE(sample_stack, 2048);
K_THREAD_STACK_DEFINE(process_stack, 2048);
K_THREAD_STACK_DEFINE(display_stack, 2048);
K_THREAD_STACK_DEFINE(alarm_stack, 2048);
K_THREAD_STACK_DEFINE(network_stack, 4096);

static struct k_thread sample_thread_data;
static struct k_thread process_thread_data;
static struct k_thread display_thread_data;
static struct k_thread alarm_thread_data;
static struct k_thread network_thread_data;

/*
 * 主函数
 */
int main(void)
{
    printk("\n=== Zephyr RTOS Temperature Monitor ===\n\n");

    /* 初始化定时器 */
    k_timer_init(&sample_timer, sample_timer_callback, NULL);
    k_timer_start(&sample_timer, K_SECONDS(5), K_SECONDS(5));

    /* 创建线程 */
    k_thread_create(&sample_thread_data, sample_stack,
                    K_THREAD_STACK_SIZEOF(sample_stack),
                    sample_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);
    k_thread_name_set(&sample_thread_data, "sample");

    k_thread_create(&process_thread_data, process_stack,
                    K_THREAD_STACK_SIZEOF(process_stack),
                    process_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    k_thread_name_set(&process_thread_data, "process");

    k_thread_create(&display_thread_data, display_stack,
                    K_THREAD_STACK_SIZEOF(display_stack),
                    display_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);
    k_thread_name_set(&display_thread_data, "display");

    k_thread_create(&alarm_thread_data, alarm_stack,
                    K_THREAD_STACK_SIZEOF(alarm_stack),
                    alarm_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);
    k_thread_name_set(&alarm_thread_data, "alarm");

    k_thread_create(&network_thread_data, network_stack,
                    K_THREAD_STACK_SIZEOF(network_stack),
                    network_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);
    k_thread_name_set(&network_thread_data, "network");

    LOG_INF("[Main] System initialized");

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(30));
    }

    return 0;
}
