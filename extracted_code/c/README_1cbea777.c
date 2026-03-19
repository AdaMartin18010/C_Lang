/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 702
 * Language: c
 * Block ID: 1cbea777
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* events/types.h */
#ifndef EVENT_TYPES_H
#define EVENT_TYPES_H

/* 事件类型枚举 */
enum {
    EVENT_NONE = 0,

    /* 输入事件 */
    EVENT_KEY_PRESSED,
    EVENT_KEY_RELEASED,
    EVENT_MOUSE_MOVED,
    EVENT_MOUSE_BUTTON,

    /* 游戏逻辑事件 */
    EVENT_ENTITY_SPAWNED,
    EVENT_ENTITY_DESTROYED,
    EVENT_COLLISION,
    EVENT_DAMAGE_DEALT,

    /* 系统事件 */
    EVENT_WINDOW_RESIZE,
    EVENT_RESOURCE_LOADED,
    EVENT_SCENE_CHANGE,

    EVENT_COUNT
};

/* 事件数据结构 */
typedef struct {
    int key_code;
    int modifiers;  /* Shift, Ctrl, Alt */
} key_event_t;

typedef struct {
    uint32_t entity_a;
    uint32_t entity_b;
    float contact_point[3];
    float normal[3];
    float penetration_depth;
} collision_event_t;

typedef struct {
    uint32_t entity_id;
    float damage;
    uint32_t source_entity;
    uint32_t damage_type;
} damage_event_t;

#endif
