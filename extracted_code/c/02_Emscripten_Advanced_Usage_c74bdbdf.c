/*
 * Auto-generated from: 10_WebAssembly_C\02_Emscripten_Advanced_Usage.md
 * Line: 759
 * Language: c
 * Block ID: c74bdbdf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// emscripten/emscripten.h 中的定义
#ifndef EMSCRIPTEN_KEEPALIVE
#ifdef __EMSCRIPTEN__
#include "em_asm.h"
// 使用 __attribute__((used)) 防止被优化器移除
// 使用 __attribute__((visibility("default"))) 确保导出
#define EMSCRIPTEN_KEEPALIVE __attribute__((used)) __attribute__((visibility("default")))
#else
#define EMSCRIPTEN_KEEPALIVE
#endif
#endif
