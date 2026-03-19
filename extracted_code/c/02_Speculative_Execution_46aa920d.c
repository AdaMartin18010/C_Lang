/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 234
 * Language: c
 * Block ID: 46aa920d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Meltdown：乱序执行绕过页表权限检查

// 用户空间代码尝试读取内核内存
void meltdown_exploit(void *kernel_addr) {
    // 权限检查与内存加载并行执行
    // 权限检查失败，但加载结果已进入缓存

    uint8_t value = *(uint8_t *)kernel_addr;  // 权限错误，但...

    // 利用缓存侧信道提取value
    temp &= probe_array[value * 4096];
}

// 缓解：KAISER/KPTI - 用户和内核页表分离
