/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\README.md
 * Line: 637
 * Language: c
 * Block ID: 87c2be3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void setup_multithreading(AVCodecContext* ctx) {
    // 根据 CPU 核心数设置线程数
    int cpu_count = av_cpu_count();
    ctx->thread_count = (cpu_count > 4) ? 4 : cpu_count;

    // 帧级多线程（适用于部分编码器）
    ctx->thread_type = FF_THREAD_FRAME | FF_THREAD_SLICE;
}
