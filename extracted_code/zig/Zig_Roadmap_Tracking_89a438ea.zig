//
// Auto-generated from: 00_VERSION_TRACKING\Zig_Roadmap_Tracking.md
// Line: 191
// Language: zig
// Block ID: 89a438ea
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 旧代码 (0.14及之前):
var writer = file.writer();
writer.print("Hello\n");  // 可能隐式缓冲

// 新代码 (0.15.1+):
var buf_writer = std.io.bufferedWriter(file.writer());
buf_writer.writer().print("Hello\n");
try buf_writer.flush();  // 必须显式刷新
