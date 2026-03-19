# 视频编解码技术指南


---

## 📑 目录

- [视频编解码技术指南](#视频编解码技术指南)
  - [📑 目录](#-目录)
  - [概述](#概述)
  - [视频编码基础](#视频编码基础)
    - [视频压缩原理](#视频压缩原理)
    - [常见视频编码标准](#常见视频编码标准)
  - [FFmpeg C API 基础](#ffmpeg-c-api-基础)
    - [环境配置](#环境配置)
    - [基本数据结构](#基本数据结构)
  - [H.264/H.265 编码实现](#h264h265-编码实现)
    - [H.264 编码器配置](#h264-编码器配置)
    - [H.265/HEVC 编码](#h265hevc-编码)
  - [视频解码实现](#视频解码实现)
    - [H.264 解码器](#h264-解码器)
  - [视频流处理](#视频流处理)
    - [RTSP 流处理](#rtsp-流处理)
    - [视频转封装（无需重新编码）](#视频转封装无需重新编码)
  - [编解码器选择策略](#编解码器选择策略)
    - [场景化选择建议](#场景化选择建议)
    - [编码参数优化](#编码参数优化)
  - [性能优化技巧](#性能优化技巧)
    - [硬件加速编码](#硬件加速编码)
    - [多线程编码](#多线程编码)
  - [调试与错误处理](#调试与错误处理)
    - [错误日志处理](#错误日志处理)
  - [总结](#总结)


---

## 概述

视频编解码是将原始视频数据压缩编码以减少存储空间和传输带宽，并在播放时解码还原的技术。在现代多媒体应用中，视频编解码器是核心组件，直接影响视频质量、文件大小和播放性能。本指南详细介绍使用 C 语言和 FFmpeg 库进行视频编解码开发的技术细节。

## 视频编码基础

### 视频压缩原理

视频压缩利用空间冗余（帧内预测）和时间冗余（帧间预测）来减少数据量：

- **I 帧（关键帧）**：独立编码，不依赖其他帧
- **P 帧（预测帧）**：基于前一帧预测编码
- **B 帧（双向预测帧）**：基于前后帧预测编码

### 常见视频编码标准

| 标准 | 特点 | 应用场景 |
|------|------|----------|
| H.264/AVC | 高压缩率，广泛支持 | 流媒体、视频会议 |
| H.265/HEVC | 比 H.264 节省 50% 码率 | 4K/8K 视频 |
| VP9 | 开源，WebM 格式 | YouTube、Web 视频 |
| AV1 | 下一代开源标准 | 流媒体服务 |

## FFmpeg C API 基础

### 环境配置

```bash
# Ubuntu/Debian
sudo apt-get install libavcodec-dev libavformat-dev libavutil-dev \
                     libswscale-dev libavfilter-dev

# 从源码编译
wget https://ffmpeg.org/releases/ffmpeg-6.0.tar.xz
tar -xf ffmpeg-6.0.tar.xz
cd ffmpeg-6.0
./configure --enable-shared --enable-gpl --enable-libx264 --enable-libx265
make -j$(nproc)
sudo make install
```

### 基本数据结构

```c
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>

// FFmpeg 核心数据结构：
// AVFormatContext - 封装格式上下文
// AVCodecContext  - 编解码器上下文
// AVFrame         - 原始视频帧
// AVPacket        - 压缩数据包
// AVStream        - 媒体流
```

## H.264/H.265 编码实现

### H.264 编码器配置

```c
#include <libavcodec/avcodec.h>
#include <libavutil/opt.h>
#include <stdio.h>

typedef struct {
    AVCodecContext* codec_ctx;
    AVFrame* frame;
    AVPacket* packet;
    FILE* output_file;
    int frame_count;
} H264Encoder;

H264Encoder* h264_encoder_create(const char* filename, int width, int height,
                                  int fps, int bitrate) {
    H264Encoder* enc = (H264Encoder*)calloc(1, sizeof(H264Encoder));

    // 查找 H.264 编码器
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "H.264 编码器未找到\n");
        free(enc);
        return NULL;
    }

    // 分配编码器上下文
    enc->codec_ctx = avcodec_alloc_context3(codec);
    if (!enc->codec_ctx) {
        fprintf(stderr, "无法分配编码器上下文\n");
        free(enc);
        return NULL;
    }

    // 配置编码参数
    enc->codec_ctx->width = width;
    enc->codec_ctx->height = height;
    enc->codec_ctx->time_base = (AVRational){1, fps};
    enc->codec_ctx->framerate = (AVRational){fps, 1};
    enc->codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    enc->codec_ctx->bit_rate = bitrate;

    // GOP 设置
    enc->codec_ctx->gop_size = 12;  // I 帧间隔
    enc->codec_ctx->max_b_frames = 2;

    // 预设和优化
    av_opt_set(enc->codec_ctx->priv_data, "preset", "medium", 0);
    av_opt_set(enc->codec_ctx->priv_data, "tune", "zerolatency", 0);
    av_opt_set(enc->codec_ctx->priv_data, "profile", "high", 0);

    // 打开编码器
    if (avcodec_open2(enc->codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "无法打开编码器\n");
        avcodec_free_context(&enc->codec_ctx);
        free(enc);
        return NULL;
    }

    // 分配帧和包
    enc->frame = av_frame_alloc();
    enc->frame->format = enc->codec_ctx->pix_fmt;
    enc->frame->width = enc->codec_ctx->width;
    enc->frame->height = enc->codec_ctx->height;
    av_frame_get_buffer(enc->frame, 0);

    enc->packet = av_packet_alloc();
    enc->output_file = fopen(filename, "wb");
    enc->frame_count = 0;

    return enc;
}

void h264_encoder_encode_frame(H264Encoder* enc, uint8_t* rgb_data, int linesize) {
    // RGB 转 YUV420P
    struct SwsContext* sws_ctx = sws_getContext(
        enc->codec_ctx->width, enc->codec_ctx->height, AV_PIX_FMT_RGB24,
        enc->codec_ctx->width, enc->codec_ctx->height, AV_PIX_FMT_YUV420P,
        SWS_BILINEAR, NULL, NULL, NULL);

    uint8_t* in_data[1] = {rgb_data};
    int in_linesize[1] = {linesize};

    sws_scale(sws_ctx, in_data, in_linesize, 0, enc->codec_ctx->height,
              enc->frame->data, enc->frame->linesize);
    sws_freeContext(sws_ctx);

    enc->frame->pts = enc->frame_count++;

    // 发送帧到编码器
    int ret = avcodec_send_frame(enc->codec_ctx, enc->frame);
    if (ret < 0) {
        fprintf(stderr, "发送帧失败\n");
        return;
    }

    // 接收编码后的包
    while (ret >= 0) {
        ret = avcodec_receive_packet(enc->codec_ctx, enc->packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            break;
        } else if (ret < 0) {
            fprintf(stderr, "编码错误\n");
            return;
        }

        // 写入文件
        fwrite(enc->packet->data, 1, enc->packet->size, enc->output_file);
        av_packet_unref(enc->packet);
    }
}

void h264_encoder_close(H264Encoder* enc) {
    // 刷新编码器
    avcodec_send_frame(enc->codec_ctx, NULL);

    int ret;
    while ((ret = avcodec_receive_packet(enc->codec_ctx, enc->packet)) >= 0) {
        fwrite(enc->packet->data, 1, enc->packet->size, enc->output_file);
        av_packet_unref(enc->packet);
    }

    fclose(enc->output_file);
    av_packet_free(&enc->packet);
    av_frame_free(&enc->frame);
    avcodec_free_context(&enc->codec_ctx);
    free(enc);
}
```

### H.265/HEVC 编码

```c
#include <libavcodec/avcodec.h>

int h265_encode_setup(AVCodecContext** ctx, int width, int height, int bitrate) {
    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_HEVC);
    if (!codec) {
        fprintf(stderr, "HEVC 编码器未找到\n");
        return -1;
    }

    *ctx = avcodec_alloc_context3(codec);
    (*ctx)->width = width;
    (*ctx)->height = height;
    (*ctx)->bit_rate = bitrate;
    (*ctx)->pix_fmt = AV_PIX_FMT_YUV420P;
    (*ctx)->time_base = (AVRational){1, 25};

    // HEVC 特定优化参数
    av_opt_set((*ctx)->priv_data, "preset", "slow", 0);
    av_opt_set((*ctx)->priv_data, "x265-params",
               "crf=23:psy-rd=1:aq-mode=3", 0);

    if (avcodec_open2(*ctx, codec, NULL) < 0) {
        fprintf(stderr, "无法打开 HEVC 编码器\n");
        return -1;
    }

    return 0;
}
```

## 视频解码实现

### H.264 解码器

```c
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>

typedef struct {
    AVCodecContext* codec_ctx;
    AVFrame* frame;
    AVPacket* packet;
    AVCodecParserContext* parser;
} H264Decoder;

H264Decoder* h264_decoder_create(void) {
    H264Decoder* dec = (H264Decoder*)calloc(1, sizeof(H264Decoder));

    const AVCodec* codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "H.264 解码器未找到\n");
        free(dec);
        return NULL;
    }

    dec->parser = av_parser_init(AV_CODEC_ID_H264);
    dec->codec_ctx = avcodec_alloc_context3(codec);

    if (avcodec_open2(dec->codec_ctx, codec, NULL) < 0) {
        fprintf(stderr, "无法打开解码器\n");
        free(dec);
        return NULL;
    }

    dec->frame = av_frame_alloc();
    dec->packet = av_packet_alloc();

    return dec;
}

int h264_decoder_decode(H264Decoder* dec, uint8_t* data, int size,
                        void (*on_frame)(AVFrame* frame, void* user_data),
                        void* user_data) {
    uint8_t* data_ptr = data;
    int data_size = size;

    while (data_size > 0) {
        int ret = av_parser_parse2(dec->parser, dec->codec_ctx,
                                   &dec->packet->data, &dec->packet->size,
                                   data_ptr, data_size,
                                   AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);

        if (ret < 0) {
            fprintf(stderr, "解析错误\n");
            return -1;
        }

        data_ptr += ret;
        data_size -= ret;

        if (dec->packet->size) {
            ret = avcodec_send_packet(dec->codec_ctx, dec->packet);
            if (ret < 0) {
                fprintf(stderr, "发送包错误\n");
                return -1;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(dec->codec_ctx, dec->frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    fprintf(stderr, "解码错误\n");
                    return -1;
                }

                if (on_frame) {
                    on_frame(dec->frame, user_data);
                }
            }
        }
    }

    return 0;
}

void h264_decoder_close(H264Decoder* dec) {
    av_parser_close(dec->parser);
    av_packet_free(&dec->packet);
    av_frame_free(&dec->frame);
    avcodec_free_context(&dec->codec_ctx);
    free(dec);
}
```

## 视频流处理

### RTSP 流处理

```c
#include <libavformat/avformat.h>

int process_rtsp_stream(const char* url) {
    AVFormatContext* fmt_ctx = NULL;
    AVCodecContext* codec_ctx = NULL;

    // 初始化网络
    avformat_network_init();

    // 设置 RTSP 选项
    AVDictionary* opts = NULL;
    av_dict_set(&opts, "rtsp_transport", "tcp", 0);
    av_dict_set(&opts, "stimeout", "5000000", 0);  // 5秒超时

    // 打开输入
    if (avformat_open_input(&fmt_ctx, url, NULL, &opts) < 0) {
        fprintf(stderr, "无法打开 RTSP 流: %s\n", url);
        av_dict_free(&opts);
        return -1;
    }
    av_dict_free(&opts);

    // 查找流信息
    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) {
        fprintf(stderr, "无法查找流信息\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // 查找视频流
    int video_stream = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream = i;
            break;
        }
    }

    if (video_stream == -1) {
        fprintf(stderr, "未找到视频流\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    // 获取编解码器参数
    AVCodecParameters* codecpar = fmt_ctx->streams[video_stream]->codecpar;
    const AVCodec* codec = avcodec_find_decoder(codecpar->codec_id);

    if (!codec) {
        fprintf(stderr, "未找到解码器\n");
        avformat_close_input(&fmt_ctx);
        return -1;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    avcodec_parameters_to_context(codec_ctx, codecpar);
    avcodec_open2(codec_ctx, codec, NULL);

    printf("视频信息: %dx%d, %s\n",
           codec_ctx->width, codec_ctx->height, codec->name);

    // 读取帧
    AVPacket* packet = av_packet_alloc();
    AVFrame* frame = av_frame_alloc();
    int frame_count = 0;

    while (av_read_frame(fmt_ctx, packet) >= 0) {
        if (packet->stream_index == video_stream) {
            int ret = avcodec_send_packet(codec_ctx, packet);
            if (ret < 0) continue;

            while (ret >= 0) {
                ret = avcodec_receive_frame(codec_ctx, frame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) break;

                frame_count++;
                printf("解码帧 #%d, pts: %ld\n", frame_count, frame->pts);
            }
        }
        av_packet_unref(packet);
    }

    av_packet_free(&packet);
    av_frame_free(&frame);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
    avformat_network_deinit();

    return 0;
}
```

### 视频转封装（无需重新编码）

```c
#include <libavformat/avformat.h>

int remux_video(const char* input_file, const char* output_file) {
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;

    // 打开输入文件
    if (avformat_open_input(&ifmt_ctx, input_file, NULL, NULL) < 0) {
        fprintf(stderr, "无法打开输入文件\n");
        return -1;
    }

    avformat_find_stream_info(ifmt_ctx, NULL);

    // 分配输出上下文
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, output_file);
    if (!ofmt_ctx) {
        fprintf(stderr, "无法创建输出上下文\n");
        avformat_close_input(&ifmt_ctx);
        return -1;
    }

    // 复制流
    for (unsigned int i = 0; i < ifmt_ctx->nb_streams; i++) {
        AVStream* in_stream = ifmt_ctx->streams[i];
        AVStream* out_stream = avformat_new_stream(ofmt_ctx, NULL);

        avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
        out_stream->codecpar->codec_tag = 0;
    }

    // 打开输出文件
    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_open(&ofmt_ctx->pb, output_file, AVIO_FLAG_WRITE);
    }

    avformat_write_header(ofmt_ctx, NULL);

    // 复制包
    AVPacket* packet = av_packet_alloc();
    int64_t start_time = av_gettime();

    while (av_read_frame(ifmt_ctx, packet) >= 0) {
        AVStream* in_stream = ifmt_ctx->streams[packet->stream_index];
        AVStream* out_stream = ofmt_ctx->streams[packet->stream_index];

        // 时间戳转换
        packet->pts = av_rescale_q_rnd(packet->pts, in_stream->time_base,
                                       out_stream->time_base,
                                       AV_ROUND_NEAR_INF);
        packet->dts = av_rescale_q_rnd(packet->dts, in_stream->time_base,
                                       out_stream->time_base,
                                       AV_ROUND_NEAR_INF);
        packet->duration = av_rescale_q(packet->duration, in_stream->time_base,
                                        out_stream->time_base);
        packet->pos = -1;

        av_interleaved_write_frame(ofmt_ctx, packet);
        av_packet_unref(packet);
    }

    av_write_trailer(ofmt_ctx);

    av_packet_free(&packet);
    avformat_close_input(&ifmt_ctx);

    if (!(ofmt_ctx->oformat->flags & AVFMT_NOFILE)) {
        avio_closep(&ofmt_ctx->pb);
    }
    avformat_free_context(ofmt_ctx);

    return 0;
}
```

## 编解码器选择策略

### 场景化选择建议

```c
// 编解码器选择决策结构
typedef enum {
    USE_CASE_STREAMING,      // 实时流媒体
    USE_CASE_STORAGE,        // 存储归档
    USE_CASE_EDITING,        // 视频编辑
    USE_CASE_CONFERENCING    // 视频会议
} UseCase;

AVCodecID select_codec(UseCase use_case, int priority_quality) {
    switch (use_case) {
        case USE_CASE_STREAMING:
            // 流媒体优先考虑兼容性和码率
            return priority_quality ? AV_CODEC_ID_HEVC : AV_CODEC_ID_H264;

        case USE_CASE_STORAGE:
            // 存储优先考虑压缩效率
            return AV_CODEC_ID_HEVC;

        case USE_CASE_EDITING:
            // 编辑需要帧内编码或低延迟
            return AV_CODEC_ID_PRORES;  // 或 DNxHD

        case USE_CASE_CONFERENCING:
            // 会议需要低延迟
            return AV_CODEC_ID_H264;

        default:
            return AV_CODEC_ID_H264;
    }
}
```

### 编码参数优化

```c
void optimize_encoding_params(AVCodecContext* ctx, UseCase use_case) {
    switch (use_case) {
        case USE_CASE_STREAMING:
            ctx->gop_size = 2 * ctx->time_base.den / ctx->time_base.num;
            ctx->max_b_frames = 0;  // 低延迟
            av_opt_set(ctx->priv_data, "tune", "zerolatency", 0);
            break;

        case USE_CASE_STORAGE:
            ctx->gop_size = 250;
            ctx->max_b_frames = 3;
            av_opt_set(ctx->priv_data, "preset", "veryslow", 0);
            av_opt_set(ctx->priv_data, "crf", "18", 0);
            break;

        case USE_CASE_CONFERENCING:
            ctx->gop_size = 30;
            ctx->max_b_frames = 0;
            ctx->bit_rate = 2000000;  // 2 Mbps
            av_opt_set(ctx->priv_data, "tune", "zerolatency", 0);
            break;
    }
}
```

## 性能优化技巧

### 硬件加速编码

```c
#include <libavcodec/avcodec.h>

int setup_hardware_encoding(AVCodecContext* ctx, int width, int height) {
    // 尝试 NVENC (NVIDIA GPU)
    const AVCodec* nvenc_codec = avcodec_find_encoder_by_name("h264_nvenc");
    if (nvenc_codec) {
        printf("使用 NVIDIA NVENC 硬件编码\n");
        avcodec_free_context(&ctx);
        ctx = avcodec_alloc_context3(nvenc_codec);
        // NVENC 参数配置
        av_opt_set(ctx->priv_data, "preset", "p4", 0);
        av_opt_set(ctx->priv_data, "tune", "hq", 0);
        av_opt_set(ctx->priv_data, "rc", "vbr", 0);
        return 0;
    }

    // 尝试 VAAPI (Intel/AMD)
    const AVCodec* vaapi_codec = avcodec_find_encoder_by_name("h264_vaapi");
    if (vaapi_codec) {
        printf("使用 VAAPI 硬件编码\n");
        return 0;
    }

    // 尝试 VideoToolbox (macOS)
    const AVCodec* vt_codec = avcodec_find_encoder_by_name("h264_videotoolbox");
    if (vt_codec) {
        printf("使用 VideoToolbox 硬件编码\n");
        return 0;
    }

    printf("使用软件编码\n");
    return -1;
}
```

### 多线程编码

```c
void setup_multithreading(AVCodecContext* ctx) {
    // 根据 CPU 核心数设置线程数
    int cpu_count = av_cpu_count();
    ctx->thread_count = (cpu_count > 4) ? 4 : cpu_count;

    // 帧级多线程（适用于部分编码器）
    ctx->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;
}
```

## 调试与错误处理

### 错误日志处理

```c
#include <libavutil/log.h>

void setup_ffmpeg_logging(void) {
    av_log_set_level(AV_LOG_DEBUG);

    av_log_set_callback([](void* ptr, int level, const char* fmt, va_list vl) {
        if (level > av_log_get_level()) return;

        const char* level_str = "UNKNOWN";
        switch (level) {
            case AV_LOG_PANIC:   level_str = "PANIC"; break;
            case AV_LOG_FATAL:   level_str = "FATAL"; break;
            case AV_LOG_ERROR:   level_str = "ERROR"; break;
            case AV_LOG_WARNING: level_str = "WARN"; break;
            case AV_LOG_INFO:    level_str = "INFO"; break;
            case AV_LOG_DEBUG:   level_str = "DEBUG"; break;
        }

        fprintf(stderr, "[FFMPEG %s] ", level_str);
        vfprintf(stderr, fmt, vl);
    });
}
```

## 总结

视频编解码是一个复杂的领域，涉及多种算法和标准。使用 FFmpeg C API 进行开发时，需要注意：

1. **资源管理**：正确分配和释放 AVFrame、AVPacket 等资源
2. **线程安全**：多线程环境下的编解码器使用
3. **错误处理**：完善的错误检查和日志记录
4. **性能优化**：硬件加速和参数调优
5. **兼容性**：考虑不同平台和播放器的支持

掌握这些技术可以构建高效、稳定的视频处理系统。


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
