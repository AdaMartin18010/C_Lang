/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 470
 * Language: c
 * Block ID: dbac04a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdbool.h>

typedef struct config_entry {
    char key[128];
    char value[512];
    struct config_entry *next;
} config_entry_t;

typedef struct config {
    config_entry_t *entries;
    char *source_file;
    time_t last_modified;
} config_t;

/* Configuration file operations */
config_t* config_load(const char *path);
void config_free(config_t *cfg);
int config_reload(config_t *cfg);

/* Value getters */
const char* config_get_string(const config_t *cfg, const char *key,
                              const char *default_val);
int config_get_int(const config_t *cfg, const char *key, int default_val);
bool config_get_bool(const config_t *cfg, const char *key, bool default_val);
double config_get_double(const config_t *cfg, const char *key, double default_val);

/* Value setters */
void config_set_string(config_t *cfg, const char *key, const char *value);
void config_set_int(config_t *cfg, const char *key, int value);
void config_set_bool(config_t *cfg, const char *key, bool value);

/* File watching for hot reload */
bool config_has_changed(const config_t *cfg);

#endif /* CONFIG_H */
