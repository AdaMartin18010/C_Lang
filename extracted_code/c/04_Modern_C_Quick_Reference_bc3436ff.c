/*
 * Auto-generated from: 06_Thinking_Representation\08_Index\04_Modern_C_Quick_Reference.md
 * Line: 182
 * Language: c
 * Block ID: bc3436ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C11前: 编译器扩展
__thread int tls_var;  // GCC
__declspec(thread) int tls_var;  // MSVC

// C11起
_Thread_local int tls_var;

// C23简化 (不推荐)
thread_local int tls_var;  // 需要头文件
