/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\README.md
 * Line: 534
 * Language: c
 * Block ID: 6f293fa2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
