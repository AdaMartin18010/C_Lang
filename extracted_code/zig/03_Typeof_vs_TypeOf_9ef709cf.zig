//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\03_Typeof_vs_TypeOf.md
// Line: 893
// Language: zig
// Block ID: 9ef709cf
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 函数类型推导

fn add(a: i32, b: i32) i32 {
    return a + b;
}

fn compute(x: i32, y: f64) f64 {
    return @as(f64, @floatFromInt(x)) + y;
}

fn zigFunctionTypeof() void {
    // 函数类型推导
    const AddType = @TypeOf(add);        // fn(i32, i32) i32
    const ComputeType = @TypeOf(compute); // fn(i32, f64) f64

    // 函数指针
    var add_ptr: *const fn (i32, i32) i32 = &add;
    const AddPtrType = @TypeOf(add_ptr); // *const fn(i32, i32) i32

    // 函数调用结果
    const ResultType = @TypeOf(add(1, 2));  // i32

    _ = .{ AddType, ComputeType, AddPtrType, ResultType };
}

// 函数类型构造
fn createFnType() type {
    return fn (i32, i32) i32;
}

fn zigFnTypeConstruction() void {
    const MyFnType = createFnType();
    var fn_ptr: *const MyFnType = &add;
    _ = fn_ptr;
}
