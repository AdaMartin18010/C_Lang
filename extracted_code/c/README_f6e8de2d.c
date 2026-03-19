/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\06_C_Assembly_Mapping\README.md
 * Line: 373
 * Language: c
 * Block ID: f6e8de2d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C++源码
namespace math {
    int add(int a, int b);
    double add(double a, double b);
}

// GCC修饰后的符号
// _ZN4math3addEii    (int版本)
// _ZN4math3addEdd    (double版本)

// 使用extern "C"禁用修饰
extern "C" {
    int c_add(int a, int b);  // 符号为: c_add
}
