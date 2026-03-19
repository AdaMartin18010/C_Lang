//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\02_CImport_Correctness.md
// Line: 370
// Language: zig
// Block ID: 96f193e0
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 最佳实践 1: 使用命名空间组织
const stdlib = @cImport({
    @cInclude("stdlib.h");
});

const stdio = @cImport({
    @cInclude("stdio.h");
});

// 使用
_ = stdlib.malloc(1024);
_ = stdio.printf("hello\n");

// 最佳实践 2: 类型安全检查
const MyStruct = @cImport({
    @cInclude("mystruct.h");
}).MyStruct;

comptime {
    // 验证类型大小
    const expected_size = 16; // C 中的大小
    if (@sizeOf(MyStruct) != expected_size) {
        @compileError("结构体大小不匹配");
    }
}

// 最佳实践 3: 包装不安全函数
fn safeMalloc(size: usize) ![]u8 {
    const ptr = stdlib.malloc(size) orelse return error.OutOfMemory;
    return @as([*]u8, @ptrCast(ptr))[0..size];
}

fn safeFree(ptr: []u8) void {
    stdlib.free(ptr.ptr);
}

// 最佳实践 4: 条件编译
const c = @cImport({
    if (@import("builtin").os.tag == .windows) {
        @cDefine("WINDOWS", "1");
    } else {
        @cDefine("UNIX", "1");
    }
    @cInclude("platform.h");
});
