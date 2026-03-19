//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 382
// Language: zig
// Block ID: 59dbed9a
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码 - 条件已求值
// (根据目标平台不同)
pub const EXTERN_C = {};  // 空结构或适当值

pub const EXPORT = if (@import("builtin").target.os.tag == .windows)
    @compileError("TODO: Windows dllexport")
else
    @attribute(.visibility, "default");
