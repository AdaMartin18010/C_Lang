//
// Auto-generated from: Zig\2026_latest\Zig_2026_Roadmap.md
// Line: 118
// Language: zig
// Block ID: 7411c8c6
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 2026 新特性: 编译器插件
// 允许自定义编译时分析和代码生成

const Plugin = @import("std").compiler.Plugin;

// 自定义 lint 规则
pub const MyLinter = Plugin.create(.{
    .name = "my_linter",
    .analyzeDecl = analyzeDecl,
});

fn analyzeDecl(decl: Plugin.Decl) void {
    // 检查命名规范
    if (!std.ascii.isUpper(decl.name[0])) {
        Plugin.emitWarning(.{
            .message = "类型名应该大写开头",
            .location = decl.location,
        });
    }
}
