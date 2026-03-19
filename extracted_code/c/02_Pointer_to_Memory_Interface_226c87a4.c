/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 293
 * Language: c
 * Block ID: 226c87a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 寄存器文件 - 完全分区的存储器
void register_file(int regs[32], int raddr1, int raddr2, int waddr,
                   int wdata, int we, int *rdata1, int *rdata2) {
    #pragma HLS INTERFACE mode=ap_memory port=regs
    #pragma HLS ARRAY_PARTITION variable=regs complete

    // 两个读端口
    *rdata1 = regs[raddr1];
    *rdata2 = regs[raddr2];

    // 一个写端口
    if (we) {
        regs[waddr] = wdata;
    }
}

// 使用ap_uint索引的寄存器文件
#include "ap_int.h"
void advanced_register_file(
    ap_uint<32> regs[32],
    ap_uint<5> raddr1, ap_uint<5> raddr2, ap_uint<5> waddr,
    ap_uint<32> wdata, ap_uint<1> we,
    ap_uint<32> *rdata1, ap_uint<32> *rdata2
) {
    #pragma HLS ARRAY_PARTITION variable=regs complete
    #pragma HLS INTERFACE mode=ap_ctrl_chain port=return

    // 组合逻辑读（无延迟）
    *rdata1 = regs[raddr1];
    *rdata2 = regs[raddr2];

    // 时序写
    if (we) {
        regs[waddr] = wdata;
    }
}
