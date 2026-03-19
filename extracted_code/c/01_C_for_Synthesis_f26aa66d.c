/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\01_C_for_Synthesis.md
 * Line: 574
 * Language: c
 * Block ID: f26aa66d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 1. ap_none - 简单线连接（默认标量）
void interface_none(
    int *a,  // ap_none: 简单的输入/输出线
    int *b
) {
    #pragma HLS INTERFACE mode=ap_none port=a
    #pragma HLS INTERFACE mode=ap_none port=b
    *b = *a + 1;
}

// 2. ap_vld - 带有效信号的握手
void interface_vld(
    int *a,  // ap_vld: a_vld信号表示数据有效
    int *b   // ap_vld: b_vld信号表示输出有效
) {
    #pragma HLS INTERFACE mode=ap_vld port=a
    #pragma HLS INTERFACE mode=ap_vld port=b
    *b = *a * 2;
}

// 3. ap_ack - 带应答信号的握手
void interface_ack(
    int *a,  // ap_ack: a_ack表示数据被接收
    int *b
) {
    #pragma HLS INTERFACE mode=ap_ack port=a
    #pragma HLS INTERFACE mode=ap_ack port=b
    *b = *a << 1;
}

// 4. ap_hs - 完整握手（vld + ack）
void interface_hs(
    int *a,  // ap_hs: a_vld + a_ack
    int *b   // ap_hs: b_vld + b_ack
) {
    #pragma HLS INTERFACE mode=ap_hs port=a
    #pragma HLS INTERFACE mode=ap_hs port=b
    *b = *a ^ 0xFF;
}

// 5. 存储器接口（数组指针）
void interface_memory(
    int mem[1024],  // 映射为RAM接口
    int addr,
    int *data_out
) {
    #pragma HLS INTERFACE mode=ram_1p port=mem
    *data_out = mem[addr];
}

// 6. AXI接口（用于SoC集成）
void interface_axi(
    int *a,
    int *b,
    int c[100]
) {
    #pragma HLS INTERFACE mode=ap_ctrl_chain port=return
    #pragma HLS INTERFACE mode=s_axilite port=a
    #pragma HLS INTERFACE mode=s_axilite port=b
    #pragma HLS INTERFACE mode=m_axi port=c offset=slave

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        c[i] = c[i] + *a;
    }
    *b = c[99];
}
