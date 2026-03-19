/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\02_Atomic_Operations.md
 * Line: 54
 * Language: c
 * Block ID: 319c2613
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 非原子操作的问题
volatile int counter = 0;

void increment() {
    counter++;  // 实际上: 读-改-写 (3步)
}

// 汇编级展开 (x86):
// mov eax, [counter]   ; 读取
// inc eax              ; 增加
// mov [counter], eax   ; 写回
//
// 问题: 3条指令之间可能被其他线程打断！
