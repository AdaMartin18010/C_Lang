/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 887
 * Language: c
 * Block ID: 1801107e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用局部存储实现显式突发
#define BURST_LEN 64

void explicit_burst_opt(
    int *global_mem,    // 外部DDR
    int n
) {
    #pragma HLS INTERFACE mode=m_axi port=global_mem \
                max_read_burst_length=64 \
                max_write_burst_length=64

    // 局部缓冲区匹配突发长度
    int local_buf[BURST_LEN];
    #pragma HLS ARRAY_PARTITION variable=local_buf cyclic factor=8

    for (int base = 0; base < n; base += BURST_LEN) {
        // 阶段1：突发读取（自动合并）
        READ_BURST:
        for (int i = 0; i < BURST_LEN; i++) {
            #pragma HLS PIPELINE II=1
            local_buf[i] = global_mem[base + i];
        }

        // 阶段2：本地处理（高带宽访问）
        PROCESS:
        for (int i = 0; i < BURST_LEN; i++) {
            #pragma HLS PIPELINE II=1
            #pragma HLS UNROLL factor=8
            local_buf[i] = local_buf[i] * local_buf[i] + 1;
        }

        // 阶段3：突发写回
        WRITE_BURST:
        for (int i = 0; i < BURST_LEN; i++) {
            #pragma HLS PIPELINE II=1
            global_mem[base + i] = local_buf[i];
        }
    }
}
