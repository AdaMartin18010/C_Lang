/*
 * Auto-generated from: 12_Practice_Exercises\13_Real_World_Projects.md
 * Line: 2025
 * Language: c
 * Block ID: 33bdc118
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * Configuration Management - JSON/INI style configuration file support
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include "config.h"

static char* trim(char *str) {
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;

    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';

    return str;
}

static config_entry_t* find_entry(const config_t *cfg, const char *key) {
    for (config_entry_t *e = cfg->entries; e; e = e->next) {
        if (strcmp(e->key, key) == 0) {
            return e;
        }
    }
    return NULL;
}

config_t* config_load(const char *path) {
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;

    config_t *cfg = calloc(1, sizeof(config_t));
    if (!cfg) {
        fclose(fp);
        return NULL;
    }

    cfg->source_file = strdup(path);

    struct stat st;
    if (stat(path, &st) == 0) {
        cfg->last_modified = st.st_mtime;
    }

    char line[1024];
    config_entry_t *tail = NULL;

    while (fgets(line, sizeof(line), fp)) {
        char *p = trim(line);

        /* Skip empty lines and comments */
        if (*p == '\0' || *p == '#' || *p == ';') continue;

        /* Parse key = value */
        char *eq = strchr(p, '=');
        if (!eq) continue;

        *eq = '\0';
        char *key = trim(p);
        char *value = trim(eq + 1);

        /* Remove quotes if present */
        if ((*value == '"' || *value == '\'') && value[strlen(value)-1] == *value) {
            value[strlen(value)-1] = '\0';
            value++;
        }

        config_entry_t *entry = calloc(1, sizeof(config_entry_t));
        strncpy(entry->key, key, sizeof(entry->key) - 1);
        strncpy(entry->value, value, sizeof(entry->value) - 1);

        if (!cfg->entries) {
            cfg->entries = entry;
        } else {
            tail->next = entry;
        }
        tail = entry;
    }

    fclose(fp);
    return cfg;
}

void config_free(config_t *cfg) {
    if (!cfg) return;

    config_entry_t *e = cfg->entries;
    while (e) {
        config_entry_t *next = e->next;
        free(e);
        e = next;
    }

    free(cfg->source_file);
    free(cfg);
}

int config_reload(config_t *cfg) {
    if (!cfg || !cfg->source_file) return -1;

    /* Clear existing entries */
    config_entry_t *e = cfg->entries;
    while (e) {
        config_entry_t *next = e->next;
        free(e);
        e = next;
    }
    cfg->entries = NULL;

    /* Reload from file */
    FILE *fp = fopen(cfg->source_file, "r");
    if (!fp) return -1;

    struct stat st;
    if (stat(cfg->source_file, &st) == 0) {
        cfg->last_modified = st.st_mtime;
    }

    char line[1024];
    config_entry_t *tail = NULL;

    while (fgets(line, sizeof(line), fp)) {
        char *p = trim(line);
        if (*p == '\0' || *p == '#') continue;

        char *eq = strchr(p, '=');
        if (!eq) continue;

        *eq = '\0';
        char *key = trim(p);
        char *value = trim(eq + 1);

        config_entry_t *entry = calloc(1, sizeof(config_entry_t));
        strncpy(entry->key, key, sizeof(entry->key) - 1);
        strncpy(entry->value, value, sizeof(entry->value) - 1);

        if (!cfg->entries) {
            cfg->entries = entry;
        } else {
            tail->next = entry;
        }
        tail = entry;
    }

    fclose(fp);
    return 0;
}

const char* config_get_string(const config_t *cfg, const char *key,
                              const char *default_val) {
    config_entry_t *e = find_entry(cfg, key);
    return e ? e->value : default_val;
}

int config_get_int(const config_t *cfg, const char *key, int default_val) {
    const char *val = config_get_string(cfg, key, NULL);
    return val ? atoi(val) : default_val;
}

bool config_get_bool(const config_t *cfg, const char *key, bool default_val) {
    const char *val = config_get_string(cfg, key, NULL);
    if (!val) return default_val;

    return (strcasecmp(val, "true") == 0 ||
            strcasecmp(val, "yes") == 0 ||
            strcasecmp(val, "1") == 0);
}

double config_get_double(const config_t *cfg, const char *key, double default_val) {
    const char *val = config_get_string(cfg, key, NULL);
    return val ? atof(val) : default_val;
}

void config_set_string(config_t *cfg, const char *key, const char *value) {
    config_entry_t *e = find_entry(cfg, key);
    if (e) {
        strncpy(e->value, value, sizeof(e->value) - 1);
    } else {
        e = calloc(1, sizeof(config_entry_t));
        strncpy(e->key, key, sizeof(e->key) - 1);
        strncpy(e->value, value, sizeof(e->value) - 1);
        e->next = cfg->entries;
        cfg->entries = e;
    }
}

void config_set_int(config_t *cfg, const char *key, int value) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", value);
    config_set_string(cfg, key, buf);
}

void config_set_bool(config_t *cfg, const char *key, bool value) {
    config_set_string(cfg, key, value ? "true" : "false");
}

bool config_has_changed(const config_t *cfg) {
    if (!cfg || !cfg->source_file) return false;

    struct stat st;
    if (stat(cfg->source_file, &st) != 0) return false;

    return st.st_mtime != cfg->last_modified;
}
