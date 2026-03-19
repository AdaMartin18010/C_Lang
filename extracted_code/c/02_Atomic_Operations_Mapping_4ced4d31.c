/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 714
 * Language: c
 * Block ID: 4ced4d31
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 信号栅栏
volatile sig_atomic_t signal_flag = 0;

void signal_handler(int sig) {
    // 处理...
    atomic_signal_fence(memory_order_release);
    signal_flag = 1;
}
