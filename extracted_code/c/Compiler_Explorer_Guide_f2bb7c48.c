/*
 * Auto-generated from: 14_Video_Tutorials\02_Compiler_Explorer_Integration\Compiler_Explorer_Guide.md
 * Line: 316
 * Language: c
 * Block ID: f2bb7c48
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

volatile int flag = 0;

void wait() {
    while (!flag);  // 不会优化掉
}
