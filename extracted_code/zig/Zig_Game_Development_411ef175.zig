//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 388
// Language: zig
// Block ID: 411ef175
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

pub const Texture = struct {
    id: c_uint,
    width: i32,
    height: i32,

    pub fn loadFromFile(path: []const u8) !Texture {
        // 使用 stb_image 加载
        var width: c_int = 0;
        var height: c_int = 0;
        var channels: c_int = 0;

        const data = c.stbi_load(path.ptr, &width, &height, &channels, 4);
        defer c.stbi_image_free(data);

        if (data == null) return error.ImageLoadFailed;

        var id: c_uint = 0;
        c.glGenTextures(1, &id);
        c.glBindTexture(c.GL_TEXTURE_2D, id);

        // 设置纹理参数
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_WRAP_S, c.GL_REPEAT);
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_WRAP_T, c.GL_REPEAT);
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_MIN_FILTER, c.GL_LINEAR_MIPMAP_LINEAR);
        c.glTexParameteri(c.GL_TEXTURE_2D, c.GL_TEXTURE_MAG_FILTER, c.GL_LINEAR);

        c.glTexImage2D(c.GL_TEXTURE_2D, 0, c.GL_RGBA, width, height, 0, c.GL_RGBA, c.GL_UNSIGNED_BYTE, data);
        c.glGenerateMipmap(c.GL_TEXTURE_2D);

        return .{
            .id = id,
            .width = width,
            .height = height,
        };
    }

    pub fn bind(self: Texture, slot: c_uint) void {
        c.glActiveTexture(c.GL_TEXTURE0 + slot);
        c.glBindTexture(c.GL_TEXTURE_2D, self.id);
    }

    pub fn getRegion(self: Texture, x: i32, y: i32, w: i32, h: i32) TextureRegion {
        const fw = @as(f32, @floatFromInt(self.width));
        const fh = @as(f32, @floatFromInt(self.height));

        return .{
            .u = @as(f32, @floatFromInt(x)) / fw,
            .v = @as(f32, @floatFromInt(y)) / fh,
            .w = @as(f32, @floatFromInt(w)) / fw,
            .h = @as(f32, @floatFromInt(h)) / fh,
        };
    }
};
