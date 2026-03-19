/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 153
 * Language: c
 * Block ID: 8bffc869
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不可变借用：允许多个读者
typedef struct {
    const Resource *resource;
    int borrow_id;
} ImmutableBorrow;

// 借用表（全局管理）
typedef struct {
    ImmutableBorrow *borrows;
    int count;
    int capacity;
} BorrowRegistry;

static BorrowRegistry registry = {0};

// 获取不可变借用
const void *borrow_imm(Resource *r, int *borrow_id) {
    if (!r->valid) {
        return NULL;  // 资源无效
    }

    // 检查没有可变借用
    // ... (检查逻辑)

    // 注册借用
    if (registry.count >= registry.capacity) {
        registry.capacity = registry.capacity ? registry.capacity * 2 : 10;
        registry.borrows = realloc(registry.borrows,
                                   registry.capacity * sizeof(ImmutableBorrow));
    }

    *borrow_id = registry.count;
    registry.borrows[registry.count++] = (ImmutableBorrow){r, *borrow_id};

    return r->ptr;
}

// 释放借用
void release_borrow(int borrow_id) {
    // 标记借用结束
    // ...
}
