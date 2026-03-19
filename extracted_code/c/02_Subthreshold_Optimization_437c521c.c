/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Cryogenic_Superconducting\02_Subthreshold_Optimization.md
 * Line: 106
 * Language: c
 * Block ID: 437c521c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================================
// 亚阈值电路优化 - C语言模拟与优化工具
// ============================================================================

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

// 物理常数
#define Q_e         1.602e-19     // 元电荷 (C)
#define K_B         1.381e-23     // 玻尔兹曼常数 (J/K)
#define VT_300K     0.0258        // 300K热电压 (V)

// 工艺参数 (65nm典型值)
typedef struct {
    float vth_n;            // NMOS阈值电压 (V)
    float vth_p;            // PMOS阈值电压 (V)
    float u0_n;             // 迁移率 (cm²/V·s)
    float u0_p;
    float cox;              // 栅氧电容 (F/cm²)
    float n_subthreshold;   // 亚阈值斜率系数
    float lambda;           // 沟道长度调制系数
    float tox;              // 栅氧厚度 (nm)
    float leff;             // 有效沟道长度 (nm)
} ProcessParams;

const ProcessParams PROCESS_65NM = {
    .vth_n = 0.25f,
    .vth_p = -0.25f,
    .u0_n = 350.0f,
    .u0_p = 90.0f,
    .cox = 1.5e-6f,
    .n_subthreshold = 1.4f,
    .lambda = 0.1f,
    .tox = 1.8f,
    .leff = 60.0f
};

// ============================================================================
// 亚阈值电流模型
// ============================================================================

// 计算亚阈值漏电流 (A)
float subthreshold_current(const ProcessParams *proc, float vgs,
                           float vds, float vbs, float w, float l,
                           bool is_nmos) {
    float vth = is_nmos ? proc->vth_n : proc->vth_p;
    float u0 = is_nmos ? proc->u0_n : proc->u0_p;
    float temp = 300.0f;  // K

    float vt = K_B * temp / Q_e;  // 热电压

    // 体效应
    float gamma = 0.5f;  // 体效应系数
    float phi_f = 0.6f;  // 费米势
    float vth_eff = vth + gamma * (sqrtf(2.0f * phi_f - vbs) - sqrtf(2.0f * phi_f));

    // DIBL效应
    float eta = 0.05f;   // DIBL系数
    vth_eff -= eta * vds;

    // 亚阈值电流
    float ids0 = u0 * proc->cox * (w / l) * vt * vt *
                 expf((vgs - vth_eff) / (proc->n_subthreshold * vt));

    // 沟长调制
    float effective_vds = 1.0f - expf(-vds / vt);

    return ids0 * effective_vds;
}

// 计算栅极泄漏 (简化)
float gate_leakage(const ProcessParams *proc, float vgs, float w, float l) {
    // 隧穿电流
    float eox = 3.9f * 8.854e-12f;  // 介电常数
    float phi_b = 3.1f;              // 势垒高度 (eV)

    // 简化模型
    float jg = 1e-10f * expf(-proc->tox * sqrtf(2.0f * 9.11e-31f * Q_e * phi_b) /
                            (1.055e-34f));

    return jg * w * l * 1e-12f;  // A
}
