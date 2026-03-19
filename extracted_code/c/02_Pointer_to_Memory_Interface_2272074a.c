/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 447
 * Language: c
 * Block ID: 2272074a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// AXI-Lite从接口 - 用于寄存器配置
void axi_lite_slave_example(
    int *in1, int *in2,     // 配置参数（从寄存器读取）
    int *out,               // 状态输出（写入寄存器）
    int data[1000]          // 数据存储器（AXI主接口）
) {
    #pragma HLS INTERFACE mode=s_axilite port=in1 bundle=CTRL
    #pragma HLS INTERFACE mode=s_axilite port=in2 bundle=CTRL
    #pragma HLS INTERFACE mode=s_axilite port=out bundle=CTRL
    #pragma HLS INTERFACE mode=s_axilite port=return bundle=CTRL
    #pragma HLS INTERFACE mode=m_axi port=data offset=slave

    int threshold = *in1;
    int scale = *in2;
    int count = 0;

    for (int i = 0; i < 1000; i++) {
        #pragma HLS PIPELINE
        if (data[i] > threshold) {
            data[i] = data[i] * scale;
            count++;
        }
    }

    *out = count;
}

/* 寄存器映射（AXI-Lite）：
   偏移地址    名称              描述
   ─────────────────────────────────────────
   0x00        ap_ctrl           控制寄存器
               [0] ap_start      启动信号
               [1] ap_done       完成标志
               [2] ap_idle       空闲标志
               [3] ap_ready      就绪标志
               [7] auto_restart  自动重启

   0x04        GIE               全局中断使能
   0x08        IER               中断使能寄存器
   0x0C        ISR               中断状态寄存器
   0x10        in1               输入参数1
   0x18        in2               输入参数2
   0x20        out               输出结果
   0x28        data              数据缓冲区地址
*/
