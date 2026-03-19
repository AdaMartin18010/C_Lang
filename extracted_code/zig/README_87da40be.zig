//
// Auto-generated from: Zig\formal_models\README.md
// Line: 244
// Language: zig
// Block ID: 87da40be
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生命周期注解的形式化概念
// (Zig隐式推导，无需显式标注)

fn example() void {
    var x: i32 = 5;           // x 的生命周期开始
    {                           // 内部作用域
        var y: i32 = 10;        // y 的生命周期开始
        const ref = &y;         // ref 借用 y
        _ = ref.*;              // 有效使用
    }                           // y 的生命周期结束，ref 变为无效
    // _ = ref.*;              // 编译错误：ref 已失效
}

// 悬垂指针防护
fn dangling_pointer_demo() void {
    var ptr: *i32 = undefined;
    {
        var x: i32 = 42;
        ptr = &x;               // 编译错误：x 生命周期不足
    }
    _ = ptr.*;
}
