/*
 * Auto-generated from: 17_Ada_SPARK\01_Ada_Basics\01_Syntax_Types.md
 * Line: 405
 * Language: c
 * Block ID: 8ee63155
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C没有内置合约检查
float sqrt(float x) {
    // 必须手动检查
    if (x < 0) { /* 错误处理 */ }
    return sqrtf(x);
}
