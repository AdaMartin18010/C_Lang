/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\02_Custom_IO.md
 * Line: 178
 * Language: c
 * Block ID: ec485895
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 创建自定义IO上下文 */
AVFormatContext* create_custom_io_context(CustomIOContext *custom_ctx) {
    AVFormatContext *fmt_ctx = NULL;
    AVIOContext *avio_ctx = NULL;
    uint8_t *avio_buffer = NULL;

    /* 分配FFmpeg内部缓冲区（用于缓冲优化） */
    int avio_buffer_size = 32768;  /* 32KB默认 */
    avio_buffer = (uint8_t *)av_malloc(avio_buffer_size);
    if (!avio_buffer) {
        return NULL;
    }

    /* 创建AVIOContext */
    avio_ctx = avio_alloc_context(
        avio_buffer,           /* 内部缓冲区 */
        avio_buffer_size,      /* 缓冲区大小 */
        0,                     /* 写入标志: 0=只读, 1=只写 */
        custom_ctx,            /* opaque指针 */
        custom_read,           /* 读回调 */
        NULL,                  /* 写回调（只读模式） */
        custom_seek            /* 定位回调 */
    );

    if (!avio_ctx) {
        av_free(avio_buffer);
        return NULL;
    }

    /* 创建AVFormatContext */
    fmt_ctx = avformat_alloc_context();
    if (!fmt_ctx) {
        avio_context_free(&avio_ctx);
        return NULL;
    }

    /* 绑定自定义IO */
    fmt_ctx->pb = avio_ctx;
    fmt_ctx->flags |= AVFMT_FLAG_CUSTOM_IO;

    return fmt_ctx;
}
