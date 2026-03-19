/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_Algebraic_Topology\01_Type_Algebra.md
 * Line: 157
 * Language: c
 * Block ID: 7f58d938
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// B^A = A -> B (函数类型)

// 基数计算:
// |bool -> bool| = |bool|^|bool| = 2^2 = 4

// 四个可能的函数:
bool f1(bool x) { return true; }   // const true
bool f2(bool x) { return false; }  // const false
bool f3(bool x) { return x; }      // identity
bool f4(bool x) { return !x; }     // negation

// 高阶类型
// (B^A)^C = C -> (A -> B) ≅ (C × A) -> B = B^(C×A)
// 柯里化/反柯里化

typedef B (*Curried)(A);          // C -> (A -> B)
typedef B (*Uncurried)(C, A);     // (C × A) -> B

Uncurried uncurry(Curried f) {
    return lambda(void, (C c, A a), {
        return f(c)(a);
    });
}
