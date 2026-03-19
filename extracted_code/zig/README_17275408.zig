//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\README.md
// Line: 282
// Language: zig
// Block ID: 17275408
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub export fn run_with_valgrind() void {
    // 在调试模式下使用 page_allocator 使 Valgrind 更容易检测问题
    const allocator = if (@import("builtin").mode == .Debug)
        std.heap.page_allocator
    else
        std.heap.c_allocator;

    const data = allocator.alloc(u8, 1024) catch return;
    // 处理数据...
    allocator.free(data);
}
