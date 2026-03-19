/*
 * Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
 * Line: 1607
 * Language: c
 * Block ID: 4c581d1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码: event_system.h
#ifndef EVENT_SYSTEM_H
#define EVENT_SYSTEM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* 事件类型 */
typedef enum {
    EVENT_NONE = 0,
    EVENT_KEY_PRESS,
    EVENT_KEY_RELEASE,
    EVENT_MOUSE_MOVE,
    EVENT_MOUSE_CLICK,
    EVENT_WINDOW_RESIZE,
    EVENT_CUSTOM = 100
} EventType;

/* 事件数据联合体 */
typedef union {
    struct {
        int keycode;
        int modifiers;
    } key;
    struct {
        int x, y;
        int button;
    } mouse;
    struct {
        int width, height;
    } window;
    struct {
        uint32_t id;
        void* data;
    } custom;
} EventData;

/* 事件结构体 */
typedef struct {
    EventType type;
    uint64_t timestamp;
    EventData data;
} Event;

/* 事件处理器函数指针类型 */
typedef bool (*EventHandler)(const Event* event, void* user_data);

/* 事件过滤器函数指针类型 - 返回 true 表示事件被消费 */
typedef bool (*EventFilter)(const Event* event);

/* 事件系统 API */
typedef struct EventSystem EventSystem;

EventSystem* event_system_create(void);
void event_system_destroy(EventSystem* system);

/* 注册事件处理器 */
int event_system_register_handler(
    EventSystem* system,
    EventType type,
    EventHandler handler,
    void* user_data,
    int priority
);

/* 注销事件处理器 */
void event_system_unregister_handler(
    EventSystem* system,
    int handler_id
);

/* 设置全局事件过滤器 */
void event_system_set_filter(EventSystem* system, EventFilter filter);

/* 分发事件 */
void event_system_dispatch(EventSystem* system, const Event* event);

/* 处理所有挂起事件 */
void event_system_process_events(EventSystem* system);

/* 迭代器回调 */
typedef void (*EventIteratorCallback)(const Event* event, int index, void* ctx);
void event_system_foreach_pending(EventSystem* system, EventIteratorCallback callback, void* ctx);

/* 高级: 处理器链 */
typedef EventHandler (*HandlerChainNext)(EventHandler current, void* ctx);
EventHandler event_system_create_handler_chain(
    EventSystem* system,
    HandlerChainNext next_fn,
    void* ctx
);

#ifdef __cplusplus
}
#endif

#endif
