//
// Auto-generated from: Zig\README.md
// Line: 1115
// Language: zig
// Block ID: 4c87d4d6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 根据编译时参数生成不同类型
fn Vector(comptime T: type, comptime N: comptime_int) type {
    return struct {
        data: [N]T,

        pub fn add(self: @This(), other: @This()) @This() {
            var result: @This() = undefined;
            comptime var i = 0;
            inline while (i < N) : (i += 1) {
                result.data[i] = self.data[i] + other.data[i];
            }
            return result;
        }
    };
}

// 使用生成的类型
const Vec3f = Vector(f32, 3);

pub fn main() void {
    const v1 = Vec3f{ .data = .{1.0, 2.0, 3.0} };
    const v2 = Vec3f{ .data = .{4.0, 5.0, 6.0} };
    const v3 = v1.add(v2);
    // v3.data = {5.0, 7.0, 9.0}
}
