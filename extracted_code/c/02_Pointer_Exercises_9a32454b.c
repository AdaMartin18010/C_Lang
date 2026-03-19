/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 330
 * Language: c
 * Block ID: 9a32454b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用mprotect检测越界
void* protected_alloc(size_t size) {
    // 分配保护页
    // 任何越界访问将触发SIGSEGV
}
