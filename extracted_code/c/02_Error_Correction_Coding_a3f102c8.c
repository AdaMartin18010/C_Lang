/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\02_Error_Correction_Coding.md
 * Line: 762
 * Language: c
 * Block ID: a3f102c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 指数表访问越界
uint8_t gf_mul_bad(const GFTables *gf, uint8_t a, uint8_t b) {
    return gf->exp[gf->log[a] + gf->log[b]];  // 可能 >= 255
}

// ✅ 正确: 使用扩展表或取模
uint8_t gf_mul_good(const GFTables *gf, uint8_t a, uint8_t b) {
    if (a == 0 || b == 0) return 0;
    int sum = gf->log[a] + gf->log[b];
    return gf->exp[sum];  // exp表大小为512，自动处理溢出
}
