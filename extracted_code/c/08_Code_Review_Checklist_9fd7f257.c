/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 1702
 * Language: c
 * Block ID: 9fd7f257
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file function_length.c
 * @brief 函数长度控制规范
 *
 * 推荐标准：
 * - 理想：<= 30 行（不含空行和注释）
 * - 可接受：<= 50 行
 * - 最大：<= 100 行（超过需要拆分）
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== 超长函数示例（约80行）========== */

/**
 * ❌ 函数过长，职责过多
 */
int parse_config_file_long(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        fprintf(stderr, "Cannot open file: %s\n", filename);
        return -1;
    }

    char line[256];
    int line_num = 0;
    int error_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        /* 跳过空行和注释 */
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '#') {
            continue;
        }

        /* 解析 key = value */
        char key[64] = {0};
        char value[128] = {0};

        /* 提取key */
        int i = 0;
        while (*p && *p != ' ' && *p != '\t' && *p != '=' && i < 63) {
            key[i++] = *p++;
        }
        key[i] = '\0';

        /* 跳过空白和= */
        while (*p == ' ' || *p == '\t') p++;
        if (*p != '=') {
            fprintf(stderr, "Line %d: expected '='\n", line_num);
            error_count++;
            continue;
        }
        p++;  /* 跳过= */

        /* 跳过空白 */
        while (*p == ' ' || *p == '\t') p++;

        /* 提取value */
        i = 0;
        while (*p && *p != '\n' && *p != '#' && i < 127) {
            value[i++] = *p++;
        }
        value[i] = '\0';

        /* 去除value尾部空白 */
        while (i > 0 && (value[i-1] == ' ' || value[i-1] == '\t')) {
            value[--i] = '\0';
        }

        /* 应用配置 */
        if (strcmp(key, "timeout") == 0) {
            int timeout = atoi(value);
            if (timeout > 0) {
                g_config.timeout = timeout;
            } else {
                fprintf(stderr, "Line %d: invalid timeout\n", line_num);
                error_count++;
            }
        } else if (strcmp(key, "max_retries") == 0) {
            int retries = atoi(value);
            if (retries >= 0) {
                g_config.max_retries = retries;
            } else {
                fprintf(stderr, "Line %d: invalid max_retries\n", line_num);
                error_count++;
            }
        } else if (strcmp(key, "log_level") == 0) {
            if (strcmp(value, "debug") == 0) {
                g_config.log_level = LOG_DEBUG;
            } else if (strcmp(value, "info") == 0) {
                g_config.log_level = LOG_INFO;
            } else if (strcmp(value, "warn") == 0) {
                g_config.log_level = LOG_WARN;
            } else if (strcmp(value, "error") == 0) {
                g_config.log_level = LOG_ERROR;
            } else {
                fprintf(stderr, "Line %d: unknown log_level\n", line_num);
                error_count++;
            }
        } else {
            fprintf(stderr, "Line %d: unknown key '%s'\n", line_num, key);
            error_count++;
        }
    }

    fclose(fp);
    return error_count;
}

/* ========== 重构后（拆分多个小函数）========== */

typedef struct {
    int timeout;
    int max_retries;
    int log_level;
} config_t;

static config_t g_config = {30, 3, 1};

#define LOG_DEBUG 0
#define LOG_INFO  1
#define LOG_WARN  2
#define LOG_ERROR 3

/* 辅助函数：去除字符串首尾空白 */
static void trim_whitespace(char *str)
{
    if (str == NULL) return;

    /* 去尾部 */
    size_t len = strlen(str);
    while (len > 0 && (str[len-1] == ' ' || str[len-1] == '\t')) {
        str[--len] = '\0';
    }

    /* 去首部 */
    char *start = str;
    while (*start == ' ' || *start == '\t') start++;
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

/* 辅助函数：解析 key = value 格式 */
static int parse_key_value(const char *line, char *key, size_t key_size,
                            char *value, size_t value_size)
{
    const char *eq = strchr(line, '=');
    if (eq == NULL) return -1;

    /* 提取key */
    size_t key_len = eq - line;
    if (key_len >= key_size) key_len = key_size - 1;
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    trim_whitespace(key);

    /* 提取value */
    const char *val_start = eq + 1;
    while (*val_start == ' ' || *val_start == '\t') val_start++;
    strncpy(value, val_start, value_size - 1);
    value[value_size - 1] = '\0';
    trim_whitespace(value);

    return 0;
}

/* 处理具体配置项（每个函数 < 20行） */
static int apply_timeout(const char *value)
{
    int timeout = atoi(value);
    if (timeout <= 0) return -1;
    g_config.timeout = timeout;
    return 0;
}

static int apply_max_retries(const char *value)
{
    int retries = atoi(value);
    if (retries < 0) return -1;
    g_config.max_retries = retries;
    return 0;
}

static int apply_log_level(const char *value)
{
    struct { const char *name; int level; } levels[] = {
        {"debug", LOG_DEBUG}, {"info", LOG_INFO},
        {"warn", LOG_WARN}, {"error", LOG_ERROR}
    };
    for (size_t i = 0; i < sizeof(levels)/sizeof(levels[0]); i++) {
        if (strcmp(value, levels[i].name) == 0) {
            g_config.log_level = levels[i].level;
            return 0;
        }
    }
    return -1;
}

/* 主函数现在只有约30行 */
int parse_config_file_simple(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) return -1;

    char line[256];
    int errors = 0;
    int line_num = 0;

    while (fgets(line, sizeof(line), fp)) {
        line_num++;

        /* 跳过空行和注释 */
        char *p = line;
        while (*p == ' ' || *p == '\t') p++;
        if (*p == '\0' || *p == '\n' || *p == '#') continue;

        /* 解析 */
        char key[64], value[128];
        if (parse_key_value(p, key, sizeof(key), value, sizeof(value)) < 0) {
            fprintf(stderr, "Line %d: parse error\n", line_num);
            errors++;
            continue;
        }

        /* 应用 */
        int rc = -1;
        if (strcmp(key, "timeout") == 0) rc = apply_timeout(value);
        else if (strcmp(key, "max_retries") == 0) rc = apply_max_retries(value);
        else if (strcmp(key, "log_level") == 0) rc = apply_log_level(value);

        if (rc < 0) {
            fprintf(stderr, "Line %d: invalid %s\n", line_num, key);
            errors++;
        }
    }

    fclose(fp);
    return errors;
}
