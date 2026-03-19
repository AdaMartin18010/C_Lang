/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\03_Loops_to_Pipelines.md
 * Line: 635
 * Language: c
 * Block ID: 434fb870
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 类型1：RAW (Read After Write) - 真依赖
void raw_dependency(int a[100], int *result) {
    int acc = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        // 迭代i+1读取acc，迭代i写入acc
        acc = acc + a[i];  // RAW依赖
    }
    *result = acc;
}

// 类型2：WAR (Write After Read) - 反依赖
void war_dependency(int a[100], int b[100]) {
    int prev = 0;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        int temp = prev + a[i];  // 读取prev
        prev = b[i];             // 写入prev（反依赖）
        b[i] = temp;
    }
}

// 类型3：WAW (Write After Write) - 输出依赖
void waw_dependency(int a[100], int *result) {
    int x;
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        x = a[i] * 2;  // WAW：连续写入x
    }
    *result = x;
}

// 类型4：数组下标依赖
void array_dependency(int a[100], int idx[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        a[idx[i]] = a[idx[i]] + 1;  // 可能存在依赖
    }
}
