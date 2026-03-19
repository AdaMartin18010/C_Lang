//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 372
// Language: zig
// Block ID: 8e8fc629
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig @TypeOf 基本语法
const std = @import("std");

// ========== 基本用法 ==========

fn basicUsage() void {
    var x: i32 = 42;

    // 从表达式推导类型
    var y: @TypeOf(x) = 100;           // y 是 i32
    var pi: @TypeOf(3.14) = 3.14159;   // pi 是 comptime_float (需要具体化)

    // 具体化类型
    const T = @TypeOf(x);
    var z: T = 200;
    _ = z;
}

// ========== 复杂表达式 ==========

fn complexExpressions() void {
    var arr: [10]i32 = undefined;
    var ptr: *i32 = &arr[0];

    // 数组推导
    const ArrType = @TypeOf(arr);      // [10]i32
    var arr2: ArrType = undefined;

    const ElemType = @TypeOf(arr[0]);  // i32
    var elem: ElemType = 0;

    // 指针推导
    const PtrType = @TypeOf(ptr);      // *i32
    var ptr2: PtrType = &elem;

    const DerefType = @TypeOf(ptr.*);  // i32
    var val: DerefType = 42;

    const AddrType = @TypeOf(&val);    // *i32
    var ptr3: AddrType = &val;

    _ = .{ arr2, elem, ptr2, val, ptr3 };
}

// ========== 编译时常量 ==========

fn comptimeConstants() void {
    // @TypeOf 在编译时求值
    comptime {
        const T = @TypeOf(42);
        std.debug.assert(T == i32);
    }

    // 用于类型声明
    const IntType = @TypeOf(42);
    var x: IntType = 100;
    _ = x;
}
