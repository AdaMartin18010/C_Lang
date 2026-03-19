/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 549
 * Language: c
 * Block ID: 23b1da47
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* events/bus.h */
#ifndef EVENT_BUS_H
#define EVENT_BUS_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t event_type_t;
typedef void (*event_handler_t)(const void* event_data, void* user_data);

typedef struct event_bus event_bus_t;

event_bus_t* event_bus_create(void);
void event_bus_destroy(event_bus_t* bus);

/* 订阅事件 */
void event_bus_subscribe(event_bus_t* bus,
                         event_type_t type,
                         event_handler_t handler,
                         void* user_data);
void event_bus_unsubscribe(event_bus_t* bus,
                           event_type_t type,
                           event_handler_t handler);

/* 发布事件 */
void event_bus_publish(event_bus_t* bus,
                       event_type_t type,
                       const void* data,
                       size_t data_size);

/* 发布延迟事件（下一帧处理） */
void event_bus_publish_delayed(event_bus_t* bus,
                                event_type_t type,
                                const void* data,
                                size_t data_size,
                                float delay_seconds);

/* 处理所有延迟事件 */
void event_bus_process_delayed(event_bus_t* bus, float delta_time);

#endif
