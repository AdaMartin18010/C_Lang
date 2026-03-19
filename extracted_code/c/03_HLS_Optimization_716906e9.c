/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 187
 * Language: c
 * Block ID: 716906e9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 存储器访问是常见瓶颈
 */

// 问题：非对齐访问
void memory_misaligned(int *mem, int out[100]) {
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        out[i] = mem[i * 3];  // 步长3，无法突发传输
    }
}

// 优化1：连续访问模式
void memory_sequential(int *mem, int out[100]) {
    #pragma HLS INTERFACE mode=m_axi port=mem max_read_burst_length=16

    int local_buf[100];

    // 连续突发读取
    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE
        local_buf[i] = mem[i];
    }

    // 本地处理
    for (int i = 0; i < 100; i++) {
        #pragma HLS UNROLL factor=4
        out[i] = process(local_buf[i]);
    }
}

// 优化2：数组分区增加端口
void memory_partition(int a[100], int b[100], int c[100], int d[100]) {
    #pragma HLS ARRAY_PARTITION variable=a cyclic factor=4
    #pragma HLS ARRAY_PARTITION variable=b cyclic factor=4
    #pragma HLS ARRAY_PARTITION variable=c cyclic factor=4
    #pragma HLS ARRAY_PARTITION variable=d cyclic factor=4

    for (int i = 0; i < 100; i++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL factor=4
        // 4端口同时访问
        process4(a[i], b[i], c[i], d[i]);
    }
}

// 优化3：数据打包减少访问次数
void memory_packing(ap_uint<512> *packed_mem, int out[64]) {
    // 512位总线 = 16个32位整数
    for (int i = 0; i < 4; i++) {
        #pragma HLS PIPELINE II=1
        ap_uint<512> data = packed_mem[i];

        // 解包处理
        for (int j = 0; j < 16; j++) {
            #pragma HLS UNROLL
            out[i*16+j] = data.range(j*32+31, j*32);
        }
    }
}
// 访问次数从64次减少到4次

// 优化4：双缓冲隐藏延迟
void memory_double_buffer(int *src, int *dst, int n) {
    #pragma HLS INTERFACE mode=m_axi port=src
    #pragma HLS INTERFACE mode=m_axi port=dst

    int buf0[256], buf1[256];
    #pragma HLS ARRAY_PARTITION variable=buf0 cyclic factor=8
    #pragma HLS ARRAY_PARTITION variable=buf1 cyclic factor=8

    for (int base = 0; base < n; base += 256) {
        #pragma HLS PIPELINE off

        if ((base / 256) % 2 == 0) {
            // 读入buf0，处理buf1
            load_buf(src, buf0, base, 256);
            process_buf(buf1, dst, base - 256, 256);
        } else {
            // 读入buf1，处理buf0
            load_buf(src, buf1, base, 256);
            process_buf(buf0, dst, base - 256, 256);
        }
    }
}
