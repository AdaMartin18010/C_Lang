/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\02_Pointer_Deep_Dive.md
 * Line: 671
 * Language: c
 * Block ID: 386c4b4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 所有权注释示例
// caller owns: 调用者负责释放
// callee owns: 被调函数负责释放

// 返回caller拥有的内存
caller owns char* read_file(const char* path);

// 接收caller拥有的内存，接管所有权
callee owns void process_data(caller owns void* data);
