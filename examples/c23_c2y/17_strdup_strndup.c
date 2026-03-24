/*
 * C23 strdup 和 strndup 示例
 * 编译: gcc -std=c23 -o 17_strdup_strndup 17_strdup_strndup.c
 *       clang -std=c23 -o 17_strdup_strndup 17_strdup_strndup.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 复制字符串（标准方式，不再需要自定义实现）
void demonstrate_strdup(void) {
    const char *original = "Hello, C23 World!";
    
    // strdup - 复制整个字符串
    char *copy = strdup(original);
    if (copy) {
        printf("Original: %s\n", original);
        printf("Copy:     %s\n", copy);
        printf("Different addresses: original=%p, copy=%p\n", 
               (void*)original, (void*)copy);
        
        // 修改副本不影响原字符串
        copy[0] = 'h';
        printf("After modification:\n");
        printf("Original: %s\n", original);
        printf("Copy:     %s\n", copy);
        
        free(copy);
    }
}

// strndup - 复制指定长度
void demonstrate_strndup(void) {
    const char *long_string = "This is a very long string that we might want to truncate";
    
    // 复制前20个字符
    char *truncated = strndup(long_string, 20);
    if (truncated) {
        printf("\nOriginal: %s\n", long_string);
        printf("Truncated (20 chars): %s\n", truncated);
        printf("Length: %zu\n", strlen(truncated));
        free(truncated);
    }
    
    // 当n大于字符串长度时
    char *full_copy = strndup(long_string, 1000);
    if (full_copy) {
        printf("\nWith large n: %s\n", full_copy);
        printf("Length: %zu (same as original)\n", strlen(full_copy));
        free(full_copy);
    }
    
    // 处理可能的溢出
    const char *sensitive = "password123456";
    char *safe = strndup(sensitive, 8);  // 最多8个字符
    if (safe) {
        printf("\nSafe copy (max 8 chars): %s\n", safe);
        free(safe);
    }
}

// 实际应用：字符串数组处理
char** duplicate_string_array(const char **arr, size_t count) {
    char **copy = malloc((count + 1) * sizeof(char*));
    if (!copy) return NULL;
    
    for (size_t i = 0; i < count; i++) {
        copy[i] = strdup(arr[i]);
        if (!copy[i]) {
            // 清理已分配的内存
            for (size_t j = 0; j < i; j++) {
                free(copy[j]);
            }
            free(copy);
            return NULL;
        }
    }
    copy[count] = NULL;  // NULL终止
    
    return copy;
}

void free_string_array(char **arr) {
    if (!arr) return;
    
    for (size_t i = 0; arr[i] != NULL; i++) {
        free(arr[i]);
    }
    free(arr);
}

// 配置解析示例
typedef struct {
    char *key;
    char *value;
} ConfigEntry;

ConfigEntry* parse_config_line(const char *line) {
    ConfigEntry *entry = malloc(sizeof(ConfigEntry));
    if (!entry) return NULL;
    
    // 查找等号
    const char *equal = strchr(line, '=');
    if (!equal) {
        free(entry);
        return NULL;
    }
    
    size_t key_len = equal - line;
    entry->key = strndup(line, key_len);
    entry->value = strdup(equal + 1);
    
    // 去除value的换行符
    size_t value_len = strlen(entry->value);
    if (value_len > 0 && entry->value[value_len - 1] == '\n') {
        entry->value[value_len - 1] = '\0';
    }
    
    return entry;
}

void free_config_entry(ConfigEntry *entry) {
    if (entry) {
        free(entry->key);
        free(entry->value);
        free(entry);
    }
}

int main(void) {
    printf("C23 strdup and strndup Demo\n");
    printf("============================\n\n");
    
    demonstrate_strdup();
    demonstrate_strndup();
    
    printf("\n--- String Array Duplication ---\n");
    const char *fruits[] = {"Apple", "Banana", "Cherry", "Date"};
    size_t count = sizeof(fruits) / sizeof(fruits[0]);
    
    char **fruit_copy = duplicate_string_array(fruits, count);
    if (fruit_copy) {
        printf("Original array:\n");
        for (size_t i = 0; i < count; i++) {
            printf("  [%zu] %s (%p)\n", i, fruits[i], (void*)fruits[i]);
        }
        
        printf("\nDuplicated array:\n");
        for (size_t i = 0; fruit_copy[i] != NULL; i++) {
            printf("  [%zu] %s (%p)\n", i, fruit_copy[i], (void*)fruit_copy[i]);
        }
        
        free_string_array(fruit_copy);
    }
    
    printf("\n--- Config Parsing ---\n");
    const char *config_line = "database_host=localhost";
    ConfigEntry *entry = parse_config_line(config_line);
    if (entry) {
        printf("Config: %s = %s\n", entry->key, entry->value);
        free_config_entry(entry);
    }
    
    printf("\nstrdup/strndup demo complete!\n");
    
    return 0;
}
