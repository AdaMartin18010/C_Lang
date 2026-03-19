//
// Auto-generated from: Zig\Zig_Formal_Semantics.md
// Line: 666
// Language: zig
// Block ID: d4923053
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 错误流分析

// 显式错误传播
fn explicitPropagation() !void {
    const result = mayFail();  // 可能返回错误

    // 方法 1: try - 传播错误
    const v1 = try mayFail();
    _ = v1;

    // 方法 2: catch - 处理错误
    const v2 = mayFail() catch 0;
    _ = v2;

    // 方法 3: if-else - 精细控制
    if (mayFail()) |value| {
        _ = value;
    } else |err| {
        _ = err;
    }

    // 方法 4: 忽略错误 (危险!)
    const v4 = mayFail() catch unreachable;
    _ = v4;
}

fn mayFail() error{Foo}!i32 {
    return 42;
}

// 错误合并规则
fn errorMerging() !void {
    // 多个可能错误的表达式合并为联合错误集
    const x = foo();  // error{A}!i32
    const y = bar();  // error{B}!i32

    _ = x;
    _ = y;

    // x 和 y 同时使用时，错误集合并为 error{A, B}
}

fn foo() error{A}!i32 { return 1; }
fn bar() error{B}!i32 { return 2; }
