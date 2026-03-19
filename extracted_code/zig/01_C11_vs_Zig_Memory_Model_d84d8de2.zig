//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\01_C11_vs_Zig_Memory_Model.md
// Line: 465
// Language: zig
// Block ID: d84d8de2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 原子类型声明
const std = @import("std");
const Atomic = std.atomic.Value;

// 基本原子类型
var atomic_int = Atomic(i32).init(0);
var atomic_u64 = Atomic(u64).init(0);
// Zig 不支持原子浮点（需手动实现）
var atomic_ptr = Atomic(?*i32).init(null);

// 原子结构体（需确保可原子操作）
const Point = extern struct { x: i32, y: i32 };
var atomic_point: Atomic(Point) = undefined;

// 初始化
var counter = Atomic(i32).init(0);

// 类型修饰符等价
var ptr_to_atomic: *Atomic(i32) = &counter;  // 指向原子 int 的指针
var atomic_ptr_to_int = Atomic(*i32).init(undefined); // 原子指针
