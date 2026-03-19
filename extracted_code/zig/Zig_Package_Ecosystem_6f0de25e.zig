//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 112
// Language: zig
// Block ID: 6f0de25e
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig.zon - 包清单文件

.{
    // 包名 (必需)
    .name = "my-awesome-lib",

    // 版本 (语义化版本)
    .version = "1.2.3",

    // 依赖列表
    .dependencies = .{
        // GitHub 依赖
        .zap = .{
            .url = "https://github.com/zigzap/zap/archive/refs/tags/v0.5.0.tar.gz",
            .hash = "12200d135ec55385defa4df6a7087e7b4fd2657d8d6b6a32e0ec8077a0a3a8e7c8f3",
        },

        // 本地路径依赖
        .local_utils = .{
            .path = "../utils",
        },

        // HTTP URL 依赖
        .http_lib = .{
            .url = "https://example.com/lib.tar.gz",
            .hash = "sha256-abc123...",
        },

        // Git 仓库依赖
        .git_lib = .{
            .url = "https://github.com/user/repo/archive/main.tar.gz",
            .hash = "...",
        },
    },

    // 包含在包中的文件路径
    .paths = .{
        "build.zig",
        "build.zig.zon",
        "src",
        "LICENSE",
        "README.md",
    },
}
