/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 199
 * Language: c
 * Block ID: 15723eab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 真双端口RAM - 两个独立访问端口
void true_dual_port_ram(
    int mem[1024],
    int addr_a, int we_a, int wdata_a, int *rdata_a,
    int addr_b, int we_b, int wdata_b, int *rdata_b
) {
    #pragma HLS INTERFACE mode=ram_t2p port=mem

    // 端口A
    if (we_a) mem[addr_a] = wdata_a;
    *rdata_a = mem[addr_a];

    // 端口B
    if (we_b) mem[addr_b] = wdata_b;
    *rdata_b = mem[addr_b];
}

/* 硬件接口：
   ┌────────────────────────────────────────────────┐
   │            True Dual-Port RAM                  │
   ├────────────────────────────────────────────────┤
   │  Port A                Port B                  │
   │  ─────────────────    ─────────────────        │
   │  clk_a ──────────┐   clk_b ──────────┐        │
   │  addr_a[9:0] ────┼── addr_b[9:0] ────┼──▶     │
   │  we_a ───────────┼── we_b ───────────┼──▶     │
   │  wdata_a[31:0] ──┼── wdata_b[31:0] ──┼──▶     │
   │  rdata_a[31:0] ◀─┘   rdata_b[31:0] ◀─┘        │
   └────────────────────────────────────────────────┘
*/

// 简单双端口RAM - 一个只写端口，一个只读端口
void simple_dual_port_ram(
    int mem[1024],
    int waddr, int we, int wdata,
    int raddr, int *rdata
) {
    #pragma HLS INTERFACE mode=ram_s2p port=mem

    if (we) mem[waddr] = wdata;
    *rdata = mem[raddr];  // 可同时读写不同地址
}
