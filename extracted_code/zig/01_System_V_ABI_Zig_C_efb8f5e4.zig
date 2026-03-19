//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 572
// Language: zig
// Block ID: efb8f5e4
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: test_int.zig
// 编译: zig build-obj -O ReleaseFast -target x86_64-linux-gnu test_int.zig

export fn addInts(a: i32, b: i32, c: i32) i32 {
    return a + b + c;
}
