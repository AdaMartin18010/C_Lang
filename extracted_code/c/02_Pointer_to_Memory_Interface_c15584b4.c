/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 140
 * Language: c
 * Block ID: c15584b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// static指针 - 保留状态，映射为寄存器或RAM
void static_pointer_example(int *in, int *out) {
    static int history[10];  // 映射为10个寄存器或一个小RAM
    static int ptr = 0;      // 映射为4位计数器

    history[ptr] = *in;
    *out = history[(ptr + 5) % 10];  // 5级延迟线
    ptr = (ptr + 1) % 10;
}

// const指针 - 只读存储器
void const_pointer(const int rom[256], int addr, int *data) {
    #pragma HLS INTERFACE mode=ap_memory port=rom
    *data = rom[addr];  // rom映射为只读存储器
}

// volatile指针 - 外部可见，禁用优化
void volatile_pointer(volatile int *status_reg) {
    // 每次读取都从外部获取最新值
    while ((*status_reg & 0x01) == 0) {
        #pragma HLS PIPELINE
        // 忙等待
    }
}
