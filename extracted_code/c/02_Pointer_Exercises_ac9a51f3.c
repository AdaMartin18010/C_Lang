/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 319
 * Language: c
 * Block ID: ac9a51f3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用宏标记已释放指针
#ifdef DEBUG
    #define FREE(ptr) do { free(ptr); ptr = NULL; } while(0)
#else
    #define FREE(ptr) free(ptr)
#endif
