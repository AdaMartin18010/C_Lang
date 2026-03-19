//
// Auto-generated from: Zig\Zig_Performance_Optimization.md
// Line: 376
// Language: zig
// Block ID: 385384ec
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 对齐优化

// 自然对齐
const AlignedStruct = extern struct {
    a: u8,      // offset 0
    // padding 7 bytes
    b: u64,     // offset 8
    c: u32,     // offset 16
    d: u8,      // offset 20
    // padding 3 bytes
    // total: 24 bytes
};

// 重新排序减少填充
const PackedStruct = extern struct {
    b: u64,     // offset 0
    c: u32,     // offset 8
    a: u8,      // offset 12
    d: u8,      // offset 13
    // padding 2 bytes
    // total: 16 bytes
};

// 显式对齐
const CacheAligned = extern struct {
    data: [64]u8 align(64),  // 64 字节对齐 (缓存行)
};

// SIMD 对齐
const SimdAligned = extern struct {
    vec: @Vector(8, f32) align(32),  // 32 字节对齐 (AVX)
};
