/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\README.md
 * Line: 569
 * Language: c
 * Block ID: 5f281805
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 心智模型: C 内存布局
// 理解栈帧、堆、数据段的区别

int global_var = 10;        // 数据段 (.data)
const int const_var = 20;   // 只读数据段 (.rodata)

void function(int param) {  // param 在栈上
    int local_var = 30;     // 栈分配
    int* heap_ptr = malloc(sizeof(int));  // 堆分配
    *heap_ptr = 40;

    // 心智模型可视化:
    // ┌─────────────────┐ 高地址
    // │   栈 (向下增长)  │
    // │   param: 值     │
    // │   local_var: 30 │
    // ├─────────────────┤
    // │   ...           │
    // ├─────────────────┤
    // │   堆 (向上增长)  │
    // │   *heap_ptr: 40 │
    // ├─────────────────┤
    // │   数据段         │
    // │   global_var: 10│
    // │   const_var: 20 │
    // └─────────────────┘ 低地址

    free(heap_ptr);
}
