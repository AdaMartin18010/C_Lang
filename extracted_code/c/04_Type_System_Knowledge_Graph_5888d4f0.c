/*
 * Auto-generated from: 06_Thinking_Representation\07_Knowledge_Graph\04_Type_System_Knowledge_Graph.md
 * Line: 249
 * Language: c
 * Block ID: 5888d4f0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

const int ci = 10;              // ci 是常量，不可修改
int const ic = 10;              // 等价写法

const int *pci;                 // pci 指向常量 int（值不可改）
int * const cpi = &x;           // cpi 是常量指针（指向不可改）
const int * const cpci = &y;    // 两者都不可改

volatile int vi;                // vi 可能被外部修改，禁止优化

void func(int *restrict p, int *restrict q) {
    // p 和 q 指向不同内存，可优化访问
}

_Atomic int ai;                 // ai 是原子类型，线程安全
