/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\README.md
 * Line: 64
 * Language: c
 * Block ID: 6696db7e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 表达式小步语义的C实现框架
typedef enum {
    EXPR_CONST,
    EXPR_VAR,
    EXPR_ADD,
    EXPR_MUL
} ExprType;

typedef struct Expr {
    ExprType type;
    union {
        int value;           // 常量
        char var[32];        // 变量
        struct { struct Expr *left, *right; } binop;
    };
} Expr;

// 状态：变量到值的映射
typedef struct State {
    char vars[100][32];
    int values[100];
    int count;
} State;

// 小步求值：返回是否可继续规约
int small_step(Expr *e, State *s, Expr **result) {
    switch (e->type) {
        case EXPR_CONST:
            // 常量已是最简形式，不可规约
            return 0;

        case EXPR_VAR:
            // 变量查找替换为值
            for (int i = 0; i < s->count; i++) {
                if (strcmp(s->vars[i], e->var) == 0) {
                    *result = malloc(sizeof(Expr));
                    (*result)->type = EXPR_CONST;
                    (*result)->value = s->values[i];
                    return 1;
                }
            }
            return 0; // 未定义变量

        case EXPR_ADD:
            if (e->binop.left->type == EXPR_CONST &&
                e->binop.right->type == EXPR_CONST) {
                // 两个子表达式都是常量，可以规约
                *result = malloc(sizeof(Expr));
                (*result)->type = EXPR_CONST;
                (*result)->value = e->binop.left->value +
                                   e->binop.right->value;
                return 1;
            }
            // 先尝试规约左子表达式
            if (small_step(e->binop.left, s, result)) {
                Expr *new_expr = malloc(sizeof(Expr));
                new_expr->type = EXPR_ADD;
                new_expr->binop.left = *result;
                new_expr->binop.right = e->binop.right;
                *result = new_expr;
                return 1;
            }
            // 再尝试规约右子表达式
            if (small_step(e->binop.right, s, result)) {
                Expr *new_expr = malloc(sizeof(Expr));
                new_expr->type = EXPR_ADD;
                new_expr->binop.left = e->binop.left;
                new_expr->binop.right = *result;
                *result = new_expr;
                return 1;
            }
            return 0;
    }
    return 0;
}
