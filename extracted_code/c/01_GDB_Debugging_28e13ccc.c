/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\08_Debugging_Tools\01_GDB_Debugging.md
 * Line: 236
 * Language: c
 * Block ID: 28e13ccc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// -O2优化可能删除变量或重排代码
gcc -g -O2 program.c  // 可能无法打印某些变量

// 推荐
gcc -g -O0 program.c  // 无优化
