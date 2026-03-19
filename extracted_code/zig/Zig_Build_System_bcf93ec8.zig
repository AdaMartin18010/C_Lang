//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 467
// Language: zig
// Block ID: bcf93ec8
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成代码的构建步骤

fn addGeneratedCode(b: *std.Build, exe: *std.Build.Step.Compile) void {
    // 运行代码生成器
    const generate = b.addRunArtifact(b.addExecutable(.{
        .name = "codegen",
        .root_source_file = .{ .path = "tools/codegen.zig" },
        .target = b.host,
    }));

    // 添加输入文件
    generate.addFileArg(.{ .path = "schema/api.json" });

    // 捕获输出
    const generated_file = generate.captureStdOut();

    // 将生成的文件添加到可执行文件
    exe.addAnonymousModule("generated", .{
        .source_file = generated_file,
    });
}
