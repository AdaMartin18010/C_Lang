//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\01_System_V_ABI_Zig_C.md
// Line: 814
// Language: zig
// Block ID: 52bb0947
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 代码: 内存传递的结构体

// 大结构体
const BigStruct = extern struct {
    a: i64,
    b: i64,
    c: i64,    // 24 bytes
};
extern fn bigStruct(s: BigStruct) void;
// 注意: 通过值传递大结构体效率低

// 推荐做法: 传递指针
extern fn bigStructPtr(s: *const BigStruct) void;
// s: %rdi

// Zig 可以检查结构体大小
comptime {
    const size = @sizeOf(BigStruct);
    if (size > 16) {
        @compileError("结构体过大，考虑传递指针");
    }
}
