//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
// Line: 317
// Language: zig
// Block ID: be4d6a2b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig: 错误联合类型
const Error = error{ DivisionByZero };

fn divideChecked(a: i32, b: i32) Error!i32 {
    if (b == 0) return error.DivisionByZero;
    return @divTrunc(a, b);
}

// 使用 - 强制错误处理
const result = divideChecked(10, 0) catch |err| {
    std.debug.print("Error: {}\n", .{err});
    return;
};
