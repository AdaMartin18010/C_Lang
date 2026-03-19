//
// Auto-generated from: Zig\README.md
// Line: 1200
// Language: zig
// Block ID: 6f94ad10
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 跟踪分配的分配器
var tracking_allocator = std.heap.loggingAllocator(
    std.heap.page_allocator
);
const allocator = tracking_allocator.allocator();

// 所有分配和释放都会被记录
