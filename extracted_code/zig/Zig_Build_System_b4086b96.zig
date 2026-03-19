//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 493
// Language: zig
// Block ID: b4086b96
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 将静态资源编译进二进制

fn embedResources(b: *std.Build, exe: *std.Build.Step.Compile) void {
    const embed_tool = b.addExecutable(.{
        .name = "embed",
        .root_source_file = .{ .path = "tools/embed.zig" },
        .target = b.host,
    });

    const embed_step = b.addRunArtifact(embed_tool);
    embed_step.addDirectoryArg(.{ .path = "assets" });

    const embedded = embed_step.captureStdOut();

    exe.addAnonymousModule("assets", .{
        .source_file = embedded,
    });
}
