/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\12_Compiler_Optimization\04_Auto_Vectorization.md
 * Line: 100
 * Language: c
 * Block ID: d4c12548
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 无法向量化的模式

// 1. 数据依赖
void dependent_loop(float *a, int n) {
    for (int i = 1; i < n; i++) {
        a[i] = a[i-1] + 1;  // 流依赖
    }
}

// 2. 函数调用
void call_in_loop(float *a, int n) {
    for (int i = 0; i < n; i++) {
        a[i] = sin(a[i]);  // 外部调用
    }
}

// 3. 复杂控制流
void complex_control(float *a, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] > 0) {
            for (int j = 0; j < i; j++) {  // 嵌套循环
                a[i] += a[j];
            }
        }
    }
}

// 4. 指针别名
void alias_problem(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = b[i] + c[i];
        // 编译器不确定a/b/c是否重叠
    }
}
