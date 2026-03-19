/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 197
 * Language: c
 * Block ID: ff94106c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题：边界可能在运行时变化
void variable_bound(int n, int a[100]) {
    for (int i = 0; i < n; i++) {  // n是变量
        #pragma HLS PIPELINE  // 可能导致问题
        a[i] = process(a[i]);
    }
}

// ✅ 解决方案1：使用常量上限
void fixed_bound(int n, int a[100]) {
    for (int i = 0; i < 100; i++) {  // 常量边界
        #pragma HLS PIPELINE
        if (i < n) {  // 条件执行
            a[i] = process(a[i]);
        }
    }
}

// ✅ 解决方案2：使用tripcount指示
void tripcount_hint(int n, int a[100]) {
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        #pragma HLS LOOP_TRIPCOUNT min=10 max=100 avg=50
        a[i] = process(a[i]);
    }
}

// ✅ 解决方案3：模板参数化（C++）
template<int N>
void template_bound(int a[N]) {
    for (int i = 0; i < N; i++) {
        #pragma HLS UNROLL factor=4
        a[i] = process(a[i]);
    }
}
