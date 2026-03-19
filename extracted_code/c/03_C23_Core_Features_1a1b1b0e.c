/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 305
 * Language: c
 * Block ID: 1a1b1b0e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// static_assert 不再需要在 <assert.h>
static_assert(sizeof(int) == 4, "int must be 32-bit");

// thread_local 替代 _Thread_local
thread_local int thread_data = 0;

// alignas 和 alignof 不再需要在 <stdalign.h>
alignas(64) char cache_line[64];
static_assert(alignof(max_align_t) >= 8);

// bool、true、false 成为关键字
bool flag = true;
