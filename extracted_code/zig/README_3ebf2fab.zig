//
// Auto-generated from: Zig\README.md
// Line: 1259
// Language: zig
// Block ID: 3ebf2fab
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

fn allocateResources() !void {
    const resource1 = try allocate1();
    errdefer free1(resource1);  // 错误时自动执行

    const resource2 = try allocate2();
    errdefer free2(resource2);  // 错误时自动执行

    // 如果执行到这里，不会触发 errdefer
    useResources(resource1, resource2);
}
