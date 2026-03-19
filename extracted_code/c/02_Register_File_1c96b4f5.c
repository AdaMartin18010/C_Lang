/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\02_Register_File.md
 * Line: 1017
 * Language: c
 * Block ID: 1c96b4f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 当寄存器不足时，变量溢出到栈

void many_locals() {
    int a, b, c, d, e, f, g, h, i, j;
    int k, l, m, n, o, p, q, r, s, t;
    int u, v, w, x, y, z;
    // 26个局部变量！

    a = 1; b = 2; c = 3; d = 4; e = 5;
    // ... 更多操作
}

/*
 * 寄存器分配（假设只有12个可用寄存器）：
 *
 * ┌─────────────────────────────────────────────────────────────┐
 * │  变量    分配位置        访问方式                              │
 * │  ─────────────────────────────────────────────────────────  │
 * │  a-f     x5-x10        寄存器直接访问                         │
 * │  g-l     x11-x16       寄存器直接访问                         │
 * │  m-z     栈内存        需要load/store访问                     │
 * │                                                                 │
 * │  栈布局：                                                      │
 * │  ┌─────────────────────────┐                                  │
 * │  │  保存的寄存器            │ 高地址                            │
 * │  ├─────────────────────────┤                                  │
 * │  │  局部变量 m (4字节)      │                                  │
 * │  │  局部变量 n (4字节)      │                                  │
 * │  │  ...                    │                                  │
 * │  │  局部变量 z (4字节)      │                                  │
 * │  ├─────────────────────────┤                                  │
 * │  │  其他数据               │                                  │
 * │  └─────────────────────────┘ 低地址 (SP指向)                    │
 * │                                                                 │
 * │  访问栈上变量的汇编：                                           │
 * │  lw   t0, -24(s0)       # 加载变量m到t0                        │
 * │  addi t0, t0, 1         # m++                                  │
 * │  sw   t0, -24(s0)       # 存回变量m                            │
 * └─────────────────────────────────────────────────────────────┘
 */
