/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 1229
 * Language: c
 * Block ID: 46afd164
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 内存接口调试辅助代码

#ifndef __SYNTHESIS__
// 仅在C仿真时包含
#include <stdio.h>
#include <assert.h>

void debug_memory_access(int *mem, int addr, int value, int is_write) {
    if (is_write) {
        printf("[MEM WRITE] addr=0x%08X, value=0x%08X\n", addr, value);
        assert(addr >= 0 && addr < MEM_SIZE);
    } else {
        printf("[MEM READ]  addr=0x%08X, value=0x%08X\n", addr, value);
    }
}

void verify_burst_pattern(int *mem, int base, int len, int expected_incr) {
    for (int i = 1; i < len; i++) {
        int expected = mem[base + i - 1] + expected_incr;
        assert(mem[base + i] == expected);
    }
}
#endif

// 带调试信息的设计
void debuggable_design(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src
    #pragma HLS INTERFACE mode=m_axi port=dst

    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        int val = src[i];

        #ifndef __SYNTHESIS__
        debug_memory_access(src, i, val, 0);
        #endif

        int result = val * 2 + 1;
        dst[i] = result;

        #ifndef __SYNTHESIS__
        debug_memory_access(dst, i, result, 1);
        #endif
    }
}
