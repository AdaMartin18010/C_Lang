//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\02_CImport_Correctness.md
// Line: 71
// Language: zig
// Block ID: 1aa320c7
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// @cImport 是 translate-c 的高层封装

// 方式 1: 使用 @cImport (推荐)
const c = @cImport({
    @cDefine("NDEBUG", "1");
    @cInclude("stdio.h");
    @cInclude("stdlib.h");
});

// 方式 2: 使用 translate-c 命令行
// $ zig translate-c header.h > header.zig
// 然后在 Zig 中 @import("header.zig")

// 方式 3: 使用 Zig 构建系统
// build.zig 中:
// const lib = b.addTranslateC(.{
//     .root_source_file = b.path("header.h"),
// });
