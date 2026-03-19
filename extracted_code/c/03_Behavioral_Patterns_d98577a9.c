/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\03_Behavioral_Patterns.md
 * Line: 59
 * Language: c
 * Block ID: d98577a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int (*execute)(void* strategy, int a, int b);
} strategy_ops_t;

typedef struct {
    strategy_ops_t* ops;
    void* data;
} context_t;

int add_execute(void* s, int a, int b) {
    (void)s;
    return a + b;
}

int context_execute(context_t* ctx, int a, int b) {
    return ctx->ops->execute(ctx->data, a, b);
}
