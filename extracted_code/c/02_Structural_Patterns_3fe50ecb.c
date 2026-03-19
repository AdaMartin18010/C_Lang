/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 107
 * Language: c
 * Block ID: 3fe50ecb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 组件接口 */
typedef struct {
    void (*write)(void*, const char*, size_t);
    void (*flush)(void*);
} writer_t;

/* 基础实现: 文件写入 */
typedef struct {
    writer_t interface;
    FILE* file;
} file_writer_t;

void file_write(void* self, const char* data, size_t len) {
    file_writer_t* fw = self;
    fwrite(data, 1, len, fw->file);
}

/* 装饰器: 缓冲 */
typedef struct {
    writer_t interface;
    writer_t* wrapped;
    char buffer[4096];
    size_t buffered;
} buffered_writer_t;

void buffered_write(void* self, const char* data, size_t len) {
    buffered_writer_t* bw = self;

    /* 尝试放入缓冲区 */
    if (bw->buffered + len < sizeof(bw->buffer)) {
        memcpy(bw->buffer + bw->buffered, data, len);
        bw->buffered += len;
    } else {
        /* 刷新缓冲区 */
        bw->wrapped->write(bw->wrapped, bw->buffer, bw->buffered);
        bw->buffered = 0;

        /* 写入新数据 */
        if (len < sizeof(bw->buffer)) {
            memcpy(bw->buffer, data, len);
            bw->buffered = len;
        } else {
            bw->wrapped->write(bw->wrapped, data, len);
        }
    }
}

void buffered_flush(void* self) {
    buffered_writer_t* bw = self;
    if (bw->buffered > 0) {
        bw->wrapped->write(bw->wrapped, bw->buffer, bw->buffered);
        bw->buffered = 0;
    }
    bw->wrapped->flush(bw->wrapped);
}

writer_t* buffer_decorator_create(writer_t* wrapped) {
    buffered_writer_t* bw = malloc(sizeof(buffered_writer_t));
    bw->interface.write = buffered_write;
    bw->interface.flush = buffered_flush;
    bw->wrapped = wrapped;
    bw->buffered = 0;
    return (writer_t*)bw;
}

/* 装饰器: 压缩 */
typedef struct {
    writer_t interface;
    writer_t* wrapped;
    z_stream zstream;
} compressed_writer_t;

void compressed_write(void* self, const char* data, size_t len) {
    compressed_writer_t* cw = self;
    /* 压缩并写入 */
    deflate(&cw->zstream, Z_NO_FLUSH);
    /* ... */
}

/* 链式装饰: File -> Buffer -> Compress */
void example_decorator(void) {
    writer_t* file = file_writer_create("output.txt");
    writer_t* buffered = buffer_decorator_create(file);
    writer_t* compressed = compress_decorator_create(buffered);

    compressed->write(compressed, "Hello", 5);
    compressed->flush(compressed);
}
