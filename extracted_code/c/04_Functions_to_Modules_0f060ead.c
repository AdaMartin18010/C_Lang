/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\04_Functions_to_Modules.md
 * Line: 242
 * Language: c
 * Block ID: 0f060ead
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 模式1：纯组合逻辑接口（ap_none）
int combinational_submodule(int a, int b, int c) {
    #pragma HLS INTERFACE mode=ap_none port=a
    #pragma HLS INTERFACE mode=ap_none port=b
    #pragma HLS INTERFACE mode=ap_none port=c

    // 纯组合逻辑，无寄存器
    return a * b + c;
}
/* 生成接口：
   input  wire [31:0] a,
   input  wire [31:0] b,
   input  wire [31:0] c,
   output wire [31:0] ap_return
*/

// 模式2：带有效信号的寄存器接口（ap_vld）
void register_interface(
    int input_data,
    int *output_data,
    char control
) {
    #pragma HLS INTERFACE mode=ap_vld port=input_data
    #pragma HLS INTERFACE mode=ap_vld port=output_data
    #pragma HLS INTERFACE mode=ap_none port=control

    static int internal_reg;

    if (control) {
        internal_reg = input_data;
    }
    *output_data = internal_reg;
}
/* 生成接口：
   input  wire [31:0] input_data,
   input  wire        input_data_ap_vld,
   output wire [31:0] output_data,
   output wire        output_data_ap_vld,
   input  wire [7:0]  control
*/

// 模式3：握手接口（ap_hs）
void handshake_interface(
    int *input_data,
    int *output_data
) {
    #pragma HLS INTERFACE mode=ap_hs port=input_data
    #pragma HLS INTERFACE mode=ap_hs port=output_data

    *output_data = process(*input_data);
}
/* 生成接口：
   input  wire [31:0] input_data,
   input  wire        input_data_ap_vld,
   output wire        input_data_ap_ack,
   output wire [31:0] output_data,
   output wire        output_data_ap_vld,
   input  wire        output_data_ap_ack
*/

// 模式4：存储器接口（ap_memory）
void memory_interface(
    int mem[1024],
    int addr,
    int we,
    int wdata,
    int *rdata
) {
    #pragma HLS INTERFACE mode=ap_memory port=mem

    if (we) {
        mem[addr] = wdata;
    }
    *rdata = mem[addr];
}
/* 生成接口：
   input  wire [9:0]  mem_address0,
   input  wire        mem_ce0,
   output wire [31:0] mem_q0,
   input  wire        mem_we0,
   input  wire [31:0] mem_d0
*/
