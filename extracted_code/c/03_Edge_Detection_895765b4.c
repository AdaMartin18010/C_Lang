/*
 * Auto-generated from: 03_System_Technology_Domains\03_Edge_Detection.md
 * Line: 522
 * Language: c
 * Block ID: 895765b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 可能越界
for (int y = 0; y < h; y++) {
    for (int x = 0; x < w; x++) {
        // 访问 (y-1)*w + (x-1) 可能越界
    }
}

// ✅ 正确: 限制范围
for (int y = 1; y < h - 1; y++) {
    for (int x = 1; x < w - 1; x++) {
        // 安全访问
    }
}
