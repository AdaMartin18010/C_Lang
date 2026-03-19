/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\README.md
 * Line: 813
 * Language: c
 * Block ID: deaafad9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* resources/manager.c */
#include "manager.h"
#include "core/hash_map.h"
#include <stdlib.h>
#include <string.h>

#define MAX_RESOURCES 4096
#define MAX_RESOURCE_TYPES 32

typedef struct {
    void* data;
    char* path;
    resource_type_t type;
    uint32_t ref_count;
    uint64_t last_modified;  /* 用于热重载 */
    size_t size;
} resource_entry_t;

struct resource_manager {
    resource_entry_t resources[MAX_RESOURCES];
    hash_map_t* path_to_handle;  /* 路径 -> 句柄映射 */

    resource_loader_t loaders[MAX_RESOURCE_TYPES];
    void* loader_user_data[MAX_RESOURCE_TYPES];

    size_t total_memory;
    size_t max_memory;
    size_t used_memory;
};

resource_manager_t* resource_manager_create(size_t max_memory) {
    resource_manager_t* manager = calloc(1, sizeof(resource_manager_t));
    manager->path_to_handle = hash_map_create(MAX_RESOURCES);
    manager->max_memory = max_memory;
    return manager;
}

resource_handle_t resource_manager_load(resource_manager_t* manager,
                                        resource_type_t type,
                                        const char* path) {
    /* 检查是否已加载 */
    resource_handle_t* existing = hash_map_get(manager->path_to_handle, path);
    if (existing) {
        manager->resources[*existing].ref_count++;
        return *existing;
    }

    /* 查找空闲槽位 */
    resource_handle_t handle = INVALID_HANDLE;
    for (uint32_t i = 1; i < MAX_RESOURCES; i++) {
        if (manager->resources[i].ref_count == 0) {
            handle = i;
            break;
        }
    }

    if (handle == INVALID_HANDLE) {
        /* 尝试卸载未使用的资源 */
        resource_manager_evict_unused(manager);
        /* 再次尝试 */
        for (uint32_t i = 1; i < MAX_RESOURCES; i++) {
            if (manager->resources[i].ref_count == 0) {
                handle = i;
                break;
            }
        }
    }

    if (handle == INVALID_HANDLE) {
        return INVALID_HANDLE;  /* 资源槽位已满 */
    }

    /* 加载资源 */
    resource_entry_t* entry = &manager->resources[handle];
    entry->data = manager->loaders[type].load(path,
        manager->loader_user_data[type]);

    if (!entry->data) {
        return INVALID_HANDLE;
    }

    entry->path = strdup(path);
    entry->type = type;
    entry->ref_count = 1;
    entry->size = manager->loaders[type].get_size(entry->data);
    entry->last_modified = file_get_modified_time(path);

    manager->used_memory += entry->size;
    hash_map_put(manager->path_to_handle, path, &handle, sizeof(handle));

    return handle;
}

void* resource_manager_get(resource_manager_t* manager,
                           resource_handle_t handle) {
    if (handle == INVALID_HANDLE || handle >= MAX_RESOURCES) {
        return NULL;
    }
    resource_entry_t* entry = &manager->resources[handle];
    if (entry->ref_count == 0) {
        return NULL;
    }
    return entry->data;
}

void resource_manager_release(resource_manager_t* manager,
                              resource_handle_t handle) {
    if (handle == INVALID_HANDLE || handle >= MAX_RESOURCES) {
        return;
    }

    resource_entry_t* entry = &manager->resources[handle];
    if (entry->ref_count > 0) {
        entry->ref_count--;

        if (entry->ref_count == 0) {
            /* 延迟卸载，保留在内存中以便复用 */
            /* 实际实现可以添加 LRU 缓存 */
        }
    }
}

void resource_manager_hot_reload(resource_manager_t* manager) {
    for (uint32_t i = 1; i < MAX_RESOURCES; i++) {
        resource_entry_t* entry = &manager->resources[i];
        if (entry->ref_count == 0) continue;

        uint64_t current_modified = file_get_modified_time(entry->path);
        if (current_modified > entry->last_modified) {
            printf("Hot reloading: %s\n", entry->path);

            /* 卸载旧资源 */
            manager->loaders[entry->type].unload(entry->data,
                manager->loader_user_data[entry->type]);

            /* 加载新资源 */
            entry->data = manager->loaders[entry->type].load(entry->path,
                manager->loader_user_data[entry->type]);
            entry->last_modified = current_modified;
        }
    }
}
