/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md
 * Line: 193
 * Language: c
 * Block ID: 79ba759b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 示例：图像处理加速器顶层模块
void image_accelerator(
    // AXI4-Lite控制接口
    int *cmd_status,
    int *config_params,

    // AXI4主数据接口（DDR访问）
    int *src_image,
    int *dst_image,

    // AXI-Stream视频流接口
    hls::stream<ap_axiu<32,1,1,1>> &video_in,
    hls::stream<ap_axiu<32,1,1,1>> &video_out
) {
    // 控制接口
    #pragma HLS INTERFACE mode=s_axilite port=cmd_status bundle=ctrl
    #pragma HLS INTERFACE mode=s_axilite port=config_params bundle=ctrl
    #pragma HLS INTERFACE mode=s_axilite port=return bundle=ctrl

    // 存储器接口
    #pragma HLS INTERFACE mode=m_axi port=src_image \
                offset=slave bundle=gmem0 depth=2073600
    #pragma HLS INTERFACE mode=m_axi port=dst_image \
                offset=slave bundle=gmem1 depth=2073600

    // 流接口
    #pragma HLS INTERFACE mode=axis port=video_in
    #pragma HLS INTERFACE mode=axis port=video_out

    // 功能实现
    int operation = *cmd_status;

    switch(operation) {
        case 0: // 从DDR读取处理
            process_from_ddr(src_image, dst_image, config_params);
            break;
        case 1: // 流式处理
            process_stream(video_in, video_out, config_params);
            break;
        case 2: // 混合处理
            process_hybrid(src_image, video_out, config_params);
            break;
    }
}
