/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Industrial_Advanced.md
 * Line: 968
 * Language: c
 * Block ID: 3de1930d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// RT-Thread: 国产RTOS，组件丰富
// Zephyr: Linux基金会项目，安全认证
// ============================================================

// ========== RT-Thread 示例 ==========
#ifdef RT_USING_RTTHREAD

#include <rtthread.h>
#include <rtdevice.h>

// 线程创建
static rt_thread_t sensor_thread = RT_NULL;

static void sensor_entry(void *parameter) {
    while (1) {
        // 读取传感器
        float temp = read_temperature();

        // 发送消息到消息队列
        rt_mq_send(&sensor_mq, &temp, sizeof(temp));

        rt_thread_mdelay(1000);
    }
}

void sensor_thread_init(void) {
    sensor_thread = rt_thread_create("sensor",
                                     sensor_entry,
                                     RT_NULL,
                                     1024,
                                     RT_THREAD_PRIORITY_MAX / 3,
                                     20);
    if (sensor_thread != RT_NULL) {
        rt_thread_startup(sensor_thread);
    }
}

// 设备驱动框架
static rt_size_t sensor_read(rt_device_t dev,
                              rt_off_t pos,
                              void *buffer,
                              rt_size_t size) {
    struct sensor_device *sensor = (struct sensor_device *)dev;
    return sensor->ops->read(sensor, pos, buffer, size);
}

// 自动初始化
INIT_APP_EXPORT(sensor_thread_init);

#endif

// ========== Zephyr RTOS 示例 ==========
#ifdef CONFIG_ZEPHYR

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

// 工作队列 (类似Linux workqueue)
static struct k_work sensor_work;
static struct k_work_q sensor_work_q;
static K_THREAD_STACK_DEFINE(sensor_stack, 2048);

static void sensor_work_handler(struct k_work *work) {
    const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(bme280));
    struct sensor_value temp, press, humidity;

    sensor_sample_fetch(dev);
    sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &temp);
    sensor_channel_get(dev, SENSOR_CHAN_PRESS, &press);
    sensor_channel_get(dev, SENSOR_CHAN_HUMIDITY, &humidity);

    // 处理数据...
}

void zephyr_init(void) {
    // 初始化工作队列
    k_work_queue_init(&sensor_work_q);
    k_work_queue_start(&sensor_work_q, sensor_stack,
                       K_THREAD_STACK_SIZEOF(sensor_stack),
                       10, NULL);

    // 初始化工作项
    k_work_init(&sensor_work, sensor_work_handler);

    // 提交工作
    k_work_submit_to_queue(&sensor_work_q, &sensor_work);
}

// 定时器 (k_timer)
static struct k_timer sensor_timer;

static void timer_expiry(struct k_timer *timer_id) {
    k_work_submit(&sensor_work);
}

void timer_init(void) {
    k_timer_init(&sensor_timer, timer_expiry, NULL);
    k_timer_start(&sensor_timer, K_SECONDS(1), K_SECONDS(1));
}

#endif
