/*
 * Auto-generated from: 03_System_Technology_Domains\04_Garbage_Collection.md
 * Line: 7
 * Language: c
 * Block ID: 4a814a8f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int* ptr = (int*)malloc(sizeof(int) * 100);
// 使用内存...
free(ptr);      // 必须手动释放
