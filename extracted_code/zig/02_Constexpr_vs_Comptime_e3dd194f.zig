//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 3008
// Language: zig
// Block ID: e3dd194f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 运行时性能特征

// comptime 确保编译时计算
const TABLE_SIZE = comptime blk: {
    var size: usize = 1;
    var i: usize = 0;
    while (i < 10) : (i += 1) {
        size *= 2;
    }
    break :blk size;
};  // 1024，编译时已知

var table: [TABLE_SIZE]i32 = undefined;

// comptime 类型构造产生单态化代码
fn Vector(comptime n: usize, comptime T: type) type {
    return struct {
        data: [n]T,
        // 为特定 n 和 T 生成专门代码
    };
}

// Vec3f 是完全特化的类型，可能有 SIMD 优化
const Vec3f = Vector(3, f32);

// 优化特性：
// - 编译时已知大小允许更好的内存布局
// - 类型特化允许内联和向量化
// - 编译时分支消除产生无分支代码
