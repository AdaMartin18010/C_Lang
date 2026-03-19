/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 5824
 * Language: c
 * Block ID: 2e33e455
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Configuration Management Library
 *
 * A comprehensive, thread-safe configuration library supporting
 * multiple formats, hot reload, and schema validation.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "configlib.h"

/* Path parsing */
char** config_split_path(const char *path, size_t *count) {
    if (!path || !count) return NULL;

    *count = 0;
    char **parts = calloc(MAX_PATH_DEPTH, sizeof(char*));
    if (!parts) return NULL;

    char *path_copy = strdup(path);
    char *p = path_copy;
    char *token;

    /* Handle leading '.' for relative paths */
    if (*p == '.') p++;
    if (*p == '/') p++;

    while ((token = strsep(&p, "./")) != NULL && *count < MAX_PATH_DEPTH) {
        if (*token == '\0') continue;
        parts[(*count)++] = strdup(token);
    }

    free(path_copy);
    return parts;
}

void config_free_path_parts(char **parts, size_t count) {
    for (size_t i = 0; i < count; i++) {
        free(parts[i]);
    }
    free(parts);
}

/* Value operations */
config_value_t* config_value_create_null(void) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_NULL;
    return val;
}

config_value_t* config_value_create_bool(bool b) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_BOOL;
    val->data.b = b;
    return val;
}

config_value_t* config_value_create_int(int64_t i) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_INT;
    val->data.i = i;
    return val;
}

config_value_t* config_value_create_float(double f) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_FLOAT;
    val->data.f = f;
    return val;
}

config_value_t* config_value_create_string(const char *s) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_STRING;
    val->data.s.data = strdup(s);
    val->data.s.len = strlen(s);
    return val;
}

config_value_t* config_value_create_array(void) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_ARRAY;
    return val;
}

config_value_t* config_value_create_object(void) {
    config_value_t *val = calloc(1, sizeof(config_value_t));
    val->type = CONFIG_OBJECT;
    return val;
}

void config_value_destroy(config_value_t *val) {
    if (!val) return;

    switch (val->type) {
        case CONFIG_STRING:
            free(val->data.s.data);
            break;
        case CONFIG_ARRAY:
            for (size_t i = 0; i < val->data.array.count; i++) {
                config_value_destroy(val->data.array.items[i]);
            }
            free(val->data.array.items);
            break;
        case CONFIG_OBJECT:
            for (size_t i = 0; i < val->data.object.count; i++) {
                free(val->data.object.keys[i]);
                config_value_destroy(val->data.object.values[i]);
            }
            free(val->data.object.keys);
            free(val->data.object.values);
            break;
        default:
            break;
    }

    free(val);
}

/* Navigate to value by path */
static config_value_t* navigate_path(config_value_t *root, const char **parts,
                                     size_t count, bool create_missing) {
    config_value_t *current = root;

    for (size_t i = 0; i < count && current; i++) {
        if (current->type != CONFIG_OBJECT) {
            if (!create_missing) return NULL;
            /* Convert to object */
            config_value_destroy(current);
            current->type = CONFIG_OBJECT;
        }

        /* Find key in object */
        config_value_t *next = NULL;
        for (size_t j = 0; j < current->data.object.count; j++) {
            if (strcmp(current->data.object.keys[j], parts[i]) == 0) {
                next = current->data.object.values[j];
                break;
            }
        }

        if (!next && create_missing && i == count - 1) {
            /* Create new entry */
            size_t idx = current->data.object.count++;
            current->data.object.keys = realloc(current->data.object.keys,
                                               idx * sizeof(char*));
            current->data.object.values = realloc(current->data.object.values,
                                                 idx * sizeof(config_value_t*));
            current->data.object.keys[idx] = strdup(parts[i]);
            current->data.object.values[idx] = config_value_create_null();
            next = current->data.object.values[idx];
        }

        current = next;
    }

    return current;
}

/* Config lifecycle */
config_t* config_create(void) {
    config_t *cfg = calloc(1, sizeof(config_t));
    if (!cfg) return NULL;

    cfg->root = config_value_create_object();
    pthread_rwlock_init(&cfg->lock, NULL);

    return cfg;
}

void config_destroy(config_t *cfg) {
    if (!cfg) return;

    if (cfg->watcher) {
        config_disable_reload(cfg);
    }

    config_value_destroy(cfg->root);
    config_value_destroy(cfg->defaults);
    schema_destroy(cfg->schema);
    free(cfg->source_file);
    pthread_rwlock_destroy(&cfg->lock);
    free(cfg);
}

