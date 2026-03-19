//
// Auto-generated from: Zig\Zig_WebAssembly.md
// Line: 112
// Language: zig
// Block ID: 5d249ebe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// lib.zig - 可导出的 WASM 库

// 导出函数给 JavaScript
export fn add(a: i32, b: i32) i32 {
    return a + b;
}

export fn factorial(n: u32) u32 {
    if (n <= 1) return 1;
    return n * factorial(n - 1);
}

// 内存管理
extern "env" fn malloc(size: usize) ?[*]u8;
extern "env" fn free(ptr: [*]u8) void;

export fn alloc_buffer(size: usize) ?[*]u8 {
    return malloc(size);
}

export fn free_buffer(ptr: [*]u8) void {
    free(ptr);
}
