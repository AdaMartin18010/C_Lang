/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 1020
 * Language: c
 * Block ID: 95a513e1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 示例8：综合实战 - 使用_Countof构建类型安全容器
 * 展示如何在实际项目中应用_Countof
 */
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

// ==================== 固定大小数组容器 ====================

#define DEFINE_FIXED_ARRAY(Type, Name) \
    typedef struct { \
        Type data[]; \
        size_t count; \
        size_t capacity; \
    } Name

// 栈上固定数组
typedef struct {
    int data[100];
    size_t size;
} FixedIntArray100;

void fixed_array_init(FixedIntArray100 *arr) {
    arr->size = 0;
}

bool fixed_array_push(FixedIntArray100 *arr, int value) {
    if (arr->size >= _Countof(arr->data)) {
        return false;  // 已满
    }
    arr->data[arr->size++] = value;
    return true;
}

// ==================== 环形缓冲区 ====================

typedef struct {
    char buffer[1024];
    size_t head;
    size_t tail;
    size_t count;
} RingBuffer;

void ring_buffer_init(RingBuffer *rb) {
    rb->head = rb->tail = rb->count = 0;
}

bool ring_buffer_write(RingBuffer *rb, char c) {
    if (rb->count >= _Countof(rb->buffer)) {
        return false;  // 满
    }
    rb->buffer[rb->tail] = c;
    rb->tail = (rb->tail + 1) % _Countof(rb->buffer);
    rb->count++;
    return true;
}

bool ring_buffer_read(RingBuffer *rb, char *c) {
    if (rb->count == 0) {
        return false;  // 空
    }
    *c = rb->buffer[rb->head];
    rb->head = (rb->head + 1) % _Countof(rb->buffer);
    rb->count--;
    return true;
}

// ==================== 查找表 ====================

typedef struct {
    int key;
    const char *value;
} KeyValuePair;

typedef struct {
    KeyValuePair pairs[50];
    size_t count;
} LookupTable;

void lookup_table_init(LookupTable *table) {
    table->count = 0;
}

bool lookup_table_insert(LookupTable *table, int key, const char *value) {
    if (table->count >= _Countof(table->pairs)) {
        return false;
    }
    table->pairs[table->count++] = (KeyValuePair){key, value};
    return true;
}

const char *lookup_table_find(LookupTable *table, int key) {
    for (size_t i = 0; i < table->count; i++) {
        if (table->pairs[i].key == key) {
            return table->pairs[i].value;
        }
    }
    return NULL;
}

// ==================== 编译期配置表 ====================

typedef struct {
    const char *name;
    int min_value;
    int max_value;
    int default_value;
} ConfigParam;

// 使用静态数组定义配置参数
static const ConfigParam config_params[] = {
    {"max_connections", 1, 1000, 100},
    {"timeout_seconds", 1, 300, 30},
    {"buffer_size", 1024, 65536, 4096},
    {"retry_count", 0, 10, 3},
    {"log_level", 0, 5, 2},
};

void print_config_params(void) {
    printf("Configuration Parameters (%zu total):\n", _Countof(config_params));
    printf("%-20s %10s %10s %10s\n", "Name", "Min", "Max", "Default");
    printf("-----------------------------------------------------\n");

    for (size_t i = 0; i < _Countof(config_params); i++) {
        const ConfigParam *p = &config_params[i];
        printf("%-20s %10d %10d %10d\n",
               p->name, p->min_value, p->max_value, p->default_value);
    }
}

const ConfigParam *find_config_param(const char *name) {
    for (size_t i = 0; i < _Countof(config_params); i++) {
        if (strcmp(config_params[i].name, name) == 0) {
            return &config_params[i];
        }
    }
    return NULL;
}

// ==================== 测试代码 ====================

int main(void) {
    printf("=== FixedIntArray100 Test ===\n");
    FixedIntArray100 arr;
    fixed_array_init(&arr);

    for (int i = 0; i < 105; i++) {  // 尝试添加超过容量
        if (!fixed_array_push(&arr, i * 10)) {
            printf("Array full at index %d (capacity: %zu)\n",
                   i, _Countof(arr.data));
            break;
        }
    }
    printf("Final size: %zu\n\n", arr.size);

    printf("=== RingBuffer Test ===\n");
    RingBuffer rb;
    ring_buffer_init(&rb);

    const char *msg = "Hello, Ring Buffer!";
    for (size_t i = 0; i < strlen(msg); i++) {
        ring_buffer_write(&rb, msg[i]);
    }

    printf("Buffer capacity: %zu\n", _Countof(rb.buffer));
    printf("Written: \"%s\"\n", msg);
    printf("Read back: \"");

    char c;
    while (ring_buffer_read(&rb, &c)) {
        putchar(c);
    }
    printf("\"\n\n");

    printf("=== LookupTable Test ===\n");
    LookupTable table;
    lookup_table_init(&table);

    lookup_table_insert(&table, 1, "One");
    lookup_table_insert(&table, 2, "Two");
    lookup_table_insert(&table, 3, "Three");

    printf("Table capacity: %zu\n", _Countof(table.pairs));
    printf("Lookup 2: %s\n", lookup_table_find(&table, 2));
    printf("Lookup 5: %s\n\n", lookup_table_find(&table, 5) ?: "Not found");

    printf("=== Config Parameters ===\n");
    print_config_params();

    printf("\nFind 'timeout_seconds': ");
    const ConfigParam *p = find_config_param("timeout_seconds");
    if (p) {
        printf("default=%d, range=[%d, %d]\n",
               p->default_value, p->min_value, p->max_value);
    }

    return 0;
}