/* JSON Parser (simplified) */
static config_value_t* parse_json_value(const char **ptr);

static void skip_whitespace(const char **ptr) {
    while (isspace((unsigned char)**ptr)) (*ptr)++;
}

static config_value_t* parse_json_string(const char **ptr) {
    if (**ptr != '"') return NULL;
    (*ptr)++;

    const char *start = *ptr;
    while (**ptr && **ptr != '"') {
        if (**ptr == '\\') (*ptr)++;
        (*ptr)++;
    }

    size_t len = *ptr - start;
    char *str = malloc(len + 1);
    memcpy(str, start, len);
    str[len] = '\0';

    if (**ptr == '"') (*ptr)++;

    config_value_t *val = config_value_create_string(str);
    free(str);
    return val;
}

static config_value_t* parse_json_number(const char **ptr) {
    const char *start = *ptr;
    bool is_float = false;

    if (**ptr == '-') (*ptr)++;

    while (isdigit((unsigned char)**ptr)) (*ptr)++;

    if (**ptr == '.') {
        is_float = true;
        (*ptr)++;
        while (isdigit((unsigned char)**ptr)) (*ptr)++;
    }

    if (**ptr == 'e' || **ptr == 'E') {
        is_float = true;
        (*ptr)++;
        if (**ptr == '+' || **ptr == '-') (*ptr)++;
        while (isdigit((unsigned char)**ptr)) (*ptr)++;
    }

    char *num_str = strndup(start, *ptr - start);
    config_value_t *val;

    if (is_float) {
        val = config_value_create_float(atof(num_str));
    } else {
        val = config_value_create_int(atoll(num_str));
    }

    free(num_str);
    return val;
}

static config_value_t* parse_json_object(const char **ptr) {
    if (**ptr != '{') return NULL;
    (*ptr)++;

    config_value_t *obj = config_value_create_object();
    skip_whitespace(ptr);

    if (**ptr == '}') {
        (*ptr)++;
        return obj;
    }

    while (**ptr) {
        skip_whitespace(ptr);

        config_value_t *key = parse_json_string(ptr);
        if (!key) goto error;

        skip_whitespace(ptr);
        if (**ptr != ':') goto error;
        (*ptr)++;

        skip_whitespace(ptr);
        config_value_t *val = parse_json_value(ptr);
        if (!val) goto error;

        /* Add to object */
        size_t idx = obj->data.object.count++;
        obj->data.object.keys = realloc(obj->data.object.keys,
                                       idx * sizeof(char*));
        obj->data.object.values = realloc(obj->data.object.values,
                                         idx * sizeof(config_value_t*));
        obj->data.object.keys[idx] = key->data.s.data;
        key->data.s.data = NULL;
        config_value_destroy(key);
        obj->data.object.values[idx] = val;

        skip_whitespace(ptr);

        if (**ptr == '}') {
            (*ptr)++;
            return obj;
        }

        if (**ptr != ',') goto error;
        (*ptr)++;
    }

error:
    config_value_destroy(obj);
    return NULL;
}

static config_value_t* parse_json_array(const char **ptr) {
    if (**ptr != '[') return NULL;
    (*ptr)++;

    config_value_t *arr = config_value_create_array();
    skip_whitespace(ptr);

    if (**ptr == ']') {
        (*ptr)++;
        return arr;
    }

    while (**ptr) {
        config_value_t *val = parse_json_value(ptr);
        if (!val) goto error;

        size_t idx = arr->data.array.count++;
        arr->data.array.items = realloc(arr->data.array.items,
                                       idx * sizeof(config_value_t*));
        arr->data.array.items[idx] = val;

        skip_whitespace(ptr);

        if (**ptr == ']') {
            (*ptr)++;
            return arr;
        }

        if (**ptr != ',') goto error;
        (*ptr)++;
    }

error:
    config_value_destroy(arr);
    return NULL;
}

static config_value_t* parse_json_value(const char **ptr) {
    skip_whitespace(ptr);

    if (**ptr == '"') return parse_json_string(ptr);
    if (**ptr == '{') return parse_json_object(ptr);
    if (**ptr == '[') return parse_json_array(ptr);
    if (**ptr == 't' && strncmp(*ptr, "true", 4) == 0) {
        *ptr += 4;
        return config_value_create_bool(true);
    }
    if (**ptr == 'f' && strncmp(*ptr, "false", 5) == 0) {
        *ptr += 5;
        return config_value_create_bool(false);
    }
    if (**ptr == 'n' && strncmp(*ptr, "null", 4) == 0) {
        *ptr += 4;
        return config_value_create_null();
    }
    if (**ptr == '-' || isdigit((unsigned char)**ptr)) {
        return parse_json_number(ptr);
    }

    return NULL;
}

