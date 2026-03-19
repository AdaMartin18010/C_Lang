/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 1384
 * Language: c
 * Block ID: 4d4c68cd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 常用设计模式模板
 */

// 模式1：流式处理
void pattern_streaming(
    hls::stream<data_t> &in,
    hls::stream<data_t> &out
) {
    #pragma HLS INTERFACE mode=axis port=in
    #pragma HLS INTERFACE mode=axis port=out
    #pragma HLS INTERFACE mode=ap_ctrl_none port=return

    while (!in.empty()) {
        #pragma HLS PIPELINE II=1
        data_t data = in.read();
        out.write(process(data));
    }
}

// 模式2：帧处理
void pattern_frame(
    data_t in[FRAME_SIZE],
    data_t out[FRAME_SIZE]
) {
    #pragma HLS INTERFACE mode=ap_memory port=in
    #pragma HLS INTERFACE mode=ap_memory port=out
    #pragma HLS ARRAY_PARTITION variable=in cyclic factor=8
    #pragma HLS ARRAY_PARTITION variable=out cyclic factor=8

    for (int i = 0; i < FRAME_SIZE; i += 8) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=8
        for (int j = 0; j < 8; j++) {
            out[i+j] = process(in[i+j]);
        }
    }
}

// 模式3：数据流处理链
void pattern_dataflow_chain(
    data_t in[SIZE],
    data_t out[SIZE]
) {
    #pragma HLS DATAFLOW

    hls::stream<data_t> s1, s2;
    #pragma HLS STREAM variable=s1 depth=32
    #pragma HLS STREAM variable=s2 depth=32

    stage1(in, s1, SIZE);
    stage2(s1, s2, SIZE);
    stage3(s2, out, SIZE);
}

// 模式4：带配置的处理
void pattern_configurable(
    data_t in[SIZE],
    data_t out[SIZE],
    config_t *config
) {
    #pragma HLS INTERFACE mode=s_axilite port=config bundle=control
    #pragma HLS INTERFACE mode=m_axi port=in
    #pragma HLS INTERFACE mode=m_axi port=out

    config_t cfg = *config;

    switch(cfg.mode) {
        case MODE_A: process_a(in, out, SIZE); break;
        case MODE_B: process_b(in, out, SIZE); break;
        default:     process_default(in, out, SIZE);
    }
}
