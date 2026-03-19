/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 72
 * Language: c
 * Block ID: 7e8e879b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* engine.h - 引擎主头文件 */
#ifndef ENGINE_H
#define ENGINE_H

#include "core/memory.h"      /* 内存管理必须在最底层 */
#include "core/log.h"         /* 日志系统 */
#include "core/time.h"        /* 时间系统 */
#include "platform/window.h"  /* 平台窗口 */
#include "renderer/rhi.h"     /* 渲染硬件接口 */
#include "ecs/world.h"        /* ECS 世界 */
#include "resources/manager.h" /* 资源管理 */
#include "events/bus.h"       /* 事件总线 */

typedef struct {
    memory_pool_t* global_pool;
    window_t* window;
    rhi_context_t* renderer;
    world_t* world;
    resource_manager_t* resources;
    event_bus_t* events;
    bool running;
} engine_t;

engine_t* engine_create(const engine_config_t* config);
void engine_run(engine_t* engine);
void engine_destroy(engine_t* engine);

#endif
