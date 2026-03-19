/*
 * Auto-generated from: 04_Industrial_Scenarios\10_DNA_Storage\02_Error_Correction_Coding.md
 * Line: 778
 * Language: c
 * Block ID: 6491a41a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 忽略解码失败，返回未纠正数据
rs_decode(rs, received, corrected);
return corrected;  // 可能仍然是错的!

// ✅ 正确: 检查返回值并处理失败
int num_errors = rs_decode(rs, received, corrected);
if (num_errors < 0) {
    // 解码失败，错误数超过纠错能力
    // 标记为擦除，请求重传或使用更强编码
    return ERR_UNCORRECTABLE;
} else if (num_errors > rs->t / 2) {
    // 检测到但可能未完全纠正
    log_warning("High error count: %d", num_errors);
}
