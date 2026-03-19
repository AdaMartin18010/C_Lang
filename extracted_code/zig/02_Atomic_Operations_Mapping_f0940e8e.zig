//
// Auto-generated from: 08_Zig_C_Connection\02_Memory_Model_Bridge\02_Atomic_Operations_Mapping.md
// Line: 983
// Language: zig
// Block ID: f0940e8e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 缓存行填充
const CACHE_LINE_SIZE = 64;

const PaddedCounter = struct {
    count: Atomic(u64),
    padding: [CACHE_LINE_SIZE - @sizeOf(Atomic(u64))]u8,
};
