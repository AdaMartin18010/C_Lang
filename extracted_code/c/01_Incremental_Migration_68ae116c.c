/*
 * Auto-generated from: 15_Memory_Safe_Languages\02_Migration_Strategies\01_Incremental_Migration.md
 * Line: 682
 * Language: c
 * Block ID: 68ae116c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* config.h */
#ifndef CONFIG_H
#define CONFIG_H

typedef struct {
    char key[64];
    char value[256];
} config_entry_t;

typedef struct {
    config_entry_t* entries;
    size_t count;
    size_t capacity;
} config_t;

config_t* config_load(const char* filename);
const char* config_get(config_t* cfg, const char* key);
void config_free(config_t* cfg);

#endif
