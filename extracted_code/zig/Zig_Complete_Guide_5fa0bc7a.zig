//
// Auto-generated from: Zig\Zig_Complete_Guide.md
// Line: 433
// Language: zig
// Block ID: 5fa0bc7a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 整数类型
const a: i8 = -128;           // 有符号 8 位
const b: u8 = 255;            // 无符号 8 位
const c: i32 = -1000000;      // 32 位
const d: u64 = 1 << 63;       // 64 位
const e: isize = -1;          // 目标指针大小的有符号整数
const f: usize = 100;         // 目标指针大小的无符号整数

// 浮点类型
const f32_val: f32 = 3.14;
const f64_val: f64 = 3.14159265358979;

// 布尔类型
const flag: bool = true;

// 字符类型
const letter: u8 = 'A';       // 实际上是 u8
const unicode: u21 = '🔥';    // Unicode 码点

// void 类型
fn do_nothing() void {
    return;
}

// noreturn 类型 (无限循环或崩溃)
fn infinite_loop() noreturn {
    while (true) {}
}
