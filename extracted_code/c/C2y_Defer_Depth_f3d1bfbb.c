/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3142
 * Language: c
 * Block ID: f3d1bfbb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// config_parser.c
// 演示defer在实际项目中的应用

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

#define MAX_LINE_LENGTH 1024
#define MAX_KEY_LENGTH 256
#define MAX_VALUE_LENGTH 768

typedef struct ConfigEntry {
    char key[MAX_KEY_LENGTH];
    char value[MAX_VALUE_LENGTH];
    struct ConfigEntry *next;
} ConfigEntry;

typedef struct {
    ConfigEntry *head;
    ConfigEntry *tail;
    size_t count;
} Config;

// 创建配置对象
Config *config_create(void) {
    Config *config = calloc(1, sizeof(Config));
    return config;
}

// 释放配置对象
void config_destroy(Config *config) {
    if (!config) return;

    ConfigEntry *entry = config->head;
    while (entry) {
        ConfigEntry *next = entry->next;
        free(entry);
        entry = next;
    }

    free(config);
}

// 添加配置项
int config_set(Config *config, const char *key, const char *value) {
    if (!config || !key || !value) return -1;

    // 查找现有项
    ConfigEntry *entry = config->head;
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            strncpy(entry->value, value, MAX_VALUE_LENGTH - 1);
            entry->value[MAX_VALUE_LENGTH - 1] = '\0';
            return 0;
        }
        entry = entry->next;
    }

    // 创建新项
    entry = malloc(sizeof(ConfigEntry));
    if (!entry) return -1;
    defer free(entry);  // 安全网

    strncpy(entry->key, key, MAX_KEY_LENGTH - 1);
    entry->key[MAX_KEY_LENGTH - 1] = '\0';
    strncpy(entry->value, value, MAX_VALUE_LENGTH - 1);
    entry->value[MAX_VALUE_LENGTH - 1] = '\0';
    entry->next = NULL;

    // 添加到链表
    if (config->tail) {
        config->tail->next = entry;
    } else {
        config->head = entry;
    }
    config->tail = entry;
    config->count++;

    // 取消defer
    return 0;
}

// 获取配置项
const char *config_get(Config *config, const char *key) {
    if (!config || !key) return NULL;

    ConfigEntry *entry = config->head;
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

// 去除字符串首尾空白
static void trim(char *str) {
    if (!str) return;

    // 去除尾部空白
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) {
        *end-- = '\0';
    }

    // 去除首部空白
    char *start = str;
    while (isspace((unsigned char)*start)) {
        start++;
    }

    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

// 解析配置文件
Config *config_load(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Cannot open config file '%s': %s\n",
                filename, strerror(errno));
        return NULL;
    }
    defer fclose(fp);

    Config *config = config_create();
    if (!config) {
        fprintf(stderr, "Failed to create config object\n");
        return NULL;
    }
    defer config_destroy(config);  // 安全网

    char line[MAX_LINE_LENGTH];
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        // 去除换行符
        line[strcspn(line, "\n")] = '\0';

        // 去除注释
        char *comment = strchr(line, '#');
        if (comment) *comment = '\0';

        // 去除空白
        trim(line);

        // 跳过空行
        if (strlen(line) == 0) continue;

        // 解析键值对
        char *equals = strchr(line, '=');
        if (!equals) {
            fprintf(stderr, "Warning: Line %d has no '=' sign: %s\n",
                    line_num, line);
            continue;
        }

        *equals = '\0';
        char *key = line;
        char *value = equals + 1;

        trim(key);
        trim(value);

        if (strlen(key) == 0) {
            fprintf(stderr, "Warning: Line %d has empty key\n", line_num);
            continue;
        }

        // 添加到配置
        if (config_set(config, key, value) != 0) {
            fprintf(stderr, "Error: Failed to set config value at line %d\n",
                    line_num);
            return NULL;  // config自动销毁
        }
    }

    if (ferror(fp)) {
        fprintf(stderr, "Error reading config file\n");
        return NULL;
    }

    // 成功，取消defer
    Config *result = config;
    return result;
}

// 保存配置文件
int config_save(const Config *config, const char *filename) {
    if (!config || !filename) return -1;

    // 使用临时文件
    char temp_filename[256];
    snprintf(temp_filename, sizeof(temp_filename), "%s.tmp", filename);

    FILE *fp = fopen(temp_filename, "w");
    if (!fp) {
        fprintf(stderr, "Cannot create temp file: %s\n", strerror(errno));
        return -1;
    }
    defer {
        fclose(fp);
        // 如果失败，删除临时文件
        if (rename_successful == 0) {
            remove(temp_filename);
        }
    };

    int rename_successful = 0;

    fprintf(fp, "# Auto-generated configuration file\n");
    fprintf(fp, "# Do not edit manually\n\n");

    ConfigEntry *entry = config->head;
    while (entry) {
        if (fprintf(fp, "%s=%s\n", entry->key, entry->value) < 0) {
            fprintf(stderr, "Error writing to config file\n");
            return -1;
        }
        entry = entry->next;
    }

    // 关闭文件
    fclose(fp);

    // 原子替换
    if (rename(temp_filename, filename) != 0) {
        fprintf(stderr, "Error replacing config file: %s\n", strerror(errno));
        return -1;
    }

    rename_successful = 1;
    return 0;
}

// 打印配置
void config_print(const Config *config) {
    if (!config) {
        printf("(null config)\n");
        return;
    }

    printf("Configuration (%zu entries):\n", config->count);

    ConfigEntry *entry = config->head;
    while (entry) {
        printf("  %s = %s\n", entry->key, entry->value);
        entry = entry->next;
    }
}

// 主函数
int main(int argc, char *argv[]) {
    const char *config_file = argc > 1 ? argv[1] : "test.conf";

    // 加载配置
    Config *config = config_load(config_file);
    if (!config) {
        printf("Creating new configuration\n");
        config = config_create();
        if (!config) {
            fprintf(stderr, "Failed to create config\n");
            return 1;
        }
    }
    defer config_destroy(config);

    // 打印当前配置
    config_print(config);

    // 设置一些值
    config_set(config, "app.name", "MyApp");
    config_set(config, "app.version", "1.0.0");
    config_set(config, "server.host", "localhost");
    config_set(config, "server.port", "8080");

    printf("\nAfter updates:\n");
    config_print(config);

    // 保存配置
    if (config_save(config, config_file) == 0) {
        printf("\nConfiguration saved to %s\n", config_file);
    } else {
        fprintf(stderr, "Failed to save configuration\n");
        return 1;
    }

    return 0;
}
