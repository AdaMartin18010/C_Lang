/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 785
 * Language: c
 * Block ID: b3a69c33
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 类型安全对象池 - 针对特定数据结构优化
 * ============================================================================ */

/* object_pool.h */
#ifndef OBJECT_POOL_H
#define OBJECT_POOL_H

#include <stdint.h>
#include <stdbool.h>

/* 对象池宏 - 生成类型安全的对象池 */
#define DEFINE_OBJECT_POOL(type, name, count) \
    static type name##_storage[count]; \
    static uint8_t name##_used[count] = {0}; \
    static const uint16_t name##_count = count; \
    \
    static inline type *name##_alloc(void) { \
        for (uint16_t i = 0; i < count; i++) { \
            if (!name##_used[i]) { \
                name##_used[i] = 1; \
                return &name##_storage[i]; \
            } \
        } \
        return NULL; \
    } \
    \
    static inline void name##_free(type *ptr) { \
        if (ptr >= &name##_storage[0] && ptr < &name##_storage[count]) { \
            name##_used[ptr - name##_storage] = 0; \
        } \
    } \
    \
    static inline uint16_t name##_free_count(void) { \
        uint16_t free = 0; \
        for (uint16_t i = 0; i < count; i++) { \
            if (!name##_used[i]) free++; \
        } \
        return free; \
    }

/* 显式对象池结构 */
#define OBJECT_POOL(type, count) \
    struct { \
        type storage[count]; \
        uint8_t used[count]; \
        const uint16_t capacity; \
        uint16_t used_count; \
    }

/* 初始化对象池 */
#define OBJECT_POOL_INIT(pool) \
    do { \
        memset((pool)->used, 0, sizeof((pool)->used)); \
        *(uint16_t*)&(pool)->capacity = sizeof((pool)->storage) / sizeof((pool)->storage[0]); \
        (pool)->used_count = 0; \
    } while(0)

#endif /* OBJECT_POOL_H */

/* object_pool_example.c */
#include "object_pool.h"
#include <string.h>
#include <stdio.h>

/* 定义传感器数据结构 */
typedef struct {
    uint16_t sensor_id;
    int16_t temperature;    /* 0.1度为单位 */
    uint16_t humidity;      /* 0.1%为单位 */
    uint32_t timestamp;
    uint8_t status;
    uint8_t battery_level;
} sensor_data_t;

/* 定义传感器对象池 - 最多8个传感器 */
DEFINE_OBJECT_POOL(sensor_data_t, sensor_pool, 8)

/* 定义消息缓冲区对象池 */
typedef struct {
    uint8_t data[64];
    uint8_t length;
    uint8_t type;
} message_buffer_t;

DEFINE_OBJECT_POOL(message_buffer_t, msg_pool, 4)

/* 使用示例 */
void sensor_pool_example(void)
{
    printf("Object Pool Example\n");
    printf("===================\n");

    /* 分配传感器对象 */
    sensor_data_t *sensor1 = sensor_pool_alloc();
    sensor_data_t *sensor2 = sensor_pool_alloc();

    if (sensor1 && sensor2) {
        /* 初始化数据 */
        sensor1->sensor_id = 0x1234;
        sensor1->temperature = 256;  /* 25.6度 */
        sensor1->humidity = 456;     /* 45.6% */
        sensor1->timestamp = 0;
        sensor1->status = 1;
        sensor1->battery_level = 85;

        sensor2->sensor_id = 0x5678;
        sensor2->temperature = 189;  /* 18.9度 */
        sensor2->humidity = 600;     /* 60.0% */

        printf("Sensor 1: ID=0x%04X, Temp=%.1fC, Hum=%.1f%%\n",
               sensor1->sensor_id, sensor1->temperature / 10.0,
               sensor1->humidity / 10.0);
        printf("Sensor 2: ID=0x%04X, Temp=%.1fC, Hum=%.1f%%\n",
               sensor2->sensor_id, sensor2->temperature / 10.0,
               sensor2->humidity / 10.0);

        printf("Free sensors: %d\n", sensor_pool_free_count());
    }

    /* 释放 */
    sensor_pool_free(sensor1);
    printf("After free: %d free sensors\n", sensor_pool_free_count());
}

/* 显式对象池使用 */
typedef struct sensor_node {
    uint16_t id;
    struct sensor_node *next;
    uint8_t data[20];
} sensor_node_t;

/* 定义节点池 */
static struct {
    sensor_node_t nodes[10];
    uint8_t used[10];
    uint16_t capacity;
    uint16_t used_count;
} node_pool;

void explicit_pool_init(void)
{
    memset(node_pool.used, 0, sizeof(node_pool.used));
    node_pool.capacity = 10;
    node_pool.used_count = 0;
}

sensor_node_t *node_alloc(void)
{
    for (uint16_t i = 0; i < node_pool.capacity; i++) {
        if (!node_pool.used[i]) {
            node_pool.used[i] = 1;
            node_pool.used_count++;
            memset(&node_pool.nodes[i], 0, sizeof(sensor_node_t));
            return &node_pool.nodes[i];
        }
    }
    return NULL;
}

void node_free(sensor_node_t *node)
{
    if (node >= &node_pool.nodes[0] &&
        node < &node_pool.nodes[node_pool.capacity]) {
        uint16_t index = node - node_pool.nodes;
        if (node_pool.used[index]) {
            node_pool.used[index] = 0;
            node_pool.used_count--;
        }
    }
}
