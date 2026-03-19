/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\03_FreeRTOS_vs_Zephyr_Comparison.md
 * Line: 2559
 * Language: c
 * Block ID: b02cb14e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Zephyr RTOS FIFO与消息队列示例
 * 文件: zephyr_queue_example.c
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(queue_example, LOG_LEVEL_DBG);

/* 消息类型 */
enum msg_type {
    MSG_TYPE_SENSOR_DATA = 0,
    MSG_TYPE_CONTROL_CMD,
    MSG_TYPE_STATUS_REQ,
    MSG_TYPE_CONFIG_UPDATE
};

/* 传感器数据 */
struct sensor_data {
    uint32_t timestamp;
    float temperature;
    float humidity;
    float pressure;
    uint32_t sequence_num;
};

/* 控制命令 */
struct control_command {
    uint32_t command_id;
    uint32_t param1;
    uint32_t param2;
    char description[32];
};

/* FIFO节点 - 用于FIFO方式传递 */
struct data_item {
    void *fifo_reserved;  /* FIFO内部使用 */
    enum msg_type type;
    union {
        struct sensor_data sensor;
        struct control_command command;
        uint32_t raw_data[16];
    } payload;
};

/* 定义FIFO */
K_FIFO_DEFINE(sensor_fifo);

/* 定义消息队列 - 用于传递数据的副本 */
K_MSGQ_DEFINE(sensor_msgq, sizeof(struct sensor_data), 20, 4);
K_MSGQ_DEFINE(command_msgq, sizeof(struct control_command), 10, 4);

/* 内存池 - 用于FIFO节点分配 */
K_MEM_SLAB_DEFINE(data_item_slab, sizeof(struct data_item), 30, 4);

/* 统计 */
static atomic_t messages_sent;
static atomic_t messages_received;
static atomic_t queue_full_count;

/*
 * 传感器数据采集线程 - 使用FIFO
 */
static void sensor_fifo_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t sequence = 0;

    LOG_INF("[SensorFIFOThread] Started");

    while (1) {
        /* 从内存池分配节点 */
        struct data_item *item;
        int ret = k_mem_slab_alloc(&data_item_slab, (void**)&item, K_MSEC(100));

        if (ret == 0) {
            /* 填充数据 */
            sequence++;
            item->type = MSG_TYPE_SENSOR_DATA;
            item->payload.sensor.timestamp = k_uptime_get_32();
            item->payload.sensor.temperature = 25.0f + (float)(sequence % 10);
            item->payload.sensor.humidity = 50.0f + (float)(sequence % 20);
            item->payload.sensor.pressure = 1013.25f + (float)(sequence % 5);
            item->payload.sensor.sequence_num = sequence;

            /* 放入FIFO */
            k_fifo_put(&sensor_fifo, item);
            atomic_inc(&messages_sent);

            if (sequence % 10 == 0) {
                LOG_INF("[SensorFIFOThread] Sent sensor data #%u (T=%.1f, H=%.1f)",
                        sequence, item->payload.sensor.temperature,
                        item->payload.sensor.humidity);
            }
        } else {
            atomic_inc(&queue_full_count);
            LOG_WRN("[SensorFIFOThread] Memory slab full, data dropped");
        }

        k_sleep(K_MSEC(100));
    }
}

/*
 * 传感器数据采集线程 - 使用消息队列
 */
static void sensor_msgq_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t sequence = 0;

    LOG_INF("[SensorMsgqThread] Started");

    while (1) {
        struct sensor_data data;

        /* 填充数据 */
        sequence++;
        data.timestamp = k_uptime_get_32();
        data.temperature = 25.0f + (float)(sequence % 10);
        data.humidity = 50.0f + (float)(sequence % 20);
        data.pressure = 1013.25f + (float)(sequence % 5);
        data.sequence_num = sequence;

        /* 发送到消息队列 */
        int ret = k_msgq_put(&sensor_msgq, &data, K_MSEC(100));

        if (ret == 0) {
            atomic_inc(&messages_sent);
        } else {
            atomic_inc(&queue_full_count);
            LOG_WRN("[SensorMsgqThread] Message queue full");
        }

        k_sleep(K_MSEC(100));
    }
}

/*
 * 数据处理线程 - 从FIFO接收
 */
static void data_fifo_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    LOG_INF("[DataFIFOThread] Started");

    while (1) {
        /* 从FIFO获取 */
        struct data_item *item = k_fifo_get(&sensor_fifo, K_FOREVER);

        if (item != NULL) {
            atomic_inc(&messages_received);

            if (item->type == MSG_TYPE_SENSOR_DATA) {
                struct sensor_data *sensor = &item->payload.sensor;

                LOG_DBG("[DataFIFOThread] Processing sensor data #%u",
                        sensor->sequence_num);

                /* 温度检查 */
                if (sensor->temperature > 30.0f) {
                    LOG_WRN("[DataFIFOThread] High temperature: %.2f",
                            sensor->temperature);
                }
            }

            /* 释放内存回slab */
            k_mem_slab_free(&data_item_slab, (void*)item);
        }
    }
}

/*
 * 数据处理线程 - 从消息队列接收
 */
static void data_msgq_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct sensor_data data;

    LOG_INF("[DataMsgqThread] Started");

    while (1) {
        /* 从消息队列获取 - 数据被复制到data */
        int ret = k_msgq_get(&sensor_msgq, &data, K_FOREVER);

        if (ret == 0) {
            atomic_inc(&messages_received);

            LOG_DBG("[DataMsgqThread] Processing sensor data #%u (T=%.2f)",
                    data.sequence_num, data.temperature);

            /* 数据处理...
               数据是副本，可以直接修改 */
            float processed_temp = data.temperature * 1.8f + 32.0f;
            (void)processed_temp;  /* 仅演示 */
        }
    }
}

