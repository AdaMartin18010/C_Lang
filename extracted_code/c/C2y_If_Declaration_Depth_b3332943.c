/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 203
 * Language: c
 * Block ID: b3332943
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C++17 if-init 示例
if (auto fp = std::fopen("file.txt", "r"); fp != nullptr) {
    // 使用 fp
    std::fclose(fp);
}
// fp 已超出作用域
