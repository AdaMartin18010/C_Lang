/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 488
 * Language: c
 * Block ID: 129de98f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 检查CPU微码版本和缓解状态

#include <cpuid.h>

void check_mitigation_status(void) {
    // 检查IBPB (Indirect Branch Predictor Barrier)
    // 检查IBRS (Indirect Branch Restricted Speculation)
    // 检查STIBP (Single Thread Indirect Branch Predictors)

    uint32_t eax, ebx, ecx, edx;

    // 检查ARCH_CAPABILITIES MSR
    __get_cpuid(7, &eax, &ebx, &ecx, &edx);
    bool has_arch_caps = edx & (1 << 29);

    if (has_arch_caps) {
        printf("CPU支持ARCH_CAPABILITIES MSR\n");
        // 可以查询具体的缓解能力
    }
}

// Linux下查看缓解状态
// $ cat /proc/cpuinfo | grep bugs
// bugs : cpu_meltdown spectre_v1 spectre_v2 spec_store_bypass l1tf mds swapgs
