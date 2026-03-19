//
// Auto-generated from: 07_Modern_Toolchain\13_Zig_C_Interop\01_Zig_C_Interoperability.md
// Line: 184
// Language: zig
// Block ID: 3bd7ab06
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 处理条件编译和宏
const c = @cImport({
    // 定义宏
    @cDefine("SDL_MAIN_HANDLED", "1");
    @cDefine("GL_GLEXT_PROTOTYPES", "1");

    // 包含路径
    @cInclude("GL/gl.h");
    @cInclude("GL/glext.h");
    @cInclude("SDL2/SDL.h");
});

// 处理系统特定头文件
const system_c = if (@import("builtin").os.tag == .windows)
    @cImport({
        @cInclude("windows.h");
        @cInclude("winsock2.h");
    })
else
    @cImport({
        @cInclude("unistd.h");
        @cInclude("sys/socket.h");
        @cInclude("netinet/in.h");
    });
