/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 545
 * Language: c
 * Block ID: 905796e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整的数据处理加速器
void accelerator_top(
    // AXI-Lite控制接口
    int *cmd,
    int *status,

    // AXI主数据接口
    int *src1,
    int *src2,
    int *dst,
    int length,

    // AXI Stream旁路接口
    hls::stream<axis_t> &side_in,
    hls::stream<axis_t> &side_out
) {
    #pragma HLS INTERFACE mode=s_axilite port=cmd bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=status bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=length bundle=control
    #pragma HLS INTERFACE mode=s_axilite port=return bundle=control

    #pragma HLS INTERFACE mode=m_axi port=src1 offset=slave bundle=gmem0
    #pragma HLS INTERFACE mode=m_axi port=src2 offset=slave bundle=gmem1
    #pragma HLS INTERFACE mode=m_axi port=dst offset=slave bundle=gmem0

    #pragma HLS INTERFACE mode=axis port=side_in
    #pragma HLS INTERFACE mode=axis port=side_out

    int operation = *cmd;

    switch(operation) {
        case 0:  // 向量加法
            vector_add(src1, src2, dst, length);
            break;
        case 1:  // 向量乘法
            vector_mul(src1, src2, dst, length);
            break;
        case 2:  // 流处理
            stream_process(side_in, side_out, length);
            break;
        // ...
    }

    *status = 0;  // 完成
}
