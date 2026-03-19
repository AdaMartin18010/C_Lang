/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 657
 * Language: c
 * Block ID: fcb8d796
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* config_manager.h */
#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    CONFIG_TYPE_STRING = 0,
    CONFIG_TYPE_INT,
    CONFIG_TYPE_FLOAT,
    CONFIG_TYPE_BOOL,
    CONFIG_TYPE_ARRAY,
    CONFIG_TYPE_OBJECT
} config_value_type_t;

typedef struct {
    config_value_type_t type;
    union {
        char *string_val;
        int64_t int_val;
        double float_val;
        bool bool_val;
        struct {
            void **items;
            size_t count;
        } array_val;
    };
} config_value_t;

/* 配置项 */
typedef struct config_item {
    char *key;
    config_value_t value;
    char *source;           /* 配置来源 */
    uint32_t priority;      /* 优先级 */
    bool encrypted;         /* 是否加密 */
    struct config_item *next;
} config_item_t;

/* 配置管理器 */
typedef struct {
    config_item_t *items;
    char *config_dir;
    void (*on_change)(const char *key, config_value_t *old_val, config_value_t *new_val);
} config_manager_t;

/* API */
config_manager_t *cm_create(const char *config_dir);
void cm_destroy(config_manager_t *cm);

/* 加载配置 */
int cm_load_from_file(config_manager_t *cm, const char *path);
int cm_load_from_env(config_manager_t *cm, const char *prefix);
int cm_load_from_args(config_manager_t *cm, int argc, char **argv);

/* 获取/设置配置 */
const config_value_t *cm_get(config_manager_t *cm, const char *key);
int cm_set(config_manager_t *cm, const char *key, const config_value_t *value,
           uint32_t priority, const char *source);

/* 类型安全的获取 */
int64_t cm_get_int(config_manager_t *cm, const char *key, int64_t default_val);
double cm_get_float(config_manager_t *cm, const char *key, double default_val);
bool cm_get_bool(config_manager_t *cm, const char *key, bool default_val);
const char *cm_get_string(config_manager_t *cm, const char *key, const char *default_val);

/* 热重载 */
int cm_watch(config_manager_t *cm);
int cm_reload(config_manager_t *cm);

#endif /* CONFIG_MANAGER_H */
