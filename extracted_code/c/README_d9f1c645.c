/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Avionics_Systems\README.md
 * Line: 657
 * Language: c
 * Block ID: d9f1c645
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file apex_api.c
 * @brief ARINC 653 APEX API 核心实现示例
 *
 * 展示APEX接口的核心概念，简化版实现
 */

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* ============================================================================
 * ARINC 653 标准类型定义
 * ============================================================================ */

/* 返回码 */
typedef enum {
    NO_ERROR = 0,
    NO_ACTION = 1,
    NOT_AVAILABLE = 2,
    INVALID_PARAM = 3,
    INVALID_CONFIG = 4,
    INVALID_MODE = 5,
    TIMED_OUT = 6
} RETURN_CODE_TYPE;

/* 分区状态 */
typedef enum {
    IDLE = 0,
    COLD_START = 1,
    WARM_START = 2,
    NORMAL = 3
} OPERATING_MODE_TYPE;

/* 进程状态 */
typedef enum {
    DORMANT = 0,
    READY = 1,
    RUNNING = 2,
    WAITING = 3
    /* 其他状态... */
} PROCESS_STATE_TYPE;

/* 进程优先级 */
typedef int32_t PRIORITY_TYPE;
#define MAX_PRIORITY 31
#define MIN_PRIORITY 1
#define DEFAULT_PRIORITY 10

/* 系统时间类型（纳秒）*/
typedef uint64_t SYSTEM_TIME_TYPE;

/* 分区标识符 */
typedef uint32_t PARTITION_ID_TYPE;
#define MAX_PARTITIONS 32

/* 进程标识符 */
typedef uint32_t PROCESS_ID_TYPE;
#define MAX_PROCESSES_PER_PARTITION 128

/* 消息大小 */
typedef uint32_t MESSAGE_SIZE_TYPE;
#define MAX_MESSAGE_SIZE 8192

/* ============================================================================
 * 分区管理数据结构
 * ============================================================================ */

/**
 * @brief 分区属性
 */
typedef struct {
    PARTITION_ID_TYPE identifier;
    char name[32];
    SYSTEM_TIME_TYPE period;            /* 分区周期 */
    SYSTEM_TIME_TYPE duration;          /* 分区执行时间 */
    OPERATING_MODE_TYPE operating_mode;
    uint32_t start_condition;
} PARTITION_STATUS_TYPE;

/**
 * @brief 进程属性
 */
typedef struct {
    PROCESS_ID_TYPE id;
    char name[32];
    SYSTEM_TIME_TYPE period;
    SYSTEM_TIME_TYPE time_capacity;     /* 时间容量（WCET） */
    PRIORITY_TYPE current_priority;
    PRIORITY_TYPE base_priority;
    PROCESS_STATE_TYPE process_state;
    void (*entry_point)(void);          /* 入口函数 */
    uint32_t stack_size;
} PROCESS_ATTRIBUTE_TYPE;

typedef PROCESS_ATTRIBUTE_TYPE* PROCESS_ATTRIBUTE_TYPE_PTR;

/**
 * @brief 进程状态信息
 */
typedef struct {
    PROCESS_ATTRIBUTE_TYPE attr;
    SYSTEM_TIME_TYPE deadline_time;
    uint32_t context_switches;
} PROCESS_STATUS_TYPE;

/* ============================================================================
 * 分区控制接口
 * ============================================================================ */

static PARTITION_STATUS_TYPE g_partitions[MAX_PARTITIONS];
static uint32_t g_num_partitions = 0;

/**
 * @brief 获取当前分区状态
 * @param status 输出分区状态
 * @return RETURN_CODE_TYPE 返回码
 */
RETURN_CODE_TYPE GET_PARTITION_STATUS(PARTITION_STATUS_TYPE *status) {
    if (status == NULL) {
        return INVALID_PARAM;
    }

    /* 获取当前分区ID（假设当前为分区0） */
    PARTITION_ID_TYPE current_id = 0;

    memcpy(status, &g_partitions[current_id], sizeof(PARTITION_STATUS_TYPE));
    return NO_ERROR;
}

/**
 * @brief 设置分区操作模式
 * @param operating_mode 目标模式
 */
RETURN_CODE_TYPE SET_PARTITION_MODE(OPERATING_MODE_TYPE operating_mode) {
    if (operating_mode > NORMAL) {
        return INVALID_PARAM;
    }

    PARTITION_ID_TYPE current_id = 0;
    g_partitions[current_id].operating_mode = operating_mode;

    printf("[APEX] Partition %d mode set to %d\n", current_id, operating_mode);
    return NO_ERROR;
}

/* ============================================================================
 * 进程管理接口
 * ============================================================================ */

