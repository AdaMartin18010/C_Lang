/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\01_Pointer_Memory_Mapping.md
 * Line: 341
 * Language: c
 * Block ID: b733d17f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 单一所有权模式
typedef struct {
    char *data;      // 拥有该内存
    size_t len;
} String;

void string_free(String *s) {
    free(s->data);   // 释放所有权
    s->data = NULL;
}

// 借用模式（非拥有）
void print_string(const String *s) {
    // 只读访问，不获取所有权
    printf("%.*s\n", (int)s->len, s->data);
}
