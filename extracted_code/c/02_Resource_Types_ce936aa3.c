/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\02_Resource_Types.md
 * Line: 239
 * Language: c
 * Block ID: ce936aa3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的借用检查器
typedef enum {
    STATE_OWNED,
    STATE_BORROWED_IMM,
    STATE_BORROWED_MUT,
    STATE_MOVED
} ResourceState;

typedef struct {
    Resource *resource;
    ResourceState state;
    int imm_borrow_count;
} ResourceTracker;

// 检查操作是否合法
bool check_operation(ResourceTracker *tracker, const char *op) {
    if (strcmp(op, "move") == 0) {
        return tracker->state == STATE_OWNED;
    }
    else if (strcmp(op, "borrow_imm") == 0) {
        return tracker->state == STATE_OWNED ||
               tracker->state == STATE_BORROWED_IMM;
    }
    else if (strcmp(op, "borrow_mut") == 0) {
        return tracker->state == STATE_OWNED;
    }
    else if (strcmp(op, "use") == 0) {
        return tracker->state != STATE_MOVED;
    }
    return false;
}

// 模拟操作
void apply_operation(ResourceTracker *tracker, const char *op) {
    if (!check_operation(tracker, op)) {
        fprintf(stderr, "借用检查失败: %s\n", op);
        abort();
    }

    if (strcmp(op, "move") == 0) {
        tracker->state = STATE_MOVED;
    }
    else if (strcmp(op, "borrow_imm") == 0) {
        tracker->imm_borrow_count++;
        tracker->state = STATE_BORROWED_IMM;
    }
    else if (strcmp(op, "borrow_mut") == 0) {
        tracker->state = STATE_BORROWED_MUT;
    }
}
