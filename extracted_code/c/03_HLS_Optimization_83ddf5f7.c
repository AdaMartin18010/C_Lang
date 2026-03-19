/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 1016
 * Language: c
 * Block ID: 83ddf5f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 视频处理链优化
 * 功能：去噪 -> 边缘检测 -> 增强
 */

// 未优化：顺序执行
void video_chain_sequential(
    pixel_t in[1920*1080],
    pixel_t out[1920*1080]
) {
    pixel_t temp1[1920*1080];
    pixel_t temp2[1920*1080];

    denoise(in, temp1, 1920*1080);      // 33ms @ 60fps
    edge_detect(temp1, temp2, 1920*1080); // 33ms
    enhance(temp2, out, 1920*1080);      // 33ms
    // 总延迟：99ms = 10fps ❌
}

// 优化：DATAFLOW流水线
void video_chain_dataflow(
    hls::stream<pixel_t> &in,
    hls::stream<pixel_t> &out
) {
    #pragma HLS INTERFACE mode=axis port=in
    #pragma HLS INTERFACE mode=axis port=out
    #pragma HLS DATAFLOW

    hls::stream<pixel_t> s1("s1");
    hls::stream<pixel_t> s2("s2");
    #pragma HLS STREAM variable=s1 depth=1920*3  // 3行缓冲
    #pragma HLS STREAM variable=s2 depth=1920*3

    // 三个阶段并发执行
    denoise_stream(in, s1);
    edge_detect_stream(s1, s2);
    enhance_stream(s2, out);
    // 总延迟：约33ms + 填充 = 35ms = 28fps
}

// 进一步优化：每级II=1
void video_chain_optimized(
    hls::stream<pixel_t> &in,
    hls::stream<pixel_t> &out
) {
    #pragma HLS INTERFACE mode=axis port=in
    #pragma HLS INTERFACE mode=axis port=out
    #pragma HLS DATAFLOW

    hls::stream<pixel_t> s1("s1");
    hls::stream<pixel_t> s2("s2");
    #pragma HLS STREAM variable=s1 depth=64
    #pragma HLS STREAM variable=s2 depth=64

    denoise_ii1(in, s1, 1920*1080);
    edge_detect_ii1(s1, s2, 1920*1080);
    enhance_ii1(s2, out, 1920*1080);
    // 每周期1像素，满足60fps @ 150MHz
}
