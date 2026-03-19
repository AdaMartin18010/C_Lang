/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 487
 * Language: c
 * Block ID: fb8bf986
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// II=1：理想情况
void ideal_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        c[i] = a[i] + b[i];  // 无依赖，可实现II=1
    }
}

// II>1：存在资源或数据依赖限制
void limited_ii(int a[100], int b[100], int c[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=2  // 可能需要II=2
        c[i] = a[i] * b[i];  // 乘法器资源限制
    }
}
