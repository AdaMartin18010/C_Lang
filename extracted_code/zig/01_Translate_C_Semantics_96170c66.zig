//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 186
// Language: zig
// Block ID: 96170c66
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 遍历 C 翻译单元的顶层声明
fn transTranslationUnitDecl(c: *Context, tu: *const clang.TranslationUnitDecl) !ZigNode {
    var root = ZigNode.init(.root);

    // 遍历所有顶层声明
    for (tu.decls()) |decl| {
        const zig_decl = try transDecl(c, decl);
        try root.appendChild(zig_decl);
    }

    return root;
}
