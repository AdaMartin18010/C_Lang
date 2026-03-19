/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\01_Formal_Semantics\README.md
 * Line: 342
 * Language: c
 * Block ID: db070815
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型表示
typedef enum {
    TYPE_INT,
    TYPE_BOOL,
    TYPE_FUNC,
    TYPE_PTR
} TypeKind;

typedef struct Type {
    TypeKind kind;
    union {
        struct { struct Type *arg, *ret; } func;  // 函数类型
        struct Type *pointee;                     // 指针类型
    };
} Type;

// 类型环境
typedef struct TypeEnv {
    char name[64];
    Type *type;
    struct TypeEnv *next;
} TypeEnv;

// 类型检查函数
int type_check(Expr *e, TypeEnv *env, Type **result);

// 示例：函数类型的类型检查
int check_function_app(Expr *fn, Expr *arg, TypeEnv *env, Type **result) {
    Type *fn_type, *arg_type;

    if (!type_check(fn, env, &fn_type)) return 0;
    if (!type_check(arg, env, &arg_type)) return 0;

    if (fn_type->kind != TYPE_FUNC) {
        fprintf(stderr, "Error: expected function type\n");
        return 0;
    }

    if (!type_equal(fn_type->func.arg, arg_type)) {
        fprintf(stderr, "Error: argument type mismatch\n");
        return 0;
    }

    *result = fn_type->func.ret;
    return 1;
}
