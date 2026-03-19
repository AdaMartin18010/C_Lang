//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\02_Constexpr_vs_Comptime.md
// Line: 432
// Language: zig
// Block ID: 47bbd9db
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: comptime 与类型系统深度集成

// 类型作为参数
fn makeArray(comptime T: type, comptime size: usize) type {
    return struct {
        data: [size]T,
        len: usize = size,

        const Self = @This();

        fn get(self: Self, idx: usize) T {
            return self.data[idx];
        }

        fn set(self: *Self, idx: usize, value: T) void {
            self.data[idx] = value;
        }
    };
}

// 类型构造
const IntArray100 = makeArray(i32, 100);
const FloatArray50 = makeArray(f32, 50);

// 类型内省
fn hasMethod(comptime T: type, name: []const u8) bool {
    const info = @typeInfo(T);
    if (info != .Struct) return false;
    inline for (info.Struct.decls) |decl| {
        if (std.mem.eql(u8, decl.name, name)) return true;
    }
    return false;
}

// 类型约束泛型
fn sort(comptime T: type, arr: []T) void {
    comptime {
        // 编译时验证类型可比较
        const info = @typeInfo(T);
        if (info != .Int and info != .Float) {
            @compileError("Type must be numeric for sorting");
        }
    }
    // 排序实现...
}

// 类型级编程
fn Largest(comptime A: type, comptime B: type) type {
    return if (@sizeOf(A) >= @sizeOf(B)) A else B;
}

const Biggest = Largest(u32, u64);  // u64
