//
// Auto-generated from: Zig\README.md
// Line: 1188
// Language: zig
// Block ID: 97660309
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用栈分配器避免堆分配
var buffer: [1024]u8 = undefined;
var fba = std.heap.FixedBufferAllocator.init(&buffer);
const allocator = fba.allocator();

const data = try allocator.alloc(u8, 100);
// 无需 free，随栈自动释放
