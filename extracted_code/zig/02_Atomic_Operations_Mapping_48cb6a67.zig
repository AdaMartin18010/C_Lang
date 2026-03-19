//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 159
// Language: zig
// Block ID: 48cb6a67
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig std.atomic.Value 完整指南
const std = @import("std");
const Atomic = std.atomic.Value;

// 基本原子类型
var atomic_int = Atomic(i32).init(0);
var atomic_u64 = Atomic(u64).init(0);
var atomic_bool = Atomic(bool).init(false);
var atomic_ptr = Atomic(?*anyopaque).init(null);

// 枚举原子类型
const State = enum(i32) { idle, running, stopped };
var atomic_state = Atomic(State).init(.idle);

// 类型修饰符
var ptr_to_atomic: *Atomic(i32) = &atomic_int;
var atomic_ptr_to_int = Atomic(*i32).init(undefined);
