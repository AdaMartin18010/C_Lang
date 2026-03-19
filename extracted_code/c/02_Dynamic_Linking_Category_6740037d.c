/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 336
 * Language: c
 * Block ID: 6740037d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 插件系统接口

// 插件API结构
typedef struct {
    int version;
    const char *name;
    void (*init)(void);
    void (*fini)(void);
    void *(*get_interface)(const char *name);
} PluginAPI;

// 插件管理器
typedef struct {
    void **handles;
    PluginAPI **apis;
    int count;
    int capacity;
} PluginManager;

// 加载插件
PluginAPI *plugin_load(PluginManager *pm, const char *path) {
    void *handle = dlopen(path, RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        fprintf(stderr, "Failed to load %s: %s\n", path, dlerror());
        return NULL;
    }

    // 获取插件API
    PluginAPI *(*get_api)(void) = dlsym(handle, "plugin_get_api");
    if (!get_api) {
        dlclose(handle);
        return NULL;
    }

    PluginAPI *api = get_api();
    if (api->version != CURRENT_PLUGIN_VERSION) {
        fprintf(stderr, "Version mismatch: %d vs %d\n",
                api->version, CURRENT_PLUGIN_VERSION);
        dlclose(handle);
        return NULL;
    }

    // 初始化插件
    if (api->init) api->init();

    // 保存句柄
    pm->handles[pm->count] = handle;
    pm->apis[pm->count] = api;
    pm->count++;

    return api;
}

// 插件示例代码
/*
// plugin_example.c
#include "plugin_api.h"

static void my_init(void) {
    printf("Plugin initialized\n");
}

static void *my_get_interface(const char *name) {
    if (strcmp(name, "my_feature") == 0) {
        return &my_feature_impl;
    }
    return NULL;
}

static PluginAPI api = {
    .version = 1,
    .name = "example_plugin",
    .init = my_init,
    .get_interface = my_get_interface
};

PluginAPI *plugin_get_api(void) {
    return &api;
}
*/
