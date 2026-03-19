/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\03_Linear_Logic\01_Linear_Logic_Theory.md
 * Line: 298
 * Language: c
 * Block ID: a397ed86
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 直和类型的C模拟（标记联合体）
typedef struct {
    enum { LEFT, RIGHT } tag;
    union {
        Resource *left;
        Resource *right;
    } data;
} Sum;

Sum *sum_left(Resource *a) {
    Sum *s = malloc(sizeof(Sum));
    s->tag = LEFT;
    s->data.left = a;
    return s;
}

Sum *sum_right(Resource *b) {
    Sum *s = malloc(sizeof(Sum));
    s->tag = RIGHT;
    s->data.right = b;
    return s;
}

// 消去（case分析）
Resource *sum_elim(Sum *s,
                    Resource *(*on_left)(Resource *),
                    Resource *(*on_right)(Resource *)) {
    Resource *result;
    if (s->tag == LEFT) {
        result = on_left(s->data.left);
    } else {
        result = on_right(s->data.right);
    }
    free(s);
    return result;
}
