/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 200
 * Language: c
 * Block ID: b2790005
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 可变借用：唯一写入者
typedef struct {
    Resource *resource;
    bool active;
} MutableBorrow;

static MutableBorrow mut_borrow = {0};

// 获取可变借用
void *borrow_mut(Resource *r) {
    if (!r->valid) {
        return NULL;
    }

    // 检查没有其他借用
    if (registry.count > 0 || mut_borrow.active) {
        return NULL;  // 借用冲突
    }

    mut_borrow.resource = r;
    mut_borrow.active = true;

    return r->ptr;
}

// 释放可变借用
void release_mut_borrow(void) {
    mut_borrow.active = false;
    mut_borrow.resource = NULL;
}

// 借用规则：
// 1. 任意数量的不可变借用，或
// 2. 恰好一个可变借用
