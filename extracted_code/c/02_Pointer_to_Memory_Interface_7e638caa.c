/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 173
 * Language: c
 * Block ID: 7e638caa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 单端口RAM - 读写不能同时进行
void single_port_ram(int mem[1024], int addr, int we, int wdata, int *rdata) {
    #pragma HLS INTERFACE mode=ram_1p port=mem

    if (we) {
        mem[addr] = wdata;
    }
    *rdata = mem[addr];
}

/* 硬件接口：
   ┌────────────────────────────────────────┐
   │           Single-Port RAM              │
   ├────────────────────────────────────────┤
   │  clk      ────────┐                    │
   │  address[9:0] ────┼──▶ 存储器阵列      │
   │  we ──────────────┼──▶ 写使能          │
   │  wdata[31:0] ─────┼──▶ 写数据          │
   │  rdata[31:0] ◀────┘    读数据          │
   └────────────────────────────────────────┘
*/
