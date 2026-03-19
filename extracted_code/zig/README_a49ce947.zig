//
// Auto-generated from: Zig\formal_models\README.md
// Line: 172
// Language: zig
// Block ID: a49ce947
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// comptime 实际示例

// 编译时类型参数
fn makeArray(comptime T: type, size: usize) []T {
    return allocator.alloc(T, size) catch unreachable;
}

// 编译时求值
const array_size = blk: {
    var sum: usize = 0;
    var i: usize = 0;
    while (i < 10) : (i += 1) {
        sum += i;
    }
    break :blk sum;  // 编译时常量 45
};

// 类型生成
fn Vector(comptime T: type, comptime n: usize) type {
    return struct {
        data: [n]T,

        pub fn add(self: @This(), other: @This()) @This() {
            var result: @This() = undefined;
            for (&result.data, self.data, other.data) |*r, a, b| {
                r.* = a + b;
            }
            return result;
        }
    };
}

const Vec3f = Vector(f32, 3);
