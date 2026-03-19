//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 374
// Language: zig
// Block ID: f58ba657
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig.zon - 依赖清单

.{
    .name = "myproject",
    .version = "1.0.0",

    .dependencies = .{
        // Git 依赖
        .zap = .{
            .url = "https://github.com/zigzap/zap/archive/refs/tags/v0.5.0.tar.gz",
            .hash = "12200d135ec55385defa4df6a7087e7b4fd2657d8d6b6a32e0ec8077a0a3a8e7c8f3",
        },

        // 本地路径依赖
        .local_lib = .{
            .path = "../local-lib",
        },

        // HTTP 依赖
        .http_lib = .{
            .url = "https://example.com/lib.tar.gz",
            .hash = "sha256-abc123...",
        },
    },

    .paths = .{
        "build.zig",
        "build.zig.zon",
        "src",
    },
}
