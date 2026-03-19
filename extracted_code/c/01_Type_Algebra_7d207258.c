/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\01_Type_Algebra.md
 * Line: 90
 * Language: c
 * Block ID: 7d207258
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 交换律: A × B ≅ B × A
struct AB { A a; B b; };
struct BA { B b; A a; };
// 同构: swap :: AB -> BA
struct BA swap(struct AB x) {
    return (struct BA){ x.b, x.a };
}

// 结合律: (A × B) × C ≅ A × (B × C)
struct AB_C { struct AB ab; C c; };
struct A_BC { A a; struct BC bc; };

// 单位律: A × 1 ≅ A
struct A_Unit { A a; void unit; };
// ≅ A（unit 不增加信息）

// 分配律: A × (B + C) ≅ (A × B) + (A × C)
// 左侧
struct A_BC_sum {
    A a;
    struct Either bc;
};
// 右侧
struct Either_AB_AC {
    enum Tag tag;
    union {
        struct { A a; B b; } ab;
        struct { A a; C c; } ac;
    };
};
