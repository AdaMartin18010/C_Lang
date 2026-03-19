//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\02_CImport_Correctness.md
// Line: 98
// Language: zig
// Block ID: 416ac2da
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// @cImport 中的预处理指令

const c = @cImport({
    // 定义宏 (相当于 -D)
    @cDefine("DEBUG", "1");
    @cDefine("VERSION", "\"1.0.0\"");

    // 添加包含路径 (相当于 -I)
    @cInclude("/usr/local/include");

    // 包含头文件
    @cInclude("myheader.h");
});