static PROCESS_STATUS_TYPE g_processes[MAX_PROCESSES_PER_PARTITION];
static uint32_t g_num_processes = 0;

/**
 * @brief 创建进程
 * @param name 进程名称
 * @param attributes 进程属性
 * @param process_id 输出进程ID
 */
RETURN_CODE_TYPE CREATE_PROCESS(
    const char *name,
    PROCESS_ATTRIBUTE_TYPE_PTR attributes,
    PROCESS_ID_TYPE *process_id)
{
    if (name == NULL || attributes == NULL || process_id == NULL) {
        return INVALID_PARAM;
    }

    if (g_num_processes >= MAX_PROCESSES_PER_PARTITION) {
        return INVALID_CONFIG;
    }

    /* 分配进程ID */
    *process_id = g_num_processes;

    /* 复制进程属性 */
    memcpy(&g_processes[*process_id].attr, attributes, sizeof(PROCESS_ATTRIBUTE_TYPE));
    strncpy(g_processes[*process_id].attr.name, name, 31);
    g_processes[*process_id].attr.name[31] = '\0';
    g_processes[*process_id].attr.id = *process_id;

    /* 初始化状态 */
    g_processes[*process_id].attr.process_state = DORMANT;
    g_processes[*process_id].deadline_time = 0;
    g_processes[*process_id].context_switches = 0;

    g_num_processes++;

    printf("[APEX] Process '%s' created with ID %d\n", name, *process_id);
    return NO_ERROR;
}

/**
 * @brief 设置进程优先级
 */
RETURN_CODE_TYPE SET_PRIORITY(PROCESS_ID_TYPE process_id, PRIORITY_TYPE priority) {
    if (process_id >= g_num_processes) {
        return INVALID_PARAM;
    }

    if (priority < MIN_PRIORITY || priority > MAX_PRIORITY) {
        return INVALID_PARAM;
    }

    g_processes[process_id].attr.current_priority = priority;
    return NO_ERROR;
}

/**
 * @brief 挂起当前进程至下一个周期
 */
RETURN_CODE_TYPE PERIODIC_WAIT(void) {
    /* 模拟挂起操作 */
    printf("[APEX] Process periodic wait\n");
    /* 实际实现：触发上下文切换，等待下一个周期 */
    return NO_ERROR;
}

/**
 * @brief 获取进程ID
 */
RETURN_CODE_TYPE GET_MY_ID(PROCESS_ID_TYPE *process_id) {
    if (process_id == NULL) {
        return INVALID_PARAM;
    }
    /* 假设当前进程ID为0 */
    *process_id = 0;
    return NO_ERROR;
}

/* ============================================================================
 * 时间管理接口
 * ============================================================================ */

/**
 * @brief 获取当前系统时间
 */
RETURN_CODE_TYPE GET_TIME(SYSTEM_TIME_TYPE *system_time) {
    if (system_time == NULL) {
        return INVALID_PARAM;
    }

    /* 模拟获取时间 */
    static SYSTEM_TIME_TYPE simulated_time = 0;
    simulated_time += 1000000; /* 1ms增量 */
    *system_time = simulated_time;

    return NO_ERROR;
}

/**
 * @brief 相对时间延迟
 * @param delay_time 延迟时间（纳秒）
 */
RETURN_CODE_TYPE TIMED_WAIT(SYSTEM_TIME_TYPE delay_time) {
    printf("[APEX] Timed wait for %llu ns\n", (unsigned long long)delay_time);
    /* 实际实现：设置定时器，挂起进程 */
    return NO_ERROR;
}

/* ============================================================================
 * 进程间通信 (IPC) - 采样端口示例
 * ============================================================================ */

/**
 * @brief 采样端口配置
 */
typedef struct {
    char name[32];
    MESSAGE_SIZE_TYPE max_message_size;
    SYSTEM_TIME_TYPE refresh_period;    /* 数据刷新周期 */
} SAMPLING_PORT_CONFIG_TYPE;

/**
 * @brief 采样端口状态
 */
typedef struct {
    SAMPLING_PORT_CONFIG_TYPE config;
    uint8_t buffer[MAX_MESSAGE_SIZE];
    MESSAGE_SIZE_TYPE message_size;
    SYSTEM_TIME_TYPE last_write_time;
    bool valid;
} SAMPLING_PORT_TYPE;

static SAMPLING_PORT_TYPE g_sampling_ports[64];
static uint32_t g_num_sampling_ports = 0;

/**
 * @brief 创建采样端口
 */
