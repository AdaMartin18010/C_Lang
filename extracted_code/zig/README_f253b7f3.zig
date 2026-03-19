//
// Auto-generated from: 08_Zig_C_Connection\06_C23_Zig_Parallel\README.md
// Line: 439
// Language: zig
// Block ID: f253b7f3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// build.zig (Zig)
const std = @import("std");

pub fn build(b: *std.build.Builder) void {
    const target = b.standardTargetOptions(.{});
    const mode = b.standardReleaseOptions();

    const exe = b.addExecutable("myapp", "src/main.zig");
    exe.setTarget(target);
    exe.setBuildMode(mode);
    exe.install();

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    // 链接C库
    exe.linkLibC();
    exe.addIncludePath("include");
    exe.addCSourceFile("src/legacy.c", &.{"-std=c23"});
}
