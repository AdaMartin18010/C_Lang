//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\README.md
// Line: 59
// Language: zig
// Block ID: dc98908b
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig代码
export fn add(a: i32, b: i32) i32 {
    return a + b;
}

export fn fibonacci(n: u32) u32 {
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}
