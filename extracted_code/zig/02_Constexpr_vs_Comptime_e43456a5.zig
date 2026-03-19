//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 175
// Language: zig
// Block ID: e43456a5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: const 与 comptime 常量
const std = @import("std");

// 基本常量（自动 comptime）
const MAX_SIZE = 1024;
const PI = 3.14159265358979323846;

// 显式 comptime 块（复杂计算）
const BUFFER_SIZE = comptime blk: {
    const base = MAX_SIZE;
    const multiplier = 2;
    break :blk base * multiplier;
};

// 位运算
const MASK = comptime (1 << 32) - 1;

// 数组类型
var buffer: [BUFFER_SIZE]u8 = undefined;

// 标志常量
const FLAGS_READ = 0x01;
const FLAGS_WRITE = 0x02;
const FLAGS_EXEC = 0x04;
