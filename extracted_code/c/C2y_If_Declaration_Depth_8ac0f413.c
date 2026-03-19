/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 1734
 * Language: c
 * Block ID: 8ac0f413
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例 7: 字符串解析
 * 演示使用 if 声明进行复杂的字符串解析
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// 解析整数（带错误检查）
bool parse_int_safe(const char* str, int* result) {
    if (str == NULL || *str == '\0') {
        return false;
    }

    if (char* endptr; true) {
        long val = strtol(str, &endptr, 10);

        // 检查是否有未解析字符
        if (*endptr != '\0') {
            return false;
        }

        // 检查范围
        if (val < INT_MIN || val > INT_MAX) {
            return false;
        }

        *result = (int)val;
        return true;
    }
}

// 解析浮点数
bool parse_double_safe(const char* str, double* result) {
    if (str == NULL || *str == '\0') {
        return false;
    }

    if (char* endptr; true) {
        double val = strtod(str, &endptr);

        if (*endptr != '\0') {
            return false;
        }

        *result = val;
        return true;
    }
}

// 解析键值对（key=value）
bool parse_key_value(const char* str, char* key, size_t key_size,
                     char* value, size_t value_size) {
    if (str == NULL) return false;

    if (const char* eq = strchr(str, '='); eq == NULL) {
        return false;
    } else {
        size_t key_len = eq - str;
        if (key_len >= key_size) {
            return false;
        }

        memcpy(key, str, key_len);
        key[key_len] = '\0';

        strncpy(value, eq + 1, value_size - 1);
        value[value_size - 1] = '\0';

        return true;
    }
}

// 解析 CSV 行
int parse_csv_line(const char* line, char** fields, int max_fields) {
    if (line == NULL || fields == NULL || max_fields <= 0) {
        return 0;
    }

    int count = 0;
    const char* p = line;

    while (*p && count < max_fields) {
        // 跳过前导空白
        while (isspace(*p)) p++;

        if (*p == '\0') break;

        // 处理带引号的字段
        if (*p == '"') {
            p++;
            if (char* field = malloc(strlen(line) + 1); field == NULL) {
                // 清理已分配的字段
                for (int i = 0; i < count; i++) {
                    free(fields[i]);
                }
                return -1;
            } else {
                size_t i = 0;
                while (*p && *p != '"') {
                    field[i++] = *p++;
                }
                field[i] = '\0';

                if (*p == '"') p++;  // 跳过结束引号

                fields[count++] = field;
            }
        } else {
            // 处理普通字段
            if (char* field = malloc(strlen(line) + 1); field == NULL) {
                for (int i = 0; i < count; i++) {
                    free(fields[i]);
                }
                return -1;
            } else {
                size_t i = 0;
                while (*p && *p != ',' && !isspace(*p)) {
                    field[i++] = *p++;
                }
                field[i] = '\0';

                fields[count++] = field;
            }
        }

        // 跳过逗号
        if (*p == ',') p++;
    }

    return count;
}

// 解析 URL 查询参数
bool parse_query_param(const char* query, const char* param_name,
                       char* value, size_t value_size) {
    if (query == NULL || param_name == NULL) return false;

    size_t name_len = strlen(param_name);
    const char* p = query;

    while (*p) {
        // 查找参数名
        if (strncmp(p, param_name, name_len) == 0 && p[name_len] == '=') {
            p += name_len + 1;

            // 复制值
            size_t i = 0;
            while (*p && *p != '&' && i < value_size - 1) {
                value[i++] = *p++;
            }
            value[i] = '\0';

            return true;
        }

        // 跳到下一个参数
        while (*p && *p != '&') p++;
        if (*p == '&') p++;
    }

    return false;
}

// 解析时间字符串（HH:MM:SS）
bool parse_time(const char* str, int* hour, int* minute, int* second) {
    if (str == NULL) return false;

    if (int h = 0, m = 0, s = 0;
        sscanf(str, "%d:%d:%d", &h, &m, &s) == 3) {
        if (h >= 0 && h < 24 && m >= 0 && m < 60 && s >= 0 && s < 60) {
            *hour = h;
            *minute = m;
            *second = s;
            return true;
        }
    }

    return false;
}

// 解析版本号（major.minor.patch）
bool parse_version(const char* str, int* major, int* minor, int* patch) {
    if (str == NULL) return false;

    if (char* p1; true) {
        long maj = strtol(str, &p1, 10);
        if (*p1 != '.') return false;

        if (char* p2; true) {
            long min = strtol(p1 + 1, &p2, 10);
            if (*p2 != '.') return false;

            if (char* p3; true) {
                long pat = strtol(p2 + 1, &p3, 10);
                if (*p3 != '\0') return false;

                *major = (int)maj;
                *minor = (int)min;
                *patch = (int)pat;
                return true;
            }
        }
    }

    return false;
}

int main(void) {
    printf("=== String Parsing Demo ===\n\n");

    // 整数解析
    printf("1. Integer parsing:\n");
    if (int val; parse_int_safe("12345", &val)) {
        printf("  Parsed: %d\n", val);
    }
    if (int val; !parse_int_safe("12.34", &val)) {
        printf("  Correctly rejected: '12.34'\n");
    }

    // 键值对解析
    printf("\n2. Key-value parsing:\n");
    char key[64], value[256];
    if (parse_key_value("name=John Doe", key, sizeof(key), value, sizeof(value))) {
        printf("  Key: '%s', Value: '%s'\n", key, value);
    }

    // CSV 解析
    printf("\n3. CSV parsing:\n");
    char* fields[10];
    if (int count = parse_csv_line("apple,\"banana split\",cherry,date",
                                    fields, 10); count > 0) {
        printf("  Parsed %d fields:\n", count);
        for (int i = 0; i < count; i++) {
            printf("    [%d]: '%s'\n", i, fields[i]);
            free(fields[i]);
        }
    }

    // 查询参数解析
    printf("\n4. Query parameter parsing:\n");
    char param_value[256];
    if (parse_query_param("name=Alice&age=30&city=Beijing", "age",
                          param_value, sizeof(param_value))) {
        printf("  age = %s\n", param_value);
    }

    // 时间解析
    printf("\n5. Time parsing:\n");
    if (int h, m, s; parse_time("14:30:45", &h, &m, &s)) {
        printf("  Time: %02d:%02d:%02d\n", h, m, s);
    }

    // 版本号解析
    printf("\n6. Version parsing:\n");
    if (int maj, min, pat; parse_version("2.15.3", &maj, &min, &pat)) {
        printf("  Version: %d.%d.%d\n", maj, min, pat);
    }

    return 0;
}


### 4.8 复杂资源管理

