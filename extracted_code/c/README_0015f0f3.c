/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 1255
 * Language: c
 * Block ID: 0015f0f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// plugin.h
typedef struct {
    const char* name;
    const char* version;
    int (*init)(void);
    void (*shutdown)(void);
    int (*execute)(const char* args);
} Plugin;

// 插件导出宏
#define PLUGIN_EXPORT __attribute__((visibility("default")))

#define DEFINE_PLUGIN(name, ver, init_fn, shutdown_fn, exec_fn) \
    PLUGIN_EXPORT const Plugin plugin_info = { \
        .name = name, \
        .version = ver, \
        .init = init_fn, \
        .shutdown = shutdown_fn, \
        .execute = exec_fn \
    }

// 插件加载器
typedef struct {
    void* handle;
    const Plugin* info;
} LoadedPlugin;

LoadedPlugin* plugin_load(const char* path);
void plugin_unload(LoadedPlugin* p);

// 使用统一接口调用任何插件
int plugin_execute(LoadedPlugin* p, const char* args) {
    return p->info->execute(args);
}
