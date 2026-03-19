//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 140
// Language: zig
// Block ID: 26f136f8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// wasm_allocator.zig

const std = @import("std");

// 使用 WebAssembly 内存作为堆
var gpa = std.heap.GeneralPurposeAllocator(.{}){};
const allocator = gpa.allocator();

// 导出的内存
export var memory: [1024 * 1024]u8 align(16) = undefined;  // 1MB 初始内存

// 使用固定缓冲区分配器
var fba = std.heap.FixedBufferAllocator.init(&memory);
export const wasm_allocator = fba.allocator();

// 导出分配函数
export fn wasm_alloc(size: usize) ?[*]u8 {
    const ptr = wasm_allocator.alloc(u8, size) catch return null;
    return ptr.ptr;
}

export fn wasm_free(ptr: [*]u8, size: usize) void {
    wasm_allocator.free(ptr[0..size]);
}
