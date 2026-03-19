/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 471
 * Language: c
 * Block ID: 5f315240
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* plugin_system.c - 插件管理器实现 */
#include "plugin_system.h"
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* 已加载插件节点 */
typedef struct plugin_node {
    plugin_interface_t *interface;
    void *handle;           /* dlopen句柄 */
    char path[256];
    bool initialized;
    struct plugin_node *next;
} plugin_node_t;

struct plugin_manager {
    host_api_t host_api;
    plugin_node_t *plugins;
    int plugin_count;
};

plugin_manager_t *pm_create(const host_api_t *host_api) {
    plugin_manager_t *pm = calloc(1, sizeof(plugin_manager_t));
    if (pm && host_api) {
        pm->host_api = *host_api;
    }
    return pm;
}

int pm_load_plugin(plugin_manager_t *pm, const char *path) {
    if (!pm || !path) return -1;

    void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "Failed to load plugin %s: %s\n", path, dlerror());
        return -1;
    }

    /* 获取插件接口 */
    const plugin_interface_t *(*get_interface)(void) =
        dlsym(handle, "get_plugin_interface");

    if (!get_interface) {
        fprintf(stderr, "Plugin %s missing get_plugin_interface\n", path);
        dlclose(handle);
        return -1;
    }

    const plugin_interface_t *iface = get_interface();

    /* 验证插件 */
    if (!iface || iface->info.magic != PLUGIN_MAGIC) {
        fprintf(stderr, "Plugin %s has invalid magic\n", path);
        dlclose(handle);
        return -1;
    }

    if (iface->info.api_version != PLUGIN_API_VERSION) {
        fprintf(stderr, "Plugin %s API version mismatch\n", path);
        dlclose(handle);
        return -1;
    }

    /* 创建节点 */
    plugin_node_t *node = calloc(1, sizeof(plugin_node_t));
    node->interface = (plugin_interface_t *)iface;
    node->handle = handle;
    strncpy(node->path, path, sizeof(node->path) - 1);

    /* 添加到列表 */
    node->next = pm->plugins;
    pm->plugins = node;
    pm->plugin_count++;

    printf("Loaded plugin: %s v%s (%s)\n",
           iface->info.name, iface->info.version, iface->info.description);

    return 0;
}

int pm_init_all(plugin_manager_t *pm) {
    plugin_node_t *node = pm->plugins;
    while (node) {
        if (!node->initialized && node->interface->init) {
            if (node->interface->init(&pm->host_api) == 0) {
                node->initialized = true;
            } else {
                fprintf(stderr, "Failed to initialize plugin %s\n",
                        node->interface->info.name);
            }
        }
        node = node->next;
    }
    return 0;
}
