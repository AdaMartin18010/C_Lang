//
// Auto-generated from: Zig\Zig_Build_System.md
// Line: 616
// Language: zig
// Block ID: 59b4399c
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 使用 Docker 进行交叉编译

fn addDockerCrossCompile(b: *std.Build) void {
    // Linux ARM 交叉编译
    const docker_build = b.addSystemCommand(&.{
        "docker", "run", "--rm",
        "-v", b.fmt("{s}:/workspace", .{b.build_root.path.?}),
        "-w", "/workspace",
        "zig-cross-arm64",
        "zig", "build", "-Dtarget=aarch64-linux-gnu",
    });

    const docker_step = b.step("docker-build", "Build using Docker");
    docker_step.dependOn(&docker_build.step);
}
