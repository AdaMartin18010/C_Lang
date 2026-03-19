//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 487
// Language: zig
// Block ID: 46a5b2d7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 类型级编程

// 1. 类型特征 (Type Traits)
fn isInteger(comptime T: type) bool {
    return switch (@typeInfo(T)) {
        .Int => true,
        else => false,
    };
}

fn isPointer(comptime T: type) bool {
    return switch (@typeInfo(T)) {
        .Pointer => true,
        else => false,
    };
}

// 2. 类型变换
fn RemovePointer(comptime T: type) type {
    return switch (@typeInfo(T)) {
        .Pointer => |p| p.child,
        else => @compileError("T 必须是指针类型"),
    };
}

const T1 = RemovePointer(*i32);  // i32
const T2 = RemovePointer([]const u8);  // u8

// 3. 递归类型计算
fn Factorial(comptime n: usize) usize {
    if (n == 0) return 1;
    return n * Factorial(n - 1);
}

const fact5 = Factorial(5);  // 120 (编译时计算)

// 4. 编译时类型集合操作
fn TypeSet(comptime types: []const type) type {
    return struct {
        pub const count = types.len;

        pub fn contains(comptime T: type) bool {
            inline for (types) |t| {
                if (T == t) return true;
            }
            return false;
        }
    };
}

const NumericTypes = TypeSet(&.{ i32, i64, f32, f64 });
comptime assert(NumericTypes.contains(i32));
comptime assert(!NumericTypes.contains([]u8));

fn assert(comptime cond: bool) void {
    if (!cond) @compileError("断言失败");
}
