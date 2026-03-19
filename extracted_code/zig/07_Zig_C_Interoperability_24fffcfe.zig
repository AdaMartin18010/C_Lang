//
// Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\07_Zig_C_Interoperability.md
// Line: 794
// Language: zig
// Block ID: 24fffcfe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig.zon - 包配置
.{
    .name = "my_project",
    .version = "0.1.0",
    .dependencies = .{
        .sqlite = .{
            .url = "https://sqlite.org/.../sqlite-amalgamation-...zip",
            .hash = "sha256-...",
        },
        .zig_clap = .{
            .url = "https://github.com/.../clap/archive/...tar.gz",
            .hash = "sha256-...",
        },
    },
}

// build.zig中使用依赖
const sqlite_dep = b.dependency("sqlite", .{});
exe.addIncludePath(sqlite_dep.path("."));
