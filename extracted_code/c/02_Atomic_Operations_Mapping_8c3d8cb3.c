/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 203
 * Language: c
 * Block ID: 8c3d8cb3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 复杂原子类型
#include <stdatomic.h>

// 枚举原子类型
typedef enum { RED, GREEN, BLUE } Color;
_Atomic Color current_color = ATOMIC_VAR_INIT(RED);

// 位标志
_Atomic uint64_t atomic_flags = ATOMIC_VAR_INIT(0);
#define FLAG_1 (1 << 0)
void set_flag(int flag) {
    atomic_fetch_or_explicit(&atomic_flags, flag, memory_order_relaxed);
}