config_t* config_load_json(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *content = malloc(size + 1);
    fread(content, 1, size, fp);
    content[size] = '\0';
    fclose(fp);

    const char *ptr = content;
    config_value_t *root = parse_json_value(&ptr);
    free(content);

    if (!root) return NULL;

    config_t *cfg = config_create();
    config_value_destroy(cfg->root);
    cfg->root = root;
    cfg->source_file = strdup(path);

    /* Get file modification time */
    struct stat st;
    if (stat(path, &st) == 0) {
        cfg->last_modified = st.st_mtime;
    }

    return cfg;
}

/* Getters with type checking */
int config_get_bool(config_t *cfg, const char *path, bool *out) {
    pthread_rwlock_rdlock(&cfg->lock);

    size_t count;
    char **parts = config_split_path(path, &count);
    config_value_t *val = navigate_path(cfg->root, (const char**)parts, count, false);
    config_free_path_parts(parts, count);

    if (!val || val->type != CONFIG_BOOL) {
        pthread_rwlock_unlock(&cfg->lock);
        return CONFIG_NOT_FOUND;
    }

    *out = val->data.b;
    pthread_rwlock_unlock(&cfg->lock);
    return CONFIG_OK;
}

int config_get_int(config_t *cfg, const char *path, int64_t *out) {
    pthread_rwlock_rdlock(&cfg->lock);

    size_t count;
    char **parts = config_split_path(path, &count);
    config_value_t *val = navigate_path(cfg->root, (const char**)parts, count, false);
    config_free_path_parts(parts, count);

    if (!val) {
        pthread_rwlock_unlock(&cfg->lock);
        return CONFIG_NOT_FOUND;
    }

    if (val->type == CONFIG_INT) {
        *out = val->data.i;
    } else if (val->type == CONFIG_FLOAT) {
        *out = (int64_t)val->data.f;
    } else {
        pthread_rwlock_unlock(&cfg->lock);
        return CONFIG_TYPE_MISMATCH;
    }

    pthread_rwlock_unlock(&cfg->lock);
    return CONFIG_OK;
}

int config_get_string(config_t *cfg, const char *path, char *out, size_t len) {
    pthread_rwlock_rdlock(&cfg->lock);

    size_t count;
    char **parts = config_split_path(path, &count);
    config_value_t *val = navigate_path(cfg->root, (const char**)parts, count, false);
    config_free_path_parts(parts, count);

    if (!val || val->type != CONFIG_STRING) {
        pthread_rwlock_unlock(&cfg->lock);
        return CONFIG_NOT_FOUND;
    }

    strncpy(out, val->data.s.data, len - 1);
    out[len - 1] = '\0';

    pthread_rwlock_unlock(&cfg->lock);
    return CONFIG_OK;
}

/* Environment variable resolution */
static char* resolve_env_var(const char *str, size_t *consumed) {
    if (str[0] != '$' || str[1] != '{') {
        *consumed = 0;
        return NULL;
    }

    const char *end = strchr(str + 2, '}');
    if (!end) {
        *consumed = 0;
        return NULL;
    }

    size_t var_len = end - (str + 2);
    char var_name[256];
    memcpy(var_name, str + 2, var_len);
    var_name[var_len] = '\0';

    *consumed = (end - str) + 1;

    /* Check for default value */
    char *default_val = strchr(var_name, ':');
    if (default_val) {
        *default_val = '\0';
        default_val += 2;  /* Skip ":-" */
    }

    const char *env = getenv(var_name);
    if (env) {
        return strdup(env);
    } else if (default_val) {
        return strdup(default_val);
    }

    return strdup("");
}

static void resolve_env_in_string(config_value_t *val) {
    if (val->type != CONFIG_STRING) return;

    char *str = val->data.s.data;
    size_t len = val->data.s.len;
    char *result = malloc(len * 2);
    size_t result_pos = 0;

    for (size_t i = 0; i < len; ) {
        if (str[i] == '$' && i + 1 < len && str[i+1] == '{') {
            size_t consumed;
            char *replacement = resolve_env_var(str + i, &consumed);
            if (replacement) {
                size_t repl_len = strlen(replacement);
                result = realloc(result, result_pos + repl_len + (len - i - consumed) + 1);
                memcpy(result + result_pos, replacement, repl_len);
                result_pos += repl_len;
                free(replacement);
                i += consumed;
            } else {
                result[result_pos++] = str[i++];
            }
        } else {
            result[result_pos++] = str[i++];
        }
    }

    result[result_pos] = '\0';
    free(val->data.s.data);
    val->data.s.data = result;
    val->data.s.len = result_pos;
}

