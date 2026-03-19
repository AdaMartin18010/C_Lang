//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 516
// Language: zig
// Block ID: 3dddb18c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 自动生成版本信息

fn addVersionInfo(b: *std.Build, exe: *std.Build.Step.Compile) void {
    const version_tool = b.addExecutable(.{
        .name = "version",
        .root_source_file = .{ .path = "tools/version.zig" },
        .target = b.host,
    });

    const version_step = b.addRunArtifact(version_tool);

    // 获取 git 版本
    const git_describe = b.run(&.{ "git", "describe", "--tags", "--always" });
    version_step.addArg(git_describe);

    // 获取构建时间
    const build_time = b.run(&.{ "date", "-u", "+%Y-%m-%dT%H:%M:%SZ" });
    version_step.addArg(build_time);

    const version_file = version_step.captureStdOut();

    exe.addAnonymousModule("version_info", .{
        .source_file = version_file,
    });
}
