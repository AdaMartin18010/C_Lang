//
// Auto-generated from: Zig\README.md
// Line: 1543
// Language: zig
// Block ID: 2e46fc56
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 导出函数供 JavaScript 调用
export fn add(a: i32, b: i32) i32 {
    return a + b;
}

export fn fibonacci(n: i32) i32 {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

// 内存管理
var buffer: [1024]u8 = undefined;
var buffer_used: usize = 0;

export fn alloc(size: usize) ?[*]u8 {
    if (buffer_used + size > buffer.len) return null;
    const ptr = &buffer[buffer_used];
    buffer_used += size;
    return ptr;
}

export fn memory() [*]u8 {
    return &buffer;
}
