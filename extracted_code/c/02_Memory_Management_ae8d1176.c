/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1689
 * Language: c
 * Block ID: ae8d1176
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 所有权不清晰
typedef struct {
    char *data;
} Container;

Container *create_bad(void) {
    Container *c = malloc(sizeof(Container));
    c->data = malloc(100);  // 分配了，但谁负责释放？
    return c;
}

void destroy_bad(Container *c) {
    free(c);  // ❌ 泄漏 c->data！
}

// ✅ 正确: 明确所有权语义
Container *create_good(void) {
    Container *c = malloc(sizeof(Container));
    if (!c) return NULL;

    c->data = malloc(100);
    if (!c->data) {
        free(c);  // 部分失败时清理
        return NULL;
    }
    return c;
}

void destroy_good(Container *c) {
    if (c) {
        free(c->data);  // 先释放成员
        free(c);         // 再释放容器
    }
}
