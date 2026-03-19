/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 761
 * Language: c
 * Block ID: 9a028188
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* resources/manager.h */
#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t resource_handle_t;
typedef uint32_t resource_type_t;

#define INVALID_HANDLE 0

/* 资源加载器接口 */
typedef struct {
    void* (*load)(const char* path, void* user_data);
    void (*unload)(void* resource, void* user_data);
    size_t (*get_size)(const void* resource);
} resource_loader_t;

typedef struct resource_manager resource_manager_t;

resource_manager_t* resource_manager_create(size_t max_memory);
void resource_manager_destroy(resource_manager_t* manager);

/* 注册资源类型 */
void resource_manager_register_type(resource_manager_t* manager,
                                     resource_type_t type,
                                     const resource_loader_t* loader,
                                     void* user_data);

/* 加载资源 */
resource_handle_t resource_manager_load(resource_manager_t* manager,
                                        resource_type_t type,
                                        const char* path);

/* 获取资源 */
void* resource_manager_get(resource_manager_t* manager,
                           resource_handle_t handle);

/* 释放资源引用 */
void resource_manager_release(resource_manager_t* manager,
                              resource_handle_t handle);

/* 热重载 */
void resource_manager_hot_reload(resource_manager_t* manager);

#endif