/*
 * 命令处理线程
 */
static void command_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct control_command cmd;

    LOG_INF("[CommandThread] Started");

    while (1) {
        /* 接收命令 */
        int ret = k_msgq_get(&command_msgq, &cmd, K_FOREVER);

        if (ret == 0) {
            atomic_inc(&messages_received);

            LOG_INF("[CommandThread] Executing command %u: %s",
                    cmd.command_id, cmd.description);
            LOG_INF("  Parameters: 0x%X, 0x%X", cmd.param1, cmd.param2);

            /* 模拟执行 */
            k_sleep(K_MSEC(50));

            LOG_INF("[CommandThread] Command %u completed", cmd.command_id);
        }
    }
}

/*
 * 命令发送线程
 */
static void command_sender_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    uint32_t cmd_id = 0;

    while (1) {
        k_sleep(K_SECONDS(3));

        struct control_command cmd;
        cmd.command_id = ++cmd_id;
        cmd.param1 = cmd_id * 10;
        cmd.param2 = cmd_id * 20;
        snprintf(cmd.description, 32, "TestCommand_%u", cmd_id);

        int ret = k_msgq_put(&command_msgq, &cmd, K_NO_WAIT);

        if (ret == 0) {
            atomic_inc(&messages_sent);
            LOG_INF("[CmdSender] Sent command %u", cmd_id);
        } else {
            LOG_WRN("[CmdSender] Failed to send command");
        }
    }
}

/*
 * 消息队列查看示例
 */
static void peek_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    struct sensor_data data;

    while (1) {
        k_sleep(K_SECONDS(1));

        /* 查看队列头部而不移除 */
        int ret = k_msgq_peek(&sensor_msgq, &data);

        if (ret == 0) {
            LOG_INF("[PeekThread] Next in queue: seq=%u, temp=%.1f",
                    data.sequence_num, data.temperature);
        } else {
            LOG_DBG("[PeekThread] Queue empty");
        }
    }
}

/*
 * 队列监控线程
 */
static void monitor_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1);
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);

    while (1) {
        k_sleep(K_SECONDS(5));

        LOG_INF("\n[Monitor] Statistics:");
        LOG_INF("  Messages sent: %u", atomic_get(&messages_sent));
        LOG_INF("  Messages received: %u", atomic_get(&messages_received));
        LOG_INF("  Queue full events: %u", atomic_get(&queue_full_count));
        LOG_INF("  Msgq used: %u/%u", k_msgq_num_used_get(&sensor_msgq),
                k_msgq_num_msgs_get(&sensor_msgq));

        /* 清理消息队列（演示） */
        if (k_msgq_num_used_get(&sensor_msgq) > 15) {
            LOG_WRN("[Monitor] Purging sensor message queue");
            k_msgq_purge(&sensor_msgq);
        }
    }
}

/* 线程栈 */
K_THREAD_STACK_DEFINE(sensor_fifo_stack, 2048);
K_THREAD_STACK_DEFINE(sensor_msgq_stack, 2048);
K_THREAD_STACK_DEFINE(data_fifo_stack, 2048);
K_THREAD_STACK_DEFINE(data_msgq_stack, 2048);
K_THREAD_STACK_DEFINE(command_stack, 2048);
K_THREAD_STACK_DEFINE(cmd_sender_stack, 2048);
K_THREAD_STACK_DEFINE(peek_stack, 2048);
K_THREAD_STACK_DEFINE(monitor_stack, 2048);

static struct k_thread sensor_fifo_thread_data;
static struct k_thread sensor_msgq_thread_data;
static struct k_thread data_fifo_thread_data;
static struct k_thread data_msgq_thread_data;
static struct k_thread command_thread_data;
static struct k_thread cmd_sender_thread_data;
static struct k_thread peek_thread_data;
static struct k_thread monitor_thread_data;

int main(void)
{
    printk("\n=== Zephyr RTOS Queue Demo ===\n\n");

    /* 创建线程 */
    k_thread_create(&sensor_fifo_thread_data, sensor_fifo_stack,
                    K_THREAD_STACK_SIZEOF(sensor_fifo_stack),
                    sensor_fifo_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&sensor_msgq_thread_data, sensor_msgq_stack,
                    K_THREAD_STACK_SIZEOF(sensor_msgq_stack),
                    sensor_msgq_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(5), 0, K_NO_WAIT);

    k_thread_create(&data_fifo_thread_data, data_fifo_stack,
                    K_THREAD_STACK_SIZEOF(data_fifo_stack),
                    data_fifo_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);

    k_thread_create(&data_msgq_thread_data, data_msgq_stack,
                    K_THREAD_STACK_SIZEOF(data_msgq_stack),
                    data_msgq_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(4), 0, K_NO_WAIT);

    k_thread_create(&command_thread_data, command_stack,
                    K_THREAD_STACK_SIZEOF(command_stack),
                    command_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);

    k_thread_create(&cmd_sender_thread_data, cmd_sender_stack,
                    K_THREAD_STACK_SIZEOF(cmd_sender_stack),
                    command_sender_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(3), 0, K_NO_WAIT);

    k_thread_create(&peek_thread_data, peek_stack,
                    K_THREAD_STACK_SIZEOF(peek_stack),
                    peek_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(2), 0, K_NO_WAIT);

    k_thread_create(&monitor_thread_data, monitor_stack,
                    K_THREAD_STACK_SIZEOF(monitor_stack),
                    monitor_thread, NULL, NULL, NULL,
                    K_PRIO_PREEMPT(1), 0, K_NO_WAIT);

    /* 主循环 */
    while (1) {
        k_sleep(K_SECONDS(30));
    }

    return 0;
}
