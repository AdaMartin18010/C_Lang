//
// Auto-generated from: Zig\2026_latest\README.md
// Line: 285
// Language: zig
// Block ID: a36ff7bf
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig.zon - 2026年格式
.{
    .name = "my-project",
    .version = "1.0.0",
    .minimum_zig_version = "0.14.0",

    .dependencies = .{
        // 远程依赖
        .httpz = .{
            .url = "https://github.com/karlseguin/http.zig/archive/refs/tags/v0.22.0.tar.gz",
            .hash = "1220b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a0b0a",
        },

        // 本地路径依赖
        .local_lib = .{
            .path = "../local-lib",
        },

        // Git依赖
        .git_lib = .{
            .url = "git+https://github.com/user/repo.git#v1.2.3",
            .hash = "1220...",
        },
    },

    .paths = .{
        "build.zig",
        "build.zig.zon",
        "src",
        "LICENSE",
        "README.md",
    },
}
