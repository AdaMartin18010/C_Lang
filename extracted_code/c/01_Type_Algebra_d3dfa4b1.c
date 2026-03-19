/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\01_Type_Algebra.md
 * Line: 125
 * Language: c
 * Block ID: d3dfa4b1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 自然数: μX. 1 + X
// Nat = Zero | Succ Nat
struct Nat;
struct Nat {
    bool is_zero;
    struct Nat *succ;  // 如果 !is_zero
};

// 列表: μX. 1 + (A × X)
// List A = Nil | Cons A (List A)
struct List {
    bool is_nil;
    struct {
        int head;
        struct List *tail;
    } cons;
};

// 二叉树: μX. 1 + (X × A × X)
struct Tree {
    bool is_leaf;
    struct {
        struct Tree *left;
        int value;
        struct Tree *right;
    } node;
};
