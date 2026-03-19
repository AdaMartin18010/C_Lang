/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\14_C_to_Hardware_Mapping\02_Pointer_to_Memory_Interface.md
 * Line: 246
 * Language: c
 * Block ID: 005fa325
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// FIFO队列 - 先进先出缓冲区
void fifo_example(
    int fifo[DEPTH],
    int din, int *dout,
    int *empty, int *full,
    int wr_en, int rd_en
) {
    #pragma HLS INTERFACE mode=ap_fifo port=fifo

    static int head = 0, tail = 0, count = 0;

    *empty = (count == 0);
    *full = (count == DEPTH);

    if (wr_en && !*full) {
        fifo[tail] = din;
        tail = (tail + 1) % DEPTH;
        count++;
    }

    if (rd_en && !*empty) {
        *dout = fifo[head];
        head = (head + 1) % DEPTH;
        count--;
    }
}

/* FIFO硬件结构：
   ┌─────────────────────────────────────────┐
   │              FIFO Memory                │
   ├─────────────────────────────────────────┤
   │  din ──────────┐                        │
   │  wr_en ────────┼──▶ ┌─────┐             │
   │  full ◀────────┘    │ RAM │             │
   │                     └──┬──┘             │
   │  dout ◀────────────────┘                │
   │  rd_en ────────┐                        │
   │  empty ◀───────┘                        │
   │                                         │
   │  [head pointer] [tail pointer] [count]  │
   └─────────────────────────────────────────┘
*/
