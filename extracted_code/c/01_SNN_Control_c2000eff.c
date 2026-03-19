/*
 * Auto-generated from: 04_Industrial_Scenarios\08_Neuromorphic\01_SNN_Control.md
 * Line: 594
 * Language: c
 * Block ID: c2000eff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 指数可能溢出
float exp_val = expf(v_mem * 0.1f);  // v_mem很大时溢出

// ✅ 使用安全范围检查
float safe_exp(float x) {
    if (x > 80.0f) return expf(80.0f);
    if (x < -80.0f) return 0.0f;
    return expf(x);
}
