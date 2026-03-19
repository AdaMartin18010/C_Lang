//
// Auto-generated from: 08_Zig_C_Connection\03_ABI_Formalization\README.md
// Line: 41
// Language: zig
// Block ID: 4a9787b8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 如果 ABI 不匹配，这段代码将导致未定义行为
const c_lib = @cImport({
    @cInclude("legacy_c_lib.h");
});

// 假设 C 库期望 System-V AMD64 ABI
// 但编译器使用了不同的调用约定
pub export fn call_c_function(x: f64, y: f64) f64 {
    // 如果浮点参数没有正确放入 XMM 寄存器
    // C 库将读取到错误的值
    return c_lib.compute_distance(x, y);
}
