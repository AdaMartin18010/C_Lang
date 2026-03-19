/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 497
 * Language: c
 * Block ID: 61a91967
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AXI Stream接口 - 流式数据处理
#include "ap_axi_sdata.h"

typedef ap_axiu<32, 0, 0, 0> axis_t;  // 32位数据，无sideband信号

void axi_stream_example(
    hls::stream<axis_t> &in_stream,   // 输入流
    hls::stream<axis_t> &out_stream   // 输出流
) {
    #pragma HLS INTERFACE mode=axis port=in_stream
    #pragma HLS INTERFACE mode=axis port=out_stream
    #pragma HLS INTERFACE mode=ap_ctrl_none port=return

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE

        // 从输入流读取
        axis_t in_data = in_stream.read();

        // 处理
        int result = in_data.data * 2 + 1;

        // 写入输出流
        axis_t out_data;
        out_data.data = result;
        out_data.keep = -1;  // 所有字节有效
        out_data.strb = -1;
        out_data.last = (i == 999);  // 标记最后一个数据
        out_stream.write(out_data);
    }
}

/* AXI Stream信号：
   - TDATA    - 传输数据
   - TKEEP    - 字节有效指示
   - TSTRB    - 字节类型指示（数据/位置）
   - TLAST    - 包结束指示
   - TVALID   - 主设备数据有效
   - TREADY   - 从设备准备好接收
   - TID      - 数据流ID（可选）
   - TDEST    - 目标路由（可选）
   - TUSER    - 用户自定义（可选）
*/
