/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 303
 * Language: c
 * Block ID: 95faabac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// PCF类型到博弈场的构造器
typedef struct TypeArena {
    enum { NAT, ARROW } kind;
    union {
        struct { Arena *arg; Arena *ret; } arrow;
    } data;
    Arena *arena;
} TypeArena;

TypeArena *create_nat_arena(void) {
    TypeArena *ta = malloc(sizeof(TypeArena));
    ta->kind = NAT;
    ta->arena = malloc(sizeof(Arena));
    ta->arena->name = "N";
    ta->arena->num_moves = 2;  // q 和 n
    // 初始化移动和启动关系
    return ta;
}

TypeArena *create_arrow_arena(TypeArena *arg, TypeArena *ret) {
    TypeArena *ta = malloc(sizeof(TypeArena));
    ta->kind = ARROW;
    ta->data.arrow.arg = arg->arena;
    ta->data.arrow.ret = ret->arena;
    // 构造A ⊸ B的博弈场
    ta->arena = construct_linear_arrow(arg->arena, ret->arena);
    return ta;
}
