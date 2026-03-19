//
// Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\README.md
// Line: 193
// Language: zig
// Block ID: df8849a5
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// PNG 解码器的 Zig 实现
pub const PNGDecoder = struct {
    width: u32,
    height: u32,
    bit_depth: u8,
    color_type: ColorType,

    pub const ColorType = enum(u8) {
        grayscale = 0,
        rgb = 2,
        palette = 3,
        grayscale_alpha = 4,
        rgba = 6,
    };

    pub fn decode(allocator: Allocator, data: []const u8) !Image {
        // 安全解析 PNG 文件
        if (!std.mem.eql(u8, data[0..8], &PNG_SIGNATURE)) {
            return error.InvalidSignature;
        }

        // 逐块解析，边界检查
        var offset: usize = 8;
        while (offset < data.len) {
            const chunk_len = std.mem.readInt(u32, data[offset..][0..4], .big);
            if (offset + 12 + chunk_len > data.len) {
                return error.TruncatedChunk;
            }
            // 处理块...
            offset += 12 + chunk_len;
        }
    }
};
