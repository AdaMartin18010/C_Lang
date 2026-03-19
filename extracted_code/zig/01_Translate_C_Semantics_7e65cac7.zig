//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 155
// Language: zig
// Block ID: 7e65cac7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// Zig 编译器内部：translate_c.zig 核心数据结构

const Context = struct {
    // Clang AST 上下文
    clang_context: *clang.ASTContext,

    // 符号表：C 名字 → Zig 名字
    global_symbol_table: std.StringHashMap(ZigNode),
    local_symbol_table: std.StringHashMap(ZigNode),

    // 类型映射缓存
    type_map: std.HashMap(clang.QualType, ZigType),

    // 匿名类型命名计数器
    anon_record_counter: u32 = 0,

    // 宏定义表
    macro_table: std.StringHashMap(MacroDef),
};

const TransScope = struct {
    parent: ?*TransScope,
    locals: std.StringHashMap(ZigNode),
    labels: std.StringHashMap(ZigLabel),
    // ...
};
