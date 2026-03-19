/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 493
 * Language: c
 * Block ID: 8b6bd262
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 所有权系统模拟
typedef struct {
    void *data;
    enum { OWNED, BORROWED_MUT, BORROWED_IMM } mode;
    int borrow_count;
} OwnedPtr;

// 转移所有权
OwnedPtr *move(OwnedPtr *p) {
    OwnedPtr *new_p = malloc(sizeof(OwnedPtr));
    *new_p = *p;
    p->mode = BORROWED_IMM;  // 原变量无效
    return new_p;
}

// 可变借用
void *borrow_mut(OwnedPtr *p) {
    if (p->borrow_count > 0) {
        // 错误：已有借用
        return NULL;
    }
    p->mode = BORROWED_MUT;
    return p->data;
}

// 不可变借用
const void *borrow_imm(OwnedPtr *p) {
    if (p->mode == BORROWED_MUT) {
        // 错误：已有可变借用
        return NULL;
    }
    p->borrow_count++;
    return p->data;
}
