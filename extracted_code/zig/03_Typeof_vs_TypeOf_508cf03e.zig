//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 435
// Language: zig
// Block ID: 508cf03e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 编译时类型推导深度分析

// ========== 编译时类型计算 ==========

fn comptimeTypeComputation() void {
    // 编译时确定类型
    comptime {
        const t1 = @TypeOf(42);           // i32
        const t2 = @TypeOf(42.0);         // comptime_float
        const t3 = @TypeOf(true);         // bool
        const t4 = @TypeOf("hello");      // *const [5:0]u8

        // 类型比较
        std.debug.assert(t1 == i32);
        std.debug.assert(t3 == bool);
    }
}

// ========== 类型构造基础 ==========

fn typeConstruction() void {
    const BaseType = i32;

    // 构造指针类型
    const PtrType = *BaseType;           // *i32
    const ConstPtrType = *const BaseType; // *const i32

    // 构造数组类型
    const ArrayType = [10]BaseType;      // [10]i32
    const SliceType = []BaseType;        // []i32

    // 构造可选类型
    const OptionalType = ?BaseType;      // ?i32
    const OptionalPtr = ?*BaseType;      // ?*i32

    // 使用构造的类型
    var p: PtrType = undefined;
    var arr: ArrayType = undefined;
    var opt: OptionalType = null;

    _ = .{ p, arr, opt };
}

// ========== 类型推导与泛型 ==========

fn genericWithTypeof(comptime T: type, value: T) @TypeOf(value) {
    return value;
}

fn demoGeneric() void {
    const x = genericWithTypeof(i32, 42);
    const y = genericWithTypeof(f64, 3.14);

    std.debug.assert(@TypeOf(x) == i32);
    std.debug.assert(@TypeOf(y) == f64);
}
