/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 440
 * Language: c
 * Block ID: a673d53b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 超前进位加法器 - O(log n) 延迟

// 生成（Generate）和传播（Propagate）信号
// G_i = A_i · B_i
// P_i = A_i ⊕ B_i
// C_{i+1} = G_i + P_i · C_i

// 4位CLA组
typedef struct {
    uint8_t sum;      // 4位和
    Bool g_out;       // 组生成
    Bool p_out;       // 组传播
    Bool c_out;       // 进位输出
} CLA4Result;

CLA4Result cla4(uint8_t a, uint8_t b, Bool c_in) {
    CLA4Result r = {0};

    Bool g[4], p[4];  // 每位生成和传播
    Bool c[5];        // 进位链
    c[0] = c_in;

    // 计算每位的G和P
    for (int i = 0; i < 4; i++) {
        Bool ai = (a >> i) & 1;
        Bool bi = (b >> i) & 1;
        g[i] = ai && bi;  // 生成
        p[i] = ai ^ bi;   // 传播（实际上是XOR）
    }

    // 超前进位计算（并行）
    c[1] = g[0] || (p[0] && c[0]);
    c[2] = g[1] || (p[1] && g[0]) || (p[1] && p[0] && c[0]);
    c[3] = g[2] || (p[2] && g[1]) || (p[2] && p[1] && g[0]) ||
           (p[2] && p[1] && p[0] && c[0]);
    c[4] = g[3] || (p[3] && g[2]) || (p[3] && p[2] && g[1]) ||
           (p[3] && p[2] && p[1] && g[0]) ||
           (p[3] && p[2] && p[1] && p[0] && c[0]);

    // 计算和
    for (int i = 0; i < 4; i++) {
        Bool sum_bit = p[i] ^ c[i];  // S_i = P_i ⊕ C_i
        r.sum |= (sum_bit << i);
    }

    r.c_out = c[4];
    r.g_out = g[3] || (p[3] && g[2]) || (p[3] && p[2] && g[1]) ||
              (p[3] && p[2] && p[1] && g[0]);
    r.p_out = p[3] && p[2] && p[1] && p[0];

    return r;
}

// 16位CLA：4个4位CLA组 + 第二级进位链
// 延迟：2级门延迟（vs 16级行波进位）
