/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 595
 * Language: c
 * Block ID: 4744ff34
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* events/bus.c */
#include "bus.h"
#include "core/memory_pool.h"
#include <stdlib.h>
#include <string.h>

#define MAX_HANDLERS_PER_EVENT 32
#define MAX_DELAYED_EVENTS 256

typedef struct {
    event_handler_t handler;
    void* user_data;
} subscription_t;

typedef struct {
    event_type_t type;
    void* data;
    size_t data_size;
    float remaining_time;
} delayed_event_t;

struct event_bus {
    subscription_t* subscriptions[256];  /* 按事件类型索引 */
    uint32_t subscription_counts[256];

    delayed_event_t delayed_events[MAX_DELAYED_EVENTS];
    uint32_t delayed_count;
    memory_pool_t* event_data_pool;
};

event_bus_t* event_bus_create(void) {
    event_bus_t* bus = calloc(1, sizeof(event_bus_t));
    bus->event_data_pool = memory_pool_create(256, 1024, 64);
    return bus;
}

void event_bus_subscribe(event_bus_t* bus,
                         event_type_t type,
                         event_handler_t handler,
                         void* user_data) {
    if (bus->subscription_counts[type] >= MAX_HANDLERS_PER_EVENT) {
        return;
    }

    if (!bus->subscriptions[type]) {
        bus->subscriptions[type] = malloc(sizeof(subscription_t) *
                                          MAX_HANDLERS_PER_EVENT);
    }

    subscription_t* sub = &bus->subscriptions[type][bus->subscription_counts[type]++];
    sub->handler = handler;
    sub->user_data = user_data;
}

void event_bus_publish(event_bus_t* bus,
                       event_type_t type,
                       const void* data,
                       size_t data_size) {
    if (!bus->subscriptions[type]) return;

    /* 复制事件数据，避免生命周期问题 */
    void* event_copy = NULL;
    if (data && data_size > 0) {
        event_copy = memory_pool_alloc(bus->event_data_pool);
        if (event_copy) {
            memcpy(event_copy, data, data_size > 256 ? 256 : data_size);
        }
    }

    /* 通知所有订阅者 */
    for (uint32_t i = 0; i < bus->subscription_counts[type]; i++) {
        subscription_t* sub = &bus->subscriptions[type][i];
        sub->handler(event_copy, sub->user_data);
    }

    if (event_copy) {
        memory_pool_free(bus->event_data_pool, event_copy);
    }
}

void event_bus_process_delayed(event_bus_t* bus, float delta_time) {
    uint32_t write_index = 0;

    for (uint32_t i = 0; i < bus->delayed_count; i++) {
        delayed_event_t* evt = &bus->delayed_events[i];
        evt->remaining_time -= delta_time;

        if (evt->remaining_time <= 0) {
            /* 触发事件 */
            event_bus_publish(bus, evt->type, evt->data, evt->data_size);
            free(evt->data);
        } else {
            /* 保留未到期的事件 */
            if (write_index != i) {
                bus->delayed_events[write_index] = *evt;
            }
            write_index++;
        }
    }

    bus->delayed_count = write_index;
}
