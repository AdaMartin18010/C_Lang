/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 380
 * Language: c
 * Block ID: 6fffeb77
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AXI主接口 - HLS IP访问外部存储器
#include "ap_axi_sdata.h"

void axi_master_example(
    int *mem,           // 映射为AXI4主接口
    int base_addr,
    int size
) {
    #pragma HLS INTERFACE mode=m_axi port=mem \
                offset=slave bundle=gmem0 \
                depth=1024 max_read_burst_length=16 max_write_burst_length=16

    // 本地缓冲区
    int local_buf[256];
    #pragma HLS ARRAY_PARTITION variable=local_buf cyclic factor=4

    // 从DDR读取数据（突发传输）
    for (int i = 0; i < size; i += 256) {
        // HLS自动合并为突发传输
        for (int j = 0; j < 256; j++) {
            #pragma HLS PIPELINE
            local_buf[j] = mem[base_addr + i + j];
        }

        // 处理数据
        for (int j = 0; j < 256; j++) {
            #pragma HLS PIPELINE
            local_buf[j] = process(local_buf[j]);
        }

        // 写回DDR（突发传输）
        for (int j = 0; j < 256; j++) {
            #pragma HLS PIPELINE
            mem[base_addr + i + j] = local_buf[j];
        }
    }
}

/* AXI4接口信号：
   全局信号：
   - ACLK, ARESETN

   写地址通道：
   - AWADDR[31:0]  - 写地址
   - AWLEN[7:0]    - 突发长度-1
   - AWSIZE[2:0]   - 数据宽度
   - AWBURST[1:0]  - 突发类型
   - AWVALID/AWREADY

   写数据通道：
   - WDATA[31:0]   - 写数据
   - WSTRB[3:0]    - 字节使能
   - WLAST         - 最后传输
   - WVALID/WREADY

   写响应通道：
   - BRESP[1:0]    - 响应状态
   - BVALID/BREADY

   读地址通道（类似写地址）
   读数据通道（类似写数据，无WSTRB）
*/
