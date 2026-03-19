/*
 * Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
 * Line: 442
 * Language: c
 * Block ID: 8b7030ae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11 原子类型声明
#include <stdatomic.h>
#include <stdint.h>

// 基本原子类型
_Atomic int atomic_int;
_Atomic uint64_t atomic_u64;
_Atomic double atomic_double;  // C11 支持（通常需要锁）
_Atomic void* atomic_ptr;

// 原子结构体（C11 支持）
typedef struct { int x, y; } Point;
_Atomic Point atomic_point;  // 可能需要锁

// 初始化
_Atomic int counter = ATOMIC_VAR_INIT(0);

// 类型修饰符等价
_Atomic int* ptr_to_atomic;      // 指向原子 int 的指针
_Atomic(int*) atomic_ptr_to_int; // 原子指针指向 int
