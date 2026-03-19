//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 414
// Language: zig
// Block ID: 9822b0b4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Comptime 语义示例

// 1. 编译时常量
const array_size = comptime calculateSize(10);  // 编译时求值

fn calculateSize(n: usize) usize {
    return n * 2;
}

// 2. 编译时类型生成
fn Vector(comptime T: type, comptime n: usize) type {
    return struct {
        data: [n]T,

        pub fn get(self: @This(), i: usize) T {
            return self.data[i];
        }
    };
}

const Vec3f = Vector(f32, 3);  // 编译时生成类型

// 3. 编译时元编程
fn makeEnum(comptime names: []const []const u8) type {
    var fields: [names.len]std.builtin.Type.EnumField = undefined;

    for (names, 0..) |name, i| {
        fields[i] = .{
            .name = name,
            .value = i,
        };
    }

    return @Type(.{
        .Enum = .{
            .tag_type = u8,
            .fields = &fields,
            .decls = &.{},
            .is_exhaustive = true,
        },
    });
}

const Colors = makeEnum(&.{ "Red", "Green", "Blue" });
// 等价于:
// const Colors = enum { Red, Green, Blue };

// 4. 编译时条件编译
const Config = struct {
    target_os: enum { linux, windows, macos } = .linux,
    debug: bool = true,
};

const config: Config = .{};

const OSFile = switch (config.target_os) {
    .linux => @import("os/linux.zig"),
    .windows => @import("os/windows.zig"),
    .macos => @import("os/macos.zig"),
};

// 5. 编译时断言和检查
comptime {
    // 编译时验证
    if (@sizeOf(usize) != 8) {
        @compileError("只支持 64 位平台");
    }
}