RETURN_CODE_TYPE CREATE_SAMPLING_PORT(
    const char *name,
    MESSAGE_SIZE_TYPE max_message_size,
    SYSTEM_TIME_TYPE refresh_period,
    uint32_t *port_id)
{
    if (g_num_sampling_ports >= 64) {
        return INVALID_CONFIG;
    }

    *port_id = g_num_sampling_ports;
    strncpy(g_sampling_ports[*port_id].config.name, name, 31);
    g_sampling_ports[*port_id].config.max_message_size = max_message_size;
    g_sampling_ports[*port_id].config.refresh_period = refresh_period;
    g_sampling_ports[*port_id].valid = false;

    g_num_sampling_ports++;

    printf("[APEX] Sampling port '%s' created with ID %d\n", name, *port_id);
    return NO_ERROR;
}

/**
 * @brief 向采样端口写入消息
 */
RETURN_CODE_TYPE WRITE_SAMPLING_MESSAGE(
    uint32_t port_id,
    const uint8_t *message,
    MESSAGE_SIZE_TYPE message_size)
{
    if (port_id >= g_num_sampling_ports) {
        return INVALID_PARAM;
    }

    if (message_size > g_sampling_ports[port_id].config.max_message_size) {
        return INVALID_CONFIG;
    }

    /* 写入消息 */
    memcpy(g_sampling_ports[port_id].buffer, message, message_size);
    g_sampling_ports[port_id].message_size = message_size;
    GET_TIME(&g_sampling_ports[port_id].last_write_time);
    g_sampling_ports[port_id].valid = true;

    return NO_ERROR;
}

/**
 * @brief 从采样端口读取消息
 */
RETURN_CODE_TYPE READ_SAMPLING_MESSAGE(
    uint32_t port_id,
    uint8_t *message,
    MESSAGE_SIZE_TYPE *message_size,
    SYSTEM_TIME_TYPE *time_stamp,
    RETURN_CODE_TYPE *return_code)
{
    if (port_id >= g_num_sampling_ports) {
        *return_code = INVALID_PARAM;
        return *return_code;
    }

    if (!g_sampling_ports[port_id].valid) {
        *return_code = NOT_AVAILABLE;
        return *return_code;
    }

    /* 复制消息 */
    memcpy(message, g_sampling_ports[port_id].buffer,
           g_sampling_ports[port_id].message_size);
    *message_size = g_sampling_ports[port_id].message_size;
    *time_stamp = g_sampling_ports[port_id].last_write_time;

    *return_code = NO_ERROR;
    return NO_ERROR;
}

/* ============================================================================
 * 使用示例
 * ============================================================================ */

void flight_control_task(void) {
    printf("[FCS Task] Running flight control algorithm\n");
    /* 飞控算法实现 */
}

void navigation_task(void) {
    printf("[NAV Task] Running navigation update\n");
    /* 导航算法实现 */
}

int main(void) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║        ARINC 653 APEX API 演示                          ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");

    /* 初始化分区 */
    g_partitions[0].identifier = 0;
    strcpy(g_partitions[0].name, "FCS_Partition");
    g_partitions[0].period = 10000000;      /* 10ms */
    g_partitions[0].duration = 5000000;     /* 5ms */
    g_partitions[0].operating_mode = NORMAL;
    g_num_partitions = 1;

    /* 创建飞控进程 */
    PROCESS_ATTRIBUTE_TYPE fcs_attr = {
        .period = 10000000,                 /* 10ms周期 */
        .time_capacity = 5000000,           /* 5ms WCET */
        .base_priority = 20,
        .current_priority = 20,
        .entry_point = flight_control_task,
        .stack_size = 65536
    };

    PROCESS_ID_TYPE fcs_pid;
    CREATE_PROCESS("FCS_Process", &fcs_attr, &fcs_pid);

    /* 创建导航进程 */
    PROCESS_ATTRIBUTE_TYPE nav_attr = {
        .period = 20000000,                 /* 20ms周期 */
        .time_capacity = 3000000,           /* 3ms WCET */
        .base_priority = 15,
        .current_priority = 15,
        .entry_point = navigation_task,
        .stack_size = 32768
    };

    PROCESS_ID_TYPE nav_pid;
    CREATE_PROCESS("NAV_Process", &nav_attr, &nav_pid);

    /* 创建采样端口 */
    uint32_t sensor_port;
    CREATE_SAMPLING_PORT("Sensor_Data", 256, 10000000, &sensor_port);

    /* 模拟写入传感器数据 */
    uint8_t sensor_data[] = {0x01, 0x02, 0x03, 0x04};
    WRITE_SAMPLING_MESSAGE(sensor_port, sensor_data, sizeof(sensor_data));

    printf("\nARINC 653分区操作系统初始化完成\n");

    return 0;
}
