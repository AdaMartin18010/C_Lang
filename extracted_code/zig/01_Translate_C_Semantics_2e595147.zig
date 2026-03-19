//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\01_Translate_C_Semantics.md
// Line: 350
// Language: zig
// Block ID: 2e595147
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 生成的 Zig 代码
pub inline fn container_of(
    ptr: anytype,
    comptime T: type,
    comptime member: []const u8,
) *T {
    const member_ptr = ptr;
    const member_offset = @offsetOf(T, member);
    return @ptrFromInt(@intFromPtr(member_ptr) - member_offset);
}
