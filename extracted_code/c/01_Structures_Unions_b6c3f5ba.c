/*
 * Auto-generated from: 01_Core_Knowledge_System\03_Construction_Layer\01_Structures_Unions.md
 * Line: 177
 * Language: c
 * Block ID: b6c3f5ba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C99 柔性数组成员
typedef struct {
    size_t len;
    char data[];  // 柔性数组，不占用结构体大小
} FlexibleString;

// 分配
FlexibleString *create_string(const char *src) {
    size_t len = strlen(src);
    // 分配结构体 + 数组空间
    FlexibleString *s = malloc(sizeof(FlexibleString) + len + 1);
    if (s) {
        s->len = len;
        memcpy(s->data, src, len + 1);
    }
    return s;
}

// 使用
void use_fam(void) {
    FlexibleString *s = create_string("Hello, World!");
    printf("len=%zu, data=%s\n", s->len, s->data);
    free(s);
}

// 对比：指针版本（需要两次分配）
typedef struct {
    size_t len;
    char *data;  // 需要单独分配
} PointerString;

PointerString *create_ptr_string(const char *src) {
    PointerString *s = malloc(sizeof(PointerString));
    s->len = strlen(src);
    s->data = malloc(s->len + 1);  // 第二次分配
    memcpy(s->data, src, s->len + 1);
    return s;
}
// 需要两次free，更容易出错
