//
// Auto-generated from: Zig\Zig_Package_Ecosystem.md
// Line: 547
// Language: zig
// Block ID: 627672d2
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig.zon

.{
    .name = "my-app",
    .version = "1.0.0",

    .dependencies = .{
        .private_lib = .{
            .url = "http://internal-server:8080/packages/private-lib-1.0.0.tar.gz",
            .hash = "...",
        },
    },
}
