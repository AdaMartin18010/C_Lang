//
// Auto-generated from: 08_Zig_C_Connection\04_Translation_Validation\README.md
// Line: 173
// Language: zig
// Block ID: 16f7feab
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

/// C 代码:
/// int read_config(const char* path, Config* out_cfg) {
///     FILE* f = fopen(path, "r");
///     if (!f) return -1;
///     if (fread(out_cfg, sizeof(Config), 1, f) != 1) {
///         fclose(f);
///         return -2;
///     }
///     fclose(f);
///     return 0;
/// }

// Zig 翻译 - 使用错误联合类型
pub const ConfigError = error{
    FileNotFound,
    ReadError,
    InvalidFormat,
};

pub const Config = extern struct {
    version: u32,
    timeout_ms: u32,
    // ...
};

pub fn readConfig(path: []const u8, allocator: std.mem.Allocator) ConfigError!Config {
    const file = std.fs.cwd().openFile(path, .{})
        catch return ConfigError.FileNotFound;
    defer file.close();

    var config: Config = undefined;
    const bytes_read = file.read(std.mem.asBytes(&config))
        catch return ConfigError.ReadError;

    if (bytes_read != @sizeOf(Config)) {
        return ConfigError.InvalidFormat;
    }

    // 字节序检查
    if (config.version != std.mem.nativeToBig(u32, config.version)) {
        config.version = std.mem.byteSwap(config.version);
        config.timeout_ms = std.mem.byteSwap(config.timeout_ms);
    }

    return config;
}
