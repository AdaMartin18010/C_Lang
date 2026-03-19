/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\README.md
 * Line: 569
 * Language: c
 * Block ID: 29901564
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
