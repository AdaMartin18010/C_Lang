/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
 * Line: 133
 * Language: c
 * Block ID: f6c31c46
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 _Atomic 类型修饰符完整指南
#include <stdatomic.h>
#include <stdint.h>
#include <stdbool.h>

// 基本原子类型
_Atomic int atomic_int;
_Atomic uint64_t atomic_u64;
_Atomic bool atomic_bool;
_Atomic void* atomic_ptr;

// 固定宽度类型
_Atomic int32_t atomic_i32;
_Atomic uint64_t atomic_u64;

// 初始化
_Atomic int counter = ATOMIC_VAR_INIT(0);

// 类型修饰符位置
_Atomic int* ptr_to_atomic;       // 指向原子 int 的指针
_Atomic(int*) atomic_ptr_to_int;  // 原子指针指向 int
