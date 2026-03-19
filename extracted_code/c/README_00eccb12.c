/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 381
 * Language: c
 * Block ID: 00eccb12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* plugin_system.h - 插件系统接口定义 */
#ifndef PLUGIN_SYSTEM_H
#define PLUGIN_SYSTEM_H

#include <stdint.h>
#include <stdbool.h>

/* 插件版本信息 */
#define PLUGIN_API_VERSION  0x00010002  /* 1.2 */
#define PLUGIN_MAGIC        0x504C5547  /* "PLUG" */

/* 插件类型 */
typedef enum {
    PLUGIN_TYPE_GENERIC = 0,
    PLUGIN_TYPE_LOGGER,
    PLUGIN_TYPE_NETWORK,
    PLUGIN_TYPE_CRYPTO,
    PLUGIN_TYPE_CUSTOM = 0x100
} plugin_type_t;

/* 插件信息结构 */
typedef struct {
    uint32_t magic;          /* 魔数验证 */
    uint32_t api_version;    /* API版本 */
    plugin_type_t type;      /* 插件类型 */
    const char *name;        /* 插件名称 */
    const char *version;     /* 插件版本 */
    const char *author;      /* 作者 */
    const char *description; /* 描述 */
} plugin_info_t;

/* 插件接口结构 - 每个插件必须实现 */
typedef struct {
    plugin_info_t info;

    /* 生命周期函数 */
    int (*init)(void *host_context);    /* 初始化 */
    int (*deinit)(void);                /* 清理 */
    int (*configure)(const char *key, const char *value); /* 配置 */

    /* 能力查询 */
    bool (*has_capability)(const char *cap);
    int (*invoke)(const char *method, void *args, void *result);
} plugin_interface_t;

/* 宿主提供的API */
typedef struct {
    void *(*malloc)(size_t size);
    void (*free)(void *ptr);
    void *(*realloc)(void *ptr, size_t size);

    /* 日志接口 */
    void (*log_debug)(const char *fmt, ...);
    void (*log_info)(const char *fmt, ...);
    void (*log_warn)(const char *fmt, ...);
    void (*log_error)(const char *fmt, ...);

    /* 事件系统 */
    int (*register_event)(const char *event, void (*callback)(void *));
    int (*trigger_event)(const char *event, void *data);

    /* 配置存储 */
    const char *(*get_config)(const char *key);
    int (*set_config)(const char *key, const char *value);
} host_api_t;

/* 插件管理器 */
typedef struct plugin_manager plugin_manager_t;

plugin_manager_t *pm_create(const host_api_t *host_api);
void pm_destroy(plugin_manager_t *pm);
int pm_load_plugin(plugin_manager_t *pm, const char *path);
int pm_unload_plugin(plugin_manager_t *pm, const char *name);
int pm_init_all(plugin_manager_t *pm);
plugin_interface_t *pm_get_plugin(plugin_manager_t *pm, const char *name);
int pm_list_plugins(plugin_manager_t *pm, plugin_info_t **info_array, int max_count);

/* 插件导出符号 - 必须实现 */
#define PLUGIN_EXPORT __attribute__((visibility("default")))

/* 插件入口点宏 */
#define DEFINE_PLUGIN(plugin_struct) \
    PLUGIN_EXPORT const plugin_interface_t *get_plugin_interface(void) { \
        return &(plugin_struct); \
    }

#endif /* PLUGIN_SYSTEM_H */
