/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 931
 * Language: c
 * Block ID: bac4c950
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 利用AXI多通道并行突发
void multi_channel_burst(
    int *src1, int *src2, int *src3,
    int *dst,
    int n
) {
    #pragma HLS INTERFACE mode=m_axi port=src1 bundle=gmem0
    #pragma HLS INTERFACE mode=m_axi port=src2 bundle=gmem1
    #pragma HLS INTERFACE mode=m_axi port=src3 bundle=gmem2
    #pragma HLS INTERFACE mode=m_axi port=dst bundle=gmem0

    // 数据流优化实现三端口并发访问
    #pragma HLS DATAFLOW

    int buf1[256], buf2[256], buf3[256];
    int result[256];

    for (int base = 0; base < n; base += 256) {
        // 并发读取三个源
        read_channel(src1, buf1, base, 256);
        read_channel(src2, buf2, base, 256);
        read_channel(src3, buf3, base, 256);

        // 处理
        compute_3input(buf1, buf2, buf3, result, 256);

        // 写回
        write_channel(dst, result, base, 256);
    }
}

// 辅助函数
void read_channel(int *src, int buf[256], int base, int n) {
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        buf[i] = src[base + i];
    }
}

void compute_3input(int a[256], int b[256], int c[256],
                    int out[256], int n) {
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        #pragma HLS UNROLL factor=4
        out[i] = a[i] + b[i] * c[i];
    }
}

void write_channel(int *dst, int buf[256], int base, int n) {
    for (int i = 0; i < n; i++) {
        #pragma HLS PIPELINE
        dst[base + i] = buf[i];
    }
}
