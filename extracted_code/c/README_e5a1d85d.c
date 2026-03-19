/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 500
 * Language: c
 * Block ID: e5a1d85d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// io_stream.h - 组件接口
typedef struct {
    int (*read)(void* self, char* buf, int size);
    int (*write)(void* self, const char* buf, int size);
    void (*close)(void* self);
} IOStream;

// 具体组件: 文件流
typedef struct {
    IOStream interface;
    FILE* file;
} FileStream;

// 装饰器基类
typedef struct {
    IOStream interface;
    IOStream* wrapped;
} StreamDecorator;

// 具体装饰器: 缓冲装饰器
typedef struct {
    StreamDecorator base;
    char buffer[4096];
    int buf_pos;
    int buf_size;
} BufferedStream;

static int buffered_read(void* self, char* buf, int size) {
    BufferedStream* bs = self;
    // 实现缓冲逻辑
    if (bs->buf_pos >= bs->buf_size) {
        // 缓冲区空，从底层读取
        bs->buf_size = bs->base.wrapped->read(bs->base.wrapped,
                                               bs->buffer,
                                               sizeof(bs->buffer));
        bs->buf_pos = 0;
        if (bs->buf_size <= 0) return bs->buf_size;
    }

    int to_copy = size < (bs->buf_size - bs->buf_pos) ?
                  size : (bs->buf_size - bs->buf_pos);
    memcpy(buf, bs->buffer + bs->buf_pos, to_copy);
    bs->buf_pos += to_copy;
    return to_copy;
}

// 具体装饰器: 压缩装饰器
typedef struct {
    StreamDecorator base;
    z_stream zstream;  // zlib
} CompressedStream;

static int compressed_write(void* self, const char* buf, int size) {
    CompressedStream* cs = self;
    // 压缩后写入底层流
    // ... zlib 压缩逻辑
    return cs->base.wrapped->write(cs->base.wrapped, buf, size);
}

// 组合使用示例
void decorator_example(void) {
    // 文件 -> 压缩 -> 缓冲
    FileStream* file = file_stream_open("data.bin");
    CompressedStream* compressed = compressed_stream_create((IOStream*)file);
    BufferedStream* buffered = buffered_stream_create((IOStream*)compressed);

    // 写入经过缓冲->压缩->文件的完整链
    char data[] = "Hello, World!";
    buffered->base.interface.write(buffered, data, sizeof(data));

    // 关闭时反向关闭
    buffered->base.interface.close(buffered);
}
