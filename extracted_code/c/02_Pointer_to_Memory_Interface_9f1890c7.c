/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 1045
 * Language: c
 * Block ID: 9f1890c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 简化的DMA控制器
 * 功能：在内存区域之间高效传输数据
 */

#include "ap_int.h"

typedef ap_uint<32> addr_t;
typedef ap_uint<32> data_t;

// DMA配置寄存器结构
typedef struct {
    addr_t src_addr;
    addr_t dst_addr;
    ap_uint<16> length;
    ap_uint<2>  burst_size;  // 0=1, 1=4, 2=16, 3=64
    ap_uint<1>  start;
    ap_uint<1>  done;
} dma_config_t;

void dma_controller(
    data_t *mem,            // 统一内存空间
    dma_config_t *config    // 配置接口
) {
    #pragma HLS INTERFACE mode=s_axilite port=config bundle=control
    #pragma HLS INTERFACE mode=m_axi port=mem offset=slave \
                max_read_burst_length=64 max_write_burst_length=64

    // 从配置寄存器读取
    addr_t src = config->src_addr;
    addr_t dst = config->dst_addr;
    ap_uint<16> len = config->length;
    ap_uint<8> burst = (1 << config->burst_size);  // 1, 4, 16, 64

    // 本地缓冲区
    data_t buf[64];
    #pragma HLS ARRAY_PARTITION variable=buf cyclic factor=8

    // DMA传输主循环
    addr_t remaining = len;
    addr_t src_offset = 0;
    addr_t dst_offset = 0;

    while (remaining > 0) {
        #pragma HLS LOOP_TRIPCOUNT min=1 max=1000

        ap_uint<8> current_burst = (remaining < burst) ?
                                   remaining.range(7,0) : burst;

        // 读阶段：突发读取
        READ_PHASE:
        for (ap_uint<8> i = 0; i < current_burst; i++) {
            #pragma HLS PIPELINE II=1
            buf[i] = mem[(src + src_offset + i) >> 2];
        }

        // 可选：数据处理阶段（如校验和、格式转换等）
        PROCESS_PHASE:
        for (ap_uint<8> i = 0; i < current_burst; i++) {
            #pragma HLS PIPELINE II=1
            // buf[i] = transform(buf[i]);
        }

        // 写阶段：突发写入
        WRITE_PHASE:
        for (ap_uint<8> i = 0; i < current_burst; i++) {
            #pragma HLS PIPELINE II=1
            mem[(dst + dst_offset + i) >> 2] = buf[i];
        }

        src_offset += current_burst;
        dst_offset += current_burst;
        remaining -= current_burst;
    }

    config->done = 1;
}