static void resolve_env_recursive(config_value_t *val) {
    switch (val->type) {
        case CONFIG_STRING:
            resolve_env_in_string(val);
            break;
        case CONFIG_ARRAY:
            for (size_t i = 0; i < val->data.array.count; i++) {
                resolve_env_recursive(val->data.array.items[i]);
            }
            break;
        case CONFIG_OBJECT:
            for (size_t i = 0; i < val->data.object.count; i++) {
                resolve_env_recursive(val->data.object.values[i]);
            }
            break;
        default:
            break;
    }
}

int config_resolve_env(config_t *cfg) {
    pthread_rwlock_wrlock(&cfg->lock);
    resolve_env_recursive(cfg->root);
    pthread_rwlock_unlock(&cfg->lock);
    return CONFIG_OK;
}

/* Hot reload with inotify */
struct config_watcher {
    int inotify_fd;
    int watch_fd;
    pthread_t thread;
    volatile bool running;
    config_t *cfg;
};

static void* watcher_thread(void *arg) {
    config_watcher_t *watcher = arg;
    char buffer[1024];

    while (watcher->running) {
        ssize_t n = read(watcher->inotify_fd, buffer, sizeof(buffer));
        if (n > 0) {
            /* File changed, reload config */
            config_reload(watcher->cfg);

            if (watcher->cfg->on_reload) {
                watcher->cfg->on_reload(watcher->cfg, watcher->cfg->user_data);
            }
        }
        usleep(100000);  /* 100ms */
    }

    return NULL;
}

int config_enable_reload(config_t *cfg) {
    if (!cfg->source_file) return CONFIG_ERROR;

    cfg->watcher = calloc(1, sizeof(config_watcher_t));
    cfg->watcher->cfg = cfg;
    cfg->watcher->running = true;

    cfg->watcher->inotify_fd = inotify_init1(IN_NONBLOCK);
    if (cfg->watcher->inotify_fd < 0) {
        free(cfg->watcher);
        cfg->watcher = NULL;
        return CONFIG_ERROR;
    }

    cfg->watcher->watch_fd = inotify_add_watch(cfg->watcher->inotify_fd,
                                                cfg->source_file,
                                                IN_MODIFY | IN_CLOSE_WRITE);
    if (cfg->watcher->watch_fd < 0) {
        close(cfg->watcher->inotify_fd);
        free(cfg->watcher);
        cfg->watcher = NULL;
        return CONFIG_ERROR;
    }

    pthread_create(&cfg->watcher->thread, NULL, watcher_thread, cfg->watcher);

    return CONFIG_OK;
}

void config_disable_reload(config_t *cfg) {
    if (!cfg->watcher) return;

    cfg->watcher->running = false;
    pthread_join(cfg->watcher->thread, NULL);

    inotify_rm_watch(cfg->watcher->inotify_fd, cfg->watcher->watch_fd);
    close(cfg->watcher->inotify_fd);

    free(cfg->watcher);
    cfg->watcher = NULL;
}

int config_reload(config_t *cfg) {
    if (!cfg->source_file) return CONFIG_ERROR;

    config_t *new_cfg = config_load_json(cfg->source_file);
    if (!new_cfg) return CONFIG_ERROR;

    pthread_rwlock_wrlock(&cfg->lock);

    config_value_destroy(cfg->root);
    cfg->root = new_cfg->root;
    cfg->last_modified = new_cfg->last_modified;

    pthread_rwlock_unlock(&cfg->lock);

    /* Free temporary structure but keep root value */
    new_cfg->root = NULL;
    config_destroy(new_cfg);

    return CONFIG_OK;
}

void config_set_reload_callback(config_t *cfg,
                                void (*callback)(config_t*, void*),
                                void *user_data) {
    cfg->on_reload = callback;
    cfg->user_data = user_data;
}

const char* config_error_string(int error) {
    switch (error) {
        case CONFIG_OK: return "OK";
        case CONFIG_ERROR: return "General error";
        case CONFIG_NOT_FOUND: return "Path not found";
        case CONFIG_TYPE_MISMATCH: return "Type mismatch";
        case CONFIG_PARSE_ERROR: return "Parse error";
        case CONFIG_VALIDATION_ERROR: return "Validation error";
        default: return "Unknown error";
    }
}
