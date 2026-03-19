/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 365
 * Language: c
 * Block ID: e2c51384
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11原子操作与推测执行

#include <stdatomic.h>

_Atomic int flag = 0;
int secret = 42;

void spectre_safe_check(void) {
    // 使用 acquire 语义
    if (atomic_load_explicit(&flag, memory_order_acquire)) {
        // 确保之前的所有操作完成
        _mm_lfence();  // 额外保护

        // 现在安全地访问secret
        use(secret);
    }
}
