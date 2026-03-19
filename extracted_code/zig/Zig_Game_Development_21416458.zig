//
// Auto-generated from: Zig\Zig_Game_Development.md
// Line: 733
// Language: zig
// Block ID: 21416458
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

const c = @cImport({
    @cInclude("AL/al.h");
    @cInclude("AL/alc.h");
});

pub const AudioSystem = struct {
    device: *c.ALCdevice,
    context: *c.ALCcontext,

    pub fn init() !AudioSystem {
        const device = c.alcOpenDevice(null) orelse return error.OpenDeviceFailed;
        const context = c.alcCreateContext(device, null) orelse return error.CreateContextFailed;

        c.alcMakeContextCurrent(context);

        return .{
            .device = device,
            .context = context,
        };
    }

    pub fn deinit(self: AudioSystem) void {
        c.alcMakeContextCurrent(null);
        c.alcDestroyContext(self.context);
        c.alcCloseDevice(self.device);
    }
};

pub const Sound = struct {
    buffer: c_uint,
    source: c_uint,

    pub fn loadFromFile(path: []const u8) !Sound {
        // 加载 WAV/OGG 文件
        // 简化示例
        var buffer: c_uint = 0;
        c.alGenBuffers(1, &buffer);

        // 解码音频数据并填充缓冲
        // ...

        var source: c_uint = 0;
        c.alGenSources(1, &source);
        c.alSourcei(source, c.AL_BUFFER, @intCast(buffer));

        return .{ .buffer = buffer, .source = source };
    }

    pub fn play(self: Sound) void {
        c.alSourcePlay(self.source);
    }

    pub fn stop(self: Sound) void {
        c.alSourceStop(self.source);
    }

    pub fn setVolume(self: Sound, volume: f32) void {
        c.alSourcef(self.source, c.AL_GAIN, volume);
    }

    pub fn setPosition(self: Sound, x: f32, y: f32, z: f32) void {
        c.alSource3f(self.source, c.AL_POSITION, x, y, z);
    }

    pub fn deinit(self: Sound) void {
        c.alDeleteSources(1, &self.source);
        c.alDeleteBuffers(1, &self.buffer);
    }
};
