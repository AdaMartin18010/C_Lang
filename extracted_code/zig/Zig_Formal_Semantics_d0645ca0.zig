//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 252
// Language: zig
// Block ID: d0645ca0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 内存区域语义

// 栈分配 - 自动生命周期
fn stackExample() void {
    const x: i32 = 10;  // 栈分配，函数返回时自动释放
    var arr: [100]u8 = undefined;  // 栈数组
}

// 堆分配 - 显式管理
fn heapExample(allocator: std.mem.Allocator) !void {
    const ptr = try allocator.alloc(u8, 100);  // 堆分配
    defer allocator.free(ptr);  // 显式释放
}

// 全局/静态存储
const global_const: i32 = 42;  // 数据段 (.rodata)
var global_var: i32 = 0;       // 数据段 (.data)
var global_bss: i32 = undefined; // BSS 段

// 内存布局控制
const MyStruct = extern struct {
    a: u8,    // 偏移 0
    // 填充 3 字节 (对齐到 4)
    b: u32,   // 偏移 4
    c: u16,   // 偏移 8
    // 填充 2 字节 (对齐到 4)
}; // 总大小: 12 字节
