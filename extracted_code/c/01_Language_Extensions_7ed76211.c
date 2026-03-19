/*
 * Auto-generated from: 01_Core_Knowledge_System\06_Advanced_Layer\01_Language_Extensions.md
 * Line: 107
 * Language: c
 * Block ID: 7ed76211
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 未初始化（不置零）
int fast_var __attribute__((section(".noinit")));

// 强制对齐
int cache_line __attribute__((aligned(64)));

// 不优化掉
volatile int debug_var __attribute__((used));

// 指定段
const char version[] __attribute__((section(".version"))) = "v1.0";
