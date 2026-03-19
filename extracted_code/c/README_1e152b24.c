/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 314
 * Language: c
 * Block ID: 1e152b24
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef enum {
    IO_SUCCESS = 0,
    IO_ERROR_EOF,
    IO_ERROR_READ,
    IO_ERROR_WRITE,
    IO_ERROR_ALLOC
} io_status_t;

io_status_t safe_file_copy(const char *src, const char *dst) {
    FILE *in = fopen(src, "rb");
    if (!in) {
        perror("打开源文件失败");
        return IO_ERROR_READ;
    }

    FILE *out = fopen(dst, "wb");
    if (!out) {
        perror("打开目标文件失败");
        fclose(in);
        return IO_ERROR_WRITE;
    }

    /* 使用大缓冲区提高性能 */
    char buffer[8192];
    setvbuf(out, buffer, _IOFBF, sizeof(buffer));

    size_t bytes_read, bytes_written;
    io_status_t status = IO_SUCCESS;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), in)) > 0) {
        bytes_written = fwrite(buffer, 1, bytes_read, out);
        if (bytes_written != bytes_read) {
            perror("写入失败");
            status = IO_ERROR_WRITE;
            break;
        }
    }

    if (ferror(in)) {
        perror("读取失败");
        status = IO_ERROR_READ;
    }

    /* 确保所有数据写入磁盘 */
    if (fflush(out) != 0) {
        perror("刷新缓冲区失败");
        status = IO_ERROR_WRITE;
    }

    fclose(in);
    fclose(out);
    return status;
}
