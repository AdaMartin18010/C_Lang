/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 5626
 * Language: c
 * Block ID: d8d1fab8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef CONFIGLIB_H
#define CONFIGLIB_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>

#define CONFIGLIB_VERSION "1.0.0"
#define MAX_PATH_DEPTH 32
#define MAX_KEY_LEN 256
#define MAX_STRING_LEN 4096

typedef enum {
    CONFIG_NULL = 0,
    CONFIG_BOOL = 1,
    CONFIG_INT = 2,
    CONFIG_FLOAT = 3,
    CONFIG_STRING = 4,
    CONFIG_ARRAY = 5,
    CONFIG_OBJECT = 6
} config_type_t;

typedef enum {
    CONFIG_OK = 0,
    CONFIG_ERROR = -1,
    CONFIG_NOT_FOUND = -2,
    CONFIG_TYPE_MISMATCH = -3,
    CONFIG_PARSE_ERROR = -4,
    CONFIG_VALIDATION_ERROR = -5
} config_error_t;

/* Forward declarations */
typedef struct config config_t;
typedef struct config_value config_value_t;
typedef struct config_schema config_schema_t;
typedef struct config_watcher config_watcher_t;

/* Config value */
struct config_value {
    config_type_t type;
    union {
        bool b;
        int64_t i;
        double f;
        struct {
            char *data;
            size_t len;
        } s;
        struct {
            config_value_t **items;
            size_t count;
        } array;
        struct {
            char **keys;
            config_value_t **values;
            size_t count;
        } object;
    } data;
};

/* Config handle */
typedef struct config {
    char *source_file;
    time_t last_modified;
    config_value_t *root;
    config_value_t *defaults;
    config_schema_t *schema;
    bool auto_reload;
    config_watcher_t *watcher;
    void (*on_reload)(config_t *cfg, void *user_data);
    void *user_data;
    pthread_rwlock_t lock;
} config_t;

/* Schema types */
typedef enum {
    SCHEMA_ANY,
    SCHEMA_STRING,
    SCHEMA_NUMBER,
    SCHEMA_INTEGER,
    SCHEMA_BOOLEAN,
    SCHEMA_ARRAY,
    SCHEMA_OBJECT
} schema_type_t;

struct config_schema {
    schema_type_t type;
    bool required;
    config_value_t *default_val;
    struct {
        config_schema_t *item_schema;
    } array;
    struct {
        char **keys;
        config_schema_t **schemas;
        size_t count;
    } object;
    struct {
        double min;
        double max;
        const char *pattern;
        size_t min_len;
        size_t max_len;
    } constraints;
};

/* Lifecycle */
config_t* config_create(void);
void config_destroy(config_t *cfg);
config_t* config_load_file(const char *path);
config_t* config_load_string(const char *str, size_t len);
int config_reload(config_t *cfg);
int config_save(config_t *cfg, const char *path);

/* Parsers */
config_t* config_load_json(const char *path);
config_t* config_load_yaml(const char *path);
config_t* config_load_ini(const char *path);
config_t* config_load_toml(const char *path);

/* Type-safe getters */
int config_get_bool(config_t *cfg, const char *path, bool *out);
int config_get_int(config_t *cfg, const char *path, int64_t *out);
int config_get_float(config_t *cfg, const char *path, double *out);
int config_get_string(config_t *cfg, const char *path, char *out, size_t len);
const char* config_get_string_ref(config_t *cfg, const char *path);
config_value_t* config_get(config_t *cfg, const char *path);

/* Setters */
int config_set_bool(config_t *cfg, const char *path, bool val);
int config_set_int(config_t *cfg, const char *path, int64_t val);
int config_set_float(config_t *cfg, const char *path, double val);
int config_set_string(config_t *cfg, const char *path, const char *val);
int config_set_null(config_t *cfg, const char *path);
int config_set_array(config_t *cfg, const char *path);
int config_set_object(config_t *cfg, const char *path);

/* Array operations */
size_t config_array_len(config_t *cfg, const char *path);
int config_array_append(config_t *cfg, const char *path, const config_value_t *val);
int config_array_get(config_t *cfg, const char *path, size_t idx, config_value_t *out);

/* Object operations */
int config_object_keys(config_t *cfg, const char *path, char **keys, size_t max_keys);
int config_object_has_key(config_t *cfg, const char *path, const char *key);
int config_object_remove(config_t *cfg, const char *path, const char *key);

/* Environment variable resolution */
int config_resolve_env(config_t *cfg);
int config_resolve_env_strict(config_t *cfg);

/* Merging */
int config_merge(config_t *dst, const config_t *src, bool override);
int config_merge_file(config_t *cfg, const char *path, bool override);

/* Validation */
config_schema_t* schema_create(schema_type_t type);
void schema_destroy(config_schema_t *schema);
int schema_add_property(config_schema_t *obj_schema, const char *key,
                        const config_schema_t *prop_schema);
int config_validate(const config_t *cfg, const config_schema_t *schema,
                    char *error, size_t error_len);

/* Hot reload */
int config_enable_reload(config_t *cfg);
void config_disable_reload(config_t *cfg);
void config_set_reload_callback(config_t *cfg,
                                void (*callback)(config_t*, void*),
                                void *user_data);

/* Value helpers */
config_value_t* config_value_create_null(void);
config_value_t* config_value_create_bool(bool val);
config_value_t* config_value_create_int(int64_t val);
config_value_t* config_value_create_float(double val);
config_value_t* config_value_create_string(const char *val);
config_value_t* config_value_create_array(void);
config_value_t* config_value_create_object(void);
void config_value_destroy(config_value_t *val);
config_value_t* config_value_copy(const config_value_t *val);
char* config_value_to_string(const config_value_t *val);
int config_value_compare(const config_value_t *a, const config_value_t *b);

/* Error handling */
const char* config_error_string(int error);
const char* config_last_error(config_t *cfg);

/* Path utilities */
char** config_split_path(const char *path, size_t *count);
void config_free_path_parts(char **parts, size_t count);

#endif /* CONFIGLIB_H */
