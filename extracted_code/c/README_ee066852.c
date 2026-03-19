/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 238
 * Language: c
 * Block ID: ee066852
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: 手动实现的代数数据类型 (类似 Rust enum)
typedef enum {
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_STRING,
    VALUE_NONE
} value_type_t;

typedef struct {
    value_type_t type;
    union {
        int int_val;
        float float_val;
        char* string_val;
    } data;
} value_t;

// 必须手动检查类型标签
void print_value(const value_t* v) {
    switch (v->type) {
        case VALUE_INT: printf("%d", v->data.int_val); break;
        case VALUE_FLOAT: printf("%f", v->data.float_val); break;
        case VALUE_STRING: printf("%s", v->data.string_val); break;
        case VALUE_NONE: printf("none"); break;
    }
}
