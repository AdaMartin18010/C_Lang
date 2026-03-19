/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 341
 * Language: c
 * Block ID: 5851c4fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Intel屏障指令

// lfence：加载屏障，阻止推测加载
// 影响：Spectre V1防护
_mm_lfence();

// sfence：存储屏障
_mm_sfence();

// mfence：全内存屏障
_mm_mfence();

// 序列化指令：阻止所有推测执行
// cpuid, iret, rsm 等

void full_serialization(void) {
    uint32_t eax, ebx, ecx, edx;
    __cpuid(0, eax, ebx, ecx, edx);  // 完全序列化
}
