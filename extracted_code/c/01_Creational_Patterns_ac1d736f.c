/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 476
 * Language: c
 * Block ID: ac1d736f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 克隆模式 */

typedef struct {
    void* (*clone)(const void*);
} cloneable_t;

typedef struct document {
    cloneable_t base;
    char* title;
    char* content;
    struct document** sections;
    int section_count;
} document_t;

void* document_clone(const void* src) {
    const document_t* orig = src;
    document_t* copy = malloc(sizeof(document_t));

    /* 深拷贝 */
    copy->title = strdup(orig->title);
    copy->content = strdup(orig->content);
    copy->section_count = orig->section_count;
    copy->sections = malloc(sizeof(document_t*) * orig->section_count);

    for (int i = 0; i < orig->section_count; i++) {
        copy->sections[i] = document_clone(orig->sections[i]);
    }

    copy->base.clone = document_clone;
    return copy;
}

document_t* document_new(const char* title) {
    document_t* doc = calloc(1, sizeof(document_t));
    doc->title = strdup(title);
    doc->base.clone = document_clone;
    return doc;
}

/* 使用 */
void example_clone(void) {
    document_t* original = document_new("Original");
    /* 添加内容... */

    document_t* copy = original->base.clone(original);
    /* 修改副本不影响原稿 */
}
