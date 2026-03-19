/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\02_Cartesian_Closed_Categories.md
 * Line: 346
 * Language: c
 * Block ID: 1cf91016
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 简单类型λ演算
 * 在C中实现λ演算的核心概念
 */

// λ项的类型
typedef enum {
    TERM_VAR,       // 变量
    TERM_ABS,       // 抽象 (λx.M)
    TERM_APP,       // 应用 (M N)
} term_type_t;

// 类型表达式
typedef struct type {
    enum {
        TYPE_BASE,      // 基本类型
        TYPE_ARROW,     // 函数类型 A → B
        TYPE_PRODUCT,   // 积类型 A × B
    } kind;
    union {
        char base_name[32];
        struct {
            struct type *domain;
            struct type *codomain;
        } arrow;
        struct {
            struct type *fst;
            struct type *snd;
        } product;
    } data;
} type_t;

// λ项
typedef struct term {
    term_type_t type;
    union {
        char var_name[32];          // 变量名
        struct {                     // 抽象
            char var[32];
            struct type *var_type;
            struct term *body;
        } abs;
        struct {                     // 应用
            struct term *fn;
            struct term *arg;
        } app;
    } data;
} term_t;

// 创建基本类型
type_t *type_base(const char *name)
{
    type_t *t = malloc(sizeof(type_t));
    t->kind = TYPE_BASE;
    strncpy(t->data.base_name, name, 31);
    return t;
}

// 创建函数类型
type_t *type_arrow(type_t *domain, type_t *codomain)
{
    type_t *t = malloc(sizeof(type_t));
    t->kind = TYPE_ARROW;
    t->data.arrow.domain = domain;
    t->data.arrow.codomain = codomain;
    return t;
}

// 创建变量项
term_t *term_var(const char *name)
{
    term_t *t = malloc(sizeof(term_t));
    t->type = TERM_VAR;
    strncpy(t->data.var_name, name, 31);
    return t;
}

// 创建抽象项 (λx:A. M)
term_t *term_abs(const char *var, type_t *var_type, term_t *body)
{
    term_t *t = malloc(sizeof(term_t));
    t->type = TERM_ABS;
    strncpy(t->data.abs.var, var, 31);
    t->data.abs.var_type = var_type;
    t->data.abs.body = body;
    return t;
}

// 创建应用项 (M N)
term_t *term_app(term_t *fn, term_t *arg)
{
    term_t *t = malloc(sizeof(term_t));
    t->type = TERM_APP;
    t->data.app.fn = fn;
    t->data.app.arg = arg;
    return t;
}

// 打印类型
void print_type(const type_t *t)
{
    switch (t->kind) {
        case TYPE_BASE:
            printf("%s", t->data.base_name);
            break;
        case TYPE_ARROW:
            printf("(");
            print_type(t->data.arrow.domain);
            printf(" → ");
            print_type(t->data.arrow.codomain);
            printf(")");
            break;
        case TYPE_PRODUCT:
            printf("(");
            print_type(t->data.product.fst);
            printf(" × ");
            print_type(t->data.product.snd);
            printf(")");
            break;
    }
}
