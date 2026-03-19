/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\02_Custom_IO.md
 * Line: 229
 * Language: c
 * Block ID: 10cf7044
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    const uint8_t *data;      /* 原始数据指针 */
    size_t         size;      /* 数据总大小 */
    size_t         pos;       /* 当前位置 */
} MemBuffer;

/* 内存读取回调 */
int mem_read(void *opaque, uint8_t *buf, int buf_size) {
    MemBuffer *mem = (MemBuffer *)opaque;

    if (mem->pos >= mem->size) {
        return AVERROR_EOF;
    }

    size_t remaining = mem->size - mem->pos;
    size_t to_read = (buf_size < remaining) ? buf_size : remaining;

    memcpy(buf, mem->data + mem->pos, to_read);
    mem->pos += to_read;

    return (int)to_read;
}

int64_t mem_seek(void *opaque, int64_t offset, int whence) {
    MemBuffer *mem = (MemBuffer *)opaque;
    int64_t new_pos;

    switch (whence) {
    case SEEK_SET:
        new_pos = offset;
        break;
    case SEEK_CUR:
        new_pos = mem->pos + offset;
        break;
    case SEEK_END:
        new_pos = mem->size + offset;
        break;
    case AVSEEK_SIZE:
        return mem->size;
    default:
        return AVERROR(EINVAL);
    }

    if (new_pos < 0 || (size_t)new_pos > mem->size) {
        return AVERROR(EINVAL);
    }

    mem->pos = new_pos;
    return new_pos;
}

/* 从内存解码视频帧 */
int decode_from_memory(const uint8_t *data, size_t size,
                       void (*on_frame)(AVFrame *)) {
    MemBuffer mem = {data, size, 0};
    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *dec_ctx = NULL;
    AVFrame *frame = NULL;
    AVPacket *pkt = NULL;
    int ret, video_stream_idx = -1;

    /* 分配AVIOContext */
    uint8_t *avio_buffer = av_malloc(32768);
    AVIOContext *avio_ctx = avio_alloc_context(
        avio_buffer, 32768, 0, &mem, mem_read, NULL, mem_seek
    );

    /* 打开输入 */
    fmt_ctx = avformat_alloc_context();
    fmt_ctx->pb = avio_ctx;

    ret = avformat_open_input(&fmt_ctx, NULL, NULL, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to open input: %s\n", av_err2str(ret));
        goto cleanup;
    }

    /* 获取流信息 */
    ret = avformat_find_stream_info(fmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Failed to find stream info\n");
        goto cleanup;
    }

    /* 查找视频流 */
    for (unsigned i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_idx = i;
            break;
        }
    }

    if (video_stream_idx < 0) {
        fprintf(stderr, "No video stream found\n");
        ret = -1;
        goto cleanup;
    }

    /* 初始化解码器 */
    AVStream *stream = fmt_ctx->streams[video_stream_idx];
    const AVCodec *codec = avcodec_find_decoder(stream->codecpar->codec_id);
    dec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(dec_ctx, stream->codecpar);
    avcodec_open2(dec_ctx, codec, NULL);

    /* 解码循环 */
    frame = av_frame_alloc();
    pkt = av_packet_alloc();

    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == video_stream_idx) {
            ret = avcodec_send_packet(dec_ctx, pkt);
            while (ret >= 0) {
                ret = avcodec_receive_frame(dec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                }
                if (ret < 0) goto cleanup;

                on_frame(frame);
            }
        }
        av_packet_unref(pkt);
    }

    /* 刷新解码器 */
    avcodec_send_packet(dec_ctx, NULL);
    while (avcodec_receive_frame(dec_ctx, frame) >= 0) {
        on_frame(frame);
    }

cleanup:
    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&dec_ctx);
    avformat_close_input(&fmt_ctx);
    avio_context_free(&avio_ctx);

    return ret;
}
