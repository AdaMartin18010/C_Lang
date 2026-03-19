/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\01_C11_Memory_Model.md
 * Line: 668
 * Language: c
 * Block ID: 0179b321
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 错误: 非原子变量多线程访问 */
int counter = 0;

void thread_func(void) {
    for (int i = 0; i < 1000000; i++) {
        counter++;  /* 数据竞争! 未定义行为 */
    }
}

/* 正确: 使用原子变量 */
_Atomic(int) counter = 0;

void thread_func_correct(void) {
    for (int i = 0; i < 1000000; i++) {
        atomic_fetch_add(&counter, 1);
    }
}
