//
// Auto-generated from: Zig\README.md
// Line: 1025
// Language: zig
// Block ID: 4e253ceb
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 端
export fn zig_do_something() c_int {
    doSomething() catch |err| {
        return switch (err) {
            error.OutOfMemory => -1,
            error.InvalidInput => -2,
            else => -99,
        };
    };
    return 0; // 成功
}
