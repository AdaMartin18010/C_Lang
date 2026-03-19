/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\04_High_Performance_Computing.md
 * Line: 345
 * Language: c
 * Block ID: 4bf3ce36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 循环内创建线程
for (int i = 0; i < 1000; i++) {
    #pragma omp parallel for  // 每次都创建/销毁线程
    process(data[i]);
}

// ✅ 外层并行
#pragma omp parallel for
for (int i = 0; i < 1000; i++) {
    process(data[i]);
}
